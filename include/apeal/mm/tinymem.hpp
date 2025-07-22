/**************************************************************************
 *   APEAL - Application Libray                                           *
 *   Copyright (C) 1996-2025 Andrei Mirzoyan                              * 
 *                                                                        *
 *   This Source Code Form is subject to the terms of the Mozilla Public  *
 *   License, v. 2.0. If a copy of the MPL was not distributed with this  *
 *   file, You can obtain one at http://mozilla.org/MPL/2.0/.             *
 **************************************************************************/
#pragma once

#include <limits>


namespace apl {


template <class T>
class ATinyAllocator {
private:
    typedef ATinyAllocator<T> X;


public: // type definitions
    typedef T* pointer;
    typedef const T* const_pointer;

    typedef T& reference;
    typedef const T& const_reference;

    typedef T value_type;

    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;


    // typename X::template rebind<U>::other
    // For all U (including T ), Y::template rebind<T>::other is X.
    // convert an allocator<T> to allocator<U>
    template <typename U>
    struct rebind {
        typedef ATinyAllocator<U> other;
    };


public: // C++ standart support
    pointer address(reference r) const { return &r; }
    const_pointer address(const_reference r) const { return &r; }


    pointer allocate(const size_type cnt, const typename ATinyAllocator<T>::const_pointer u = 0) {
        return allocate_basic_(cnt, u);
    }
    void deallocate(const pointer p, const size_type cnt) { deallocate_basic_(p, cnt); }


    size_type max_size() const throw() { return std::numeric_limits<size_type>::max() / sizeof(T); }

    bool operator==(X const& a) { return bool(this == &a); }
    bool operator!=(X const& a) { return !operator==(a); }


    explicit ATinyAllocator() throw() : buffer_block_size(9046) {
        // std::cout << this << " ctor" << std::endl ;
    }

    template <typename U>
    explicit ATinyAllocator(ATinyAllocator<U> const&) throw() : buffer_block_size(9046) {
        // std::cout << this << " copy" << std::endl ;
    }

    ~ATinyAllocator() throw() { dtor_(); }


    void construct(pointer p, const T& t) { ::new (p) T(t); }
    void destroy(pointer p) { p->~T(); }


private:
    struct buffer_type {
        buffer_type* next;

        int block_count;
        unsigned char* buffer;
        size_type buffer_size, buffer_tail;

        buffer_type() : next(0), block_count(0), buffer(0), buffer_size(0), buffer_tail(0) {}
    };


    struct buffer_list_type {
        buffer_type* first;

        buffer_list_type() : first(0) {}

        void add_buffer(buffer_type* b) {
            if (first)
                b->next = first, first = b;
            else
                b->next = 0, first = b;
        }

        void remove_buffer(buffer_type* b, buffer_type* p) {
            if (p == 0)
                for (p = first; p && p->next != b; p = p->next)
                    ;
            if (p && p->next != b)
                p = 0;
            if (b == first)
                first = b->next;
            else if (p)
                p->next = b->next;
        }
    };

    const size_type buffer_block_size;
    buffer_list_type buffer_list;


    void dtor_() {
        // std::cout << this << " dtor" << std::endl ;

        // for ( buffer_type *b = buffer_list.first ; b ; ) {
        //  buffer_type *next = b->next ;
        //  free_buffer_(b) ;
        //  b = next ;
        //}
    }


    void alloc_buffer_(buffer_type* b) { b->buffer = new unsigned char[b->buffer_size]; }

    void free_buffer_(buffer_type* b) {
        if (b->buffer)
            delete b->buffer;
        b->buffer = 0, b->buffer_size = b->buffer_tail = 0;
    }


    pointer allocate_basic_(const size_type cnt, const typename X::const_pointer u = 0) {
        // std::cout << this << " allocating: " << cnt << std::endl ;

        // if the size of the object is big, then just allocate it
        if (cnt > buffer_block_size) {
            buffer_type* b = new buffer_type;
            b->buffer = new unsigned char[cnt];
            b->buffer_size = b->buffer_tail = cnt;
            b->block_count = 1;
            buffer_list.add_buffer(b);
            return pointer(b->buffer);
        }

        // search for appropriate buffer
        pointer p = 0;
        for (buffer_type* b = buffer_list.first; b; b = b->next) {
            if (cnt < size_type(b->buffer_size - b->buffer_tail)) {
                p = pointer(b->buffer + b->buffer_tail);
                b->buffer_tail += cnt;
                b->block_count++;
                break;
            }
        }

        // check if we could allocate buffer
        if (!p) {
            buffer_type* b = new buffer_type;
            b->buffer = new unsigned char[buffer_block_size];
            b->buffer_tail = cnt;
            b->buffer_size = buffer_block_size;
            b->block_count = 1;
            buffer_list.add_buffer(b);
            p = pointer(b->buffer);
        }


        return p;
    }

    void deallocate_basic_(const pointer ptr, const size_type cnt) {
        // std::cout << this << " deallocating " << cnt << " = " << ptr[0] << std::endl ;

        const unsigned char* p = (const unsigned char*)ptr;

        // search for the block
        for (buffer_type *b = buffer_list.first, *prev = 0; b; prev = b, b = b->next) {
            // check if the memory points to the specified block
            if (p >= b->buffer && p < b->buffer + b->buffer_size) {
                // free the current block
                if (b->block_count > 0)
                    b->block_count--;
                if ((p - b->buffer) + 1 == b->buffer_tail)
                    b->buffer_tail--;
                else if (cnt > 0 && (p - b->buffer) + cnt == b->buffer_tail)
                    b->buffer_tail -= cnt;
                if (b->block_count == 0) {
                    buffer_list.remove_buffer(b, prev);
                    free_buffer_(b);
                }
                break;
            }
        }
    }
};


} // namespace apl
