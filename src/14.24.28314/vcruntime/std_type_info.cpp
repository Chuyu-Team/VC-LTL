//
// std_type_info.cpp
//
//       Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definitions of the std::type_info implementation functions, used for
// Run-Time Type Information (RTTI).
//
#include <vcruntime_internal.h>
#include <vcruntime_string.h>
#include <vcruntime_typeinfo.h>
#include <undname.h>





extern "C" int __cdecl __std_type_info_compare(
    __std_type_info_data const* const lhs,
    __std_type_info_data const* const rhs
    )
{
    if (lhs == rhs)
    {
        return 0;
    }

    return strcmp(lhs->_DecoratedName + 1, rhs->_DecoratedName + 1);
}

extern "C" size_t __cdecl __std_type_info_hash(
    __std_type_info_data const* const data
    )
{
    // FNV-1a hash function for the undecorated name

    #ifdef _WIN64
    static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
    size_t const fnv_offset_basis = 14695981039346656037ULL;
    size_t const fnv_prime        = 1099511628211ULL;
    #else
    static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
    size_t const fnv_offset_basis = 2166136261U;
    size_t const fnv_prime        = 16777619U;
    #endif

    size_t value = fnv_offset_basis;
    for (char const* it = data->_DecoratedName + 1; *it != '\0'; ++it)
    {
        value ^= static_cast<size_t>(static_cast<unsigned char>(*it));
        value *= fnv_prime;
    }

    #ifdef _WIN64
    static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
    value ^= value >> 32;
    #else
    static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
    #endif

    return value;
}

extern "C" char const* __cdecl __std_type_info_name(
    __std_type_info_data* const data,
    __type_info_node*     const root_node
    )
{
    // First check to see if we've already cached the undecorated name; if we
    // have, we can just return it:
    {
        char const* const cached_undecorated_name = __crt_interlocked_read_pointer(&data->_UndecoratedName);
        if (cached_undecorated_name)
        {
            return cached_undecorated_name;
        }
    }

    __crt_unique_heap_ptr<char> undecorated_name(__unDName(
        nullptr,
        data->_DecoratedName + 1,
        0,
        [](size_t const n) { return _malloc_crt(n); },
        [](void*  const p) { return _free_crt(p);   },
        UNDNAME_32_BIT_DECODE | UNDNAME_TYPE_ONLY));

    if (!undecorated_name)
    {
        return nullptr; // CRT_REFACTOR TODO This is nonconforming
    }

    size_t undecorated_name_length = strlen(undecorated_name.get());
    while (undecorated_name_length != 0 && undecorated_name.get()[undecorated_name_length - 1] == ' ')
    {
        undecorated_name.get()[undecorated_name_length - 1] = '\0';
        --undecorated_name_length;
    }

    size_t const undecorated_name_count = undecorated_name_length + 1;
    size_t const node_size = sizeof(SLIST_ENTRY) + undecorated_name_count;

    __crt_unique_heap_ptr<void> node_block(_malloc_crt(node_size));
    if (!node_block)
    {
        return nullptr; // CRT_REFACTOR TODO This is nonconforming
    }

    PSLIST_ENTRY const node_header = static_cast<PSLIST_ENTRY>(node_block.get());
    char*        const node_string = reinterpret_cast<char*>(node_header + 1);

    *node_header = SLIST_ENTRY{};
    strcpy_s(node_string, undecorated_name_count, undecorated_name.get());

    char const* const cached_undecorated_name = __crt_interlocked_compare_exchange_pointer(
        &data->_UndecoratedName,
        node_string,
        nullptr);

    // If the cache already contained an undecorated name pointer, another
    // thread must have cached it while we were computing the undecorated
    // name.  Discard the string we created and return the cached string:
    if (cached_undecorated_name)
    {
        return cached_undecorated_name;
    }

    // Otherwise, we've successfully cached our string; link it into the list
    // and return it:
    node_block.detach();
    InterlockedPushEntrySList(&root_node->_Header, node_header);
    return node_string;
}

// This function is called during module unload to clean up all of the undecorated
// name strings that were allocated by calls to name().
extern "C" void __cdecl __std_type_info_destroy_list(
    __type_info_node* const root_node
    )
{
    PSLIST_ENTRY current_node = InterlockedFlushSList(&root_node->_Header);
    while (current_node)
    {
        PSLIST_ENTRY const next_node = current_node->Next;
        _free_crt(current_node);
        current_node = next_node;
    }
}
