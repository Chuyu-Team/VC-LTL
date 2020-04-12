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
* concurrent_vector.cpp
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

/*
    Intel Material Copyright 2005-2008 Intel Corporation.  All Rights Reserved.
*/


#include "concrtinternal.h"
#include "concurrent_vector.h"
#include "cds_cache_aligned_allocator.h"
#include <stdexcept>

using namespace std;

namespace Concurrency
{

namespace details
{

    class _Concurrent_vector_base_v4::_Helper
    {
    public:
        // memory page size
        static const _Size_type page_size = 4096;

        inline static bool incompact_predicate(_Size_type size)
        {
            return size < page_size || ((size-1)%page_size < page_size/2 && size < page_size * 128);
        }

        inline static _Size_type find_segment_end(const _Concurrent_vector_base_v4 &v)
        {
            _Segment_index_t u = v._My_segment==(&(v._My_storage[0])) ? _Pointers_per_short_table
                                                                      : _Pointers_per_long_table;
            _Segment_index_t k = 0;
            while( k < u && v._My_segment[k]._My_array )
                ++k;
            return k;
        }

        // assign first segment size. k - is index of last segment to be allocated, not a count of segments
        static void assign_first_segment_if_necessary(_Concurrent_vector_base_v4 &v, _Segment_index_t k)
        {
            if( !v._My_first_block )
            {
                v._My_first_block._CompareAndSwap(k+1, 0); // store number of segments
            }
        }

        inline static void *allocate_segment(_Concurrent_vector_base_v4 &v, _Size_type n)
        {
            void *_Ptr = v._My_vector_allocator_ptr(v, n);
            if(!_Ptr)
                _STD _Xbad_alloc();
            return _Ptr;
        }

        // Publish segment so other threads can see it.
        inline static void publish_segment( _Segment_t& s, void* rhs )
        {
            _Subatomic_impl<sizeof s._My_array>::_StoreWithRelease(s._My_array, rhs);
        }

        inline static _Size_type enable_segment(_Concurrent_vector_base_v4 &v, _Size_type k, _Size_type element_size)
        {
            _CONCRT_ASSERT( !v._My_segment[k]._My_array ); // concurrent operation during growth?
            if( !k )
            {
                assign_first_segment_if_necessary(v, _Default_initial_segments-1);
                try
                {
                    publish_segment(v._My_segment[0], allocate_segment(v, _Segment_size(v._My_first_block) ) );
                }
                catch(...)
                {
                    // intercept exception here, assign _BAD_ALLOC_MARKER value, re-throw exception
                    publish_segment(v._My_segment[0], _BAD_ALLOC_MARKER);
                    throw;
                }
                return 2;
            }
            _Size_type m = _Segment_size(k);

            if( !v._My_first_block )
                SpinwaitWhileEq( v._My_first_block, _Segment_index_t(0) );

            if( k < v._My_first_block )
            {
                _Segment_t* s = v._My_segment;
                // s[0]._My_array is changed only once ( 0 -> !0 ) and points to uninitialized memory
                void *array0 = _Subatomic_impl<sizeof s[0]._My_array>::_LoadWithAquire(s[0]._My_array);
                if( !array0 )
                {
                    details::SpinwaitWhileEq( s[0]._My_array, (void*)0 );
                    array0 = _Subatomic_impl<sizeof s[0]._My_array>::_LoadWithAquire(s[0]._My_array);
                }
                if( array0 <= _BAD_ALLOC_MARKER ) // check for _BAD_ALLOC_MARKER of initial segment
                {
                    publish_segment(s[k], _BAD_ALLOC_MARKER); // and assign _BAD_ALLOC_MARKER here
                    _STD _Xbad_alloc();
                }
                publish_segment(s[k],
                                static_cast<void*>( static_cast<char*>(array0) + _Segment_base(k)*element_size ) );
            }
            else
            {
                try
                {
                    publish_segment(v._My_segment[k], allocate_segment(v, m));
                }
                catch(...)
                {
                    // intercept exception here, assign _BAD_ALLOC_MARKER value, re-throw exception
                    publish_segment(v._My_segment[k], _BAD_ALLOC_MARKER);
                    throw;
                }
            }
            return m;
        }

