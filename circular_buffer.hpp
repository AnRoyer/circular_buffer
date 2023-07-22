// Implementation of a circular buffer for C+20
// version 1.0.0
// https://github.com/AnRoyer/circular_buffer
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2023 Anthony Royer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

// see https://semver.org/
#define CIRCULAR_BUFFER_VERSION_MAJOR 1 // for incompatible API changes
#define CIRCULAR_BUFFER_VERSION_MINOR 0 // for adding functionality in a backwards-compatible manner
#define CIRCULAR_BUFFER_VERSION_PATCH 0 // for backwards-compatible bug fixes

#include <cstdio>
#include <memory>

namespace anr
{

  template< class T, class Allocator = std::allocator<T> >
  class circular_buffer
  {
   public:
    template< class Type >
    class circular_buffer_iterator;
    
    typedef T                                                          value_type;
    typedef Allocator                                                  allocator_type;
    typedef std::size_t                                                size_type;
    typedef std::ptrdiff_t                                             difference_type;
    typedef T&                                                         reference;
    typedef const T&                                                   const_reference;
    typedef typename std::allocator_traits< Allocator >::pointer       pointer;
    typedef typename std::allocator_traits< Allocator >::const_pointer const_pointer;
    typedef circular_buffer_iterator<T>                                iterator;
    typedef circular_buffer_iterator<const T>                          const_iterator;
    typedef std::reverse_iterator< iterator >                          reverse_iterator;
    typedef std::reverse_iterator< const_iterator >                    const_reverse_iterator;
    
    
   private:
    allocator_type _allocator;
    pointer _buffer;
    size_type _index;
    size_type _capacity;
    size_type _size;
    
    static void check_length_error(size_type new_cap, size_type max_size)
    {
      if(new_cap > max_size) {
        char buffer[256];
        std::snprintf(buffer, 256, "The new size %lu exceeds the circular buffer max size (%lu)", new_cap, max_size);
        throw std::length_error(buffer);
      }
    }
    
    void _set_invalid()
    {
      _buffer = nullptr;
      _index = 0;
      _capacity = 0;
      _size = 0;
    }
    
    void _deallocate()
    {
      if(_buffer) {
        _allocator.deallocate(_buffer, _capacity);
      }
      _set_invalid();
    }
    
    void _reallocate(size_type new_cap)
    {
      if(_capacity == new_cap) {
        return;
      }
      
      const size_type newSize = _size < new_cap ? _size : new_cap;
      pointer newBuffer = _allocator.allocate(new_cap);
      
      for(size_type i = 0; i < newSize; ++i) {
        const size_type pos = newSize - i - 1;
        std::construct_at(&newBuffer[pos], operator[](i));
      }
      
      clear();
      _deallocate();
      
      _buffer = newBuffer;
      _index = newSize-1;
      _capacity = new_cap;
      _size = newSize;
    }
    
    void _construct(size_type pos, const_reference value = value_type())
    {
      std::construct_at(&_buffer[pos], value);
    }
    
    void _construct(size_type pos, T&& value)
    {
      std::construct_at(&_buffer[pos], std::forward< T >(value));
    }
    

   public:
    constexpr explicit circular_buffer(const allocator_type& a = allocator_type()) noexcept
      : _allocator(a)
      , _buffer()
      , _index(max_size())
      , _size(0)
      , _capacity(0)
    {
    }
    
    constexpr explicit circular_buffer(size_type count, const allocator_type& alloc = allocator_type())
      : _allocator(alloc)
      , _buffer(_allocator.allocate(count))
      , _index(count-1)
      , _size(count)
      , _capacity(count)
    {
    }
    
    constexpr explicit circular_buffer(size_type count, const_reference value, const allocator_type& alloc = allocator_type())
      : _allocator(alloc)
      , _buffer(_allocator.allocate(count))
      , _index(count-1)
      , _size(count)
      , _capacity(count)
    {
      for(size_type i = 0; i < count; ++i) {
        _construct(i, value);
      }
    }
    
