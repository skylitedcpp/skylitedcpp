/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************/

#pragma once

#include <boost/pool/pool_alloc.hpp>
#include <assert.h>

template <class T>
class PoolItem{
public:
    PoolItem(){
    }

    virtual ~PoolItem(){
    }

    static void* operator new(size_t s) {
        assert(sizeof(T) == s);

        return reinterpret_cast<void*>(pool.allocate());
    }

    static void* operator new(size_t, void* m) {
        return m;
    }

    static void operator delete(void*, void*) { }

    static void operator delete(void* m, size_t s) {
        assert(sizeof(T) == s);

        pool.deallocate(reinterpret_cast<T*>(m));
    }

private:
    static boost::fast_pool_allocator<T> pool;
};

template <class T>
boost::fast_pool_allocator<T> PoolItem<T>::pool = boost::fast_pool_allocator<T>();
