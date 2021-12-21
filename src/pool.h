/*
 *
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Koynov Stas - skojnov@yandex.ru
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef POOL_H
#define POOL_H

#include <array>
#include <bitset>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <iterator>
#include <type_traits>





namespace pool_impl {



//helper with shorter name
template <typename T, typename... Args>
    inline constexpr bool is_nothrow_create = std::is_nothrow_constructible_v<T, Args...>;



template <class Impl,
          class Value,
          class Category   = std::bidirectional_iterator_tag,
          class Difference = std::ptrdiff_t>
class Iterator_facade
{
    public:
        using value_type        = std::remove_cv_t<Value>;
        using reference         = Value&;
        using pointer           = Value*;
        using difference_type   = Difference;
        using iterator_category = Category;

        reference operator*()  const noexcept { return  impl().dereference(); };
        pointer   operator->() const noexcept { return &impl().dereference(); };

        Impl& operator++() noexcept
        {
            impl().increment();
            return impl();
        }

        Impl operator++(int) noexcept
        {
            Impl result(impl());
            impl().increment();
            return result;
        }

        Impl& operator--() noexcept
        {
            impl().decrement();
            return impl();
        }

        Impl operator--(int) noexcept
        {
            Impl result(impl());
            impl().decrement();
            return result;
        }

        friend bool operator== (const Impl &lhs, const Impl &rhs) noexcept
        {
            return lhs.equal_to(rhs);
        }

        friend bool operator!= (const Impl &lhs, const Impl &rhs) noexcept
        {
            return !(lhs == rhs);
        }

    protected:
        constexpr       Impl& impl()       { return *static_cast<      Impl*>(this); }
        constexpr const Impl& impl() const { return *static_cast<const Impl*>(this); }

        constexpr bool equal_to(const Impl &other) const noexcept { return impl().equal(other); }
};





using Pool_flags_t = uint32_t;

enum Pool_flags: Pool_flags_t
{
    POOL_DTOR_OFF         = (1u << 0), //Don't generate destructor for pool
    POOL_FIXED_CAPACITY   = (1u << 1), //Add new Node only from reserve() method (only for dynamic pool)
    POOL_SELF_MOVE_GUARD  = (1u << 2), //C++ Core Guidelines c65-make-move-assignment-safe-for-self-assignment
    POOL_CREATE_EXCEPTION = (1u << 3), //Throw std::bad_alloc exception if no memory
    POOL_RESERVE_EXCEPTION= (1u << 4), //Throw std::bad_alloc exception if no memory
};





template <class Impl, Pool_flags_t Flags, typename Enable = void>
class Pool_dtor
{
    public:
        ~Pool_dtor() noexcept { static_cast<Impl*>(this)->dtor(); };
};



template <class Impl, Pool_flags_t Flags>
class Pool_dtor<Impl, Flags, std::enable_if_t< (Flags & POOL_DTOR_OFF) > >
{
};





template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          class        Impl>
class Pool_base
{
    static_assert(Align > 0, "Align == 0 is not support");

    public:
        using value_type      = T;
        using reference       = value_type&;
        using pointer         = value_type*;
        using const_reference = const value_type&;
        using const_pointer   = const value_type*;
        using size_type       = std::size_t;

        Pool_base() = default;

        static constexpr std::size_t  ALIGN   = Align;
        static constexpr Pool_flags_t FLAGS   = Flags;
        static constexpr std::size_t  N_VALUE = N;

        constexpr std::size_t size()  const noexcept { return m_size;                      }
        constexpr bool        empty() const noexcept { return size() == 0;                 }
        constexpr bool        full()  const noexcept { return size() == impl().capacity(); }

        template <typename... Args>
        T* create(Args&&... args) noexcept(is_nothrow_create<T, Args...>)
        {
            return impl().create_obj(std::forward<Args>(args)...);
        }


    protected:
        std::size_t m_size = 0;

        // Curiously Recurring Template interface
        constexpr       Impl& impl()       { return *static_cast<      Impl*>(this); }
        constexpr const Impl& impl() const { return *static_cast<const Impl*>(this); }
};





template <typename T>
constexpr Pool_flags_t SPool_base_flags(Pool_flags_t Flags)
{
    return std::is_trivially_destructible_v<T> ? (Flags | POOL_DTOR_OFF) : Flags;
}



template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          class        Impl>
class SPool_base: public Pool_base<T, N, Align, Flags, Impl>,
                  public Pool_dtor<Impl, SPool_base_flags<T>(Flags)>
{
    public:
        SPool_base() = default;

        static constexpr std::size_t capacity() noexcept { return N; }

        // disable copy/move semantics
        SPool_base(const SPool_base&)            = delete;
        SPool_base(SPool_base&&)                 = delete;
        SPool_base& operator=(const SPool_base&) = delete;
        SPool_base& operator=(SPool_base&&)      = delete;

    protected:
        void dtor() noexcept { this->impl().destroy_all(); }

        friend Pool_dtor<Impl, SPool_base_flags<T>(Flags)>;
};





template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          class        Impl>
class DPool_base: public Pool_base<T, N, Align, Flags, Impl>,
                  public Pool_dtor<Impl, Flags>
{
    public:
        DPool_base() = default;

        constexpr std::size_t capacity() const noexcept { return m_capacity; }

        template <typename... Args>
        T* create(Args&&... args) noexcept(is_nothrow_create<T, Args...>)
        {
            if constexpr ( !(Flags & POOL_FIXED_CAPACITY) )
            {
                if(this->full())
                    this->impl().add_node();
            }

            return this->impl().create_obj(std::forward<Args>(args)...);
        }


        void reserve(std::size_t new_cap) noexcept( !(Flags & POOL_RESERVE_EXCEPTION) )
        {
            for(auto i = capacity(); (capacity() < new_cap) && (i < new_cap); i++)
            {
                this->impl().add_node();
            }

            if constexpr(Flags & POOL_RESERVE_EXCEPTION)
            {
                if(capacity() < new_cap)
                    throw std::bad_alloc();
            }
        }


        // disable copy semantics
        DPool_base(const DPool_base&)            = delete;
        DPool_base& operator=(const DPool_base&) = delete;

    protected:
        std::size_t m_capacity = 0;


        Impl& move_assign_operator(Impl&& other) noexcept
        {
            auto& self = this->impl();

            if constexpr (Flags & POOL_SELF_MOVE_GUARD)
            {
                if(&self == &other)
                    return self;
            }

            self.dtor(); // Free the existing resource
            self.move_from(std::move(other));
            return self;
        }
};





template <class AlgBase, class Impl>
class Pool_node_allocator
{
    public:
        void shrink_to_fit(std::size_t new_cap = 0) noexcept
        {
            new_cap = std::max(impl().size(), new_cap);

            for(auto i = impl().capacity(); (impl().capacity() > new_cap) && (i > new_cap); i--)
            {
                del_node();
            }
        }


    protected:
        using Node = typename AlgBase::Node;

        void add_node() noexcept
        {
            auto new_node = new(std::nothrow) Node();

            if(new_node)
            {
                impl().add_to_free_nodes(new_node);
                impl().m_capacity++;
            }
        }

        void del_node() noexcept
        {
            impl().m_capacity--;
            auto top_node = impl().top_free_node();
            impl().pop_free_node();
            delete top_node;
        }

        void dtor() noexcept
        {
            while(impl().top_free_node())
                del_node();
        }

        void move_from(Impl&&) noexcept  {}

    private:
        constexpr Impl& impl() { return *static_cast<Impl*>(this); }
};





template <std::size_t  N, class AlgBase, class Impl>
class Pool_block_allocator
{
    public:
        void shrink_to_fit(std::size_t new_cap = 0) noexcept
        {
            if(!impl().empty())
                return;

            for(auto i = impl().capacity(); (impl().capacity() > new_cap) && (i > new_cap); i--)
            {
                del_node();
            }

            readd_blocks();
        }


    protected:
        using Node = typename AlgBase::Node;

        struct Block {
            Block               *next;
            std::array<Node, N>  nodes;
        };

        Block *m_blocks{nullptr};


        void add_node() noexcept
        {
            auto new_block = new(std::nothrow) Block();

            if(!new_block)
                return;

            impl().add_to_free_nodes(new_block->nodes);

            new_block->next = m_blocks;
            m_blocks        = new_block;

            impl().m_capacity += N;
        }

        void del_node() noexcept
        {
            auto block = m_blocks;
            m_blocks   = block->next;

            impl().m_capacity -= N;
            delete block;
        }

        void readd_blocks() noexcept
        {
            impl().reset_free_nodes();
            auto block = m_blocks;

            while(block)
            {
                impl().add_to_free_nodes(block->nodes);
                block = block->next;
            }
        }

        void dtor() noexcept
        {
            while(m_blocks)
                del_node();
        }

        void move_from(Impl&& other) noexcept
        {
            m_blocks       = other.m_blocks;
            other.m_blocks = nullptr;
        }

    private:
        constexpr Impl& impl() { return *static_cast<Impl*>(this); }
};





/*
 * This is a minimal implementation of a circular doubly linked list.
 * Has limited methods and is not intended to be used as a complete class.
 */