    template< class InputIt >
    constexpr circular_buffer(InputIt first, InputIt last, const allocator_type& alloc = allocator_type())
      : _allocator(alloc)
      , _buffer(nullptr)
      , _index(std::distance(first, last))
      , _size(_index+1)
      , _capacity(_size)
    {
      _buffer = _allocator.allocate(_capacity);
      size_type i = 0;
      for(const auto& it = first; it != last; ++it) {
        _construct(i, *it);
        i++;
      }
    }
    
    constexpr circular_buffer(const circular_buffer& other)
      : _allocator(other._allocator)
      , _buffer(_allocator.allocate(other._size))
      , _index(other._index)
      , _size(other._size)
      , _capacity(other._capacity)
    {
      for(size_type i = 0; i < _size; ++i) {
        _construct(i, other._buffer[i]);
      }
    }
    
    constexpr circular_buffer(const circular_buffer& other, const allocator_type& alloc)
      : _allocator(alloc)
      , _buffer(_allocator.allocate(other._size))
      , _index(other._index)
      , _size(other._size)
      , _capacity(other._capacity)
    {
      for(size_type i = 0; i < _size; ++i) {
        _construct(i, other._buffer[i]);
      }
    }
    
    constexpr circular_buffer(circular_buffer&& other) noexcept
      : _allocator(other._allocator)
      , _buffer(other._buffer)
      , _index(other._index)
      , _size(other._size)
      , _capacity(other._capacity)
    {
      other._set_invalid();
    }
    
    constexpr circular_buffer(circular_buffer&& other, const allocator_type& alloc) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value)
      : _allocator(alloc)
      , _buffer(nullptr)
      , _index(other._index)
      , _size(other._size)
      , _capacity(other._capacity)
    {
      if(_allocator == other._allocator) {
        _buffer = other._buffer;
        other._set_invalid();
      }
      else {
        _buffer = _allocator.allocate(_capacity);
        for(size_type i = 0; i < _size; ++i) {
          _construct(i, std::move(other._buffer[i]));
        }
      }
    }
    
    constexpr circular_buffer& operator=(const circular_buffer& other)
    {
      clear();
      _deallocate();
      
      if constexpr(std::allocator_traits< allocator_type >::propagate_on_container_copy_assignment::value) {
        _allocator = other._allocator;
      }
      _capacity = other._capacity;
      _buffer = _allocator.allocate(_capacity);
      _index = other._index;
      _size = other._size;
      for(size_type i = 0; i < _size; ++i) {
        _construct(i, other._buffer[i]);
      }
      
      return *this;
    }
    
    constexpr circular_buffer& operator=(circular_buffer&& other) noexcept(std::allocator_traits< allocator_type >::propagate_on_container_move_assignment::value || std::allocator_traits<allocator_type>::is_always_equal::value)
    {
      clear();
      _deallocate();
      
      if constexpr(std::allocator_traits< allocator_type >::propagate_on_container_move_assignment::value) {
        _allocator = other._allocator;
      }
      if(_allocator == other._allocator) {
        _buffer = other._buffer;
        _index = other._index;
        _size = other._size;
        _capacity = other._capacity;
        other._set_invalid();
      }
      else {
        _capacity = other._capacity;
        _buffer = _allocator.allocate(_capacity);
        _index = other._index;
        _size = other._size;
        for(size_type i = 0; i < _size; ++i) {
          _construct(i, std::move(other._buffer[i]));
        }
      }
      
      return *this;
    }
    
    constexpr ~circular_buffer()
    {
      clear();
      _deallocate();
    }
    
    constexpr allocator_type get_allocator() const noexcept
    {
      return _allocator;
    }
    
    static void check_out_of_range(size_type pos, size_type size)
    {
      if(pos >= size) {
        char buffer[256];
        std::snprintf(buffer, 256, "The position %lu exceeds the circular buffer size (%lu)", pos, size);
        throw std::out_of_range(buffer);
      }
    }
    
    constexpr reference at(size_type pos)
    {
      check_out_of_range(pos, _size);
      assert(( ((_capacity != _size) && (_index == _size-1)) || (_capacity == _size) ));
      return _buffer[(_index + _capacity - pos) % _capacity];
    }
    
    constexpr const_reference at(size_type pos) const
    {
      check_out_of_range(pos, _size);
      assert(( ((_capacity != _size) && (_index == _size-1)) || (_capacity == _size) ));
      return _buffer[(_index + _capacity - pos) % _capacity];
    }
    
