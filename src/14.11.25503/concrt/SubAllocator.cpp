// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SubAllocator.cpp
//
// Implementation of ConcRT sub allocator
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#include "concrtinternal.h"

namespace Concurrency
{
/// <summary>
///     Allocates a block of memory of the size specified.
/// </summary>
/// <param name="numBytes">
///     Number of bytes to allocate.
/// </param>
/// <returns>
///     A pointer to newly allocated memory.
/// </returns>
_CONCRTIMP void* Alloc(size_t numBytes)
{
    if (numBytes > MAXINT_PTR)
    {
        throw std::bad_alloc();
    }

    return SchedulerBase::CurrentContext()->Alloc(numBytes);
}

/// <summary>
///     Frees a block of memory previously allocated by the Alloc API.
/// </summary>
/// <param name="pAllocation">
///     A pointer to an allocation previously allocated by Alloc. If pAllocation is NULL, the API will ignore it, and return
///     immediately.
/// </param>
_Use_decl_annotations_
_CONCRTIMP void Free(void * pAllocation)
{
    if (pAllocation == NULL)
    {
        return;
    }
    SchedulerBase::CurrentContext()->Free(pAllocation);
}

namespace details
{
    //
    // Define static variables.
    //

#if defined(_DEBUG)

    // Debug patterns to fill allocated blocks (borrowed from dbgheap.c)

    static const unsigned char _bNoMansLandFill = 0xFD;   /* fill no-man's land with this */
    static const unsigned char _bAlignLandFill  = 0xED;   /* fill no-man's land for aligned routines */
    static const unsigned char _bDeadLandFill   = 0xDD;   /* fill free objects with this */
    static const unsigned char _bCleanLandFill  = 0xCD;   /* fill new objects with this */
#endif

#ifdef _WIN64
    // This supports the same number of buckets and bucket sizes as the LFH heap upto 8192 bytes, i.e., 96 buckets.
    const int SubAllocator::s_bucketSizes[SubAllocator::s_numBuckets] = {
    /* granularity -  16 */  16,   32,   48,   64,   80,   96,   112,  128,  144,  160,  176,  192,  208,  224,  240,  256, // sizeClass 0, blockUnits 1 - 16
    /* granularity -  16 */  272,  288,  304,  320,  336,  352,  368,  384,  400,  416,  432,  448,  464,  480,  496,  512, // sizeClass 0, blockUnits 17 - 32
    /* granularity -  32 */  544,  576,  608,  640,  672,  704,  736,  768,  800,  832,  864,  896,  928,  960,  992,  1024,// sizeClass 1, blockUnits 33 - 64
    /* granularity -  64 */  1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048,// sizeClass 2, blockUnits 65 - 128
    /* granularity - 128 */  2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968, 4096,// sizeClass 3, blockUnits 129 - 256
    /* granularity - 256 */  4352, 4608, 4864, 5120, 5376, 5632, 5888, 6144, 6400, 6656, 6912, 7168, 7424, 7680, 7936, 8192,// sizeClass 4, blockUnits 257 - 512
    };

    // A number such that 2 ^ GranularityShift = Granularity.
    const int SubAllocator::GranularityShift = 4;

    // The allocation size supported by the largest bucket.
    const int SubAllocator::MaxAllocationSize = 8192;

#else

    // This supports the same number of buckets and bucket sizes as the LFH heap upto 4096 bytes, i.e., 96 buckets.
    const int SubAllocator::s_bucketSizes[SubAllocator::s_numBuckets] = {
    /* granularity -   8 */  8,    16,   24,   32,   40,   48,   56,   64,   72,   80,   88,   96,   104,  112,  120,  128, // sizeClass 0, blockUnits 1 - 16
    /* granularity -   8 */  136,  144,  152,  160,  168,  176,  184,  192,  200,  208,  216,  224,  232,  240,  248,  256, // sizeClass 0, blockUnits 17 - 32
    /* granularity -  16 */  272,  288,  304,  320,  336,  352,  368,  384,  400,  416,  432,  448,  464,  480,  496,  512, // sizeClass 1, blockUnits 33 - 64
    /* granularity -  32 */  544,  576,  608,  640,  672,  704,  736,  768,  800,  832,  864,  896,  928,  960,  992,  1024,// sizeClass 2, blockUnits 65 - 128
    /* granularity -  64 */  1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048,// sizeClass 3, blockUnits 129 - 256
    /* granularity - 128 */  2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968, 4096 // sizeClass 4, blockUnits 257 - 512
    };

    // A number such that 2 ^ GranularityShift = Granularity.
    const int SubAllocator::GranularityShift = 3;

    // The allocation size supported by the largest bucket.
    const int SubAllocator::MaxAllocationSize = 4096;
#endif