struct dlist_head
{
    public:
        dlist_head *next;
        dlist_head *prev;

        //There is no constructor. We're gonna call init method when it's really necessary!
        constexpr void init()        noexcept { prev = next = this;  }
        constexpr bool empty() const noexcept { return next == this; }

        void push_back(dlist_head *node) noexcept { insert(node, prev, this); }


        /*
         * move_to_back - deletes from current list and add to new_head
         *
         * new_head: the head that will follow our node
         */
        void move_to_back(dlist_head *new_head) noexcept
        {
            del_node(this);
            new_head->push_back(this);
        }


        /*
         * remove self from list.
         * Note: empty() on node does not return true after this,
         *       the node is in an undefined state.
         */
        void remove() noexcept { del_node(this); }


        /*
         * splice_front - transfers all the elements of src
         * into the this head (copy to front)
         */
        void splice_front(dlist_head *src) noexcept
        {
            if(!src->empty())
            {
                splice(src, this, next);
                src->init();
            }
        }


    private:
        /*
         * Insert a new node between two known consecutive nodes.
         *
         * This is only for internal list manipulation when
         * we've already known both the prev/next nodes!
         *
         * before:  [prev] <-> [next]
         * after:   [prev] <-> [node] <-> [next]
         */
        static void insert(dlist_head *node,
                           dlist_head *prev,
                           dlist_head *next) noexcept
        {
            next->prev = node;
            node->next = next;
            node->prev = prev;
            prev->next = node;
        }


