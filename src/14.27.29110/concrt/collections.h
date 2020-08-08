// ==++==
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// ==--==
// =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//
// collections.h
//
// Header file containing collection classes for ConcRT
//
// These data structures assume in-data links with the names m_pNext and m_pPrev
// Currently defined collections are: Stack, LockFreeStack, SafeStack
//                                    SQueue, SafeSQueue
//                                    List, SafeRWList
//                                    Hash, SafeHash
//
// =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

#if !defined(ASSERT) && defined(_DEBUG)
#define ASSERT(x) do {_CONCRT_ASSERT(x); __assume(x);} while(false)
#elif !defined(ASSERT)
#define ASSERT(x) __assume(x)
#endif

#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field) \
    ((type *)((char *)(address) - (ULONG_PTR)(&((type *)0)->field)))
#endif

#if !defined(UNREACHED)
#define UNREACHED 0
#endif

namespace Concurrency
{
namespace details
{
    //
    // Allows multiple intrusive links within a single data structure.
    //
    struct ListEntry
    {
        ListEntry *m_pPrev;
        ListEntry *m_pNext;
    };

    //
    // Heap allocated generic list block.
    //
    template <class T>
    struct ListNode
    {
        ListNode(T* pData) : m_pData(pData)
        {
        }

        ListNode *m_pPrev;
        ListNode *m_pNext;
        T* m_pData;
    };

    class CollectionTypes
    {
    public:
        //
        // public types
        //

        class Count
        {
        public:
            Count() : m_count(0) {}
            void Increment() { ++m_count; }
            void Decrement() { --m_count; }
            int Value() const { return m_count; }
            void Clear() { m_count = 0; }
        private:
            int m_count;
        };

        class NoCount
        {
        public:
            static void Increment() {}
            static void Decrement() {}
            static int Value() { ASSERT(UNREACHED); return -1; }
            static void Clear() {}
        };
    };

    //
    // The base class of stacks.  This implementation is not thread-safe.
    //
    template <class T, class Counter = CollectionTypes::NoCount>
    class Stack : public Counter
    {
    public:
        Stack() : m_pHead(NULL) {}

        T* Pop()
        {
            if (m_pHead == NULL)
            {
                return NULL;
            }
            T* pHead = m_pHead;
            m_pHead = m_pHead->m_pNext;
            Counter::Decrement();
            return pHead;
        }

        void Push(T* pNode)
        {
            ASSERT(pNode != NULL);

            Counter::Increment();
            pNode->m_pNext = m_pHead;
            m_pHead = pNode;
        }

        bool Empty() const
        {
            return m_pHead == NULL;
        }

        int Count() const
        {
            return Counter::Value();
        }

        T* First()
        {
            return m_pHead;
        }

        T* Next(T* pNode)
        {
            //OACR_USE_PTR(this);
            return pNode->m_pNext;
        }

    private:
        T* m_pHead;
    };

    //
    // An implementation of interlocked SLIST that does not support Pop. This
    // avoids the ABA problem. The reason for this data structure is to get
    // to the top node (Windows SLIST does not provide this functionality
    // without FirstSListEntry).
    // Type T is required to have an intrusive SLIST_ENTRY m_slNext.
    //
    template <class T>
    class LockFreePushStack
    {
    public:
        LockFreePushStack()
        {
            m_pTop = NULL;
        }

        ~LockFreePushStack()
        {
            // We expect the user to have flushed the stack
            // before deleting it.
            ASSERT(m_pTop == NULL);
        }

        // Returns the current top of the stack
        // THIS OPERATION IS NOT SYNCHRONIZED
        // Anyone walking the list needs to ensure that there
        // are no concurrent push/flush operations.
        T * Unsafe_Top()
        {
            return Delta(m_pTop);
        }

        // Push an element into the stack
        void Push(T * pNode)
        {
            PSLIST_ENTRY top;

            do
            {
                // Make this node point to the head.
                // m_pTop needs to be volatile to ensure that it is not enregistered
                top = m_pTop;
                pNode->m_slNext.Next = top;

                // Make head point to this node
            }
            while ((InterlockedCompareExchangePointer(reinterpret_cast<volatile PVOID *>(&m_pTop), &pNode->m_slNext, top) != top));
        }

        // Flush all the elements in the stack
        T * Flush()
        {
            return Delta(reinterpret_cast<void *>(InterlockedExchangePointer(reinterpret_cast<volatile PVOID *>(&m_pTop), NULL)));
        }

        static T* Next(T* pNode)
        {
            return Delta(pNode->m_slNext.Next);
        }

    private:

        // m_pTop needs to be volatile to ensure that it is not enregistered
        volatile PSLIST_ENTRY m_pTop;

        static T* Delta(void* p) { return (p == NULL) ? NULL : (T*) ((BYTE*)p - offsetof(T, m_slNext)); }
    };

    //
    // Lock free stack implemented using a windows SLIST. Type T is required to have an intrusive SLIST_ENTRY m_slNext.
    //
    template <class T>
    class LockFreeStack
    {
    public:
        LockFreeStack()
        {
            InitializeSListHead(&m_head);
        }

        T* Pop()
        {
           return Delta(InterlockedPopEntrySList(&m_head));
        }

        T* Flush()
        {
            return Delta(InterlockedFlushSList(&m_head));
        }

        void Push(T* pNode)
        {
            InterlockedPushEntrySList(&m_head, &(pNode->m_slNext));
        }

        static T* Next(T* pNode)
        {
            return Delta(pNode->m_slNext.Next);
        }

        // implicit max of 64K entries
        int Count() const { return static_cast<int> (QueryDepthSList(const_cast<PSLIST_HEADER> (&m_head))); }

    private:
        SLIST_HEADER m_head; // must be 16-bye aligned in x64

        static T* Delta(void* p) { return (p == NULL) ? NULL : (T*) ((BYTE*)p - offsetof(T, m_slNext)); }
    };

    //
    // The derived SafeStack class, which acquires a lock around accesses to the stack.
    //
    template <class T, class Counter = CollectionTypes::NoCount, class LOCK = _NonReentrantLock>
    class SafeStack : public Stack<T, Counter>
    {
    public:
        T* Pop()
        {
            typename LOCK::_Scoped_lock lockHolder(m_lock);
            return Stack<T, Counter>::Pop();
        }

        void Push(T* pNode)
        {
            typename LOCK::_Scoped_lock lockHolder(m_lock);
            Stack<T, Counter>::Push(pNode);
        }

        void Acquire() const { m_lock._Acquire(); }
        void Release() const { m_lock._Release(); }

    private:
        mutable LOCK m_lock;
    };


    //
    //  The base class of singly-linked queues.  This implementation is not thread-safe.
    //
    template <class T>
    class SQueue
    {
    public:
        SQueue() : m_pHead(NULL), m_ppTail(&m_pHead) { };

        void Enqueue(T* pNode)
        {
            ASSERT(pNode != NULL);

            pNode->m_pNext = NULL;
            *m_ppTail = pNode;
            m_ppTail = &pNode->m_pNext;
        }

        T* Dequeue()
        {
            if (m_pHead == NULL)
            {
                return NULL;
            }
            T *pHead = m_pHead;
            m_pHead = m_pHead->m_pNext;
            if (m_pHead == NULL)
                m_ppTail = &m_pHead;

            return pHead;
        }

        T* Current() const { return m_pHead; }
        bool Empty() const { return m_pHead == NULL; }

    private:
        T *m_pHead;
        T **m_ppTail;
    };