    constexpr reference operator[](size_type pos) noexcept
    {
      assert(( ((_capacity != _size) && (_index == _size-1)) || (_capacity == _size) ));
      return _buffer[(_index + _capacity - pos) % _capacity];
    }
    
    constexpr const_reference operator[](size_type pos) const noexcept
    {
      assert(( ((_capacity != _size) && (_index == _size-1)) || (_capacity == _size) ));
      return _buffer[(_index + _capacity - pos) % _capacity];
    }
    
    constexpr reference front()
    {
      return _buffer[_index];
    }
    
    constexpr const_reference front() const
    {
      return _buffer[_index];
    }
    
    constexpr reference back()
    {
      return operator[](_size-1);
    }
    
    constexpr const_reference back() const
    {
      return operator[](_size-1);
    }
    
    constexpr pointer data()
    {
      return &_buffer[0];
    }
    
    constexpr const_pointer data() const
    {
      return &_buffer[0];
    }
      
    constexpr iterator begin() noexcept
    {
      return iterator(*this, 0);
    }
    
    constexpr const_iterator begin() const noexcept
    {
      return cbegin();
    }
    
    constexpr const_iterator cbegin() const noexcept
    {
      return const_iterator(*this, 0);
    }
    
    constexpr iterator end() noexcept
    {
      return iterator(*this, _size);
    }
    
    constexpr const_iterator end() const noexcept
    {
      return cend();
    }
    
    constexpr const_iterator cend() const noexcept
    {
      return const_iterator(*this, _size);
    }
    
    constexpr reverse_iterator rbegin() noexcept
    {
      return std::make_reverse_iterator(end());
    }
    
    constexpr const_reverse_iterator rbegin() const noexcept
    {
      return crbegin();
    }
    
    constexpr const_reverse_iterator crbegin() const noexcept
    {
      return std::make_reverse_iterator(cend());
    }
    
    constexpr reverse_iterator rend() noexcept
    {
      return std::make_reverse_iterator(begin());
    }
    
    constexpr const_reverse_iterator rend() const noexcept
    {
      return crend();
    }
    
    constexpr const_reverse_iterator crend() const noexcept
    {
      return std::make_reverse_iterator(cbegin());
    }
    
    [[nodiscard]] constexpr bool empty() const noexcept
    {
      return _size == 0;
    }
    
    constexpr size_type size() const noexcept
    {
      return _size;
    }
    
    constexpr size_type max_size() const noexcept
    {
      return std::numeric_limits< size_type >::max();
    }
    
    constexpr void reserve(size_type new_cap)
    {
      check_length_error(new_cap, max_size());
      _reallocate(new_cap);
    }
    
    constexpr size_type capacity() const noexcept
    {
      return _capacity;
    }
    
    constexpr void shrink_to_fit()
    {
      _reallocate(_size);
    }
    
    constexpr void clear() noexcept
    {
      if constexpr(std::is_destructible_v<value_type> && !std::is_trivially_destructible_v<value_type>) {
        for(size_type i = 0; i < _size; ++i) {
          std::destroy_at(&_buffer[i]);
        }
      }
      _index = 0;
      _size = 0;
    }
    
    constexpr void push_back(const_reference value)
    {
      _index = (_index + 1) % _capacity;
      _construct(_index, value);
      if(_size != _capacity) {
        _size ++;
      }
    }
    
    constexpr void push_back(T&& value)
    {
      _index = (_index + 1) % _capacity;
      _construct(_index, std::move(value));
      if(_size != _capacity) {
        _size ++;
      }
    }
    
    template< class... Args >
    constexpr reference emplace(Args&&... args)
    {
      value_type value(std::forward< Args >(args)...);
      push(value);
      return *_buffer[_index];
    }
    
    constexpr void resize(size_type count)
    {
      resize(count, value_type());
    }
    
    constexpr void resize(size_type count, const_reference value)
    {
      _reallocate(count);
      for(size_type i = _index+1; i < _capacity; ++i) {
        _construct(i, value);
      }
      _size = _capacity;
      _index = _capacity-1;
    }
    