        /*
         * Delete a list node by making the prev/next nodes
         * are pointing to each other
         *
         * This is only for internal list manipulation when
         * we've already known both the prev/next nodes!
         */
        static void del(dlist_head *prev, dlist_head *next) noexcept
        {
            next->prev = prev;
            prev->next = next;
        }


        /*
         * Delete node from list.
         *
         * node: the element to delete from the list.
         *
         * Note: empty() on node does not return true after this,
         *       the node is in an undefined state.
         *
         * before:  [prev] <-> [node] <-> [next]
         * after:   [prev] <-> [next];              old_val <- [node] -> old_val
         */
        static void del_node(dlist_head *node) noexcept
        {
            del(node->prev, node->next);
        }


        static void splice(dlist_head *list,
                           dlist_head *prev,
                           dlist_head *next) noexcept
        {
            dlist_head *first = list->next;
            dlist_head *last  = list->prev;

            first->prev = prev;
            prev->next  = first;

            last->next = next;
            next->prev = last;
        }
};





/*
 * A mixin class that implements a node management algorithm
 * based on a circular doubly-linked list
 *
 *  Technical details:
 *
 *  Each Node is a struct of a dlist_head and a type T.
 *  Creation and Destroying is very fast - as it is a simple replacement
 *  of pointers in a circular doubly-linked list.
 */
template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          class        Impl>
class Pool_dlist_base
{
    public:
        Pool_dlist_base() noexcept
        {
            m_used_nodes.init();
        }


        template <typename UnaryFunction>
        void for_each(UnaryFunction f)
        {
            auto iter = m_used_nodes.next;

            while(iter != &m_used_nodes)
            {
                auto next_iter = iter->next; //guard if user will delete obj(for current iter) from pool
                f((T *)get_data(iter));
                iter = next_iter;
            }
        }


        void destroy(const T* obj) noexcept
        {
            if(obj)
                destroy_obj(obj);
        }


        void destroy_all() noexcept
        {
            for_each([this](T* obj){ this->impl().destroy_obj(obj); });
        }


        template <class Value>
        class Iterator_t: public Iterator_facade<Iterator_t<Value>, Value>
        {
            public:
                explicit Iterator_t(const dlist_head *node = nullptr) noexcept:
                    m_node(const_cast<dlist_head *>(node)) {}

                template <class OtherValue>
                Iterator_t(const Iterator_t<OtherValue> &other) noexcept:
                    m_node(other.m_node) {}

            private:
                dlist_head *m_node;

                template <class OtherValue>
                bool equal(const Iterator_t<OtherValue> &other) const noexcept
                {
                    return m_node == other.m_node;
                }

                void increment() noexcept { m_node = m_node->next; }
                void decrement() noexcept { m_node = m_node->prev; }
                Value& dereference() const noexcept
                {
                    auto data = (Value *)Pool_dlist_base::get_data(m_node);
                    return *data;
                }

                template <class> friend class Iterator_t;
                friend class Iterator_facade<Iterator_t, Value>;
                friend class Pool_dlist_base;
        };

        using iterator               = Iterator_t<T>;
        using const_iterator         = Iterator_t<const T>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr auto begin() noexcept       { return iterator(m_used_nodes.next); }
        constexpr auto end()   noexcept       { return iterator(&m_used_nodes    ); }

        constexpr auto begin() const noexcept { return const_iterator(m_used_nodes.next); }
        constexpr auto end()   const noexcept { return const_iterator(&m_used_nodes    ); }
        constexpr auto cbegin()const noexcept { return const_iterator(m_used_nodes.next); }
        constexpr auto cend()  const noexcept { return const_iterator(&m_used_nodes    ); }

        constexpr auto rbegin() noexcept       { return reverse_iterator(end());   }
        constexpr auto rend()   noexcept       { return reverse_iterator(begin()); }

        constexpr auto rbegin() const noexcept { return const_reverse_iterator(end());   }
        constexpr auto rend()   const noexcept { return const_reverse_iterator(begin()); }
        constexpr auto crbegin()const noexcept { return const_reverse_iterator(cend());  }
        constexpr auto crend()  const noexcept { return const_reverse_iterator(cbegin());}