    //
    //  The derived safe singly-linked queue class.  This implementation acquires
    //  a lock before accessing the Queue.
    //
    template <class T, class LOCK = _NonReentrantLock>
    class SafeSQueue : public SQueue<T>
    {
    public:
        void Enqueue(T* pNode)
        {
            typename LOCK::_Scoped_lock lockHolder(m_lock);
            SQueue<T>::Enqueue(pNode);
        }

        T* Dequeue()
        {
            typename LOCK::_Scoped_lock lockHolder(m_lock);
            return SQueue<T>::Dequeue();
        }

        void Acquire() const { m_lock._Acquire(); }
        void Release() const { m_lock._Release(); }

    private:
        mutable LOCK m_lock;
    };

    //
    //  An unsafe circular linked list.
    //
    template <class T, class Counter = CollectionTypes::NoCount>
    class List : public Counter
    {
    public:
        List() : m_pTail(NULL)
        {
        }

        void Swap(List* pList)
        {
            T* pTail = pList->m_pTail;
            pList->m_pTail = m_pTail;
            m_pTail = pTail;
        }

        void AddHead(T* pNode)
        {
            ASSERT(pNode != NULL);

            // if the list is empty, add it accordingly
            if (m_pTail == NULL)
            {
                m_pTail = pNode;
                m_pTail->m_pPrev = m_pTail;
                m_pTail->m_pNext = m_pTail;
            }
            else
            {
                // hook up pNode
                pNode->m_pNext = m_pTail->m_pNext; // pNode->next = head
                pNode->m_pPrev = m_pTail;

                // hook up old head (viz., tail->next)
                m_pTail->m_pNext->m_pPrev = pNode; // head->prev = pNode
                m_pTail->m_pNext = pNode; // head = pNode
            }

            Counter::Increment();
        }

        void AddTail(T* pNode)
        {
            ASSERT(pNode != NULL);

            if (m_pTail == NULL)
            {
                pNode->m_pPrev = pNode->m_pNext = pNode;
            }
            else
            {
                // hook up pNode
                pNode->m_pNext = m_pTail->m_pNext; // pNode->next = head
                pNode->m_pPrev = m_pTail;

                // hook up old head (viz., tail->next)
                m_pTail->m_pNext->m_pPrev = pNode; // head->prev = pNode
                m_pTail->m_pNext = pNode; // head = pNode
            }

            m_pTail = pNode; // same as AddHead except we change the tail

            Counter::Increment();
        }

        T* RemoveHead()
        {
            if (m_pTail == NULL)
                return NULL;

            Counter::Decrement();
            T *pNode = (T*) m_pTail->m_pNext;

            if (m_pTail == pNode)
            {
                m_pTail = NULL;
            }
            else
            {
                // hook up tail to head->next
                pNode->m_pNext->m_pPrev = m_pTail; // head->next->prev = tail
                m_pTail->m_pNext = pNode->m_pNext; // tail->next = head->next
            }

            return pNode;
        }

        T* RemoveTail()
        {
            if (m_pTail == NULL)
                return NULL;

            Counter::Decrement();
            T *pNode = m_pTail;

            if (m_pTail == m_pTail->m_pNext)
            {
                m_pTail = NULL;
            }
            else
            {
                // hook up head to tail->prev and set tail = tail->prev, preserves head
                pNode->m_pNext->m_pPrev = pNode->m_pPrev; // head->prev = tail->prev
                pNode->m_pPrev->m_pNext = pNode->m_pNext; // tail->prev->next = head
                m_pTail = (T*) m_pTail->m_pPrev;
            }

            return pNode;
        }

        void Enqueue(T* pNode)
        {
            AddHead(pNode);
        }

        T* Dequeue()
        {
            return RemoveTail();
        }

        //
        // Remove an element from the list.
        // The function asserts that the write lock is held
        //
        void Remove(T* pNode)
        {
            ASSERT(pNode != NULL && m_pTail != NULL);

            Counter::Decrement();

            pNode->m_pNext->m_pPrev = pNode->m_pPrev;
            pNode->m_pPrev->m_pNext = pNode->m_pNext;
            if (pNode == m_pTail)
            {
                m_pTail = (m_pTail == m_pTail->m_pNext) ? NULL : (T*) m_pTail->m_pPrev;
            }
        }

        //
        // Iterator functions
        //
        T* First() const
        {
            return (m_pTail != NULL) ? (T*) m_pTail->m_pNext : NULL;
        }

        T* Last() const
        {
            return m_pTail;
        }

        T* Next(T* pNode) const
        {
            return (pNode != m_pTail) ? (T*) pNode->m_pNext : NULL;
        }

        static T* Next(T* pNode, T* pPosition)
        {
            return (pNode != pPosition) ? (T*) pNode->m_pNext : NULL;
        }

        static T* Prev(T* pNode, T* pPosition)
        {
            return (pNode != pPosition) ? (T*) pNode->m_pPrev : NULL;
        }

        int Count() const { return Counter::Value(); }

        bool Empty() const { return (m_pTail == NULL); }

        void Clear() { m_pTail = NULL; Counter::Clear(); }

    protected:
        T *m_pTail;
    };

    //
    //  A safe circular linked list.  This implementation uses
    //  a _ReaderWriterLock on the list accesses.
    //
    template <class T, class Counter = CollectionTypes::NoCount, class RWLOCK = _ReaderWriterLock>
    class SafeRWList : public List<T, Counter>
    {
    public:
        SafeRWList()
        {
        }

        void Swap(List<T, Counter>* pList)
        {
            typename RWLOCK::_Scoped_lock writeLock(m_lock);
            List<T, Counter>::Swap(pList);
        }

        void AddHead(T* pNode)
        {
            typename RWLOCK::_Scoped_lock writeLock(m_lock);
            List<T, Counter>::AddHead(pNode);
        }

        void AddTail(T* pNode)
        {
            typename RWLOCK::_Scoped_lock writeLock(m_lock);
            List<T, Counter>::AddTail(pNode);
        }

        T* RemoveHead()
        {
            typename RWLOCK::_Scoped_lock writeLock(m_lock);
            return List<T, Counter>::RemoveHead();
        }

        T* RemoveTail()
        {
            typename RWLOCK::_Scoped_lock writeLock(m_lock);
            return List<T, Counter>::RemoveTail();
        }

        //
        //  Wrapper functions around AddHead/RemoveTail for consistency purposes
        //
        void Enqueue(T* pNode)
        {
            AddHead(pNode);
        }

        T* Dequeue()
        {
            return RemoveTail();
        }

        //
        // Synchronized remove an element from the list.
        //
        void Remove(T* pNode)
        {
            typename RWLOCK::_Scoped_lock writeLock(m_lock);
            List<T, Counter>::Remove(pNode);
        }

#pragma region "unlocked variants"
        void UnlockedAddHead(T* pNode)
        {
            ASSERT(m_lock._HasWriteLock());
            List<T, Counter>::AddHead(pNode);
        }

        void UnlockedAddTail(T* pNode)
        {
            ASSERT(m_lock._HasWriteLock());
            List<T, Counter>::AddTail(pNode);
        }

        T* UnlockedRemoveHead()
        {
            ASSERT(m_lock._HasWriteLock());
            return List<T, Counter>::RemoveHead();
        }

        T* UnlockedRemoveTail()
        {
            ASSERT(m_lock._HasWriteLock());
            return List<T, Counter>::RemoveTail();
        }