    /// <summary>
    ///     Returns an index into the array of allocator buckets for this sub allocator. The allocation size of the
    ///     bucket is guranteed to satisfy numBytes.
    /// </summary>
    /// <param name="numBytes">
    ///     The size of the allocation. This is what the user requested plus space for the ConcRT allocator header.
    /// </param>
    /// <returns>
    ///     An index into the array of allocator buckets such that.s_bucketSizes[returnedBucketIndex] >= numBytes
    /// </returns>
    int SubAllocator::GetBucketIndex(size_t numBytes)
    {
        static const int GranularityMask = (1 << GranularityShift) - 1;

        int bucketIndex = -1;
        size_t allocationSize = (size_t) (((ULONG_PTR)numBytes + GranularityMask) & ~((ULONG_PTR)GranularityMask));

        if (allocationSize > MaxAllocationSize)
        {
            // We are unable to satisfy this allocation by an allocator bucket. It should be forwarded to the LFH heap.
            return bucketIndex;
        }

        int blockUnits = (int)(allocationSize >> GranularityShift);

        // blockUnits is the number of Granularity size chunks that make up the allocationSize. A blockUnit of 1 is satisfied
        // by allocator bucket 0. We need to find the index of the bucket that holds the minimum sized allocation that will
        // satisfy allocationSize.
        ASSERT(blockUnits > 0);

        // Detect if the allocation will fall within buckets 0 - 31
        if (blockUnits <= 32)
        {
            bucketIndex = blockUnits - 1;
        }
        else
        {
            int sizeClass = 5; // Add 1 << 5 = 32

            while ((blockUnits >> sizeClass) > 0)
            {
                sizeClass += 1;
            }

            sizeClass -= 5;

            ASSERT(sizeClass > 0);

            // Round blockUnits up to the block unit granularity of the size class.
            int sizeClassMask = (1 << sizeClass) - 1;
            blockUnits = (int) (((ULONG_PTR)blockUnits + sizeClassMask) & ~((ULONG_PTR)sizeClassMask));

            bucketIndex = (sizeClass << 4) + (blockUnits >> sizeClass) - 1;
        }

        ASSERT(allocationSize <= (size_t)s_bucketSizes[bucketIndex]);
        ASSERT(bucketIndex == 0 || allocationSize > (size_t)s_bucketSizes[bucketIndex - 1]);

        return bucketIndex;
    }

    /// <summary>
    ///     Allocates a block of memory of the size specified.
    /// </summary>
    /// <param name="numBytes">
    ///     Number of bytes to allocate.
    /// </param>
    /// <returns>
    ///     A pointer to newly allocated memory.
    /// </returns>
    void* SubAllocator::Alloc(size_t numBytes)
    {
        AllocationEntry* pAllocationEntry = NULL;
        size_t allocationSize = numBytes + sizeof(AllocationEntry);

        int bucketIndex = GetBucketIndex(allocationSize);

        if (bucketIndex != -1)
        {
            ASSERT(bucketIndex < sizeof(s_bucketSizes));
            pAllocationEntry = m_buckets[bucketIndex].Alloc();

#if defined(_DEBUG)
            if (pAllocationEntry != NULL)
            {
                InitAndCheckBlockOnAlloc(pAllocationEntry, s_bucketSizes[bucketIndex]);
            }
#endif

        }

        if (pAllocationEntry == NULL)
        {
            // We need to allocate memory from the CRT heap since either the bucket was empty,
            // or the size is not one the allocator caches.
            pAllocationEntry = (AllocationEntry*) _concrt_new char[bucketIndex == -1 ? allocationSize : s_bucketSizes[bucketIndex]];
        }

        ASSERT(pAllocationEntry != NULL);
        pAllocationEntry->m_bucketIndex = (ULONG_PTR)Security::EncodePointer((PVOID)(intptr_t)bucketIndex);

        return (void*)(pAllocationEntry + 1);
    }

    /// <summary>
    ///     Frees a block of memory previously allocated by the Alloc API.
    /// </summary>
    /// <param name="pAllocation">
    ///     A pointer to an allocation previously allocated by Alloc.
    /// </param>
    void SubAllocator::Free(void* pAllocation)
    {
        AllocationEntry* pAllocationEntry = (AllocationEntry*)pAllocation - 1;
        // Disable data truncation warning as only lower 32-bits are needed.
        #pragma warning(push)
        #pragma warning(disable: 4302)
        int bucketIndex = (int)(intptr_t)Security::DecodePointer((PVOID)pAllocationEntry->m_bucketIndex);
        #pragma warning(pop)

        ASSERT((bucketIndex == -1) || bucketIndex < sizeof(s_bucketSizes));

        if ((bucketIndex == -1) || !m_buckets[bucketIndex].Free(pAllocationEntry))
        {
            delete [] (char*)pAllocationEntry;
        }
#if defined(_DEBUG)
        else
        {
            InitAndCheckBlockOnFree(pAllocationEntry, s_bucketSizes[bucketIndex]);
        }
#endif

    }