        iterator destroy(const_iterator pos) noexcept
        {
            iterator ret(pos.m_node->next);
            destroy_obj(&(*pos));

            return ret;
        }


        iterator destroy(const_iterator first, const_iterator last) noexcept
        {
            while (first != last)
              first = destroy(first);

            return last;
        }


    protected:
        using Data = std::aligned_storage_t<sizeof(T), Align>;

        struct Node {
            union {
            Node       *next;
            dlist_head  head;
            };
            Data        data;
        };

        Node       *m_free_nodes{nullptr};
        dlist_head  m_used_nodes;

        template <typename... Args>
        T* create_obj(Args&&... args) noexcept(is_nothrow_create<T, Args...>)
        {
            if(!m_free_nodes)
                return nullptr;

            auto free_node = m_free_nodes;
            auto obj       = ::new (&free_node->data) T(std::forward<Args>(args)...);

            //---- Kalb line ----
            m_free_nodes = free_node->next;
            m_used_nodes.push_back(&free_node->head);
            impl().m_size++;

            return obj;
        }

        void destroy_obj(const T* obj) noexcept
        {
            impl().m_size--;
            std::destroy_at(obj);

            auto node = get_node(obj);
            node->head.remove();         //remove node from m_used_nodes
            add_to_free_nodes(node);
        }

        static constexpr Node* get_node(const T* obj) noexcept {
            return (Node *)((char *)obj - offsetof(Node, data));
        }

        static constexpr Data* get_data(const dlist_head* head) noexcept {
            return (Data*)((char *)head + offsetof(Node, data));
        }

        constexpr Node* top_free_node()    noexcept { return m_free_nodes;   }
        constexpr void  reset_free_nodes() noexcept { m_free_nodes = nullptr;}

        //If m_free_nodes == nullptr calling pop_free_node is undefined
        constexpr void pop_free_node() noexcept
        {
            auto free_node = m_free_nodes;
            m_free_nodes   = free_node->next;
        }

        constexpr void add_to_free_nodes(Node* node) noexcept
        {
            node->next   = m_free_nodes;
            m_free_nodes = node;
        }

        constexpr void add_to_free_nodes(std::array<Node, N> &nodes) noexcept
        {
            for(auto &node: nodes)
            {
                add_to_free_nodes(&node);
            }
        }


        void move_from(Impl&& other) noexcept //only for dynamic
        {
            impl().m_size       = other.m_size;
            impl().m_capacity   = other.m_capacity;
            impl().m_free_nodes = other.m_free_nodes;
            impl().m_used_nodes.splice_front(&other.m_used_nodes);

            other.m_size       = 0;
            other.m_capacity   = 0;
            other.m_free_nodes = nullptr;
        }


    private:
        constexpr Impl& impl() { return *static_cast<Impl*>(this); }

        friend Pool_node_allocator <Pool_dlist_base, Impl>;
        friend Pool_block_allocator<N, Pool_dlist_base, Impl>;
};





/*
 * A mixin class that implements a node management algorithm
 * based on a singly-linked list
 *
 *  Technical details:
 *
 *  Each Node is a union of a pointer and a type T.
 *  Free nodes contain a pointer to the next free node.
 *  Creation is quick as all that is necessary
 *  is to return the address of the next free node.
 *  Destroying also very quick as the node's content is simply
 *  replaced with the address of the current next free node.
 */
template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          class        Impl>
class Pool_list_base
{
    public:
        void destroy(const T* obj) noexcept
        {
            if(obj)
                destroy_obj(obj);
        }


    protected:
        using Data = std::aligned_storage_t<sizeof(T), Align>;

        union Node {
            Node* next;
            Data  data;
        };

        Node* m_free_nodes{nullptr};


        template<class S>
        class state_saver
        {
            private:
                S  stored_value;
                S& obj_ref;

            public:
                state_saver(S& object) noexcept:
                    stored_value(object),
                    obj_ref(object)
                {}

                ~state_saver() noexcept { if (enable) obj_ref = stored_value; }

                bool enable{true};
        };


        template <typename... Args>
        T* create_obj(Args&&... args) noexcept(is_nothrow_create<T, Args...>)
        {
            if(!m_free_nodes)
                return nullptr;

            auto next_node = m_free_nodes->next;

            //Saver is a RAII for restore of state(linked list)
            //in case ctor of object will thrown an exception.
            //It is needed for basic exception guarantee.
            //INFO: Some MSVC and mingw32 gcc version 7.3.0
            //get internal compiler error:(for Auto Type Deduction for state_saver)
            //The solution is to set the type manually
            state_saver<Node*> saver(m_free_nodes->next);
            auto obj     = ::new (m_free_nodes) T(std::forward<Args>(args)...);
            saver.enable = false; //ctor of object did not throw an exception, all ok

            //---- Kalb line ----
            impl().m_size++;
            m_free_nodes = next_node;

            return obj;
        }

