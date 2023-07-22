# circular_buffer (Work in progress)

![GitHub](https://img.shields.io/github/license/AnRoyer/circular_buffer) ![GitHub](https://img.shields.io/github/license/AnRoyer/circular_buffer)


circular_buffer is a C++20 library available as a header-only implementation, designed to incorporate a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) data structure.

## Installation

To integrate `circular_buffer.hpp` into your C++20 project, simply add the file to your project directory.

## Usage

The usage is pretty straighforward and follow the standard library style.

```c++
  template< 
    class T, 
    class Allocator = std::allocator<T> 
    > class circular_buffer;
```

The circular buffer stores its elements in a fixed-size contiguous array. This implies that the array size does not automatically increase. Elements are arranged such that the first added element occupies the initial position of the array. Subsequent elements are added to the next available position, as long as the buffer is not already full. When the buffer reaches its capacity, new elements replace the oldest one in a circular fashion.

Regarding the efficiency of common operations on the circular buffer:

* Random access has a constant complexity of O(1), meaning that accessing any element by its index is a fast operation, regardless of the buffer size.
* Insertion of elements at the end of the buffer also has a constant complexity of O(1). This ensures that adding elements from the buffer's tail is an efficient operation, irrespective of the buffer's current size.
In summary, the circular buffer provides constant-time performance for random access and end-based insertions or removals, making it an efficient data structure for managing elements in a circular manner.

Member functions of `anr::circular_buffer` are `constexpr`: it is possible to create and use `anr::circular_buffer` objects in the evaluation of a constant expression.

### Template parameters

| Template parameters  | Description |
| -------------------- | ----------- |
| `T`  | The type of the elements. `T` must meet the requirements of CopyAssignable and CopyConstructible. |
| `Allocator` | 	An allocator that is used to acquire/release memory and to construct/destroy the elements in that memory. The type must meet the requirements of Allocator. The program is ill-formed if `Allocator::value_type` is not the same as `T`. |

### Iterator invalidation

| Operations | Invalidated |
| ---------- | ----------- |
| All read only operations | Never |
| `swap`, `std::swap`, `resize` | `end()` |
| `clear`, `operator=` | Always |
| `reserve` | Never |
| `reserve`, `shrink_to_fit` | Never |
| `push_back`, `emplace_back` | If the circular buffer is not full, i.e. if `size() < capacity()`, `end()`is invalidated. |

### Member types

| Member type | Definition |
| ----------- | ---------- |
| value_type | `T` | 
| allocator_type | `Allocator` | 
| size_type | Unsigned integer type (`std::size_t`) |
| difference_type | Signed integer type (`std::ptrdiff_t`) |
| reference | `T&` |
| const_reference | `const T&` |
| pointer | `std::allocator_traits< Allocator >::pointer` |
| const_pointer | `std::allocator_traits< Allocator >::const_pointer` |
| iterator | LegacyRandomAccessIterator to `value_type` |
| const_iterator | LegacyRandomAccessIterator to `const value_type` |
| reverse_iterator | `std::reverse_iterator<iterator>` |
| const_reverse_iterator | `std::reverse_iterator<const_iterator>` |

### Member functions

#### Constructor

```c++
  /* (1) */ constexpr explicit circular_buffer() noexcept(noexcept(allocator_type()));
  /* (2) */ constexpr explicit circular_buffer(const allocator_type& a) noexcept;
  /* (3) */ constexpr explicit circular_buffer(size_type count, const allocator_type& alloc = allocator_type());
  /* (4) */ constexpr explicit circular_buffer(size_type count, const_reference value, const allocator_type& alloc = allocator_type());
  /*******/ template< class InputIt >
  /* (5) */ constexpr circular_buffer(InputIt first, InputIt last, const allocator_type& alloc = allocator_type());
  /* (6) */ constexpr circular_buffer(const circular_buffer& other);
  /* (7) */ constexpr circular_buffer(const circular_buffer& other, const allocator_type& alloc);
  /* (8) */ constexpr circular_buffer(circular_buffer&& other) noexcept;
  /* (9) */ constexpr circular_buffer(circular_buffer&& other, const allocator_type& alloc) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value);
```

1. The default constructor initializes an empty container using a default-constructed allocator.
1. An empty container is constructed with the specified allocator provided as an argument.
1. The container is constructed with a size of `count`, containing default-constructed elements.
1. The container is constructed with a size of `count`, where each element is a copy of the provided `value`.
1. The container is constructed by copying the elements from the range `[first, last)`.
1. The copy constructor creates a new container as a copy of the contents of another existing container (`other`).
1. Using the provided allocator, the copy constructor creates a new container as a copy of the contents of another container (`other`).
1. The move constructor constructs a new container, taking ownership of the contents of other using move semantics. The allocator is obtained through move-construction from the allocator of `other`. After the move, other will be guaranteed to be empty.
1. The allocator-extended move constructor constructs a new container using the provided `alloc` allocator and moves the contents from `other`. If `alloc != other.get_allocator()`, elements are moved individually. In this case, other may not be guaranteed to be empty after the move operation.

#### Standard functions

