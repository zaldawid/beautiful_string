/*
MIT License

Copyright (c) 2022 Dawid Zalewski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Changelog:

 2022-03-26: Initial version, not much here.
 2022-03-27: Fixes to CMakeLists and making bt a library

 */

#ifndef BEAUTIFUL_TEXT_BSTRING_H
#define BEAUTIFUL_TEXT_BSTRING_H

#include <cstdio>
#include <cstring>
#include <memory>
#include <iostream>
#include <concepts>
#include <bit>

namespace bt {

#include <cstdint>
    
    namespace detail{
    }


#define DP() printf("%s\n", __PRETTY_FUNCTION__)

//    template <typename StorageType, typename SizeType>
//    requires std::integral<StorageType> && std::unsigned_integral<SizeType>
//    struct dynamic_store {
//        using storage_type = StorageType;
//        using size_type = SizeType;
//
//        // allocated storage in bytes
//        size_type m_capacity;
//        // length in code points (- '\0')
//        size_type m_length;
//        // used storage in bytes
//        size_type m_size;
//
//        storage_type *m_str;
//    };
//
//    template <typename StorageType, typename SizeType>
//    requires std::integral<StorageType> && std::unsigned_integral<SizeType>
//    struct static_store {
//        using storage_type = StorageType;
//        using size_type = SizeType;
//
//        constexpr static size_type s_buffer_capacity = ( sizeof(dynamic_store<StorageType, SizeType>) - 1 ) / sizeof(StorageType);
//
//        union {
//            struct {
//                std::uint8_t m_size;
//                std::uint8_t m_length;
//            } m;
//            [[maybe_unused]]
//            StorageType _m_dummy;
//        } m;
//        StorageType m_buffer[s_buffer_capacity];
//    };



//    template <typename StorageType, typename SizeType>
//    requires std::integral<StorageType> && std::unsigned_integral<SizeType>
//    union store {
//        using size_type = SizeType;
//        using static_size_type = std::uint8_t;
//        dynamic_store<StorageType, SizeType> m_dynamic;
//        static_store<StorageType, SizeType> m_static;
//
//        // dynamic capacity is always even
//        constexpr static std::uint8_t s_static_mask{ std::endian::native == std::endian::little? 0x01 : 0x80 };
//        constexpr static size_type s_dynamic_mask { std::endian::native == std::endian::little? size_type{0x01} : ((~size_type{0}) >> 1) };
//
//        constexpr bool is_dynamic() const {
//            return m_static.m.m.m_size & s_static_mask;
//        }
//
//        constexpr bool is_static() const {
//            return !(m_static.m.m.m_size & s_static_mask);
//        }
//
//        constexpr void set_static_size(size_type sz) {
//            m_static.m.m.m_size = static_size_type(sz << 1);
//        }
//
//        constexpr size_type get_static_size() const {
//            return size_type (m_static.m.m.m_size >> 1);
//        }
//
//        constexpr void set_dynamic_size(size_type sz){
//            m_dynamic.m_size = sz;
//        }
//
//        constexpr size_type get_dynamic_size() const {
//            return m_dynamic.m_size;
//        }
//
//        constexpr size_type get_size() const {
//            return is_static() ? get_static_size() : get_dynamic_size();
//        }
//
//        constexpr void set_dynamic_capacity(size_type cap){
//            m_dynamic.m_capacity = cap | s_dynamic_mask;
//        }
//
//        constexpr size_type get_dynamic_capacity() const {
//            return m_dynamic.m_capacity & ~s_dynamic_mask;
//        }
//
//        constexpr size_type get_capacity() const {
//            return is_static() ? m_static.s_buffer_capacity : get_dynamic_capacity();
//        }
//    };

    template <typename Storage_type=char, typename Allocator=typename std::allocator<Storage_type>, typename Size_type = std::size_t>
    struct string_base{
    public:
        using storage_type = Storage_type;
        using size_type = Size_type;
        using allocator_type = Allocator;
        using traits = std::allocator_traits<allocator_type>;

        using pointer = Storage_type*;
        using const_pointer = const Storage_type*;

        constexpr string_base(size_type size, const Allocator& allocator) noexcept:
            m_allocator{ allocator },
            m_size{ size },
            m_str{traits::allocate(m_allocator, size) }
        {}

        explicit constexpr string_base(size_type size) noexcept(noexcept ( Allocator{} )):
            string_base{ size, Allocator{} }
        {}

        constexpr ~string_base() noexcept{
            traits::deallocate(m_allocator, m_str, m_size );
        }