        void UnlockedEnqueue(T* pNode)
        {
            ASSERT(m_lock._HasWriteLock());
            List<T, Counter>::AddHead(pNode);
        }

        T* UnlockedDequeue()
        {
            ASSERT(m_lock._HasWriteLock());
            return List<T, Counter>::RemoveTail();
        }

        //
        // Remove an element from the list.
        // The function asserts that the write lock is held
        //
        void UnlockedRemove(T* pNode)
        {
            ASSERT(m_lock._HasWriteLock());
            List<T, Counter>::Remove(pNode);
        }
#pragma endregion

        //
        // R/W Lock acquisition functions
        //
        void AcquireRead() const { m_lock._AcquireRead(); }
        bool TryAcquireRead() const { return m_lock._TryAcquireRead(); }
        void ReleaseRead() const { m_lock._ReleaseRead(); }
        void AcquireWrite() const { m_lock._AcquireWrite(); }
        bool TryAcquireWrite() const { return m_lock._TryAcquireWrite(); }
        void ReleaseWrite() const { m_lock._ReleaseWrite(); }
        void FlushWriteOwners() const { m_lock._FlushWriteOwners(); }

        /// <summary>
        ///     An exception safe RAII wrapper for writes.
        /// </summary>
        class _Scoped_lock
        {
        public:
            explicit _Scoped_lock(SafeRWList& _Lock) : _M_lock(_Lock)
            {
                _M_lock.AcquireWrite();
            }

            ~_Scoped_lock()
            {
                _M_lock.ReleaseWrite();
            }

        private:

            SafeRWList& _M_lock;

            _Scoped_lock(const _Scoped_lock&);                    // no copy constructor
            _Scoped_lock const & operator=(const _Scoped_lock&);  // no assignment operator
        };

        /// <summary>
        ///     An exception safe RAII wrapper for reads.
        /// </summary>
        class _Scoped_lock_read
        {
        public:
            explicit _Scoped_lock_read(SafeRWList& _Lock) : _M_lock(_Lock)
            {
                _M_lock.AcquireRead();
            }

            ~_Scoped_lock_read()
            {
                _M_lock.ReleaseRead();
            }

        private:

            SafeRWList& _M_lock;

            _Scoped_lock_read(const _Scoped_lock_read&);                    // no copy constructor
            _Scoped_lock_read const & operator=(const _Scoped_lock_read&);  // no assignment operator
        };

    protected:
        mutable RWLOCK m_lock;
    };

    //
    // Naive hash table implemented as an array of single linked lists.
    //
    template <class KEY, class VALUE>
    class Hash
    {
    public:
        //
        // nested classes
        //

        struct ListNode
        {
            ListNode(const KEY& key = 0, const VALUE& value = 0) :
                m_pNext(NULL),
                m_key(key),
                m_value(value)
            {
            }

            ListNode* m_pNext;
            KEY m_key;
            VALUE m_value;
        };

    public:
        //
        // ctor
        //

        Hash(int size = s_hashsize)
        {
            m_size = size;
            m_count = 0;
            m_ppHashtable = _concrt_new ListNode*[m_size];
            memset(m_ppHashtable, 0, m_size*sizeof(ListNode*));
        }

        //
        // public methods
        //

        //
        // iterator - The First() and Next() functions do not have supporting Safe versions. Currently they are used
        // by the memory dump tool which uses these APIs from a single thread. If thread safe access
        // is desired the apis must be called after acquiring the read lock in the SafeHash class.
        //
        ListNode* First(int* x)
        {
            ASSERT(x != NULL);
            *x = 0;
            return NextList(x);
        }

        ListNode* Next(int* x, ListNode* p)
        {
            ASSERT(p != NULL && x != NULL && *x < m_size);
            if (p->m_pNext != NULL)
            {
                return p->m_pNext;
            }
            else
            {
                ++*x;
                return NextList(x);
            }
        }

        ListNode* Insert(const KEY& key, const VALUE& value)
        {
            int hashValue = HashValue(key, m_size);

            ListNode* pNode = Lookup(key, hashValue);
            if (pNode == NULL)
            {
                pNode = _concrt_new ListNode(key, value);

                pNode->m_pNext = m_ppHashtable[hashValue];
                m_ppHashtable[hashValue] = pNode;

                m_count++;

                return pNode;
            }
            return NULL;
        }

        ListNode* Lookup(const KEY& key, VALUE* pValue)
        {
            ListNode* pNode = Lookup(key, HashValue(key, m_size));
            if (pNode != NULL)
            {
                *pValue = pNode->m_value;
            }
            return pNode;
        }

        bool Exists(const KEY& key)
        {
            return (Lookup(key, HashValue(key, m_size)) != NULL);
        }

        bool FindAndDelete(const KEY& key, VALUE* pValue)
        {
            ListNode* pNode = Remove(key, HashValue(key, m_size));
            if (pNode != NULL)
            {
                if (pValue != NULL)
                {
                    *pValue = pNode->m_value;
                }
                FreeNode(pNode);

                return true;
            }
            return false;
        }

        ListNode *Find(const KEY& key, VALUE* pValue)
        {
            ListNode* pNode = Lookup(key, HashValue(key, m_size));
            if (pNode != NULL)
            {
                if (pValue != NULL)
                {
                    *pValue = pNode->m_value;
                }
                return pNode;
            }
            return NULL;
        }

        bool Delete(const KEY& key)
        {
            return FindAndDelete(key, NULL);
        }

        void Wipe()
        {
            if (m_count > 0)
            {
                for (int i = 0; i < m_size; ++i)
                {
                    ListNode* pNode = m_ppHashtable[i];
                    while (pNode != NULL)
                    {
                        ListNode* pNext = pNode->m_pNext;
                        FreeNode(pNode);
                        pNode = pNext;
                    }
                }
                m_count = 0;
                memset(m_ppHashtable, 0, m_size*sizeof(ListNode*));
            }
        }

        int Count() const
        {
            return m_count;
        }

        //
        // dtor
        //

        ~Hash()
        {
            Wipe();
            delete[] m_ppHashtable;
        }

    protected:
        //
        // protected data
        //
        static const int s_hashsize = 4097;

    private:
        //
        // private data
        //

        int m_size;
        int m_count;

    ListNode** m_ppHashtable;

        //
        // private methods
        //

        ListNode* NextList(int* x)
        {
            ASSERT(x != NULL && *x >= 0 && *x <= m_size);

            for (int i = *x; i < m_size; ++i)
            {
                if (m_ppHashtable[i] != NULL)
                {
                    *x = i;
                    return m_ppHashtable[i];
                }
            }
            return NULL;
        }

        ListNode* Lookup(const KEY& key, int hashValue)
        {
            ASSERT(hashValue >= 0 && hashValue < m_size);
            ListNode* pNode = m_ppHashtable[hashValue];
            while (pNode != NULL)
            {
                if (pNode->m_key == key)
                {
                    return pNode;
                }
                pNode = pNode->m_pNext;
            }
            return NULL;
        }

        ListNode* Remove(const KEY& key, int hashValue)
        {
            ListNode* pNode = m_ppHashtable[hashValue], *pPrev = NULL;

            while (pNode != NULL)
            {
                if (pNode->m_key == key)
                {
                    if (pPrev == NULL)
                    {
                        m_ppHashtable[hashValue] = pNode->m_pNext;
                    }
                    else
                    {
                        pPrev->m_pNext = pNode->m_pNext;
                    }

                    m_count--;

                    return pNode;
                }
                pPrev = pNode;
                pNode = pNode->m_pNext;
            }
            return NULL;
        }

