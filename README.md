# Object Pool


## Description

`Pool` is C++17 object pool template with different implementations(algorithms)


#### Features

 - Header-only.
 - Supported a different implementations(algorithms) see below
 - O(1) create/destroy of objects
 - Supported alignment of objects via use of [aligned_storage](https://en.cppreference.com/w/cpp/types/aligned_storage)
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
|SPool_dlist       | Based on a nested circular doubly-linked list


**Dynamic:**
| Name            | Info
|-----------------|------
|Pool_list        | Based on a singly-linked list
|Pool_dlist       | Based on a nested circular doubly-linked list
|Pool_list_block  | Analogue of Pool_list, but memory is allocated in blocks of N nodes
|Pool_dlist_block | Analogue of Pool_dlist, but memory is allocated in blocks of N nodes


More details see: **[pool.h](./pool.h)**


## Usage

**To start working, perform the following steps:**

1. Copy the **[pool.h](./pool.h)** into your project.
2. Include **[pool.h](./pool.h)**
3. Parameterize a template (see an examples)


## Examples


```C++
#include <iostream>
#include "pool.h"

using namespace pool;

int main()
{
    const size_t N = 10;
    Pool<int, N, 8, 0, SPool_list> pool; //SPool_list don't support iterator

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
    Pool<int, N, 8> pool;

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

```
cd tests
cmake . -B ./build
cd build
make
```


## License

[BSD-3-Clause](./LICENSE)