        inline static void extend_table_if_necessary(_Concurrent_vector_base_v4 &v, _Size_type k)
        {
            if(k >= _Pointers_per_short_table && v._My_segment == v._My_storage)
                extend_segment_table(v);
        }

        static void extend_segment_table(_Concurrent_vector_base_v4 &v)
        {
            _Segment_t* s = (_Segment_t*)NFS_Allocate( _Pointers_per_long_table, sizeof(_Segment_t), NULL );
            // if( !s ) throw bad_alloc() -- implemented in NFS_Allocate
            memset( s, 0, _Pointers_per_long_table*sizeof(_Segment_t) );
            // If other threads are trying to set pointers in the short segment, wait for them to finish their
            // assigments before we copy the short segment to the long segment.  Note: grow_to_at_least depends on it.
            for(_Segment_index_t i = 0; i < _Pointers_per_short_table; i++)
            {
                if(!v._My_storage[i]._My_array)
                    SpinwaitWhileEq(v._My_storage[i]._My_array, (void*)0);
            }

            for( _Segment_index_t i = 0; i < _Pointers_per_short_table; i++)
                s[i] = v._My_storage[i];
            if( v._My_segment._CompareAndSwap( s, v._My_storage ) != v._My_storage )
                NFS_Free( s );
        }
    };

    _CONCRTIMP _Concurrent_vector_base_v4::~_Concurrent_vector_base_v4()
    {
        _Segment_t* s = _My_segment;
        if( s != _My_storage )
        {
            // Clear short segment.
            for( _Segment_index_t i = 0; i < _Pointers_per_short_table; i++)
                _My_storage[i]._My_array = NULL;
            _My_segment = _My_storage;
            NFS_Free( s );
        }
    }


    _CONCRTIMP _Concurrent_vector_base_v4::_Segment_index_t __cdecl _Concurrent_vector_base_v4::_Segment_index_of( _Size_type _Index )
    {
        return _Segment_index_t( Log2( _Index|1 ) );
    }


    _CONCRTIMP _Concurrent_vector_base_v4::_Size_type _Concurrent_vector_base_v4::_Internal_capacity() const
    {
        return _Segment_base( _Helper::find_segment_end(*this) );
    }

    _CONCRTIMP void _Concurrent_vector_base_v4::_Internal_throw_exception(_Size_type t) const
    {
        switch(t)
        {
            case 0: _STD _Xout_of_range("Index out of range");
            case 1: _STD _Xout_of_range("Index out of segments table range");
            case 2: _STD _Throw_range_error("Index is inside segment which failed to be allocated");
        }
    }

    _CONCRTIMP void _Concurrent_vector_base_v4::_Internal_reserve( _Size_type n, _Size_type element_size, _Size_type max_size )
    {
        if( n>max_size )
            _STD _Xlength_error("argument to concurrent_vector::reserve() exceeds concurrent_vector::max_size()");

        _Helper::assign_first_segment_if_necessary(*this, _Segment_index_of(n-1));
        _Segment_index_t k = _Helper::find_segment_end(*this);
        try
        {
            for (; _Segment_base(k) < n; ++k)
            {
                _Helper::extend_table_if_necessary(*this, k);
                _Helper::enable_segment(*this, k, element_size);
            }
        }
        catch (...)
        {
            // repair and rethrow
            _My_segment[k]._My_array = NULL;
            throw;
        }
    }

    _CONCRTIMP void _Concurrent_vector_base_v4::_Internal_copy( const _Concurrent_vector_base_v4& src, _Size_type element_size, _My_internal_array_op2 copy )
    {
        _Size_type n = src._My_early_size;
        _My_early_size = n;
        _My_segment = _My_storage;
        if( n )
        {
            _Helper::assign_first_segment_if_necessary(*this, _Segment_index_of(n));
            _Size_type b;
            for( _Segment_index_t k=0; (b=_Segment_base(k))<n; ++k )
            {
                if( (src._My_segment == (_Segment_t*)src._My_storage && k >= _Pointers_per_short_table)
                    || src._My_segment[k]._My_array <= _BAD_ALLOC_MARKER )
                {
                    _My_early_size = b;
                    break;
                }
                _Helper::extend_table_if_necessary(*this, k);
                _Size_type m = _Helper::enable_segment(*this, k, element_size);
                if( m > n-b )
                    m = n-b;
                copy( _My_segment[k]._My_array, src._My_segment[k]._My_array, m );
            }
        }
    }