        void destroy_obj(const T* obj) noexcept
        {
            impl().m_size--;
            std::destroy_at(obj);

            add_to_free_nodes((Node*)obj);
        }

        //This algorithm does not support the for_each method and,
        //as a consequence, the destroy_all method. But we declare it empty
        //so that we can call it in the derived (if necessary).
        //We can use SFINAE test to check if a method destroy_all() exists
        //but this is more code than just declaring a non-public empty method
        //that will be removed by the compiler.
        void destroy_all() noexcept {}

        constexpr void add_to_free_nodes(Node* node) noexcept
        {
            node->next   = m_free_nodes;
            m_free_nodes = node;
        }

        constexpr void add_to_free_nodes(std::array<Node, N> &nodes) noexcept
        {
            for(auto &node: nodes)
            {
                add_to_free_nodes(&node);
            }
        }

        constexpr Node* top_free_node()    noexcept { return m_free_nodes;   }
        constexpr void  reset_free_nodes() noexcept { m_free_nodes = nullptr;}

        //If m_free_nodes == nullptr calling pop_free_node is undefined
        constexpr void pop_free_node() noexcept
        {
            auto free_node = m_free_nodes;
            m_free_nodes   = free_node->next;
        }


        void move_from(Impl&& other) noexcept //only for dynamic
        {
            impl().m_size       = other.m_size;
            impl().m_capacity   = other.m_capacity;
            impl().m_free_nodes = other.m_free_nodes;

            other.m_size        = 0;
            other.m_capacity    = 0;
            other.m_free_nodes  = nullptr;
        }


    private:
        constexpr Impl& impl() { return *static_cast<Impl*>(this); }

        friend Pool_node_allocator <Pool_list_base, Impl>;
        friend Pool_block_allocator<N, Pool_list_base, Impl>;
};





/*
 *  Static object pool is implemented on a singly-linked list
 *
 *  Technical details:
 *
 *  We don't store information(list) of the used nodes.
 *  This gives a lot of complexity == O(N^2) for the for_each algorithm.
 */
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0>
class SPool_list: public SPool_base<T, N, Align, Flags,
                                    SPool_list<T, N, Align, Flags> >,
                  public Pool_list_base<T, N, Align, Flags,
                                        SPool_list<T, N, Align, Flags> >
{
    public:
        SPool_list() noexcept
        {
            this->add_to_free_nodes(m_pool);
        }


        template <typename UnaryFunction>
        void for_each(UnaryFunction f)
        {
            if(this->empty())
                return;


            if(this->full()) //complexity == O(N)
            {
                for(auto &node : m_pool)
                    f((T *)&node);

                return;
            }

            //The pool is not empty and not full, we have no information
            //about the nodes used. But there is a list of free ones,
            //this check leads to the complexity == O(N^2)
            for(auto &node : m_pool)
            {
                if(node_is_used(&node))
                    f((T *)&node);
            }
        }


        void destroy_all() noexcept
        {
            for_each([this](T* obj){ this->destroy_obj(obj); });
        }


    private:
        using Node = typename Pool_list_base<T, N, Align, Flags, SPool_list>::Node;

        std::array<Node, N> m_pool;

        bool node_is_used(const Node* node) const noexcept
        {
            const Node* free_node = this->m_free_nodes;

            while(free_node)
            {
                if(node == free_node)
                    return false;

                free_node = free_node->next;
            }

            return true;
        }

        friend SPool_base    <T, N, Align, Flags, SPool_list>;
        friend Pool_list_base<T, N, Align, Flags, SPool_list>;
        friend Pool_base     <T, N, Align, Flags, SPool_list>;
};





