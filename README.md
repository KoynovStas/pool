# Object Pool


## Description

`Pool` is C++17 object(memory) pool template with different implementations(algorithms)

The [**classic** object pool](https://en.wikipedia.org/wiki/Object_pool_pattern) pattern is a software creational design pattern that uses a set of **initialized** objects kept ready to use, rather than allocating and destroying them on demand.
This `Pool` calls constructor of an object in the `create` method and destructor in the `destroy` method(memory for the object remains in the pool).
This pool is more designed to optimize memory allocation for an object than to optimize construct/destruct of object.
I use this pool as a memory manager(heap) for a specific type of objects.



#### Features

 - Header-only
 - Supported a different implementations(algorithms) see below
 - O(1) create/destroy of objects
 - Supported alignment of objects via use of [alignas](https://en.cppreference.com/w/cpp/language/alignas)
 - Some algorithms provide [iterator](https://devdocs.io/cpp/iterator) support



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

##### Algorithm:

**Static:**

| Name             | Info
|------------------|------
|SPool_list        | Based on a singly-linked list
|SPool_list_bitset | Analogue of SPool_list, but we use bitset for the used nodes
|SPool_dlist       | Based on an intrusive(nested) circular doubly-linked list


**Dynamic:**
| Name            | Info
|-----------------|------
|Pool_list        | Based on a singly-linked list
|Pool_dlist       | Based on an intrusive(nested) circular doubly-linked list
|Pool_list_block  | Analogue of Pool_list, but memory is allocated in blocks of N nodes
|Pool_dlist_block | Analogue of Pool_dlist, but memory is allocated in blocks of N nodes


More details see: **[pool.h](./src/pool.h)**


## Usage

**To start working, perform the following steps:**

1. Copy the **[pool.h](./src/pool.h)** into your project.
2. Include **[pool.h](./src/pool.h)**
3. Parameterize a template (see an examples)


## Examples


```C++
#include <iostream>
#include "pool.h"

using namespace pool;

int main()
{
    const size_t N     = 10;
    const size_t Aling = alignof(int);
    Pool<int, N, Aling, 0, SPool_list> pool; //SPool_list don't support iterator

    auto i = pool.create(123);  // create obj
    std::cout << *i << "\n";    // use obj
    pool.destroy(i);            // destroy obj
}
```

We can use method `for_each` or `iterators`

```C++
#include <iostream>
#include "pool.h"

using namespace pool;

int main()
{
    const size_t N = 10;
    Pool<int, N> pool;

    for(size_t i = 0; i < N; i++)
        pool.create(i);

    pool.for_each([](int *i){ std::cout << *i << " ";});
    std::cout << "\n";

    //We can use for-range (If Impl support iterator)
    for(auto i :pool)
        std::cout << i << " ";
    std::cout << "\n";
}
```

For a more detailed description, see the [API documentation](doc/API.md).



## Tests

To run tests, perform the following steps:

```
cd tests
cmake . -B ./build
cmake --build build
```

On Windows for MinGW you can generate `MinGW Makefiles`:
```
cmake . -B ./build -G "MinGW Makefiles"
```


## License

[BSD-3-Clause](./LICENSE)
