/***
* ==++==
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
* Microsoft would like to acknowledge that this concurrency data structure implementation
* is based on Intel's implementation in its Threading Building Blocks ("Intel Material").
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* concurrent_queue.cpp
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

/*
    Intel Material Copyright 2005-2008 Intel Corporation.  All Rights Reserved.
*/


#include "concrtinternal.h"
#include "concurrent_queue.h"
#include "cds_cache_aligned_allocator.h"
#include <algorithm>

using namespace std;

namespace Concurrency
{

namespace details
{

    class _Micro_queue::_Push_finalizer
    {
    private:
        _Ticket my_ticket;
        _Micro_queue& my_queue;
    public:
        _Push_finalizer & operator = (const _Micro_queue::_Push_finalizer & other) = delete;

        _Push_finalizer( _Micro_queue& queue, _Ticket k )
            : my_ticket(k), my_queue(queue)
        {
        }

        ~_Push_finalizer()
        {
            my_queue._Tail_counter = my_ticket;
        }
    };

    class _Micro_queue::_Pop_finalizer
    {
    private:
        _Ticket my_ticket;
        _Micro_queue& my_queue;
        _Concurrent_queue_base::_Page* my_page;
        _Concurrent_queue_base &base;

    public:
        _Pop_finalizer & operator = (const _Micro_queue::_Pop_finalizer & other) = delete;

        _Pop_finalizer( _Micro_queue& queue, _Concurrent_queue_base& b, _Ticket k, _Concurrent_queue_base::_Page* p )
            : my_ticket(k), my_queue(queue), my_page(p), base(b)
        {
        }

        ~_Pop_finalizer()
        {
            _Concurrent_queue_base::_Page* p = my_page;
            if( p )
            {
                _SpinLock lock(my_queue._Page_mutex_flag);

                _Concurrent_queue_base::_Page* q = p->_Next;
                my_queue._Head_page = q;
                if( !q )
                {
                    my_queue._Tail_page = NULL;
                }
            }

            my_queue._Head_counter = my_ticket;

            if( p )
               base._Deallocate_page( p );
        }
    };


    #pragma warning( push )
    // unary minus operator applied to unsigned type, result still unsigned
    #pragma warning( disable: 4146 )


    static void* invalid_page;

    //------------------------------------------------------------------------
    // _Micro_queue
    //------------------------------------------------------------------------
    void _Micro_queue::_Push( void* item, _Ticket k, _Concurrent_queue_base& base, void (_Concurrent_queue_base:: *moveOp)(_Concurrent_queue_base_v4::_Page&, size_t, void*))
    {
        static _Concurrent_queue_base::_Page dummy = {static_cast<_Concurrent_queue_base::_Page*>((void*)1), 0};
        k &= -_Concurrent_queue_rep::_N_queue;
        _Concurrent_queue_base::_Page* p = NULL;
        size_t index = (k/_Concurrent_queue_rep::_N_queue & base._Items_per_page-1);
        if( !index )
        {
            try
            {
                p = base._Allocate_page();
            } catch (...)
            {
                // mark it so that no more pushes are allowed.
                invalid_page = &dummy;
                _SpinLock lock(_Page_mutex_flag);

                _Tail_counter = k+_Concurrent_queue_rep::_N_queue+1;
                if( _Concurrent_queue_base::_Page* q = _Tail_page )
                    q->_Next = static_cast<_Concurrent_queue_base::_Page*>(invalid_page);
                else
                    _Head_page = static_cast<_Concurrent_queue_base::_Page*>(invalid_page);
                _Tail_page = static_cast<_Concurrent_queue_base::_Page*>(invalid_page);
                throw;
            }
            p->_Mask = 0;
            p->_Next = NULL;
        }

        {
            _Push_finalizer finalizer( *this, k+_Concurrent_queue_rep::_N_queue );
            if( _Tail_counter!=k ) {
                _SpinWaitBackoffNone spinWait;
                do
                {
                    spinWait._SpinOnce();
                    // no memory. throws an exception
                    if( _Tail_counter&0x1 )
                        throw std::bad_alloc();
                } while( _Tail_counter!=k ) ;
            }

            if( p )
            {
                _SpinLock lock(_Page_mutex_flag);
                if( _Concurrent_queue_base::_Page* q = _Tail_page )
                    q->_Next = p;
                else
                    _Head_page = p;
                _Tail_page = p;
            }
            else
            {
                p = _Tail_page;
            }

            (base.*moveOp)(*p, index, item);
            // If no exception was thrown, mark item as present.
            p->_Mask |= size_t(1)<<index;
        }
    }