    _CONCRTIMP void _Concurrent_vector_base_v4::_Internal_assign( const _Concurrent_vector_base_v4& src, _Size_type element_size, _My_internal_array_op1 destroy, _My_internal_array_op2 assign, _My_internal_array_op2 copy )
    {
        _Size_type n = src._My_early_size;
        while( _My_early_size>n )
        {
            _Segment_index_t k = _Segment_index_of( _My_early_size-1 );
            _Size_type b=_Segment_base(k);
            _Size_type new_end = b>=n ? b : n;
            _CONCRT_ASSERT( _My_early_size>new_end );
            if( _My_segment[k]._My_array <= _BAD_ALLOC_MARKER) // check vector was broken before
                _STD _Xbad_alloc();
            // destructors are supposed to not throw any exceptions
            destroy( (char*)_My_segment[k]._My_array+element_size*(new_end-b), _My_early_size-new_end );
            _My_early_size = new_end;
        }
        _Size_type dst_initialized_size = _My_early_size;
        _My_early_size = n;
        _Helper::assign_first_segment_if_necessary(*this, _Segment_index_of(n));
        _Size_type b;
        for( _Segment_index_t k=0; (b=_Segment_base(k))<n; ++k )
        {
            _Helper::extend_table_if_necessary(*this, k);
            if(!_My_segment[k]._My_array)
                _Helper::enable_segment(*this, k, element_size);
            if( (src._My_segment == (_Segment_t*)src._My_storage && k >= _Pointers_per_short_table)
                || src._My_segment[k]._My_array <= _BAD_ALLOC_MARKER ) // if source is damaged
            {
                _My_early_size = b;
                break;
            }
            _Size_type m = k? _Segment_size(k) : 2;
            if( m > n-b ) m = n-b;
            _Size_type a = 0;
            if( dst_initialized_size>b )
            {
                a = dst_initialized_size-b;
                if( a>m )
                    a = m;
                assign( _My_segment[k]._My_array, src._My_segment[k]._My_array, a );
                m -= a;
                a *= element_size;
            }
            if( m>0 )
                copy( (char*)_My_segment[k]._My_array+a, (char*)src._My_segment[k]._My_array+a, m );
        }
        _CONCRT_ASSERT( src._My_early_size==n ); // detected use of ConcurrentVector::operator= with right side that was concurrently modified
    }

    _CONCRTIMP void* _Concurrent_vector_base_v4::_Internal_push_back( _Size_type element_size, _Size_type& index )
    {
        _Size_type tmp = _My_early_size++;
        index = tmp;
        _Segment_index_t k_old = _Segment_index_of( tmp );
        _Size_type base = _Segment_base(k_old);
        _Helper::extend_table_if_necessary(*this, k_old);
        _Segment_t& s = _My_segment[k_old];
        if ( !_Subatomic_impl<sizeof s._My_array>::_LoadWithAquire(s._My_array) )
        {
            // do not check for _BAD_ALLOC_MARKER because it's hard to recover after _BAD_ALLOC_MARKER correctly
            if( base==tmp )
                _Helper::enable_segment(*this, k_old, element_size);
            else
                details::SpinwaitWhileEq( s._My_array, (void*)0 );
        }
        if( s._My_array <= _BAD_ALLOC_MARKER ) // check for _BAD_ALLOC_MARKER
            _STD _Xbad_alloc();
        _Size_type j_begin = tmp-base;
        return (void*)((char*)s._My_array+element_size*j_begin);
    }