        //
        // This method could be specialized to provide better distribution for certain values of the template parameter KEY.
        //
        static unsigned int HashValue(const KEY& key, int the_size)
        {
            // We use Fowler-Noll-Vo FNV-1a hash algorithm.

            // Setup FNV constants for different size of (size_t)
#ifdef _WIN64
            static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
            const size_t FNVOffsetBasis = 14695981039346656037ULL;
            const size_t FNVPrime = 1099511628211ULL;
#else
            static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
            const size_t FNVOffsetBasis = 2166136261U;
            const size_t FNVPrime = 16777619U;
#endif

            // Mix each byte of key into hash value
            size_t hashVal = FNVOffsetBasis;
            for (size_t i = 0; i < sizeof(key); i++)
            {
                hashVal ^= reinterpret_cast<const unsigned char *>(&key)[i];
                hashVal *= FNVPrime;
            }

            // mod operation is not exactly fair, but it does not matter
            ASSERT(the_size > 0);
            return static_cast<unsigned int> (hashVal % static_cast<size_t>(the_size));
        }

        //
        // This function could be specialized to provide cleanup for non-trivial keys.
        //
        static void DeleteKey(KEY&) {}
        //
        // This function could be specialized to provide cleanup for non-trivial values.
        //
        static void DeleteValue(VALUE&) {}

        static void FreeNode(ListNode* pNode)
        {
            DeleteKey(pNode->m_key);
            DeleteValue(pNode->m_value);
            pNode->ListNode::~ListNode();
            delete pNode;
        }
    };

    //
    // reader/writer lock hash table
    // not polymorphic -- never cast to base
    //
    template <class KEY, class VALUE>
    class SafeHash : public Hash<KEY, VALUE>
    {
    public:
        typedef Hash<KEY, VALUE> Base;

        //
        // ctors
        //
        SafeHash(int size = Base::s_hashsize) : Hash(size)
        {
        }

        //
        // public methods
        //

        typename Base::ListNode* Insert(const KEY& key, const VALUE& value)
        {
            _ReaderWriterLock::_Scoped_lock writeLock(m_lock);
            return Base::Insert(key, value);
        }

        bool Exists(const KEY& key)
        {
            _ReaderWriterLock::_Scoped_lock_read readLock(m_lock);
            return Base::Exists(key);
        }

        typename Base::ListNode* Lookup(const KEY& key, VALUE* pValue)
        {
            _ReaderWriterLock::_Scoped_lock writeLock(m_lock);
            return Base::Lookup(key, pValue);
        }

        bool FindAndDelete(const KEY& key, VALUE* pValue)
        {
            _ReaderWriterLock::_Scoped_lock writeLock(m_lock);
            return Base::FindAndDelete(key, pValue);
        }

        bool Delete(const KEY& key)
        {
            return FindAndDelete(key, NULL);
        }


        void AcquireWrite() { m_lock._AcquireWrite(); }
        void ReleaseWrite() { m_lock._ReleaseWrite(); }
        void AcquireRead() { m_lock._AcquireRead(); }
        void ReleaseRead() { m_lock._ReleaseRead(); }

        //
        // dtor -- use default dtor
        //

    private:
        //
        // private data
        //

        _ReaderWriterLock m_lock;
    };

#define _ARRAYNODE_FULL    -2
#define _ARRAYNODE_NOTFULL -1

    class SchedulerBase;

    //
    // The ListArray class is a generalized array that can be accesed by index
    // Each node in the list includes an array of elements is default constructed to bucket size 256
    // and contains a pointer to the next list and a searchIndex field.
    //
    // The searchIndex field is an indicator as to whether this array is full.  A value of _ARRAYNODE_FULL
    // means the array is full and can be skipped over on insertion.  A value of _ARRAYNODE_NOTFULL means
    // the array is not full and should be searched.  A value >=0 means that that specific element in the
    // array has been removed and could be used.
    //
    // On the side, an Array pointing to the each array node is kept in m_ppArrayNodes.  This allows
    // for fast O(1) access on removal and the index operator, up to size m_arrayNodeSize*m_arrayLength
    // elements (default 512*256).
    //
    // m_ppArrayNodes:
    // +---+---+---+---+
    // | 1 | 2 |   |512|
    // +---+---+---+---+
    //   |   |
    //   |   +---------------------------------------------+
    //   |                                                 |
    //   V                                                 V
    // ArrayNode 1:                                      ArrayNode 2:
    // +---+---+---+-------+---+------+-------+          +---+---+---+-------+---+------+-------+
    // | 1 | 2 | 3 |  ...  |256| next | index |     +--> | 1 | 2 | 3 |  ...  |256| next | index |
    // +---+---+---+-------+---+------+-------+     |    +---+---+---+-------+---+------+-------+
    //                             |                |
    //                             +----------------+
    //
    // An Add(object) will currently run through the arrays for a non-full (not _ARRAYNODE_FULL) array, then start
    // searching that specific array for a non-NULL slot.  A CAS is used to place the element in that
    // slot.
    //
    // Any object that has an integer m_listArrayIndex field can be placed into this ListArray implementation.
    // In the ConcRT scheduler, it will be mainly used for R/W objects that are often read, to avoid using a
    // lock-based collection like the SafeRWList.
    //
    //
    // ELEMENT DELETION FROM LIST ARRAYS
    //
    // The general algorithm for deletion is as follows:
    //
    // ListArray.Remove() CAS's items out of the main list array and inserts them into the free pool.  After
    // the insertion, the ListArray checks to see if the number of items in the free pool has exceeded a set
    // threshold amount (stored in m_deletionThreshold).  If so, it calls the scheduler and asks it to invoke
    // the deletion at the next safe point: a point where all Contexts have reached a point in their
    // dispatch loops where they are outside of their stealing logic and could not possibly be holding a bad pointer.
    // At this point, the Remove() function moves half of the elements on the free pool over to a "elements to delete"
    // list and sets a flag (m_fHasElementsToDelete) in this ListArray indicating it is the list array awaiting contexts
    // to reach safe points.
    //
    // In the InterContextBase::Dispatch code, a single check is placed in one of its safe points which will mark the
    // virtual processor as "ReachedSafePoint" if there is a pending cleanup.
    //
    // The m_fHasElementsToDelete call prevents two outstanding invocations for deletion at once.
    //
    // Note: it is currently not safe to walk a list array from an external context if the list array is deleting items.
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to alignment specifier
    template <class ELEMENT>
    class ListArray
    {
        struct ArrayNode {
            ArrayNode(ELEMENT ** ppArray)
                : m_ppArray(ppArray), m_pNext(NULL), m_searchIndex(_ARRAYNODE_NOTFULL)
            {
            }
            // The actual array of elements being stored
            ELEMENT ** m_ppArray;
            // The next array
            ArrayNode *m_pNext;
            // A integer indicating whether this array is full, or where a free index slot is
            //  -2: array is full
            //  -1: array is not full, and should be
            // >=0: some array element has been removed
            int m_searchIndex;
        };

        //
        // Pool of free Elements, can be returned and reused
        // The user is responsible for reinitializing the elements to a correct state before using them
        //
        typedef struct __FreeElement {
            SLIST_ENTRY ItemEntry;
            ELEMENT * m_pElement;
        } FreeElement, *PFreeElement;

        // The Slist of free elements saved for reuse
        SLIST_HEADER m_freeElementPool;  // must be 16-bye aligned in x64

