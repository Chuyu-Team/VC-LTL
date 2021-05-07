// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// SubAllocator.h
//
// Class definition for the ConcRT sub allocator.
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#pragma once

namespace Concurrency
{
namespace details
{
    /// <summary>
    ///     Each allocation via the sub allocator has an AllocationEntry header. All we need the allocation entry
    ///     for, is to tell us the id of the bucket, which indicates the size of the allocation. However, the size
    ///     of the header is pointer size - since we want to align the user's allocation.
    /// </summary>
    union AllocationEntry
    {
        // The index to the bucket in the suballocator, that this entry belongs to.
        ULONG_PTR m_bucketIndex;

        // Pointer to the next allocation in the bucket. This is used to chain allocations in the bucket, and we
        // do not require a lock since only one thread is guaranteed to be touching the suballocator at a time.
        AllocationEntry* m_pNext;
    };

    // A bucket that stores a particular size memory block. A SubAllocator has several allocator buckets.
    class AllocatorBucket
    {
    public:

        /// <summary>
        ///     Constructs an allocator bucket.
        /// <summary>
        AllocatorBucket();

        /// <summary>
        ///     Destroys an allocator bucket.
        /// </summary>
        ~AllocatorBucket();

        /// <summary>
        ///     Returns an allocation from the bucket if it is non-empty, and NULL if it is empty.
        /// </summary>
        AllocationEntry* Alloc();

        /// <summary>
        ///     Adds the block to the bucket and returns true if the maximum depth is not reached.
        ///     If the bucket is 'full', it returns false, and the caller is responsible for freeing
        ///     the block to the CRT heap.
        /// </summary>
        bool Free(AllocationEntry* pAllocation);

    private:

        // The current depth of the bucket.
        int m_depth;

        // The head of the free block list.
        AllocationEntry* m_pHead;

        // The maximum number of allocations the sub allocator will cache per bucket.
        static const int s_maxBucketDepth = 32;
    };

#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    class SubAllocator
    {
    public:

        /// <summary>
        ///     Constructs a suballocator.
        /// </summary>
        SubAllocator() :
            m_fExternalAllocator(false)
        {
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
        void* Alloc(size_t numBytes);

        /// <summary>
        ///     Frees a block of memory previously allocated by the Alloc API.
        /// </summary>
        /// <param name="pAllocation">
        ///     A pointer to an allocation previously allocated by Alloc.
        /// </param>
        void Free(void* pAllocation);

        /// <summary>
        ///     A static allocation API that allocates directly from the CRT heap, and encodes the bucket id in the allocation,
        ///     based on the size of the block. This is used by callers that are unable to get access to a suballocator at
        ///     the time they are allocating memory.
        /// </summary>
        static void* StaticAlloc(size_t numBytes);

        /// <summary>
        ///     A static free API that frees directly to the CRT heap. This is used by callers that are unable to get access
        ///     to a suballocator at the time they are freeing memory.
        /// </summary>
        static void StaticFree(void* pAllocation)
        {
            delete [] (char*) ((AllocationEntry*)pAllocation - 1);
        }

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
        static int GetBucketIndex(size_t numBytes);

        /// <summary>
        ///     Every time an allocator is reused, this flag is set to denote whether it is one out of the 'fixed pool' - the set
        ///     of allocators that are used for external contexts.
        /// </summary>
        void SetExternalAllocatorFlag(bool flag) { m_fExternalAllocator = flag; }

        /// <summary>
        ///     Returns true, if this allocator is assigned to, or was last assigned to an external context.
        /// </summary>
        bool IsExternalAllocator() { return m_fExternalAllocator; }

    private:

        // private methods

#if defined(_DEBUG)

        /// <summary>
        ///     Initialize a block allocated from the freelist. Perform debug validation on the block to
        ///     detect user errors.
        /// </summary>
        bool InitAndCheckBlockOnAlloc(AllocationEntry *pAllocationEntry, size_t numBytes);

        /// <summary>
        ///     Initialize a block that is added to the freelist. Perform debug validation on the block to
        ///     detect user errors.
        /// </summary>
        bool InitAndCheckBlockOnFree(AllocationEntry *pAllocationEntry, size_t numBytes);

        /// <summary>
        ///     Helper routine that checks where the given block is filled with
        ///     the given pattern.
        /// </summary>
        bool CheckBytes(unsigned char * pBlock, unsigned char bCheck, size_t numBytes);
#endif

        // private member variables

        friend class SchedulerBase;
        template <class T> friend class LockFreeStack;

        // Entry for freelist of allocators
        SLIST_ENTRY m_slNext;

        // The total number of buckets.
        static const int s_numBuckets = 96;

        // The array of buckets that holds memory for allocation.
        AllocatorBucket m_buckets[s_numBuckets];

        // This flag is set to true, when it this suballocator is handed to a caller that invoked GetSubAllocator with an argument
        // of 'true'.
        bool m_fExternalAllocator;

        // An array that holds the bucket sizes.
        static const int s_bucketSizes[s_numBuckets];

        // A number such that 2 ^ GranularityShift = the mininum granularity of the allocation buckets.
        static const int GranularityShift;

        // The allocation size supported by the largest bucket.
        static const int MaxAllocationSize;
    };
#pragma warning(pop)
} // namespace details
} // namespace Concurrency
