#pragma once
#include <stdexcept>
#include <initializer_list>
#include "iterators.h"

template <class T, size_t N, class Alloc = std::allocator<T>>
class circular_buffer {
public:
    static_assert(N > 0, "N must be greater than 0");

    using value_type = T;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = typename std::allocator_traits<Alloc>::pointer;
    using const_pointer = typename std::allocator_traits<Alloc>::const_pointer;

    using iterator = circ_buff_iter<T>;
    using const_iterator = circ_buff_const_iter<T>;

    template <typename Iter>
    circular_buffer(Iter first, Iter last, const Alloc& alloc = Alloc()) : m_allocator(alloc)
        , m_buffer(m_allocator.allocate(N)) , m_begin(m_buffer), m_end(m_buffer + N), m_head(m_begin) {
        if (std::distance(first, last) > N || std::distance(first, last) < 0) {
            m_allocator.deallocate(m_buffer, N);
            throw std::range_error("incorrect iterators");
        }
        pointer it;
        try {
            for (it = m_begin; first != last; it++, first++) {
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(*first));
            }
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
        try {
            for ( ; it != m_end; ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(T()));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
    }
    circular_buffer(const T& val, const Alloc& alloc = Alloc()) : m_allocator(alloc), m_buffer(m_allocator.allocate(N))
        , m_begin(m_buffer), m_end(m_buffer + N), m_head(m_begin) {
        pointer it;
        try {
            for (it = m_begin; it != m_end; ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(val));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
    }
    circular_buffer(const std::initializer_list<T> &list, const Alloc& alloc = Alloc()) : m_allocator(alloc)
        , m_buffer(m_allocator.allocate(N)), m_begin(m_buffer), m_end(m_buffer + N), m_head(m_begin) {
        if (list.size() > N) {
            m_allocator.deallocate(m_buffer, N);
            throw std::range_error("initializer list length is greater then size of bufffer");
        }
        pointer it = m_begin;
        try {
            for (auto other_it = list.begin(); other_it != list.end(); it++, other_it++) {
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(*other_it));
            }
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
        try {
            for (; it != m_end; ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(T()));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
    }

    circular_buffer(const Alloc& alloc = Alloc()) : m_allocator(alloc) , m_buffer(m_allocator.allocate(N))
        , m_begin(m_buffer) , m_end(m_buffer + N) , m_head(m_begin) {
        pointer it;
        try {
            for (it = m_begin; it != m_end; ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(T()));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
    }
    circular_buffer(const circular_buffer& other)
        : m_allocator(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.m_allocator))
        , m_buffer(m_allocator.allocate(N)), m_begin(m_buffer), m_end(m_buffer + N)
        , m_head(m_begin + (other.m_head - other.m_begin)) {
        pointer it = m_begin;
        try {
            for (const_pointer other_it = other.m_begin; other_it != other.m_end; ++other_it, ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(*other_it));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
    }
    circular_buffer(circular_buffer&& other) noexcept
        : m_allocator(std::move(other.m_allocator)), m_buffer(other.m_buffer)
        , m_begin(other.m_begin), m_end(other.m_end), m_head(other.m_head) {
        other.m_buffer = nullptr;
        other.m_begin = nullptr;
        other.m_end = nullptr;
        other.m_head = nullptr;
    }
    circular_buffer& operator =(const circular_buffer& other) {
        Alloc new_allocator = std::allocator_traits<Alloc>::select_on_container_copy_construction(other.m_allocator);
        if (this != std::addressof(other)) {
            pointer new_buffer = new_allocator.allocate(N);
            pointer it = new_buffer;
            try {
                for (const_pointer other_it = other.m_begin; other_it != other.m_end; ++other_it, ++it)
                    std::allocator_traits<Alloc>::construct(new_allocator, it, std::move(*other_it));
            }
            catch (...) {
                for (pointer del_it = new_buffer; del_it != it; ++del_it)
                    std::allocator_traits<Alloc>::destroy(new_allocator, del_it);
                new_allocator.deallocate(new_buffer, N);
                throw std::exception("elements consctruction fail");
            }
            for (pointer del_it = m_begin; del_it != m_end; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);

            m_allocator = std::allocator_traits<Alloc>::select_on_container_copy_construction(other.m_allocator);
            m_buffer = new_buffer;
            m_begin = new_buffer;
            m_end = new_buffer + N;
            m_head = m_begin + (other.m_head - other.m_begin);
        }
        return *this;
    }
    circular_buffer& operator =(circular_buffer&& other) noexcept {
        for (pointer it = m_begin; it != m_end; ++it)
            std::allocator_traits<Alloc>::destroy(m_allocator, it);
        m_allocator.deallocate(m_buffer, N);

        m_allocator = std::move(other.m_allocator);
        m_buffer = other.m_buffer;
        m_begin = other.m_begin;
        m_end = other.m_end;
        m_head = other.m_head;
        other.m_buffer = nullptr;
        other.m_begin = nullptr;
        other.m_end = nullptr;
        other.m_head = nullptr;
        return *this;
    }

    iterator begin() noexcept {
        return iterator(m_begin, 0, N);
    }
    iterator end() noexcept {
        return iterator(m_begin, N, N);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(m_begin, 0, N);
    }
    const_iterator cend() const noexcept {
        return const_iterator(m_begin, N, N);
    }
 
    reference operator [](size_t offset) noexcept {
        return m_buffer[offset];
    }
    reference at(size_t offset) {
        if (offset >= N)
            throw std::out_of_range("Index of out range");
        return m_buffer[offset];
    }
    size_t size() const noexcept {
        return N;
    }
    size_t max_size() const noexcept {
        return std::numeric_limits<difference_type>::max();
    }

    reference front() noexcept {
        return m_buffer[0];
    }
    reference back() noexcept {
        return m_buffer[N - 1];
    }

    template <typename Iter>
    void insert(iterator it, Iter first, Iter last) {
        if (std::distance(first, last) > N || std::distance(first, last) < 0) {
            throw std::range_error("incorrect iterators");
        }
        for ( ; first != last; ++first) {
            this->insert(it++, std::forward<T>(*first));
            if (it == this->end())
                it = this->begin();
        }
    }
    void insert(iterator it, T&& val) {
        if (it == this->end())
            throw std::out_of_range("Invalid iterator");
        T temp = std::move(*it);
        try {
            std::allocator_traits<Alloc>::destroy(m_allocator, std::addressof(*it));
            std::allocator_traits<Alloc>::construct(m_allocator, std::addressof(*it), std::forward<T>(val));
        }
        catch (...) {
            std::allocator_traits<Alloc>::destroy(m_allocator, std::addressof(*it));
            std::allocator_traits<Alloc>::construct(m_allocator, std::addressof(*it), std::forward<T>(temp));
            throw std::exception("elements consctruction fail");
        }
    }
    void insert(iterator it, size_t n, T&& val) {
        if (it == this->end())
            throw std::out_of_range("Invalid iterator");
        if (n > N)
            throw std::range_error("too many elements");
        for (int i = 0; i < n; ++i) {
            this->insert(it++, std::forward<T>(val));
            if (it == this->end())
                it = this->begin();
        }
    } 

    template <typename... Args>
    void emplace_back(Args&&... args) {
        T temp = std::move(*m_head);
        try {
            std::allocator_traits<Alloc>::destroy(m_allocator, m_head);
            std::allocator_traits<Alloc>::construct(m_allocator, m_head, std::forward<Args>(args)...);
        }
        catch (...) {
            std::allocator_traits<Alloc>::destroy(m_allocator, m_head);
            std::allocator_traits<Alloc>::construct(m_allocator, m_head, std::move(temp));
            throw std::exception("elements consctruction fail");
        }
        ++m_head;
        if (m_head == m_end)
            m_head = m_begin;
    }
    void push_back(T&& val) {
        emplace_back(std::move(val));
    }
    void push_back(const T& val) {
        emplace_back(val);
    }

    void swap(circular_buffer& other) noexcept {
        if (this == std::addressof(other))
            return;
        std::swap(this->m_allocator, other.m_allocator);
        std::swap(this->m_begin, other.m_begin);
        std::swap(this->m_end, other.m_end);
        std::swap(this->m_buffer, other.m_buffer);
        std::swap(this->m_head, other.m_head);
    }
    void clear() {
        for (pointer del_it = m_begin; del_it != m_end; ++del_it)
            std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
        pointer it;
        try {
            for (it = m_begin; it != m_end; ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(T()));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, N);
            throw std::exception("elements consctruction fail");
        }
        m_head = m_begin;
    }
    bool empty() const noexcept {
        return false;
    }
    
    ~circular_buffer() noexcept {
        if (m_buffer == nullptr)
            return;
        for(pointer it = m_begin; it != m_end; ++it)
            std::allocator_traits<Alloc>::destroy(m_allocator, it); 
        m_allocator.deallocate(m_buffer, N);
    }
private:
    Alloc m_allocator;
    pointer m_buffer;
    pointer m_begin;
    pointer m_end;
    pointer m_head;
};
