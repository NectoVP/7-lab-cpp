#pragma once
#pragma once
#include <stdexcept>
#include <initializer_list>
#include "iterators.h"

template <class T, class Alloc = std::allocator<T>>
class dynamic_circular_buffer {
public:
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
    dynamic_circular_buffer(Iter first, Iter last, const Alloc& alloc = Alloc()) {
        if (std::distance(first, last) <= 0)
            throw std::range_error("incorrect iterators");

        m_size = std::distance(first, last);
        m_allocator = alloc;
        m_buffer = m_allocator.allocate(m_size);
        m_begin = m_buffer;
        m_end = m_buffer + m_size;
        m_head = m_begin;

        pointer it;
        try {
            for (it = m_begin; first != last; it++, first++) {
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(*first));
            }
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, m_size);
            throw std::exception("elements consctruction fail");
        }
    }
    dynamic_circular_buffer(size_t n, const T& val, const Alloc& alloc = Alloc()) {
        if (n == 0)
            throw std::exception("buffer cannot hold 0 elements of val");
        
        m_size = n;
        m_allocator = alloc;
        m_buffer = m_allocator.allocate(m_size);
        m_begin = m_buffer;
        m_end = m_buffer + n;
        m_head = m_begin;

        pointer it;
        try {
            for (it = m_begin; it != m_end; ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(val));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, n);
            throw std::exception("elements consctruction fail");
        }
    }
    dynamic_circular_buffer(const std::initializer_list<T>& list, const Alloc& alloc = Alloc()) {
        
        m_size = list.size();
        m_allocator = alloc;
        m_buffer = m_allocator.allocate(m_size);
        m_begin = m_buffer;
        m_end = m_buffer + m_size;
        m_head = m_begin;

        pointer it = m_begin;
        try {
            for (auto other_it = list.begin(); other_it != list.end(); it++, other_it++) {
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(*other_it));
            }
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, m_size);
            throw std::exception("elements consctruction fail");
        }
    }

    dynamic_circular_buffer(const Alloc& alloc = Alloc()) : m_allocator(alloc), m_begin(nullptr),
    m_end(nullptr), m_buffer(nullptr), m_head(nullptr), m_size(0) { }
    dynamic_circular_buffer(size_t n, const Alloc& alloc = Alloc()) {

        m_size = n;
        m_allocator = alloc;
        m_buffer = m_allocator.allocate(m_size);
        m_begin = m_buffer;
        m_end = m_buffer + n;
        m_head = m_begin;

        pointer it;
        try {
            for (it = m_begin; it != m_end; ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(T()));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, n);
            throw std::exception("elements consctruction fail");
        }
    }
    dynamic_circular_buffer(const dynamic_circular_buffer& other)
        : m_allocator(std::allocator_traits<Alloc>::select_on_container_copy_construction(other.m_allocator))
        , m_size(other.m_size), m_buffer(m_allocator.allocate(m_size)), m_end(m_buffer + m_size)
        , m_head(m_begin + (other.m_head - other.m_begin)), m_begin(m_buffer) {
        pointer it = m_begin;
        try {
            for (const_pointer other_it = other.m_begin; other_it != other.m_end; ++other_it, ++it)
                std::allocator_traits<Alloc>::construct(m_allocator, it, std::move(*other_it));
        }
        catch (...) {
            for (pointer del_it = m_begin; del_it != it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, m_size);
            throw std::exception("elements consctruction fail");
        }
    }
    dynamic_circular_buffer(dynamic_circular_buffer&& other) noexcept
        : m_allocator(std::move(other.m_allocator)), m_buffer(other.m_buffer), m_size(other.m_size)
        , m_begin(other.m_begin), m_end(other.m_end), m_head(other.m_head) {
        other.m_buffer = nullptr;
        other.m_begin = nullptr;
        other.m_end = nullptr;
        other.m_head = nullptr;
        other.m_size = 0;
    }
    dynamic_circular_buffer& operator =(const dynamic_circular_buffer& other) {
        Alloc new_allocator = std::allocator_traits<Alloc>::select_on_container_copy_construction(other.m_allocator);
        if (this != std::addressof(other)) {
            pointer new_buffer = new_allocator.allocate(other.m_size);
            pointer it = new_buffer;
            try {
                for (const_pointer other_it = other.m_begin; other_it != other.m_end; ++other_it, ++it)
                    std::allocator_traits<Alloc>::construct(new_allocator, it, std::move(*other_it));
            }
            catch (...) {
                for (pointer del_it = new_buffer; del_it != it; ++del_it)
                    std::allocator_traits<Alloc>::destroy(new_allocator, del_it);
                new_allocator.deallocate(new_buffer, other.m_size);
                throw std::exception("elements consctruction fail");
            }
            for (pointer del_it = m_begin; del_it != m_end; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, m_size);

            m_size = other.m_size;
            m_allocator = std::allocator_traits<Alloc>::select_on_container_copy_construction(other.m_allocator);
            m_buffer = new_buffer;
            m_begin = new_buffer;
            m_end = new_buffer + m_size;
            m_head = m_begin + (other.m_head - other.m_begin);
        }
        return *this;
    }
    dynamic_circular_buffer& operator =(dynamic_circular_buffer&& other) noexcept {
        for (pointer it = m_begin; it != m_end; ++it)
            std::allocator_traits<Alloc>::destroy(m_allocator, it);
        m_allocator.deallocate(m_buffer, m_size);

        m_allocator = std::move(other.m_allocator);
        m_buffer = other.m_buffer;
        m_begin = other.m_begin;
        m_end = other.m_end;
        m_head = other.m_head;
        other.m_buffer = nullptr;
        other.m_begin = nullptr;
        other.m_end = nullptr;
        other.m_head = nullptr;
        other.m_size = 0;
        return *this;
    }

    iterator begin() noexcept {
        return iterator(m_begin, 0, m_size);
    }
    iterator end() noexcept {
        return iterator(m_begin, m_size, m_size);
    }
    const_iterator cbegin() const noexcept {
        return const_iterator(m_begin, 0, m_size);
    }
    const_iterator cend() const noexcept {
        return const_iterator(m_begin, m_size, m_size);
    }

    reference operator [](size_t offset) noexcept {
        return m_buffer[offset];
    }
    reference at(size_t offset) noexcept {
        if (offset >= m_size)
            throw std::out_of_range("Index of out range");
        return m_buffer[offset];
    }
    size_t size() const noexcept {
        return m_size;
    }
    size_t max_size() const noexcept {
        return std::numeric_limits<difference_type>::max();
    }

    reference front() noexcept {
        return m_buffer[0];
    }
    reference back() noexcept {
        return m_buffer[m_size - 1];
    }

    template <typename Iter>
    void insert(iterator it, Iter first, Iter last) {
        if (std::distance(first, last) > m_size || std::distance(first, last) < 0)
            throw std::range_error("incorrect iterators");
        for (; first != last; ++first) {
            this->insert(it++, std::forward<T>(*first));
            if (it == this->end())
                it = this->begin();
        }
    }
    void insert(const_iterator it, T&& val) {
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
        if (n > m_size)
            throw std::range_error("too many elements");
        for (int i = 0; i < n; ++i) {
            this->insert(it++, std::forward<T>(val));
            if (it == this->end())
                it = this->begin();
        }
    }
    
    void push_back(T&& val) {
        T temp = std::move(*m_head);
        try {
            std::allocator_traits<Alloc>::destroy(m_allocator, m_end - 1);
            std::allocator_traits<Alloc>::construct(m_allocator, m_head, std::forward<T>(val));
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
    void pop_back() {
        this->erase(this->end() - 1);
    }
    void pop_front() {
        this->erase(this->begin());
    }
    void erase(iterator erase_it) {
        if (erase_it == this->end())
            throw std::out_of_range("Invalid iterator");
        if (m_size == 0)
            return;
        size_t new_size = m_size - 1;
        pointer new_m_buffer = m_allocator.allocate(new_size);
        pointer other_it = new_m_buffer;
        try {
            for (pointer it = m_begin; it != m_end; ++it) {
                if(it != std::addressof(*erase_it))
                    std::allocator_traits<Alloc>::construct(m_allocator, other_it++, std::move(*it));
            }
        }
        catch (...) {
            for (pointer del_it = new_m_buffer; del_it != other_it; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(new_m_buffer, new_size);
            throw std::exception("elements consctruction fail");
        }
        for (pointer del_it = m_begin; del_it != m_end; ++del_it)
            std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
        m_allocator.deallocate(m_buffer, m_size);
        m_buffer = new_m_buffer;
        m_size = new_size;
        m_head = m_begin = m_buffer;
        m_end = m_buffer + new_size;
    }
    
    void swap(dynamic_circular_buffer& other) noexcept {
        if (this == std::addressof(other))
            return;
        std::swap(this->m_allocator, other.m_allocator);
        std::swap(this->m_begin, other.m_begin);
        std::swap(this->m_end, other.m_end);
        std::swap(this->m_buffer, other.m_buffer);
        std::swap(this->m_head, other.m_head);
        std::swap(this->m_size, other.m_size);
    }
    void clear() {
        for (pointer del_it = m_begin; del_it != m_end; ++del_it)
            std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
        m_allocator.deallocate(m_buffer, m_size);
        m_head = m_begin = m_end = m_buffer = nullptr;
        m_size = 0;
    }
    bool empty() const noexcept {
        return ((m_size == 0) ? true : false);
    }
    void resize(size_t new_size) {
        if (new_size == m_size)
            return;
        if (new_size < m_size) {
            if (new_size == 0)
                this->clear();
            else {
                pointer new_m_buffer = m_allocator.allocate(new_size);
                pointer other_it = new_m_buffer;
                try {
                    for (pointer it = m_begin; other_it != new_m_buffer + new_size; ++it, ++other_it)
                        std::allocator_traits<Alloc>::construct(m_allocator, other_it, std::move(*it));
                }
                catch (...) {
                    for (pointer del_it = new_m_buffer; del_it != other_it; ++del_it)
                        std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
                    m_allocator.deallocate(new_m_buffer, new_size);
                    throw std::exception("elements consctruction fail");
                }
                for (pointer del_it = m_begin; del_it != m_end; ++del_it)
                    std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
                m_allocator.deallocate(m_buffer, m_size);
                m_buffer = new_m_buffer;
                m_size = new_size;
                m_head = m_begin = m_buffer;
                m_end = m_buffer + new_size;
            }
        }
        else {
            pointer new_m_buffer = m_allocator.allocate(new_size);
            pointer other_it = new_m_buffer;
            try {
                for (pointer it = m_begin; it != m_end; ++it, ++other_it)
                    std::allocator_traits<Alloc>::construct(m_allocator, other_it, std::move(*it));
                for (; other_it != new_m_buffer + new_size; ++other_it)
                    std::allocator_traits<Alloc>::construct(m_allocator, other_it, std::move(T()));
            }
            catch (...) {
                for (pointer del_it = new_m_buffer; del_it != other_it; ++del_it)
                    std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
                throw std::exception("elements consctruction fail");
                m_allocator.deallocate(new_m_buffer, new_size);
            }
            for (pointer del_it = m_begin; del_it != m_end; ++del_it)
                std::allocator_traits<Alloc>::destroy(m_allocator, del_it);
            m_allocator.deallocate(m_buffer, m_size);
            m_buffer = new_m_buffer;
            m_size = new_size;
            m_head = m_begin = m_buffer;
            m_end = m_buffer + new_size;
        }
    }

    ~dynamic_circular_buffer() noexcept {
        if (m_buffer == nullptr)
            return;
        for (pointer it = m_begin; it != m_end; ++it)
            std::allocator_traits<Alloc>::destroy(m_allocator, it);
        m_allocator.deallocate(m_buffer, m_size);
    }
private:
    Alloc m_allocator;
    size_t m_size;
    pointer m_buffer;
    pointer m_begin;
    pointer m_end;
    pointer m_head;
};