/*
 *  Static object pool is implemented on a singly-linked list + bitset
 *
 *  Technical details:
 *
 *  It's analogue of SPool_list, but we use bitset for the used nodes.
 *  This gives a complexity == O(N) for the for_each algorithm.
 */
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0>
class SPool_list_bitset: public SPool_base<T, N, Align, Flags,
                                           SPool_list_bitset<T, N, Align, Flags> >,
                         public Pool_list_base<T, N, Align, Flags,
                                               SPool_list_bitset<T, N, Align, Flags> >
{
    public:
        SPool_list_bitset() noexcept
        {
            this->add_to_free_nodes(m_pool);
        }


        template <typename... Args>
        T* create(Args&&... args) noexcept(is_nothrow_create<T, Args...>)
        {
            if(!this->m_free_nodes)
                return nullptr;

            auto i   = index_node(this->m_free_nodes);
            auto obj = this->create_obj(std::forward<Args>(args)...);

            //---- Kalb line ----
            m_used[i] = 1;

            return obj;
        }


        void destroy(const T* obj) noexcept
        {
            if(!obj)
                return;

            auto i    = index_node((const Node*)obj);
            m_used[i] = 0;
            this->destroy_obj(obj);
        }


        template <typename UnaryFunction>
        void for_each(UnaryFunction f)
        {
            if(this->empty())
                return;

            for(std::size_t i = 0; i < N; i++)
            {
                if(m_used[i])
                    f((T *)&m_pool[i]);
            }
        }


        void destroy_all() noexcept
        {
            for_each([this](T* obj){ this->destroy_obj(obj); });
            m_used.reset();
        }


        template <class Value>
        class Iterator_t: public Iterator_facade<Iterator_t<Value>, Value>
        {
            public:
                Iterator_t() noexcept: m_cur_pos(0), m_pool(nullptr) {}

                Iterator_t(const SPool_list_bitset *pool, std::size_t cur_pos) noexcept:
                    m_cur_pos(cur_pos),
                    m_pool(const_cast<SPool_list_bitset *>(pool))
                {
                    if(cur_pos != N)
                        set_first_pos();
                }

                //iterator to/from const_iterator
                template <class OtherValue>
                Iterator_t(const Iterator_t<OtherValue> &other) noexcept:
                    m_cur_pos(other.m_cur_pos),
                    m_pool(other.m_pool)
                {}

            private:
                std::size_t        m_cur_pos;
                SPool_list_bitset *m_pool;

                template <class OtherValue>
                bool equal(const Iterator_t<OtherValue> &other) const noexcept
                {
                    return m_cur_pos == other.m_cur_pos;
                }

                void increment() noexcept { next(); }
                void decrement() noexcept { prev(); }
                Value& dereference() const noexcept { return (Value &)m_pool->m_pool[m_cur_pos]; }

                void next() noexcept
                {
                    while(m_cur_pos < N)
                    {
                        m_cur_pos++;

                        if(m_pool->m_used[m_cur_pos])
                            break;
                    }
                }

                void prev() noexcept
                {
                    if(m_pool->empty())
                    {
                        m_cur_pos = N;
                        return;
                    }

                    while(m_cur_pos > 0)
                    {
                        m_cur_pos--;

                        if(m_pool->m_used[m_cur_pos])
                            return;
                    }

                    m_cur_pos = N;
                }

                void set_first_pos() noexcept
                {
                    if(m_pool->empty())
                    {
                        m_cur_pos = N;
                        return;
                    }

                    m_cur_pos = 0;

                    if(!m_pool->m_used[m_cur_pos])
                        next();
                }

                template <class> friend class Iterator_t;
                friend class Iterator_facade<Iterator_t, Value>;
                friend class SPool_list_bitset;
        };

        using iterator               = Iterator_t<T>;
        using const_iterator         = Iterator_t<const T>;
        using reverse_iterator       = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        constexpr auto begin() noexcept       { return iterator(this, 0); }
        constexpr auto end()   noexcept       { return iterator(this, N); }

        constexpr auto begin() const noexcept { return const_iterator(this, 0); }
        constexpr auto end()   const noexcept { return const_iterator(this, N); }
        constexpr auto cbegin()const noexcept { return const_iterator(this, 0); }
        constexpr auto cend()  const noexcept { return const_iterator(this, N); }

        constexpr auto rbegin() noexcept       { return reverse_iterator(end());   }
        constexpr auto rend()   noexcept       { return reverse_iterator(begin()); }

        constexpr auto rbegin() const noexcept { return const_reverse_iterator(end());   }
        constexpr auto rend()   const noexcept { return const_reverse_iterator(begin()); }
        constexpr auto crbegin()const noexcept { return const_reverse_iterator(cend());  }
        constexpr auto crend()  const noexcept { return const_reverse_iterator(cbegin());}

        iterator destroy(const_iterator pos) noexcept
        {
            auto ret = pos;
            ++ret;
            destroy(&(*pos));
            return ret;
        }


        iterator destroy(const_iterator first, const_iterator last) noexcept
        {
            while (first != last)
              first = destroy(first);

            return last;
        }


    private:
        using Node = typename Pool_list_base<T, N, Align, Flags,
                                             SPool_list_bitset>::Node;

        std::array<Node, N> m_pool;
        std::bitset<N>      m_used; //0 - free, 1 - is used

        constexpr std::size_t index_node(const Node* node) const noexcept {
            return std::distance(m_pool.cbegin(), node);
        }

        friend Pool_list_base<T, N, Align, Flags, SPool_list_bitset>;
};





// Static object pool is implemented on a circular doubly-linked list
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0>
class SPool_dlist: public SPool_base<T, N, Align, Flags,
                                     SPool_dlist<T, N, Align, Flags> >,
                   public Pool_dlist_base<T, N, Align, Flags,
                                          SPool_dlist<T, N, Align, Flags> >
{
    public:
        SPool_dlist() noexcept
        {
            this->add_to_free_nodes(m_pool);
        }


    private:
        using Node = typename Pool_dlist_base<T, N, Align, Flags,
                                              SPool_dlist>::Node;

        std::array<Node, N> m_pool;

        friend Pool_base      <T, N, Align, Flags, SPool_dlist>;
        friend SPool_base     <T, N, Align, Flags, SPool_dlist>;
        friend Pool_dlist_base<T, N, Align, Flags, SPool_dlist>;
};