    _CONCRTIMP _Concurrent_vector_base_v4::_Size_type _Concurrent_vector_base_v4::_Internal_grow_to_at_least_with_result( _Size_type new_size, _Size_type element_size, _My_internal_array_op2 init, const void *src )
    {
        _Size_type e = _My_early_size;
        while( e<new_size )
        {
            _Size_type f = _My_early_size._CompareAndSwap(new_size,e);
            if( f==e )
            {
                _Internal_grow( e, new_size, element_size, init, src );
                break;
            }
            e = f;
        }

        // Check/wait for segments allocation completes
        _Segment_index_t i, k_old = _Segment_index_of( new_size-1 );
        if( k_old >= _Pointers_per_short_table && _My_segment == _My_storage )
        {
            details::SpinwaitWhileEq( _My_segment, _My_storage );
            i = _Pointers_per_short_table; // suppose short table is filled already
        }
        else
        {
            i = 0;
        }

        while( i <= k_old )
        {
            _Segment_t &s = _My_segment[i++];
            if( !s._My_array ) // concurrent changing of my_segment is ok due to wait in extend_segment_table()
            {
                details::SpinwaitWhileEq( s._My_array, (void*)0 );
            }
        }
        _CONCRT_ASSERT( _Internal_capacity() >= new_size );
        return e;
    }

    _CONCRTIMP _Concurrent_vector_base_v4::_Size_type _Concurrent_vector_base_v4::_Internal_grow_by( _Size_type _Delta, _Size_type element_size, _My_internal_array_op2 init, const void *src )
    {
        _Size_type result = _My_early_size._FetchAndAdd(_Delta);
        _Internal_grow( result, result+_Delta, element_size, init, src );
        return result;
    }

    _Concurrent_vector_base_v4::_Size_type _Concurrent_vector_base_v4::_Internal_grow_segment( const _Size_type start, _Size_type finish, _Size_type element_size, _Segment_t** ppSegment, _Size_type* pSegStart, _Size_type* pSegFinish )
    {
        _Segment_index_t k_old = _Segment_index_of( start );
        _Size_type base = _Segment_base(k_old);
        _Helper::extend_table_if_necessary(*this, k_old);
        _Segment_t& s = _My_segment[k_old];
        if ( !_Subatomic_impl<sizeof s._My_array>::_LoadWithAquire(s._My_array) )
        {
            // do not check for _BAD_ALLOC_MARKER because it's hard to recover after _BAD_ALLOC_MARKER correctly
            if( base==start )
                _Helper::enable_segment(*this, k_old, element_size);
            else
                details::SpinwaitWhileEq( s._My_array, (void*)0 );
        }
        if( s._My_array <= _BAD_ALLOC_MARKER ) // check for _BAD_ALLOC_MARKER
            _STD _Xbad_alloc();
        _Size_type n = k_old?_Segment_size(k_old):2;
        *ppSegment = &s;
        *pSegStart = start - base;
        *pSegFinish = n > finish-base ? finish-base : n;
        return base + *pSegFinish;
    }

    void _Concurrent_vector_base_v4::_Internal_grow( const _Size_type start, _Size_type finish, _Size_type element_size, _My_internal_array_op2 init, const void *src )
    {
        _CONCRT_ASSERT( start<finish ); // start must be less than finish
        _Size_type new_start = start;
        _Helper::assign_first_segment_if_necessary(*this, _Segment_index_of(finish));
        do
        {
            _Size_type j_begin;
            _Size_type j_end;
            _Segment_t* pSegment;
            new_start = _Internal_grow_segment( new_start, finish, element_size, &pSegment, &j_begin, &j_end );

            try
            {
                init( (void*)((char*)pSegment->_My_array+element_size*j_begin), src, j_end-j_begin );
            }
            catch (...) {
                // Continue growing the remaining segments, zero-filling instead of initializing.
                do
                {
                    new_start = _Internal_grow_segment( new_start, finish, element_size, &pSegment, &j_begin, &j_end );
                    memset( (void*)((char*)pSegment->_My_array+element_size*j_begin), 0, (j_end-j_begin) * element_size );
                } while (new_start < finish);

                throw;
            }
        } while( new_start < finish );
    }