    constexpr void swap(circular_buffer& other) noexcept(std::allocator_traits< allocator_type >::propagate_on_container_swap::value || std::allocator_traits< allocator_type >::is_always_equal::value)
    {
      if(std::allocator_traits<allocator_type>::propagate_on_container_swap::value && _allocator == other._allocator) {
        std::swap(_allocator, other._allocator);
        std::swap(_buffer, other._buffer);
        std::swap(_index, other._index);
        std::swap(_capacity, other._capacity);
        std::swap(_size, other._size);
      }
    }

  };



  template< class T, class Allocator>
  template< class Type >
  class circular_buffer< T, Allocator>::circular_buffer_iterator
  {
   public:
    typedef std::ptrdiff_t                  difference_type;
    typedef std::remove_cv_t<Type>          value_type;
    typedef Type*                           pointer;
    typedef Type&                           reference;
    typedef std::random_access_iterator_tag iterator_category;
    
   private:
    size_type _offset;
    const circular_buffer< T, Allocator>& _parent;
    
    constexpr size_type _index(size_type offset) const
    {
      return (_parent._size - offset - 1) % _parent._capacity;
    }
    
    constexpr explicit circular_buffer_iterator(const circular_buffer< T, Allocator > &parent, size_type offset = 0) noexcept
      : _offset(offset)
      , _parent(parent)
    {
    }
    
   public:
    friend circular_buffer< T, Allocator>;
   
    constexpr circular_buffer_iterator(const circular_buffer_iterator& other) noexcept = default;
    constexpr circular_buffer_iterator(circular_buffer_iterator&& other) noexcept = default;
    constexpr circular_buffer_iterator& operator=(const circular_buffer_iterator& other) noexcept = default;
    constexpr circular_buffer_iterator& operator=(circular_buffer_iterator&& other) noexcept = default;
    
    ~circular_buffer_iterator() = default;
    
    constexpr reference operator*() noexcept
    {
      return _parent._buffer[_index(_offset)];
    }
    
    constexpr circular_buffer_iterator& operator++() noexcept
    {
      _offset ++;
      return *this;
    }
     
    constexpr circular_buffer_iterator& operator++(int) noexcept
    {
      circular_buffer_iterator tmp = *this;
      ++tmp;
      return tmp;
    }
    
    constexpr circular_buffer_iterator& operator--() noexcept
    {
      _offset --;
      return *this;
    }
    
    constexpr circular_buffer_iterator& operator--(int) noexcept
    {
      circular_buffer_iterator tmp = *this;
      --tmp;
      return tmp;
    }
    
    constexpr circular_buffer_iterator& operator+=(difference_type i) noexcept
    {
      _offset += i;
      return *this;
    }
    
    constexpr circular_buffer_iterator& operator-=(difference_type i) noexcept
    {
      _offset -= i;
      return *this;
    }
    
    constexpr circular_buffer_iterator operator+(difference_type i) const noexcept
    {
      circular_buffer_iterator tmp = *this;
      tmp += i;
      return tmp;
    }
    
    constexpr circular_buffer_iterator operator-(difference_type i) const noexcept
    {
      circular_buffer_iterator tmp = *this;
      tmp -= i;
      return tmp;
    }
    
    constexpr reference operator[](difference_type n) const noexcept
    {
      return _parent._buffer[_index(_offset + n)];
    }
    
    constexpr bool operator==(const circular_buffer_iterator& other) const noexcept
    {
      return _offset == other._offset;
    }
    
    constexpr bool operator!=(const circular_buffer_iterator& other) const noexcept
    {
      return _offset != other._offset;
    }
    
    constexpr bool operator<(const circular_buffer_iterator& other) const noexcept
    {
      return _offset > other._offset;
    }
    
    constexpr bool operator>(const circular_buffer_iterator& other) const noexcept
    {
      return _offset < other._offset;
    }
    
    constexpr bool operator<=(const circular_buffer_iterator& other) const noexcept
    {
      return _offset >= other._offset;
    }
    
    constexpr bool operator>=(const circular_buffer_iterator& other) const noexcept
    {
      return _offset <= other._offset;
    }
    
  };

}

#endif // CIRCULAR_BUFFER