        // Elements removed from the free pool because it exceeded its threshold size.
        // The elements are held in this list until they are safe to delete
        SLIST_HEADER m_elementsToDelete; // must be 16-bye aligned in x64

        // When a deletion is started, all elements to delete are snapped, and pointed to
        // by this SLIST_ENTRY.
        PSLIST_ENTRY m_deletionList;

        // The invocation for deletion which will happen on safe points.
        SafePointInvocation m_deletionSafePoint;

    public:
        /// <summary>
        ///     Constructor for ListArray
        /// </summary>
        /// <param name="arrayLength">
        ///     The length of each array in the list
        /// </param>
        ListArray(::Concurrency::details::SchedulerBase * pScheduler, int arrayLength = 256, int deletionThreshold = 64)
            : m_deletionList(NULL)
            , m_pScheduler(pScheduler)
            , m_shiftBits(0)
            , m_pArrayHead(NULL)
            , m_arrayNodesSize(512)
            , m_nextArrayNodeSlot(1)
            , m_maxArrayIndex(0)
            , m_deletionThreshold(deletionThreshold)
            , m_fHasElementsToDelete(FALSE)
        {
            //
            // Initialize the arrayLength to the next largest power of 2
            //
            if ((arrayLength & (arrayLength-1)) != 0)
            {
                arrayLength = (arrayLength >> 1) | arrayLength;
                arrayLength = (arrayLength >> 2) | arrayLength;
                arrayLength = (arrayLength >> 4) | arrayLength;
                arrayLength = (arrayLength >> 8) | arrayLength;
                arrayLength = (arrayLength >> 16) | arrayLength;
                arrayLength = arrayLength + 1;
            }
            m_arrayLength = arrayLength;

            // Create an array of m_arrayLength (default is 256)
            ELEMENT ** elementArray = _concrt_new ELEMENT*[m_arrayLength];
            memset(elementArray, 0, m_arrayLength*sizeof(ELEMENT*));
            m_pArrayHead = _concrt_new ArrayNode(elementArray);

            // Creates an array for quick access to the right ArrayNode
            m_ppArrayNodes = _concrt_new ArrayNode*[m_arrayNodesSize];
            m_ppArrayNodes[0] = m_pArrayHead;

            // Initialize the Free Element Pool Slist
            InitializeSListHead(&m_freeElementPool);

            InitializeSListHead(&m_elementsToDelete);

            // Precalculate number of bits to shift this arraylength
            int i = m_arrayLength;
            while ((i >>= 1) != 0)
            {
                m_shiftBits++;
            }
        }

        void SetScheduler(::Concurrency::details::SchedulerBase *pScheduler)
        {
            m_pScheduler = pScheduler;
        }

        /// <summary>
        ///     ListArray destructor
        /// </summary>
        ~ListArray()
        {
            //
            // Delete items that were added to the free list
            //
            PSLIST_ENTRY pListEntry = InterlockedFlushSList(&m_freeElementPool);
            DeleteElements(pListEntry);

            //
            // Delete items that were added to the elements to delete slist
            //
            pListEntry = InterlockedFlushSList(&m_elementsToDelete);
            DeleteElements(pListEntry);

            //
            // Delete any elements that were snapped for deletion but the
            // deletion did not actually occur yet
            //
            DeleteElements(m_deletionList);

            //
            // Delete each individual array
            //
            ArrayNode * node = m_pArrayHead;
            while (node != NULL)
            {
                for (int i = 0; i < m_arrayLength; i++)
                {
                    _InternalDeleteHelper<ELEMENT>(node->m_ppArray[i]);
                }
                ArrayNode * next = node->m_pNext;
                delete [] node->m_ppArray;
                delete node;
                node = next;
            }

            delete [] m_ppArrayNodes;
        }

        /// <summary>
        ///     Determines if there are any elements in the list array.
        ///     This routine shall only be called when the list array is
        ///.    is not being modified.
        /// </summary>
        /// <returns>
        ///     true if the list array is empty, false otherwise
        /// </returns>
        bool IsEmptyAtSafePoint()
        {
            ArrayNode * node = m_pArrayHead;
            while (node != NULL)
            {
                for (int i = 0; i < m_arrayLength; i++)
                {
                    if (node->m_ppArray[i] != NULL)
                    {
                        return false;
                    }
                }
                node = node->m_pNext;
            }

            return true;
        }

        /// <summary>
        ///     Add an element into the ListArray
        /// </summary>
        /// <param name="element">
        ///     The element being inserted
        /// </param>
        /// <returns>
        ///     The absolute index in the array that it was inserted at
        /// </returns>
        int Add(ELEMENT * element)
        {
            // A flag for whether the object has actually been inserted into the ListArray
            bool inserted = false;
            // The return value: the absolute index in the array that the item was inserted
            int index = 0;

            ASSERT(m_pArrayHead != NULL);

            ArrayNode * node = m_pArrayHead;
            while (inserted == false)
            {
                //
                // A m_searchIndex value of -1 indicates that this current array being looked at
                // is not known to be full.  A walk of the array to find a non-NULL slot is performed
                //
                if (node->m_searchIndex >= -1)
                {
                    ELEMENT ** elementArray = node->m_ppArray;

                    for (int i = 0; i < m_arrayLength; i++)
                    {
                        // Continue if the slot is non-NULL
                        if (elementArray[i] != NULL)
                        {
                            continue;
                        }

                        // Set this element's m_listArrayIndex field to point to this field we're trying
                        // to insert at.  This is set before the CAS with the ListArray bucket to avoid
                        // races that may occur if the object is immediately removed before the index field
                        // is set.
                        element->m_listArrayIndex = index+i;

                        // Check the current index to see whether or not we need to increment m_maxArrayIndex
                        // for this insertion
                        int currentMaxIndex = m_maxArrayIndex;

                        // Try to insert at array slot i
                        PVOID initPtr = InterlockedCompareExchangePointer((volatile PVOID *) &elementArray[i],
                            (PVOID) element, (PVOID) NULL);
                        if (initPtr == NULL)
                        {
                            // Mark this element as inserted at location i
                            inserted = true;
                            index += i;
                            if (index >= currentMaxIndex)
                            {
                                InterlockedIncrement(&m_maxArrayIndex);
                            }

                            // If a previous remove call had marked this location as free, reset the
                            // array as 0, so that the next call will walk again.  The return of this CAS
                            // is irrelevant, it could have been reset by another remove
                            InterlockedCompareExchange((volatile LONG *) &node->m_searchIndex,
                                (LONG) _ARRAYNODE_NOTFULL, (LONG) i);

                            break;
                        }

                    }
                }

                //
                // If nothing was inserted during this pass, try and mark the array as FULL (-2) and
                // move on to the next array to search, creating a new one if necessary.  If a remove
                // of an element in this array happened in the meantime, that's okay, the next Add to
                // this ListArray will happen in that location
                //
                if (inserted == false)
                {
                    //
                    // Try to set this array in "Full" mode.  If an intervening Remove() happened, this
                    // CAS will fail. This current element will just be stored somewhere in the next array
                    //
                    InterlockedCompareExchange((volatile LONG *) &node->m_searchIndex,
                        (LONG) _ARRAYNODE_FULL, (LONG) _ARRAYNODE_NOTFULL);
                    index += m_arrayLength;

                    //
                    // Try and increase the size of this ListArray
                    //
                    if (node->m_pNext == NULL)
                    {
                        if (InterlockedCompareExchangePointer((PVOID volatile *) &node->m_pNext, (PVOID) s_nonNull, NULL) == NULL)
                        {
                            //
                            // Create a new element array, which is where the actual elements are stored
                            //
                            ELEMENT **elementArray = _concrt_new ELEMENT*[m_arrayLength];
                            memset(elementArray, 0, m_arrayLength*sizeof(ELEMENT*));

                            //
                            // Create an ArrayNode, which is a wrapper around each element array
                            //
                            ArrayNode *pNode = _concrt_new ArrayNode(elementArray);

                            //
                            // The m_ppArrayNodes array is used for fast index into the list array
                            // It supports up to 512 ArrayNodes, each with m_arrayLength elements
                            // if we exceed this number, additional array nodes are accessed by as
                            // a linked list off of the last element.
                            //
                            // Note: this is safe since the CAS to s_nonNull is effectively a lock on this until the publication of
                            // pNode below.  **EVERYTHING** must be set up before pNode is published and the relative ordering
                            // of node, node->m_pNext must concur with m_ppArrayNodes[m_nextArrayNodeSlot - 1],
                            // m_nextArrayNodeSlot]
                            //
                            if (m_nextArrayNodeSlot < m_arrayNodesSize)
                                m_ppArrayNodes[m_nextArrayNodeSlot++] = pNode;

                            _InterlockedExchangePointer((PVOID volatile *) &node->m_pNext, pNode);

                        }
                    }

                    //
                    // Make sure the next array is ready.
                    //
                    if ((size_t) node->m_pNext == s_nonNull)
                    {
                        _SpinWaitBackoffNone spinWait;
                        do
                        {
                            // Here and in other places in the runtime, we're looping around checking the value of a non-volatile variable,
                            // but the function call _SpinOnce prevents the value from being cached. A simple _YieldProcessor() here
                            // would have resulted in an infinite loop.
                            spinWait._SpinOnce();
                        } while ((size_t) node->m_pNext == s_nonNull) ;
                    }
                }

                ASSERT(inserted == true || (inserted == false && node->m_pNext != NULL));

                // Move to the next array
                node = node->m_pNext;
            }

            ASSERT(index >= 0);

            return index;
        }