    bool _Micro_queue::_Pop( void* dst, _Ticket k, _Concurrent_queue_base& base )
    {
        k &= -_Concurrent_queue_rep::_N_queue;
        SpinwaitUntilEq( _Head_counter, k );
        SpinwaitWhileEq( _Tail_counter, k );
        _Concurrent_queue_base::_Page& p = *_Head_page;
        _CONCRT_ASSERT( &p );
        size_t index = (k/_Concurrent_queue_rep::_N_queue & base._Items_per_page-1);
        bool success = false;
        {
            _Pop_finalizer finalizer( *this, base, k+_Concurrent_queue_rep::_N_queue, index==base._Items_per_page-1 ? &p : NULL );
            if( p._Mask & size_t(1)<<index )
            {
                success = true;
                base._Assign_and_destroy_item( dst, p, index );
            }
        }
        return success;
    }

    #pragma warning( pop )

    //------------------------------------------------------------------------
    // _Concurrent_queue_base
    //------------------------------------------------------------------------
    _CONCRTIMP _Concurrent_queue_base_v4::_Concurrent_queue_base_v4( size_t _Item_size )
    {
        _Items_per_page = _Item_size<=8 ? 32 :
                         _Item_size<=16 ? 16 :
                         _Item_size<=32 ? 8 :
                         _Item_size<=64 ? 4 :
                         _Item_size<=128 ? 2 :
                         1;
        _My_rep = cache_aligned_allocator<_Concurrent_queue_rep>().allocate(1);
        _CONCRT_ASSERT( (size_t)_My_rep % NFS_GetLineSize()==0 ); // alignment error
        _CONCRT_ASSERT( (size_t)&_My_rep->_Head_counter % NFS_GetLineSize()==0 ); // alignment error
        _CONCRT_ASSERT( (size_t)&_My_rep->_Tail_counter % NFS_GetLineSize()==0 ); // alignment error
        _CONCRT_ASSERT( (size_t)&_My_rep->_Array % NFS_GetLineSize()==0 ); // alignment error
        memset(_My_rep,0,sizeof(_Concurrent_queue_rep));
        this->_Item_size = _Item_size;
    }

    _CONCRTIMP _Concurrent_queue_base_v4::~_Concurrent_queue_base_v4()
    {
        size_t nq = _My_rep->_N_queue;
        for( size_t i=0; i<nq; i++ )
            _CONCRT_ASSERT( _My_rep->_Array[i]._Tail_page==NULL ); // pages were not freed properly
        cache_aligned_allocator<_Concurrent_queue_rep>().deallocate(_My_rep,1);
    }

    _CONCRTIMP void _Concurrent_queue_base_v4::_Internal_swap( _Concurrent_queue_base_v4& other )
    {
        if( _My_rep!=other._My_rep )
        {
            std::swap(_My_rep, other._My_rep);
            // do not need to swap _Items_per_page or _Item_size
            // they should be same.
        }
    }

    _CONCRTIMP void _Concurrent_queue_base_v4::_Internal_push( const void* src )
    {
        _Concurrent_queue_rep& r = *_My_rep;
        _Ticket tail = r._Tail_counter++;

        r._Choose( tail )._Push( const_cast<void *>(src), tail, *this,
            reinterpret_cast<void (_Concurrent_queue_base::*)(_Concurrent_queue_base_v4::_Page&, size_t, void*)>(&_Concurrent_queue_base_v4::_Copy_item));
    }

    _CONCRTIMP void _Concurrent_queue_base_v4::_Internal_move_push( void* src )
    {
        _Concurrent_queue_rep& r = *_My_rep;
        _Ticket tail = r._Tail_counter++;

        r._Choose( tail )._Push( src, tail, *this, &_Concurrent_queue_base_v4::_Move_item);
    }

    _CONCRTIMP bool _Concurrent_queue_base_v4::_Internal_pop_if_present( void* dst )
    {
        _Concurrent_queue_rep& r = *_My_rep;
        _Ticket head;

        do
        {
            head = r._Head_counter;
            for(;;)
            {
                if( head == r._Tail_counter )
                {
                    // Queue is empty
                    return false;
                }
                // Queue had item with ticket k when we looked.  Attempt to get that item.
                _Ticket oldHead=head;
                head = r._Head_counter._CompareAndSwap( oldHead+1, oldHead );
                if( head==oldHead )
                    break;
                // Another thread snatched the item, retry.
            }
        } while( !r._Choose( head )._Pop( dst, head, *this ) );

        return true;
    }

    _CONCRTIMP size_t _Concurrent_queue_base_v4::_Internal_size() const
    {
        return static_cast<size_t>(_My_rep->_Tail_counter-_My_rep->_Head_counter);
    }

    _CONCRTIMP bool _Concurrent_queue_base_v4::_Internal_empty() const
    {
        _Ticket t0 = _My_rep->_Tail_counter;
        _Ticket h = _My_rep->_Head_counter;
        _Ticket t1 = _My_rep->_Tail_counter; // Load tail again to test consistency

        if (t0 == t1) {
            // We got a consistent snapshot, so it was empty when we looked and saw the tail and head were equal.
            return t0 == h;
        }

        // t0 != t1, meaning some other thread must have pushed an item -- it was therefore not empty when we looked
        return false;

        // ... Of course by the time we get here, the result is obsolete.
    }

