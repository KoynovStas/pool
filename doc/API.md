# Object Pool API documentation


## Description

`Pool` is C++17 object(memory) pool template with different implementations(algorithms)


**Template Pool has the following parameters:**
```C++
template <T, N, Align = alignof(T), Flags = 0, Impl = Pool_dlist>
```

 *  **T** - The type of the elements.
 *  **N**
    - The count of elemtnts in Static pool
    - The block size in Pool_xxx_block
    - Don't uses in Pool_xxx
 *  **Align** - The alignment of items in the pool
 *  **Flags** - The is an extended flags to tuning of implementation
 *  **Impl**  - This is a specific implementation(algorithm)

### Algorithms

**Static:**

| Name             | Short name | Info
|------------------|------------|------
|SPool_list        | SP_l       | Based on a singly-linked list
|SPool_list_bitset | SP_b       | Analogue of SPool_list, but we use [bitset](http://en.cppreference.com/w/cpp/utility/bitset) for the used nodes
|SPool_dlist       | SP_dl      | Based on an intrusive(nested) circular doubly-linked list


**Dynamic:**
| Name            | Short name | Info
|-----------------|------------|------
|Pool_list        | P_l        | Based on a singly-linked list
|Pool_dlist       | P_dl       | Based on an intrusive(nested) circular doubly-linked list
|Pool_list_block  | P_lb       | Analogue of Pool_list, but memory is allocated in blocks of N nodes
|Pool_dlist_block | P_dlb      | Analogue of Pool_dlist, but memory is allocated in blocks of N nodes


### Methods

The `size()`, `capacity()`, `empty()`, `full()`, `create()`, `destroy(T*)`
methods are named basic and are present in all implementations.
All basic methods have complexity is O(1)!

**Extended Methods:**
| method/Impl  | SP_l  | SP_b | SP_dl | P_l  | P_dl | P_lb | P_dlb
|--------------|-------|------|-------|------|------|------|------
| destroy(iter)|   -   | O(1) | O(1)  |  -   | O(1) |  -   | O(1)
| destroy(f, l)|   -   | O(N) | O(N)  |  -   | O(N) |  -   | O(N)
| destroy_all  | O(N^2)| O(N) | O(N)  |  -   | O(N) |  -   | O(N)
| for_each     | O(N^2)| O(N) | O(N)  |  -   | O(N) |  -   | O(N)
| reserve      |   -   |   -  |   -   | O(N) | O(N) | O(N) | O(N)
| shrink_to_fit|   -   |   -  |   -   | O(N) | O(N) | O(N) | O(N)
| constructor  | O(N)  | O(N) | O(N)  | O(1) | O(1) | O(1) | O(1)
| destructor   | O(N^2)| O(N) | O(N)  | O(N)*| O(N) | O(N)*| O(N)
| iterator     |   -   | Bid  | Bid   |  -   | Bid  |  -   | Bid

> Note:
> **\*** Pools `P_l`, `P_lb` don't store information about the nodes used.
Therefore, the user must ensure that all objects was be destroyed when
the destructor is called. Otherwise, it can lead to a memory leak.

---
Most of the basic methods are trivial and need not be described:

```C++
using value_type      = T;
using reference       = value_type&;
using pointer         = value_type*;
using const_reference = const value_type&;
using const_pointer   = const value_type*;
using size_type       = std::size_t;


static constexpr std::size_t  ALIGN   = Align;
static constexpr Pool_flags_t FLAGS   = Flags;
static constexpr std::size_t  N_VALUE = N;


constexpr std::size_t size()  const noexcept { return m_size;                      }
constexpr bool        empty() const noexcept { return size() == 0;                 }
constexpr bool        full()  const noexcept { return size() == impl().capacity(); }

//for Static Pool
static constexpr std::size_t capacity() noexcept { return N; }
//for Dynamic Pool
constexpr std::size_t capacity() const noexcept { return m_capacity; }
```


---
#### create:

```C++
template <typename... Args>
T* create(Args&&... args)
```

Create an item(object) in the pool and returns a pointer to it.
For an object, its constructor is called with arguments `Args`.
If the pool has no free items then a `nullptr` is returned.

For dynamic pool, method `create()` add a new node if there is no free one.
If flag `POOL_FIXED_CAPACITY` is set, new nodes are add only by the `reserve()` method.


**Exceptions**

 - Thrown [std::bad_alloc](https://en.cppreference.com/w/cpp/memory/new/bad_alloc) if can't allocate nodes and the `POOL_CREATE_EXCEPTION` flag is set.
 If an exception is thrown, this function has no effect ([strong exception guarantee](https://en.cppreference.com/w/cpp/language/exceptions)).
 - If an exception is thrown from constructor of object, this function has [basic exception guarantee](https://en.cppreference.com/w/cpp/language/exceptions).



---
#### destroy:

```C++
void destroy(const T* obj) noexcept                                  //1
iterator destroy(const_iterator pos) noexcept                        //2
iterator destroy(const_iterator first, const_iterator last) noexcept //3
```

Destroys the specified object in the pool(will call the destructor for the object).
The memory is occupied by the object remains in the pool and is ready to create a new element(object).

1) Destroys the `obj`.
2) Destroys the object at `pos`.
3) Destroys the objects in the range [`first`, `last`).

References and iterators to the destroyed elements are invalidated.
The iterator `pos` must be valid and dereferenceable.
Thus the `end()` iterator (which is valid, but is not dereferencable) cannot be used as a value for `pos`.

The iterator `first` does not need to be dereferenceable if `first`==`last`: erasing an empty range is a no-op.

**Return value:**

 - 1 `void`
 - 2-3 `Iterator` following the last removed element. If the iterator `pos` refers to the last element, the `end()` iterator is returned.


 ---
 #### destroy_all:

 ```C++
void destroy_all() noexcept
 ```

Destroys all elements from the pool. After this call, `size()` returns zero.
Invalidates any references, pointers, or iterators referring to pooled elements.

---
#### for_each:

```C++
template <typename UnaryFunction>
void for_each(UnaryFunction f)
```

Applies the given function object `f` to the all objects of pool.
The method is implemented in such a way that it guarantees that you can apply the `destroy` method for the current element.


---
#### shrink_to_fit:

```C++
void shrink_to_fit(std::size_t new_cap = 0) noexcept
```

Requests the removal of unused capacity for dynamic pool.

It is a non-binding request to reduce `capacity()` to `size()`.
For Pool_xxx_block it works only for empty pool.


---
#### reserve:

```C++
void reserve(std::size_t new_cap) noexcept
```

Increase the capacity of the pool to a value that's greater or equal to `new_cap`.
If `new_cap` is greater than the current `capacity()`, new storage is allocated,
otherwise the function does nothing.

`reserve()` does not change the size of the pool.


**Exceptions**

 - Thrown [std::bad_alloc](https://en.cppreference.com/w/cpp/memory/new/bad_alloc) if can't allocate nodes and `capacity()` < `new_cap`
 and the `POOL_RESERVE_EXCEPTION` flag is set

If an exception is thrown, this function has [basic exception guarantee](https://en.cppreference.com/w/cpp/language/exceptions).


---
#### Extended flags


```C++
using Pool_flags_t = uint32_t;

enum Pool_flags: Pool_flags_t
{
    POOL_DTOR_OFF         = (1u << 0),
    POOL_FIXED_CAPACITY   = (1u << 1),
    POOL_SELF_MOVE_GUARD  = (1u << 2),
    POOL_CREATE_EXCEPTION = (1u << 3),
    POOL_RESERVE_EXCEPTION= (1u << 4),
};
```

 - `POOL_DTOR_OFF` - Disables the pool's destructor. It is used in the embedded system to save flash memory. You must understand what you are doing.
 - `POOL_FIXED_CAPACITY` - Enables a mode when the dynamic pool will allocate new nodes, only by the reserve function.
 - `POOL_SELF_MOVE_GUARD` - Enables the self move guard for dynamic pool see: [C++ Core Guidelines c65-make-move-assignment-safe-for-self-assignment](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c65-make-move-assignment-safe-for-self-assignment)
 - `POOL_CREATE_EXCEPTION` - Throw [std::bad_alloc](https://en.cppreference.com/w/cpp/memory/new/bad_alloc) exception if no memory in `create` method
 - `POOL_RESERVE_EXCEPTION` - Throw [std::bad_alloc](https://en.cppreference.com/w/cpp/memory/new/bad_alloc) exception if no memory in `reserve` method

By default, all flags are zero, but for static pools destructor is not generated
(the `POOL_DTOR_OFF` flag is automatically set) if [is_trivially_destructible_v\<T\>](http://en.cppreference.com/w/cpp/types/is_destructible)



## Notes

#### General

For embedded systems, it makes sense to choose Static Pool, since most embedded systems don't have a memory manager.
If you only need basic functions (`create`/`destroy`) the `list` algorithm is the best choice because it is the fastest (we only work with one pointer).
If full functionality is required, the `dlist` algorithm is the best choice.
The `bitset` algorithm is a trade-off between `list` and `dlist`.
If you have chosen a dynamic pool, it is advisable to use the `reserve` method.
If the objects stored in the pool are small enough, it may make sense to use block allocation of nodes.
This will reduce the load on the memory manager.


#### Align

Pool allows you to set the required alignment of data(objects), through the template parameter `Align`.
By default, it is equal to `alignof(T)`.
But in some cases, when working with hardware(DSP, DMA...) or SIMD (SSE, AVX, Neon...), a certain alignment is required, which you can set through the template parameter.
Pool(Dynamic) uses standard C++17 language features that allow you to use the [new operator with support alignment](https://en.cppreference.com/w/cpp/memory/new/operator_new) for allocate `nodes`.
However, some compilers may require additional flags to support this feature.
For example [GCC version 7.x](https://gcc.gnu.org/gcc-7/changes.html) requires the flag: `-faligned-new=8` (for 32x bits).
Description of the flag on the [gcc website.](https://gcc.gnu.org/onlinedocs/gcc-11.2.0/gcc/C_002b_002b-Dialect-Options.html#C_002b_002b-Dialect-Options)
See an example of errors: [Default new alignment.](https://stackoverflow.com/questions/58860151/default-new-alignment)
For other compilers, check its documentation.