    _CONCRTIMP void _Concurrent_vector_base_v4::_Internal_resize( _Size_type new_size, _Size_type element_size, _Size_type max_size, _My_internal_array_op1 destroy, _My_internal_array_op2 init, const void* src)
    {
        _Size_type j = _My_early_size;
        _My_early_size = new_size;
        if( new_size > j ) // construct items
        {
            _Internal_reserve(new_size, element_size, max_size);
            _Segment_index_t k = _Segment_index_of( j );
            _Size_type i = _My_first_block; // it should be read after call to reserve
            if( k < i ) // process solid segment at a time
                k = 0;
            _Segment_index_t b = _Segment_base( k );
            new_size -= b; j -= b; // rebase as offsets from segment k
            _Size_type sz = k ? b : _Segment_size( i ); // sz==b for k>0
            while( sz < new_size ) // work for more than one segment
            {
                void *array = _My_segment[k]._My_array;
                if( array <= _BAD_ALLOC_MARKER )
                    _STD _Xbad_alloc();
                init( (void*)((char*)array+element_size*j), src, sz-j );
                new_size -= sz; j = 0; // offsets from next segment
                if( !k )
                {
                    k = i;
                }
                else
                {
                    ++k;
                    sz <<= 1;
                }
            }
            void *array = _My_segment[k]._My_array;
            if( array <= _BAD_ALLOC_MARKER )
                _STD _Xbad_alloc();
            init( (void*)((char*)array+element_size*j), src, new_size-j );
        }
        else
        {
            _Segment_index_t k = _Segment_index_of( new_size );
            _Size_type i = _My_first_block;
            if( k < i ) // process solid segment at a time
                k = 0;
            _Segment_index_t b = _Segment_base( k );
            new_size -= b; j -= b; // rebase as offsets from segment k
            _Size_type sz = k ? b : _Segment_size( i ); // sz==b for k>0
            while( sz < j ) // work for more than one segment
            {
                void *array = _My_segment[k]._My_array;
                if( array > _BAD_ALLOC_MARKER )
                    destroy( (void*)((char*)array+element_size*new_size), sz-new_size);
                j -= sz; new_size = 0;
                if( !k )
                {
                    k = i;
                }
                else
                {
                    ++k;
                    sz <<= 1;
                }
            }
            void *array = _My_segment[k]._My_array;
            if( array > _BAD_ALLOC_MARKER )
                destroy( (void*)((char*)array+element_size*new_size), j-new_size);
        }
     }

    _CONCRTIMP _Concurrent_vector_base_v4::_Segment_index_t _Concurrent_vector_base_v4::_Internal_clear( _My_internal_array_op1 destroy )
    {
        _CONCRT_ASSERT( _My_segment != NULL );
        const _Size_type k_end = _Helper::find_segment_end(*this);
        _Size_type finish = _My_early_size;
        // Set "_My_early_size" early, so that subscripting errors can be caught.
        _My_early_size = 0;
        while( finish > 0 )
        {
            _Segment_index_t k_old = _Segment_index_of(finish-1);
            _Size_type base = _Segment_base(k_old);
            _Size_type j_end = finish-base;
            finish = base;
            if( k_old <= k_end )
            {
                _Segment_t& s = _My_segment[k_old];
                _CONCRT_ASSERT( j_end );
                if( s._My_array > _BAD_ALLOC_MARKER)
                    destroy( s._My_array, j_end ); // destructors are supposed to not throw any exceptions
            }
        }
        return k_end;
    }

