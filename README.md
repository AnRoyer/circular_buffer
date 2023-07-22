# circular_buffer (Work in progress)

circular_buffer is a C++20 library available as a header-only implementation, designed to incorporate a [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer) data structure.

## Installation

To integrate ``circular_buffer.hpp`` into your C++20 project, simply add the file to your project directory.

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

Member functions of ``anr::circular_buffer`` are ``constexpr`: it is possible to create and use ``anr::circular_buffer`` objects in the evaluation of a constant expression.

### Template parameters

| ``T``  | The type of the elements. ``T` must meet the requirements of CopyAssignable and CopyConstructible. |
| ``Allocator`` | 	An allocator that is used to acquire/release memory and to construct/destroy the elements in that memory. The type must meet the requirements of Allocator. The program is ill-formed if ``Allocator::value_type`` is not the same as ``T``. |

### Iterator invalidation

TODO

### Member types

| Member type | Definition |
| ----------- | ---------- |
| value_type | ``T`` | 
| allocator_type | ``Allocator`` | 
| size_type | Unsigned integer type (``std::size_t``) |
| difference_type | Signed integer type (``std::ptrdiff_t``) |
| reference | ``T&`` |
| const_reference | ``const T&`` |
| pointer | ``std::allocator_traits< Allocator >::pointer`` |
| const_pointer | ``std::allocator_traits< Allocator >::const_pointer`` |
| iterator | TODO |
| const_iterator | TODO |
| reverse_iterator | TODO |
| const_reverse_iterator | TODO |

### Member functions

#### Constructor

```c++
  constexpr explicit circular_buffer(const allocator_type& a = allocator_type()) noexcept; \\ (1)
  constexpr explicit circular_buffer(size_type count, const allocator_type& alloc = allocator_type()); \\ (2)
  constexpr explicit circular_buffer(size_type count, const_reference value, const allocator_type& alloc = allocator_type()); \\ (3)
  template< class InputIt >
  constexpr circular_buffer(InputIt first, InputIt last, const allocator_type& alloc = allocator_type()); \\ (4)
  constexpr circular_buffer(const circular_buffer& other); \\ (5)
  constexpr circular_buffer(const circular_buffer& other, const allocator_type& alloc); \\ (6)
  constexpr circular_buffer(circular_buffer&& other) noexcept; \\ (7)
  constexpr circular_buffer(circular_buffer&& other, const allocator_type& alloc) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value); \\ (8)
```

#### Standard functions
#### Element access
#### Iterators
#### Capacity
#### Modifiers

### Example

## Contributing

Community members are encouraged to submit pull requests; however, we kindly request that each pull request focus on a single feature or change. In case of substantial modifications, it is recommended to begin by opening an issue. This approach facilitates constructive discussions about the proposed alterations before proceeding with the implementation.

Furthermore, when contributing changes, it is essential to update the relevant tests to ensure the library retains its integrity and functionality. Your cooperation in this matter is highly appreciated.

## License

Licensed under the MIT License. See the [LICENSE](https://github.com/AnRoyer/circular_buffer/blob/main/LICENSE.md) file for details.