        constexpr string_base(const string_base& other):
            m_allocator{ traits::select_on_container_copy_construction(other.m_allocator) },
            m_size{ other.m_size },
            m_str{traits::allocate(m_allocator, m_size) }
        {
            assign_from(other.m_str, m_size);
        }

        constexpr string_base& operator=(const string_base& other){
            if (this == &other) {
                return *this;
            }

            // TODO: check and clean-up mess
            if constexpr (traits::propagate_on_container_copy_assignment){
                if (m_allocator != other.m_allocator){
                    m_allocator.deallocate(m_str, m_size);

                    m_allocator = traits::select_on_container_copy_construction(other.m_allocator);
                    m_size = other.m_size;
                    m_str =  traits::allocate(m_allocator, m_size);
                }
                else if (m_size != other.m_size){
                    m_allocator.deallocate(m_str, m_size);
                    m_size = other.m_size;
                    m_str =  traits::allocate(m_allocator, m_size);
                }
            }
            else{
                // we don't care about allocators, just copy things
                if (m_size != other.m_size){
                    m_allocator.deallocate(m_str, m_size);
                    m_size = other.m_size;
                    m_str =  traits::allocate(m_allocator, m_size);
                }
            }

            assign_from(other.m_str, m_size);
            return *this;
        }

        constexpr string_base(string_base&& other) noexcept:
            m_allocator{ std::move(other.m_allocator) },
            m_size{other.m_size },
            m_str{ other.m_str }
        {
            other.m_size = 0;
            other.m_str = nullptr;
        }

        constexpr string_base& operator=(string_base&& other) noexcept {
            if (this == std::addressof(other)){
                return *this;
            }

            // if must propagate on move assignment
            if constexpr (traits::propagate_on_container_move_assignment){
                traits::deallocate(m_allocator, m_str, m_size);
                m_allocator = std::move(other.m_allocator);
                m_str = other.m_str;
                m_size = other.m_size;
                other.m_size = 0;
                other.m_str = nullptr;
            }
            else{
                // allocators are the same
                if (m_allocator == other.m_allocator){
                    traits::deallocate(m_allocator, m_str, m_size);
                    m_str = other.m_str;
                    m_size = other.m_size;
                    other.m_size = 0;
                    other.m_str = nullptr;

                }else{
                    if (m_size != other.m_size) {
                        // the bad case - resources cannot be stolen directly
                        traits::deallocate(m_allocator, m_str, m_size);
                        m_size = other.m_size;
                        traits::allocate(m_allocator, m_size);
                    }
                    assign_from(other.m_str, m_size);

                    other.clear();
                }
            }

            return *this;
        }

        template <typename _StorageType=storage_type>
        constexpr void assign_from(const _StorageType* source, size_type count){
            std::memcpy(m_str, source, count);
        }

        constexpr void assign_from(const storage_type* begin, const storage_type* end){
            std::memcpy(m_str, begin, sizeof(storage_type) * std::distance(begin, end));
        }

        template <typename _Iterator>
        constexpr void assign_from(_Iterator begin, _Iterator end){
            for(auto dest{m_str}; begin!=end; ++begin){
                *dest++ = *begin;
            }
        }

        constexpr size_type size() const {
            return m_size;
        }

        constexpr const_pointer data() const {
            return m_str;
        }

        constexpr pointer data() {
            return m_str;
        }

        constexpr void clear() noexcept{
            traits::deallocate(m_allocator, m_str, m_size);
            m_str  = nullptr;
            m_size = 0;
        }

        [[no_unique_address]]
        allocator_type m_allocator;

        size_type m_size;
        storage_type *m_str;
    };

    template <typename Storage_type=char, typename Allocator=typename std::allocator<Storage_type>, typename Size_type = std::size_t>
    class string: private string_base<Storage_type, Allocator, Size_type> {
    public:

        using storage_type = Storage_type;
        using pointer = Storage_type*;
        using const_pointer = const Storage_type*;
        using reference = Storage_type&;
        using const_reference = const Storage_type&;
        using size_type = Size_type;
        using allocator_type = Allocator;

        using string_base<Storage_type, Allocator, Size_type>::data;

        constexpr string() noexcept(noexcept( Allocator() )) :
            string(Allocator() )
            {}

        explicit constexpr string(const Allocator& alloc) noexcept:
            string("", alloc)
            {}

        constexpr string(const char* str, const Allocator& allocator = Allocator{}):
            _base{ std::strlen(str) + 1, allocator }{
            _base::template assign_from<char>(str, _base::size());
        }

    private:
        using _base = string_base<Storage_type, Allocator, Size_type>;
    };

    string(char*) -> string<char>;


}
#endif //BEAUTIFUL_TEXT_BSTRING_H