    _CONCRTIMP void _Concurrent_queue_base_v4::_Internal_finish_clear()
    {
        size_t nq = _My_rep->_N_queue;
        for( size_t i=0; i<nq; i++ )
        {
            _Page* tp = _My_rep->_Array[i]._Tail_page;
            _CONCRT_ASSERT( _My_rep->_Array[i]._Head_page==tp ); //at most one page should remain
            if( tp!=NULL)
            {
                if( tp!=invalid_page )
                    _Deallocate_page( tp );
                _My_rep->_Array[i]._Tail_page = NULL;
            }
        }
    }

    _CONCRTIMP void _Concurrent_queue_base_v4::_Internal_throw_exception() const
    {
        throw bad_alloc();
    }

    //------------------------------------------------------------------------
    // _Concurrent_queue_iterator_rep
    //------------------------------------------------------------------------
    class  _Concurrent_queue_iterator_rep
    {
    public:
        _Ticket _Head_counter;
        const _Concurrent_queue_base& my_queue;
        _Concurrent_queue_base::_Page* array[_Concurrent_queue_rep::_N_queue];
        _Concurrent_queue_iterator_rep( const _Concurrent_queue_base& queue )
            : _Head_counter(queue._My_rep->_Head_counter),
              my_queue(queue)
        {
            const _Concurrent_queue_rep& rep = *queue._My_rep;
            for( size_t k=0; k<_Concurrent_queue_rep::_N_queue; ++k )
                array[k] = rep._Array[k]._Head_page;
        }

        _Concurrent_queue_iterator_rep & operator =(const _Concurrent_queue_iterator_rep &) = delete;

        // Get pointer to kth element
        void* choose( size_t k )
        {
            if( k==my_queue._My_rep->_Tail_counter )
            {
                return NULL;
            }
            else
            {
                _Concurrent_queue_base::_Page* p = array[_Concurrent_queue_rep::_Index(k)];
                _CONCRT_ASSERT(p);
                size_t i = k/_Concurrent_queue_rep::_N_queue & my_queue._Items_per_page-1;
                return static_cast<unsigned char*>(static_cast<void*>(p+1)) + my_queue._Item_size*i;
            }
        }
    };

    //------------------------------------------------------------------------
    // concurrent_queue_iterator_base
    //------------------------------------------------------------------------
    _CONCRTIMP _Concurrent_queue_iterator_base_v4::_Concurrent_queue_iterator_base_v4( const _Concurrent_queue_base& queue )
    {
        _My_rep = cache_aligned_allocator<_Concurrent_queue_iterator_rep>().allocate(1);
        new( _My_rep ) _Concurrent_queue_iterator_rep(queue);
        _My_item = _My_rep->choose(_My_rep->_Head_counter);
    }

    _CONCRTIMP void _Concurrent_queue_iterator_base_v4::_Assign( const concurrent_queue_iterator_base& other )
    {
        if( _My_rep!=other._My_rep )
        {
            if( _My_rep )
            {
                cache_aligned_allocator<_Concurrent_queue_iterator_rep>().deallocate(_My_rep, 1);
                _My_rep = NULL;
            }
            if( other._My_rep )
            {
                _My_rep = cache_aligned_allocator<_Concurrent_queue_iterator_rep>().allocate(1);
                new( _My_rep ) _Concurrent_queue_iterator_rep( *other._My_rep );
            }
        }
        _My_item = other._My_item;
    }

    _CONCRTIMP void _Concurrent_queue_iterator_base_v4::_Advance()
    {
        _CONCRT_ASSERT( _My_item ); // attempt to increment iterator past end of queue
        size_t k = _My_rep->_Head_counter;
        const _Concurrent_queue_base& queue = _My_rep->my_queue;
        _CONCRT_ASSERT( _My_item==_My_rep->choose(k) );
        size_t i = k/_Concurrent_queue_rep::_N_queue & queue._Items_per_page-1;
        if( i==queue._Items_per_page-1 )
        {
            _Concurrent_queue_base::_Page*& root = _My_rep->array[_Concurrent_queue_rep::_Index(k)];
            root = root->_Next;
        }
        _My_rep->_Head_counter = k+1;
        _My_item = _My_rep->choose(k+1);
    }

    _CONCRTIMP _Concurrent_queue_iterator_base_v4::~_Concurrent_queue_iterator_base_v4()
    {
        cache_aligned_allocator<_Concurrent_queue_iterator_rep>().deallocate(_My_rep, 1);
        _My_rep = NULL;
    }

} // namespace details

} // namespace Concurrency