    _CONCRTIMP void *_Concurrent_vector_base_v4::_Internal_compact( _Size_type element_size, void *table, _My_internal_array_op1 destroy, _My_internal_array_op2 copy )
    {
        const _Size_type my_size = _My_early_size;
        const _Segment_index_t k_end = _Helper::find_segment_end(*this);
        // number of segments to store existing items: 0=>0; 1,2=>1; 3,4=>2; [5-8]=>3; ...
        const _Segment_index_t k_stop = my_size ? _Segment_index_of(my_size-1) + 1 : 0;
        const _Segment_index_t first_block = _My_first_block; // number of merged segments; getting values from atomics

        _Segment_index_t k = first_block;
        if(k_stop < first_block)
        {
            k = k_stop;
        }
        else
        {
            while (k < k_stop && _Helper::incompact_predicate(_Segment_size( k ) * element_size) )
                k++;
        }
        if(k_stop == k_end && k == first_block)
            return NULL;

        _Segment_t *const segment_table = _My_segment;
        _Internal_segments_table &old = *static_cast<_Internal_segments_table*>( table );
        memset(&old, 0, sizeof(old));

        if ( k != first_block && k ) // first segment optimization
        {
            // exception can occur here
            void *seg = old._Table[0] = _Helper::allocate_segment( *this, _Segment_size(k) );
            old._First_block = k; // fill info for freeing new segment if exception occurs
            // copy items to the new segment
            _Size_type my_segment_size = _Segment_size( first_block );
            for (_Segment_index_t i = 0, j = 0; i < k && j < my_size; j = my_segment_size)
            {
                _CONCRT_ASSERT( segment_table[i]._My_array > _BAD_ALLOC_MARKER );
                void *s = static_cast<void*>( static_cast<char*>(seg) + _Segment_base(i)*element_size );
                if(j + my_segment_size >= my_size)
                    my_segment_size = my_size - j;
                // exception can occur here
                copy( s, segment_table[i]._My_array, my_segment_size );
                my_segment_size = i? _Segment_size( ++i ) : _Segment_size( i = first_block );
            }
            // commit the changes
            memcpy(old._Table, segment_table, k * sizeof(_Segment_t));
            for (_Segment_index_t i = 0; i < k; i++)
            {
                segment_table[i]._My_array = static_cast<void*>( static_cast<char*>(seg) + _Segment_base(i)*element_size );
            }
            old._First_block = first_block; _My_first_block = k; // now, first_block != _My_first_block
            // destroy original copies
            my_segment_size = _Segment_size( first_block ); // old.first_block actually
            for (_Segment_index_t i = 0, j = 0; i < k && j < my_size; j = my_segment_size)
            {
                if(j + my_segment_size >= my_size)
                    my_segment_size = my_size - j;
                // destructors are supposed to not throw any exceptions
                destroy( old._Table[i], my_segment_size );
                my_segment_size = i? _Segment_size( ++i ) : _Segment_size( i = first_block );
            }
        }
        // free unnecessary segments allocated by reserve() call
        if ( k_stop < k_end )
        {
            old._First_block = first_block;
            memcpy(old._Table+k_stop, segment_table+k_stop, (k_end-k_stop) * sizeof(_Segment_t));
            memset(segment_table+k_stop, 0, (k_end-k_stop) * sizeof(_Segment_t));
            if (!k)
                _My_first_block = 0;
        }
        return table;
    }

    _CONCRTIMP void _Concurrent_vector_base_v4::_Internal_swap(_Concurrent_vector_base_v4& v)
    {
        _Size_type my_sz = _My_early_size, v_sz = v._My_early_size;
        if(!my_sz && !v_sz)
            return;

        _Size_type tmp = _My_first_block; _My_first_block = v._My_first_block; v._My_first_block = tmp;
        bool my_short = (_My_segment == _My_storage), v_short  = (v._My_segment == v._My_storage);
        if ( my_short && v_short ) // swap both tables
        {
            char tbl[_Pointers_per_short_table * sizeof(_Segment_t)];
            memcpy(tbl, _My_storage, _Pointers_per_short_table * sizeof(_Segment_t));
            memcpy(_My_storage, v._My_storage, _Pointers_per_short_table * sizeof(_Segment_t));
            memcpy(v._My_storage, tbl, _Pointers_per_short_table * sizeof(_Segment_t));
        }
        else if ( my_short ) // my -> v
        {
            memcpy(v._My_storage, _My_storage, _Pointers_per_short_table * sizeof(_Segment_t));
            _My_segment = v._My_segment; v._My_segment = v._My_storage;
        }
        else if ( v_short ) // v -> my
        {
            memcpy(_My_storage, v._My_storage, _Pointers_per_short_table * sizeof(_Segment_t));
            v._My_segment = _My_segment; _My_segment = _My_storage;
        }
        else
        {
            _Segment_t *_Ptr = _My_segment; _My_segment = v._My_segment; v._My_segment = _Ptr;
        }
        _My_early_size = v_sz; v._My_early_size = my_sz;
    }

} // namespace details

} // namespace Concurrency