    /// <summary>
    ///     A static allocation API that allocates directly from the CRT heap, and encodes the bucket id in the allocation,
    ///     based on the size of the block. This is used by callers that are unable to get access to a suballocator at
    ///     the time they are allocating memory.
    /// </summary>
    void* SubAllocator::StaticAlloc(size_t numBytes)
    {
        AllocationEntry* pAllocationEntry = NULL;
        size_t allocationSize = numBytes + sizeof(AllocationEntry);

        int bucketIndex = GetBucketIndex(allocationSize);
        pAllocationEntry = (AllocationEntry*) _concrt_new char[bucketIndex == -1 ? allocationSize : s_bucketSizes[bucketIndex]];

        ASSERT(pAllocationEntry != NULL);
        pAllocationEntry->m_bucketIndex = (ULONG_PTR)Security::EncodePointer((PVOID)(intptr_t)bucketIndex);

        return (void*)(pAllocationEntry + 1);
    }

#if defined(_DEBUG)
    /// <summary>
    ///     Initialize a block allocated from the freelist. Perform debug validation on the block to
    ///     detect user errors.
    /// </summary>
    bool SubAllocator::InitAndCheckBlockOnAlloc(AllocationEntry *pAllocationEntry, size_t numBytes)
    {
        // Validate the pointer
        ASSERT(_CrtIsValidHeapPointer((const void *)pAllocationEntry));

        unsigned char * userData = (unsigned char *)(pAllocationEntry + 1);

        ASSERT(numBytes > sizeof(AllocationEntry));
        size_t userNumBytes = numBytes - sizeof(AllocationEntry);

        // Ensure that the free block has not been overwritten.
        ASSERT(CheckBytes(userData, _bDeadLandFill, userNumBytes));

        // Initialize the new block
        memset((void *)userData, _bCleanLandFill, userNumBytes);

        return true;
    }

    /// <summary>
    ///     Initialize a block that is added to the freelist. Perform debug validation on the block to
    ///     detect user errors.
    /// </summary>
    bool SubAllocator::InitAndCheckBlockOnFree(AllocationEntry *pAllocationEntry, size_t numBytes)
    {
        // Validate the pointer.
        ASSERT(_CrtIsValidHeapPointer((const void *)pAllocationEntry));

        ASSERT(numBytes > sizeof(AllocationEntry));
        // Initialize the free block
        memset((void *)(pAllocationEntry + 1), _bDeadLandFill, (numBytes - sizeof(AllocationEntry)));

        return true;
    }

    /// <summary>
    ///     Helper routine that checks where the given block is filled with
    ///     the given pattern.
    /// </summary>
    bool SubAllocator::CheckBytes(unsigned char * pBlock, unsigned char bCheck, size_t numBytes)
    {
        while (numBytes--)
        {
            if (*pBlock++ != bCheck)
            {
                return false;
            }
        }

        return true;
    }
#endif

    /// <summary>
    ///     Constructs an allocator bucket.
    /// <summary>
    AllocatorBucket::AllocatorBucket() : m_depth(0)
    {
        m_pHead = (AllocationEntry*)Security::EncodePointer(NULL);
    }

    /// <summary>
    ///     Returns an allocation from the bucket if it is non-empty, and NULL if it is empty.
    /// </summary>
    AllocationEntry* AllocatorBucket::Alloc()
    {
        AllocationEntry* pAllocationEntry = (AllocationEntry*)Security::DecodePointer(m_pHead);

        if (pAllocationEntry != NULL)
        {
            ASSERT(m_depth > 0);
            m_pHead = pAllocationEntry->m_pNext;
            --m_depth;
        }

        return pAllocationEntry;
    }

    /// <summary>
    ///     Adds the block to the bucket and returns true if the maximum depth is not reached.
    ///     If the bucket is 'full', it returns false, and the caller is responsible for freeing
    ///     the block to the CRT heap.
    /// </summary>
    bool AllocatorBucket::Free(AllocationEntry* pAllocation)
    {
        if (m_depth < s_maxBucketDepth)
        {
            pAllocation->m_pNext = m_pHead;
            m_pHead = (AllocationEntry*)Security::EncodePointer(pAllocation);
            ++m_depth;

            ASSERT(m_depth <= s_maxBucketDepth);
            return true;
        }

        return false;
    }

    /// <summary>
    ///     Destroys an allocator bucket.
    /// </summary>
    AllocatorBucket::~AllocatorBucket()
    {
        while (m_depth != 0)
        {
            AllocationEntry * pAllocationEntry = (AllocationEntry*)Security::DecodePointer(m_pHead);
            ASSERT(pAllocationEntry != NULL);

            m_pHead = pAllocationEntry->m_pNext;
            delete [] (char*)pAllocationEntry;

            --m_depth;
        }
    }
} // namespace details
} // namespace Concurrency