```c++
  /* (1) */ constexpr circular_buffer& operator=(const circular_buffer& other);
  /* (2) */ constexpr circular_buffer& operator=(circular_buffer&& other) noexcept(std::allocator_traits< allocator_type >::propagate_on_container_move_assignment::value || std::allocator_traits<allocator_type>::is_always_equal::value);
```

1. The copy assignment operator replaces the current container's contents with a copy of the contents from another container `other`.
  If `std::allocator_traits<allocator_type>::propagate_on_container_copy_assignment::value` evaluates to `true`, the allocator of the current container (`*this`) is replaced with a copy of the allocator from `other`. If the new allocator differs from the old one, the original allocator is used to deallocate the memory, and then the new allocator is employed to allocate memory for copying the elements. However, if the allocators compare equal, memory owned by `*this` may be reused whenever possible. In any case, the elements originally present in `*this` may be either destroyed or replaced by element-wise copy-assignment.

1. The move assignment operator replaces the current container's contents with the contents from another container `other`, utilizing move semantics (i.e., moving the data from `other` into `*this`).
  If `std::allocator_traits<allocator_type>::propagate_on_container_move_assignment::value` is `true`, the allocator of the current container (`*this`) is replaced by a copy of the allocator from `other`. If it is `false`, and the allocators of `*this` and `other` differ, `*this` cannot directly take ownership of the memory managed by `other`. Instead, each element must be move-assigned individually, allocating additional memory using its own allocator when necessary. In any case, all elements originally present in `*this` are either destroyed or replaced by element-wise move-assignment. After the move assignment, the state of other is valid but unspecified.


```c++
  constexpr allocator_type get_allocator() const noexcept;
```

This function returns the allocator linked to the container.

#### Element access

```c++
  constexpr reference at(size_type pos);
  constexpr const_reference at(size_type pos) const
```

These functions provide access to the element at the specified position `pos` in the container, performing bounds checking. If `pos` falls outside the valid range of the container (`pos >= size`), the function throws an exception of type `std::out_of_range`. Otherwise, it returns a reference to the element at the given position.


```c++
  constexpr reference operator[](size_type pos) noexcept;
  constexpr const_reference operator[](size_type pos) const noexcept;
```

These functions allow direct access to the element at the specified position `pos` without performing any bounds checking.


```c++
  constexpr reference front();
  constexpr const_reference front() const;
```

These functions return a reference to the first element in the container, which corresponds to the newest element added. However, calling `front` on an empty container results in undefined behavior.


```c++
  constexpr reference back();
  constexpr const_reference back() const;
```

These functions return a reference to the last element in the container, which corresponds to the oldest element added. However, calling back` on an empty container results in undefined behavior.


```c++
  constexpr pointer data();
  constexpr const_pointer data() const;
```

These functions return a pointer to the underlying array that serves as the storage for the elements. The pointer is set such that the range `[data(), data() + size())` is always valid, even if the container is empty. However, it's essential to note that when the container is empty, the `data()` pointer is not dereferenceable, meaning that trying to access the value it points to in this case would result in undefined behavior.

#### Iterators

```c++
  constexpr iterator begin() noexcept;
  constexpr const_iterator begin() const noexcept;
  constexpr const_iterator cbegin() const noexcept;
```

These functions return an iterator pointing to the first element of the circular buffer (the newest one). If the circular buffer is empty, the returned iterator will be equal to the `end()` iterator.


```c++
  constexpr iterator end() noexcept;
  constexpr const_iterator end() const noexcept;
  constexpr const_iterator cend() const noexcept;
```

These functions return an iterator pointing to the last element of the circular buffer (the oldest one). If the circular buffer is empty, the returned iterator will be equal to the `begin()` iterator.


```c++
  constexpr reverse_iterator rbegin() noexcept;
  constexpr const_reverse_iterator rbegin() const noexcept;
  constexpr const_reverse_iterator crbegin() const noexcept;
```

These functions return a reverse iterator pointing to the first element of the reversed circular buffer (the oldest one). If the circular buffer is empty, the returned iterator will be equal to the `rend()` iterator.


```c++
  constexpr reverse_iterator rend() noexcept;
  constexpr const_reverse_iterator rend() const noexcept;
  constexpr const_reverse_iterator crend() const noexcept;
```

These functions return a reverse iterator pointing to the last element of the reversed circular buffer (the newest one). If the circular buffer is empty, the returned iterator will be equal to the `rbegin()` iterator.

#### Capacity

```c++
  [[nodiscard]] constexpr bool empty() const noexcept;
```

This function checks whether the container has no elements (`begin() == end()`).


```c++
  constexpr size_type size() const noexcept;
```

This function returns the number of elements present in the container (`std::distance(begin(), end())`).


```c++
  constexpr size_type max_size() const noexcept;
```

This function returns the maximum number of elements that the container can hold, limited by system or library implementation constraints.


```c++
  constexpr void reserve(size_type new_cap);