template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          typename     AlgBase,
          typename     AlocBase,
          typename     Impl>
class Pool_xxx: public DPool_base<T, N, Align, Flags, Impl>,
                public AlgBase,
                public AlocBase
{
    public:
        Pool_xxx() = default;


        Pool_xxx(Pool_xxx&& other) noexcept : Pool_xxx()
        {
            move_from(std::move((Impl &&)other));
        }


        Pool_xxx& operator=(Pool_xxx&& other) noexcept
        {
            return this->move_assign_operator(std::move((Impl &&)other));
        }


    private:
        void dtor() noexcept
        {
            this->destroy_all();
            AlocBase::dtor();
        }

        void move_from(Impl&& other) noexcept
        {
            AlgBase ::move_from(std::move(other));
            AlocBase::move_from(std::move(other));
        }

        friend AlgBase;
        friend AlocBase;
        friend Pool_dtor<Impl, Flags>;
        friend DPool_base<T, N, Align, Flags, Impl>;
};





// Helper class to reduce writing template parameters
template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          typename     AlgBase,
          typename     Impl>
class Pool_xxx_node: public Pool_xxx<T, N, Align, Flags,
                                     AlgBase,
                                     Pool_node_allocator<AlgBase, Impl>,
                                     Impl>
{
    using Pool_xxx<T, N, Align, Flags,
                   AlgBase,
                   Pool_node_allocator<AlgBase, Impl>,
                   Impl>::Pool_xxx; //for using explicit ctors!
};





// Dynamic object pool is implemented on a singly-linked list
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0>
class Pool_list: public Pool_xxx_node<T, N, Align, Flags,
                                      Pool_list_base<T, N, Align, Flags,
                                      Pool_list<T, N, Align, Flags> >,
                                      Pool_list<T, N, Align, Flags> >
{
    using Pool_xxx_node<T, N, Align, Flags,
                        Pool_list_base<T, N, Align, Flags, Pool_list>,
                        Pool_list>::Pool_xxx_node; //for using explicit ctors!
};





// Dynamic object pool is implemented on a circular doubly-linked list
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0>
class Pool_dlist: public Pool_xxx_node<T, N, Align, Flags,
                                       Pool_dlist_base<T, N, Align, Flags,
                                       Pool_dlist<T, N, Align, Flags> >,
                                       Pool_dlist<T, N, Align, Flags> >
{
    using Pool_xxx_node<T, N, Align, Flags,
                        Pool_dlist_base<T, N, Align, Flags, Pool_dlist>,
                        Pool_dlist>::Pool_xxx_node; //for using explicit ctors!
};





// Helper class to reduce writing template parameters
template <typename     T,
          std::size_t  N,
          std::size_t  Align,
          Pool_flags_t Flags,
          typename     AlgBase,
          typename     Impl>
class Pool_xxx_block: public Pool_xxx<T, N, Align, Flags,
                                      AlgBase,
                                      Pool_block_allocator<N, AlgBase, Impl>,
                                      Impl>
{
    using Pool_xxx<T, N, Align, Flags,
                   AlgBase,
                   Pool_block_allocator<N, AlgBase, Impl>,
                   Impl>::Pool_xxx; //for using explicit ctors!
};





/*
 *  Dynamic object pool is implemented on a singly-linked list
 *
 *  It's analogue of Pool_list, but memory is allocated in blocks of N nodes.
 */
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0>
class Pool_list_block: public Pool_xxx_block<T, N, Align, Flags,
                                             Pool_list_base<T, N, Align, Flags,
                                             Pool_list_block<T, N, Align, Flags> >,
                                             Pool_list_block<T, N, Align, Flags> >
{
    using Pool_xxx_block<T, N, Align, Flags,
                         Pool_list_base<T, N, Align, Flags, Pool_list_block>,
                         Pool_list_block>::Pool_xxx_block; //for using explicit ctors!
};





/*
 *  Dynamic object pool is implemented on a circular doubly-linked list
 *
 *  It's analogue of Pool_dlist, but memory is allocated in blocks of N nodes.
 */
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0>
class Pool_dlist_block: public Pool_xxx_block<T, N, Align, Flags,
                                              Pool_dlist_base<T, N, Align, Flags,
                                              Pool_dlist_block<T, N, Align, Flags> >,
                                              Pool_dlist_block<T, N, Align, Flags> >
{
    using Pool_xxx_block<T, N, Align, Flags,
                         Pool_dlist_base<T, N, Align, Flags, Pool_dlist_block>,
                         Pool_dlist_block>::Pool_xxx_block; //for using explicit ctors!
};



} // namespace pool_impl