        /// <summary>
        ///     Add a free element into the free pool.  Ignore depth.
        /// </summary>
        /// <param name="element">
        ///     The element being inserted
        /// </param>
        void AddToFreePool(ELEMENT * element)
        {
            //
            // Add this removed element to the free pool
            //
            InterlockedPushEntrySList(&m_freeElementPool, &(element->m_listArrayFreeLink));
        }

        /// <summary>
        ///     Remove an element from the array
        /// </summary>
        /// <param name="element">
        ///     A pointer to the element being removed
        /// </param>
        /// <param name="addToFreePool">
        ///     Whether we want this removed element to be added to the free pool for reuse
        /// </param>
        /// <returns>
        ///     False when the element does not exist.
        /// </returns>
        bool Remove(ELEMENT * element, bool addToFreePool = true)
        {
            return Remove(element, element->m_listArrayIndex, addToFreePool);
        }

        /// <summary>
        ///     Remove an element from the array
        /// </summary>
        /// <param name="element">
        ///     A pointer to the element being removed
        /// </param>
        /// <param name="listArrayIndex">
        ///     ListArrayIndex of element.
        /// </param>
        /// <param name="addToFreePool">
        ///     Whether we want this removed element to be added to the free pool for reuse
        /// </param>
        /// <returns>
        ///     False when the element does not exist.
        /// </returns>
        bool Remove(ELEMENT * element, int listArrayIndex, bool addToFreePool = true)
        {
            int arrayIndex = listArrayIndex >> m_shiftBits;
            int removeIndex = listArrayIndex & m_arrayLength-1;

            //
            // The element clearly does not exist.
            //
            if (arrayIndex >= m_nextArrayNodeSlot)
            {
                return false;
            }

            ArrayNode * node = NULL;
            if (arrayIndex >= m_arrayNodesSize)
            {
                // If this is actually an element that exceeded the m_ppArrayNodes index,
                // run through the linked list to find the right arrayNode to access.
                // This will only occur with very large number of items in the ListArray
                node = m_ppArrayNodes[m_arrayNodesSize-1];
                for (int i = 0; i <= arrayIndex - m_arrayNodesSize; i++)
                {
                    node = node->m_pNext;
                }
            }
            else
            {
                node = m_ppArrayNodes[arrayIndex];
            }

            ELEMENT ** elementArray = node->m_ppArray;

            //
            // Try and remove the element from the array
            //
            volatile PVOID oldPtr = (PVOID) element;
            volatile PVOID initPtr = InterlockedCompareExchangePointer((volatile PVOID *) &elementArray[removeIndex],
                (PVOID) NULL, (PVOID) oldPtr);

            if (initPtr == oldPtr)
            {
                //
                // If the remove was successful, then update the Array node to know that the array is not full
                //
                InterlockedCompareExchange((volatile LONG *) &node->m_searchIndex,
                        (LONG) removeIndex, (LONG) _ARRAYNODE_FULL);
            }
            else
            {
                return false;
            }

            //
            // Add this item to the free list.  The AddToFreePool flag is necessary because some elements, like the
            // WorkQueue, don't actually want to remove an element for and have it reused by someone else.  It is
            // removing it from one ListArray in order to place it on another.
            //
            if (addToFreePool)
            {
                //
                // Check if the number of elements in the free pool has exceeded the threshold for deletion
                // If so, put this element on the deletion pool rather than the free pool
                //

                if (QueryDepthSList(&m_freeElementPool) > m_deletionThreshold)
                {
                    ASSERT(m_deletionThreshold != DeletionThresholdInfinite);

                    //
                    // Add this removed element to the deletion pool
                    //
                    InterlockedPushEntrySList(&m_elementsToDelete, &(element->m_listArrayFreeLink));

                    int elementsToDeleteDepth = QueryDepthSList(&m_elementsToDelete);

                    //
                    // Try marking this list array as the one we want to delete from
                    // if the length of the deletion list has hit the threshold
                    //
                    if (elementsToDeleteDepth > m_deletionThreshold &&
                        m_pScheduler->HasCompletedShutdown() == false &&
                        InterlockedCompareExchange(&m_fHasElementsToDelete, TRUE, FALSE) == FALSE)
                    {
                        ASSERT(m_deletionList == NULL);

                        // Take a snapshot of the deletion pool, these are the elements we will actually delete
                        m_deletionList = InterlockedFlushSList(&m_elementsToDelete);

                        //
                        // Inform the scheduler to call us at the next safe point for deletion.  This will guarantee that no virtual
                        // processor has a handle to any of the objects we are deleting.
                        //
                        m_deletionSafePoint.InvokeAtNextSafePoint(reinterpret_cast<SafePointInvocation::InvocationFunction>(&CheckForDeletionBridge), this, m_pScheduler);

                    }
                }
                else
                {
                    //
                    // Add this removed element to the free pool
                    //
                    InterlockedPushEntrySList(&m_freeElementPool, &(element->m_listArrayFreeLink));
                }
            }
            return true;
        }

