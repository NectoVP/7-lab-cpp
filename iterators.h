#pragma once

template<typename T>
class circ_buff_const_iter {
public:
    using value_type = T;
    using reference = const T&;
    using pointer = const T*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    circ_buff_const_iter(pointer buffer, size_t index, size_t capacity)
        : m_buffer(buffer), m_index(index), m_capacity(capacity) {}
    circ_buff_const_iter(const circ_buff_const_iter& other)
        : m_buffer(other.m_buffer), m_index(other.m_index), m_capacity(other.m_capacity) {}

    reference operator*() {
        return m_buffer[m_index % m_capacity];
    }
    pointer operator->() {
        return &m_buffer[m_index % m_capacity];
    }

    circ_buff_const_iter& operator++() {
        ++m_index;
        return *this;
    }
    circ_buff_const_iter operator++(int) {
        circ_buff_const_iter temp(*this);
        ++(*this);
        return temp;
    }

    circ_buff_const_iter& operator--() {
        --m_index;
        return *this;
    }
    circ_buff_const_iter operator--(int) {
        circ_buff_const_iter temp(*this);
        --(*this);
        return temp;
    }

    circ_buff_const_iter& operator+(const difference_type n) const {
        return circ_buff_const_iter(*this) += n;
    }
    circ_buff_const_iter& operator-(const difference_type n) const {
        return circ_buff_const_iter(*this) -= n;
    }

    circ_buff_const_iter& operator+=(const difference_type n) {
        m_index += n;
        return *this;
    }
    circ_buff_const_iter& operator-=(const difference_type n) {
        m_index -= n;
        return *this;
    }

    difference_type operator-(const circ_buff_const_iter& other) const {
        return m_index - other.m_index;
    }

    bool operator==(const circ_buff_const_iter& other) const {
        return m_index == other.m_index;
    }
    bool operator!=(const circ_buff_const_iter& other) const {
        return !(*this == other);
    }

    bool operator<(const circ_buff_const_iter& other) const {
        return m_index < other.m_index;
    }
    bool operator>(const circ_buff_const_iter& other) const {
        return m_index > other.m_index;
    }

    bool operator<=(const circ_buff_const_iter& other) const {
        return m_index <= other.m_index;
    }
    bool operator>=(const circ_buff_const_iter& other) const {
        return m_index >= other.m_index;
    }

private:
    pointer m_buffer;
    size_t m_index;
    size_t m_capacity;
};

template<typename T>
class circ_buff_iter {
public:
    using value_type = T;
    using reference = T&;
    using pointer = T*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    circ_buff_iter(pointer buffer, size_t index, size_t capacity)
        : m_buffer(buffer), m_index(index), m_capacity(capacity) {}
    circ_buff_iter(const circ_buff_iter& other)
        : m_buffer(other.m_buffer), m_index(other.m_index), m_capacity(other.m_capacity) {}

    reference operator* () {
        return m_buffer[m_index % m_capacity];
    }
    pointer operator->() {
        return &m_buffer[m_index % m_capacity];
    }

    circ_buff_iter& operator++() {
        ++m_index;
        return *this;
    }
    circ_buff_iter operator++(int) {
        circ_buff_iter temp(*this);
        ++(*this);
        return temp;
    }

    circ_buff_iter& operator--() {
        --m_index;
        return *this;
    }
    circ_buff_iter operator--(int) {
        circ_buff_iter temp(*this);
        --(*this);
        return temp;
    }

    circ_buff_iter& operator+(const difference_type n) const {
        return circ_buff_iter(*this) += n;
    }
    circ_buff_iter& operator-(const difference_type n) const {
        return circ_buff_iter(*this) -= n;
    }

    circ_buff_iter& operator+=(const difference_type n) {
        m_index += n;
        return *this;
    }
    circ_buff_iter& operator-=(const difference_type n) {
        m_index -= n;
        return *this;
    }

    difference_type operator-(const circ_buff_iter& other) const {
        return m_index - other.m_index;
    }

    bool operator==(const circ_buff_iter& other) const {
        return m_index == other.m_index;
    }
    bool operator!=(const circ_buff_iter& other) const {
        return !(*this == other);
    }

    bool operator<(const circ_buff_iter& other) const {
        return m_index < other.m_index;
    }
    bool operator>(const circ_buff_iter& other) const {
        return m_index > other.m_index;
    }

    bool operator<=(const circ_buff_iter& other) const {
        return m_index <= other.m_index;
    }
    bool operator>=(const circ_buff_iter& other) const {
        return m_index >= other.m_index;
    }

    operator circ_buff_const_iter<T>() const {
        return circ_buff_const_iter<T>(m_buffer, m_index, m_capacity);
    }

private:
    pointer m_buffer;
    size_t m_index;
    size_t m_capacity;
};