namespace pool {



using  pool_impl::Pool_flags_t;
using  pool_impl::POOL_DTOR_OFF;
using  pool_impl::POOL_FIXED_CAPACITY;
using  pool_impl::POOL_SELF_MOVE_GUARD;
using  pool_impl::POOL_CREATE_EXCEPTION;
using  pool_impl::POOL_RESERVE_EXCEPTION;


#define POOL_USING_ALIAS(alias_name, impl_name) \
    template <typename T, std::size_t N, std::size_t Align = alignof(T), Pool_flags_t Flags = 0> \
    using alias_name = pool_impl::impl_name<T, N, Align, Flags>;


POOL_USING_ALIAS(SPool_list       , SPool_list       )
POOL_USING_ALIAS(SPool_list_bitset, SPool_list_bitset)
POOL_USING_ALIAS(SPool_dlist      , SPool_dlist      )

POOL_USING_ALIAS(Pool_list        , Pool_list        )
POOL_USING_ALIAS(Pool_list_block  , Pool_list_block  )
POOL_USING_ALIAS(Pool_dlist       , Pool_dlist       )
POOL_USING_ALIAS(Pool_dlist_block , Pool_dlist_block )




/*
 *  Object pool
 *
 *  Template parameters:
 *  T     - The type of the elements.
 *  N     - The count of elemtnts in Static pool
 *        - The block size in Pool_xxx_block
 *        - Don't uses in Pool_xxx
 *  Align - The alignment of items in the pool
 *  Flags - The is an extended flags to tuning of implementation
 *  Impl  - This is a specific implementation(algorithm) see below
 *
 *  Abbreviations (Impl):
 *             Static         |       Dynamic
 *  --------------------------|--------------------------
 *  SP_l  - SPool_list        | P_l   - Pool_list
 *  SP_b  - SPool_list_bitset | P_dl  - Pool_dlist
 *  SP_dl - SPool_dlist       | P_lb  - Pool_list_block
 *                            | P_dlb - Pool_dlist_block
 *
 *  Algorithmic complexity:
 *
 *               |      Static          ||       Dynamic
 * --------------|----------------------||---------------------------
 *  method/Impl  | SP_l  | SP_b | SP_dl || P_l  | P_dl | P_lb | P_dlb
 * --------------|-------|------|-------||------|------|------|------
 *  destroy(iter)|   -   | O(1) | O(1)  ||  -   | O(1) |  -   | O(1)
 *  destroy(f, l)|   -   | O(N) | O(N)  ||  -   | O(N) |  -   | O(N)
 *  destroy_all  | O(N^2)| O(N) | O(N)  ||  -   | O(N) |  -   | O(N)
 *  for_each     | O(N^2)| O(N) | O(N)  ||  -   | O(N) |  -   | O(N)
 *  reserve      |   -   |   -  |   -   || O(N) | O(N) | O(N) | O(N)
 *  shrink_to_fit|   -   |   -  |   -   || O(N) | O(N) | O(N) | O(N)
 *  constructor  | O(N)  | O(N) | O(N)  || O(1) | O(1) | O(1) | O(1)
 *  destructor   | O(N^2)| O(N) | O(N)  || O(N) | O(N) | O(N) | O(N)
 *  iterator     |   -   | Bid  | Bid   ||  -   | Bid  |  -   | Bid
 *
 *  All base methods have complexity is O(1)!
 *  It's methods: size, capacity, empty, full, create, destroy(T*)
 *
 *  Notes:
 *
 *  By default, all flags are zero, but for static pools destructor is not generated
 * (the POOL_DTOR_OFF flag is automatically set) if is_trivially_destructible_v<T>
 *
 *  For dynamic pool create() add a new node if there is no free one.
 *  If flag POOL_FIXED_CAPACITY is set, new nodes are add
 *  only by the reserve() method.
 *
 *  shrink_to_fit for Pool_xxx_block work only for empty pool.
 *
 *  Pool_xxx_block is an analogue of Pool_xxx, the difference is that we
 *  allocate memory in blocks of N nodes at a time. If N == 1, this does
 *  not make sense, because we do not get a profit and at the same time
 *  we have overhead costs for pointers to blocks.
 *
 *  Pools P_l, P_lb don't store information about the nodes used.
 *  Therefore, the user must ensure that all objects was be destroyed when
 *  the destructor is called. Otherwise, it can lead to a memory leak.
 *
 *  For more details see concrete implementation
 */
template <typename     T,
          std::size_t  N,
          std::size_t  Align = alignof(T),
          Pool_flags_t Flags = 0,
          template<typename, std::size_t, std::size_t, Pool_flags_t> class Impl = Pool_dlist>
class Pool: public Impl<T, N, Align, Flags>
{
    public:
        using Impl<T, N, Align, Flags>::Impl; //for using explicit ctors!

        template <typename... Args>
        T* create(Args&&... args) noexcept(pool_impl::is_nothrow_create<T, Args...> &&
                                           !(Flags & POOL_CREATE_EXCEPTION))
        {
            auto obj = Impl<T, N, Align, Flags>::create(std::forward<Args>(args)...);

            if constexpr(Flags & POOL_CREATE_EXCEPTION)
            {
                if(!obj)
                    throw std::bad_alloc();
            }

            return obj;
        }
};



} // namespace pool





#endif // POOL_H