        /// <summary>
        ///     Index operator for the ListArray
        /// </summary>
        /// <param name="index">
        ///     The index being retreived
        /// </param>
        /// <returns>
        ///     The element being accessed
        /// </returns>
        ELEMENT * operator[](int index) const
        {
            int arrayIndex = index >> m_shiftBits;

            if (arrayIndex >= m_nextArrayNodeSlot)
            {
                return NULL;
            }

            ArrayNode * node = NULL;
            if (arrayIndex >= m_arrayNodesSize)
            {
                // If this is actually an element that exceeded the m_ppArrayNodes index,
                // run through the linked list to find the right arrayNode to access.
                // This will only occur with very large number of items in the ListArray
                node = m_ppArrayNodes[m_arrayNodesSize-1];
                for (int i = 0; i <= arrayIndex - m_arrayNodesSize; i++)
                {
                    node = node->m_pNext;
                }
            }
            else
            {
                node = m_ppArrayNodes[arrayIndex];
            }

            // Index into the array at position (index & m_arrayLength-1), this is the element
            return node->m_ppArray[index & m_arrayLength-1];
        }

        /// <summary>
        ///     Called in order to retrieve an item from the free pool for reuse
        /// </summary>
        /// <remarks>
        ///     The user of this ListArray is responsible for repurposing this returned object for reuse.
        ///     Thus, reinitialization of the ELEMENT may need to occur
        /// </remarks>
        /// <returns>
        ///     An element from the free pool
        /// </returns>
        ELEMENT * PullFromFreePool()
        {
            PSLIST_ENTRY pListEntry = InterlockedPopEntrySList(&m_freeElementPool);
            if (pListEntry == NULL)
            {
                return NULL;
            }
            ELEMENT * returnElement = CONTAINING_RECORD(pListEntry, ELEMENT, m_listArrayFreeLink);
            return returnElement;
        }

        /// <summary>
        ///     Called in order to retrieve the maximum size the ListArray has grown to
        /// </summary>
        /// <remarks>
        ///     This is used to index into the array
        /// </remarks>
        /// <returns>
        ///     The maximum size of the array
        /// </returns>
        int MaxIndex()
        {
            return m_maxArrayIndex;
        }
    public:

        static const int DeletionThresholdInfinite = INT_MAX;

    private:

        /// <summary>
        ///     A function to check whether a deletion needs to occur
        /// </summary>
        /// <remarks>
        ///     This function assumes that all virtual processors have already reached their safe point.
        /// </remarks>
        void CheckForDeletion()
        {
            // Early return from this function if:
            // 1. The scheduler is already in its shutdown process -- the entire list array will be deleted anyway
            // 2. The scheduler has not been set in cleanup mode
            if (m_pScheduler->HasCompletedShutdown())
            {
                return;
            }

            DeleteElements(m_deletionList);
            m_deletionList = NULL;

            //
            // Allow other deletions to happen on this list array.
            //
            InterlockedExchange(&m_fHasElementsToDelete, FALSE);
        }

        /// <summary>
        ///     A thunk to CheckForDeletion that safe point invocation will call.
        /// </summary>
        static void CheckForDeletionBridge(ListArray<ELEMENT> *pThis)
        {
            pThis->CheckForDeletion();
        }

        /// <summary>
        ///     A function that deletes all the elements of an SList pointed to by a PSLIST_ENTRY
        /// </summary>
        /// <param name="pListEntry">
        ///     The head node of the list being deleted.
        /// </param>
        void DeleteElements(PSLIST_ENTRY pListEntry)
        {
            while (pListEntry != NULL)
            {
                ELEMENT *pElement = CONTAINING_RECORD(pListEntry, ELEMENT, m_listArrayFreeLink);
                pListEntry = pListEntry->Next;
                _InternalDeleteHelper<ELEMENT>(pElement);
            }
        }

        // The scheduler instance the listarray belongs to.
        ::Concurrency::details::SchedulerBase * m_pScheduler;

        // The bucketlength of each array
        int m_arrayLength;
        // The number of bits to shift an index by
        int m_shiftBits;

        // The head of the ListArray
        ArrayNode * m_pArrayHead;

        // An Array of pointers to each of the ArrayNodes that are created
        ArrayNode ** m_ppArrayNodes;
        // The current size of the m_ppArrayNodes array
        int m_arrayNodesSize;
        // The next ArrayNode slot that should be inserted into
        int m_nextArrayNodeSlot;

        // The farthest into the array any element has been inserted
        // used for iterating on this array
        volatile long m_maxArrayIndex;

        // The maximum number of elements this array should hold before it begins deletion some
        int m_deletionThreshold;

        // A flag indicating whether or not this ListArray has elements that it wants to delete
        // This set to true immediately after elements have successfully been moved from the
        // free pool to the deletion list.
        // It is checked in "Check for Deletion" to ensure that only one thread is actually doing
        // the delete of elements, and reset to false.
        volatile long m_fHasElementsToDelete;

        static const size_t s_nonNull = 1;
    };


    template<class T>
    struct ListArrayInlineLink
    {
        int m_listArrayIndex;
        SLIST_ENTRY m_listArrayFreeLink;

        T* m_pObject;
    };
#pragma warning(pop)

    /// <summary>
    ///     Provides a set of N bits which have usual bitwise operators on them in order to allow relatively rapid intersections of subsets
    ///     of virtual processors when checking affinity.
    /// </summary>
    class QuickBitSet
    {
    public:

        //
        // *NOTE*: There is duplication of code since we are below STL and I do not use std::move.
        //

        QuickBitSet() : m_size(0), m_pBits(NULL)
        {
        }

        QuickBitSet(unsigned int size) : m_size(size)
        {
            m_pBits = _concrt_new unsigned int[(m_size + 31) >> 5];
            memset(m_pBits, 0, sizeof(unsigned int) * ASIZE());
        }

        QuickBitSet(const QuickBitSet& copySrc) :
            m_size(0),
            m_pBits(NULL)
        {
            CopyFrom(copySrc);
        }

        QuickBitSet(QuickBitSet&& moveSrc)
        {
            m_size = moveSrc.m_size;
            m_pBits = moveSrc.m_pBits;

            moveSrc.m_size = 0;
            moveSrc.m_pBits = NULL;
        }

        ~QuickBitSet()
        {
            delete[] m_pBits;
        }

        QuickBitSet& operator=(const QuickBitSet& assignor)
        {
            CopyFrom(assignor);

            return *this;
        }

        QuickBitSet& operator=(QuickBitSet&& assignor)
        {
            delete[] m_pBits;

            m_size = assignor.m_size;
            m_pBits = assignor.m_pBits;

            assignor.m_size = 0;
            assignor.m_pBits = NULL;

            return *this;
        }

        QuickBitSet operator|(const QuickBitSet& rhs) const
        {
            ASSERT(rhs.m_size == m_size);

            QuickBitSet result(m_size);

            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize; ++i)
                result.m_pBits[i] = (m_pBits[i] | rhs.m_pBits[i]);

            return result;
        }

        QuickBitSet operator|(QuickBitSet&& rhs) const
        {
            ASSERT(rhs.m_size == m_size);

            QuickBitSet result;
            result.m_size = rhs.m_size;
            result.m_pBits = rhs.m_pBits;

            rhs.m_size = 0;
            rhs.m_pBits = NULL;

            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize; ++i)
            {
                result.m_pBits[i] |= m_pBits[i];
            }

            return result;
        }

        QuickBitSet operator&(const QuickBitSet& rhs) const
        {
            ASSERT(rhs.m_size == m_size);

            QuickBitSet result(m_size);

            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize; ++i)
                result.m_pBits[i] = (m_pBits[i] & rhs.m_pBits[i]);