```

The `reserve()` function allows increasing the capacity of a circular buffer, which refers to the total number of elements it can hold. If `new_cap` exceeds the current `capacity()`, the function allocates new storage to accommodate the increased capacity. However, if `new_cap` is less than or equal to the current `capacity()`, the function has no effect.

It's important to note that `reserve()` does not alter the size of the circular buffer; it merely ensures that enough space is available to accommodate future elements.

If `new_cap` is greater than the current `capacity(`), all iterators, including the `end()` iterator, and all references to elements in the ciruclar buffer are invalidated. This means that any existing iterators or references should not be used after a call to `reserve()` in this case. However, if `new_cap` is less than or equal to the current `capacity()`, iterators and references remain valid.


```c++
  constexpr size_type capacity() const noexcept;
```

The function returns the current number of elements for which the container has allocated space.


```c++
  constexpr void shrink_to_fit();
```

The primary purpose of this function is to decrease the number of elements that the circular buffer can store.

The function requests the removal of any unused capacity in the circular buffer to match its current `size()`. All iterators, including the past-the-end iterator, and all references to elements in the circular buffer are invalidated. Existing iterators and references should not be used anymore.

#### Modifiers

```c++
  constexpr void clear() noexcept;
```

The function erases all elements from the container, resetting its size to zero. As a consequence, the `size()` function will return zero after this operation.

Moreover, this operation invalidates any references, pointers, or iterators that were referring to the elements contained in the container. Even past-the-end iterators become invalidated.

It is important to note that despite removing all elements, the `capacity()` of the circular buffer remains unchanged. This means that the memory allocated to the circular buffer is not released or reduced; it still retains the same capacity as before.


```c++
  /* (1) */ constexpr void push_back(const_reference value);
  /* (2) */ constexpr void push_back(T&& value);
```

The function appends the provided element value to the end of the container using either of the following approaches:

1. The new element is initialized as a copy of the given value.
1. The value is moved into the new element

If the new `size()` exceeds the current `capacity()`, the oldest element is replaced by the appended one. If the new `size()` is within the current capacity, the element is simply added to the end, but the `end()` iterator is invalidated.


```c++
  template< class... Args >
  constexpr reference emplace_back(Args&&... args);
```

This function appends a new element to the end of the container. The element is constructed in-place at the provided location within the container.

Furthermore, the function allows forwarding arguments `args...` to the constructor using `std::forward<Args>(args)...`, which means `args...` can directly or indirectly refer to values already present in the container.

Regarding iterator and reference validity, if the new `size()` after appending the element exceeds the current `capacity()`, all iterators and references remain valid. However, if the new `size()` remains within the current capacity, only the `begin()` iterator is invalidated.


```c++
  /* (1) */ constexpr void resize(size_type count);
  /* (2) */ constexpr void resize(size_type count, const_reference value);
```

This function resizes the container to contain `count` elements. If `count` is equal to the current size, the function does nothing.

If the current size is greater than `count`, the container is reduced to its first `count` elements by removing the excess elements from the end of the container.

On the other hand, if the current size is less than `count`, the function performs the following:

1. It appends additional default-inserted elements to the container.
1. It appends additional copies of the specified `value` to the container.


```c++
  constexpr void swap(circular_buffer& other) noexcept(std::allocator_traits< allocator_type >::propagate_on_container_swap::value || std::allocator_traits< allocator_type >::is_always_equal::value);
```

This function swaps the contents and capacity of the container with another container called `other`. It performs this exchange without invoking any move, copy, or swap operations on individual elements within the containers. In other words, the operation solely involves exchanging the entire content and memory allocation between the two containers.

### Example

```c++
  anr::circular_buffer<int> buffer;
    
  buffer.reserve(5);

  for(auto i = 0; i < 4; ++i) {
    buffer.push_back(i);
  }
  
  std::cout << "Buffer size: " << buffer.size() << std::endl;
  std::cout << "Buffer capacity: " <<  buffer.capacity() << std::endl;

  std::cout << "Buffer values: ";
  for(auto it : buffer) {
    std::cout << it << " ";
  }
  std::cout << std::endl;
  
  buffer.clear();
  
  std::cout << "Buffer size: " << buffer.size() << std::endl;
  std::cout << "Buffer capacity: " <<  buffer.capacity() << std::endl;
  
  buffer.resize(10, 2);
  
  std::cout << "Buffer values: ";
  for(auto it : buffer) {
    std::cout << it << " ";
  }
  std::cout << std::endl;
```

Output:
```bash
  Buffer size: 4
  Buffer capacity: 5
  Buffer values: 3 2 1 0 
  Buffer size: 0
  Buffer capacity: 5
  Buffer values: 2 2 2 2 2 2 2 2 2 2 
```

## Contributing

Community members are encouraged to submit pull requests; however, we kindly request that each pull request focus on a single feature or change. In case of substantial modifications, it is recommended to begin by opening an issue. This approach facilitates constructive discussions about the proposed alterations before proceeding with the implementation.

Furthermore, when contributing changes, it is essential to update the relevant tests to ensure the library retains its integrity and functionality. Your cooperation in this matter is highly appreciated.

## License

Licensed under the MIT License. See the [LICENSE](https://github.com/AnRoyer/circular_buffer/blob/main/LICENSE.md) file for details.