            return result;
        }

        QuickBitSet operator&(QuickBitSet&& rhs) const
        {
            ASSERT(rhs.m_size == m_size);

            QuickBitSet result;
            result.m_size = rhs.m_size;
            result.m_pBits = rhs.m_pBits;

            rhs.m_size = 0;
            rhs.m_pBits = NULL;

            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize; ++i)
            {
                result.m_pBits[i] &= m_pBits[i];
            }

            return result;
        }

        void Grow(unsigned int size)
        {
            if (size != m_size)
            {
                unsigned int new_aSize = (size + 31) >> 5;
                unsigned int *pBits = _concrt_new unsigned int [new_aSize];
                unsigned int aSize = ASIZE();
                for (unsigned int i = 0; i < aSize; ++i)
                    pBits[i] = m_pBits[i];

                memset(pBits + aSize, 0, (new_aSize - aSize) * sizeof(unsigned int));

                delete[] m_pBits;
                m_pBits = pBits;
                m_size = size;

            }
        }

        bool Intersects(const QuickBitSet& comparator) const
        {
            ASSERT(comparator.m_size == m_size);

            unsigned int val = 0;

            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize && val == 0; ++i)
                val |= (m_pBits[i] & comparator.m_pBits[i]);

            return (val != 0);
        }

        bool IsSet(unsigned int bitNumber) const
        {
            return ((m_pBits [bitNumber >> 5] & (1 << (bitNumber & 0x1F))) != 0);
        }

        bool IsClear(unsigned int bitNumber) const
        {
            return !IsSet(bitNumber);
        }

        void Set(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            m_pBits[bitNumber >> 5] |= (1 << (bitNumber & 0x1F));
        }

        void Clear(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            m_pBits[bitNumber >> 5] &= ~(1 << (bitNumber & 0x1F));
        }

        void InterlockedSet(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            _InterlockedOr(reinterpret_cast<volatile long *> (m_pBits + (bitNumber >> 5)), (1 << (bitNumber & 0x1F)));
        }

        void InterlockedSet(const QuickBitSet& bitSet)
        {
            ASSERT(m_size == bitSet.m_size);

            unsigned int aSize = ASIZE();
            for(unsigned int i = 0; i < aSize; ++i)
                _InterlockedOr(reinterpret_cast<volatile long *> (m_pBits + i), bitSet.m_pBits[i]);
        }

        void InterlockedClear(const QuickBitSet& bitSet)
        {
            ASSERT(m_size == bitSet.m_size);

            unsigned int aSize = ASIZE();
            for(unsigned int i = 0; i < aSize; ++i)
                _InterlockedAnd(reinterpret_cast<volatile long *> (m_pBits + i), ~(bitSet.m_pBits[i]));
        }

        void InterlockedClear(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            _InterlockedAnd(reinterpret_cast<volatile long *> (m_pBits + (bitNumber >> 5)), ~(1 << (bitNumber & 0x1F)));
        }

        void SpinUntilClear(unsigned int bitNumber) const
        {
            volatile long *pBit = reinterpret_cast<volatile long *>(m_pBits + (bitNumber >> 5));
            long mask = 1 << (bitNumber & 0x1F);

            if ((*pBit & mask) != 0)
            {
                _SpinWaitBackoffNone spinWait(_Sleep0);

                while ((*pBit & mask) != 0)
                {
                    spinWait._SpinOnce();
                }
            }
        }

        void SpinUntilSet(unsigned int bitNumber) const
        {
            volatile long *pBit = reinterpret_cast<volatile long *>(m_pBits + (bitNumber >> 5));
            long mask = 1 << (bitNumber & 0x1F);

            if ((*pBit & mask) == 0)
            {
                _SpinWaitBackoffNone spinWait(_Sleep0);

                while ((*pBit & mask) == 0)
                {
                    spinWait._SpinOnce();
                }
            }
        }

        void Wipe()
        {
            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize; ++i)
                m_pBits[i] = 0;
        }

        void Fill()
        {
            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize; ++i)
                m_pBits[i] = 0xFFFFFFFF;
        }

        unsigned int Size() const
        {
            return m_size;
        }

        unsigned int DbgAcquireBits(unsigned int psn) const
        {
            return m_pBits[psn];
        }

    protected:

        void CopyFrom(const QuickBitSet& copySrc)
        {
            if (m_size != copySrc.m_size)
                Reallocate(copySrc.m_size);

            unsigned int aSize = ASIZE();
            for (unsigned int i = 0; i < aSize; ++i)
                m_pBits[i] = copySrc.m_pBits[i];
        }

        void Reallocate(unsigned int size)
        {
            delete[] m_pBits;
            m_size = size;
            m_pBits = _concrt_new unsigned int [ASIZE()];
        }

        unsigned int ASIZE() const
        {
            return (m_size + 31) >> 5;
        }

        unsigned int m_size;
        unsigned int *m_pBits;

    };

    /// <summary>
    ///     Provides a QuickBitSet whose set and clear operations are reference counted.
    /// </summary>
    class ReferenceCountedQuickBitSet : public QuickBitSet
    {
    public:

        ReferenceCountedQuickBitSet() : m_pRefCounts(NULL)
        {
        }

        ReferenceCountedQuickBitSet(unsigned int size) : QuickBitSet(size)
        {
            m_pRefCounts = _concrt_new LONG[size];
            memset(const_cast<LONG *>(m_pRefCounts), 0, sizeof(LONG) * size);
        }

        ~ReferenceCountedQuickBitSet()
        {
            delete[] m_pRefCounts;
        }

        void Set(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            LONG val = m_pRefCounts[bitNumber];
            ++val;
            m_pRefCounts[bitNumber] = val;
            ASSERT(val > 0);
            if (val == 1)
            {
                QuickBitSet::Set(bitNumber);
            }
        }

        void Clear(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            LONG val = m_pRefCounts[bitNumber];
            --val;
            m_pRefCounts[bitNumber] = val;
            ASSERT(val >= 0);
            if (val == 0)
            {
                QuickBitSet::Clear(bitNumber);
            }
        }

        void Grow(unsigned int size)
        {
            if (size != m_size)
            {
                unsigned int oldSize = m_size;

                QuickBitSet::Grow(size);
                volatile LONG *pRefCounts = _concrt_new LONG[size];

                for (unsigned int i = 0; i < oldSize; ++i)
                    pRefCounts[i] = m_pRefCounts[i];

                memset(const_cast<LONG *>(pRefCounts + oldSize), 0, sizeof(LONG) * (size - oldSize));

                delete[] m_pRefCounts;
                m_pRefCounts = pRefCounts;
            }
        }

        unsigned int InterlockedSet(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            LONG val = InterlockedIncrement(m_pRefCounts + bitNumber);
            ASSERT(val > 0);
            if (val == 1)
            {
                SpinUntilClear(bitNumber);
                QuickBitSet::InterlockedSet(bitNumber);
            }

            return val;
        }

        unsigned int InterlockedClear(unsigned int bitNumber)
        {
            ASSERT(bitNumber < m_size);

            LONG val = InterlockedDecrement(m_pRefCounts + bitNumber);
            ASSERT(val >= 0);
            if (val == 0)
            {
                SpinUntilSet(bitNumber);
                QuickBitSet::InterlockedClear(bitNumber);
            }

            return val;
        }

        void Wipe()
        {
            QuickBitSet::Wipe();

            for (unsigned int i = 0; i < m_size; ++i)
                m_pRefCounts[i] = 0;
        }

    protected:

        volatile LONG *m_pRefCounts;

    };

} // namespace details
} // namespace Concurrency
