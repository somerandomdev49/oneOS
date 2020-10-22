#pragma once
#include <libcxx/cstring.h>
#include <libcxx/malloc.h>
#include <libcxx/memory.h>

namespace Algo {

class String {
public:
    String() = default;

    String(const char* str)
    {
        m_size = strlen(str);
        ensure_capacity(m_size);
        memcpy((uint8_t*)m_str, (uint8_t*)str, m_size);
    }

    String(const String& s)
    {
        grow(s.m_capacity);
        m_size = s.m_size;
        memcpy((uint8_t*)m_str, (uint8_t*)s.m_str, m_size);
    }

    String(String&& s)
    {
        m_size = s.m_size;
        m_capacity = s.m_capacity;
        m_str = s.m_str;
        s.m_size = s.m_capacity = 0;
        s.m_str = nullptr;
    }

    ~String()
    {
        clear();
    }

    String& operator=(const String& s)
    {
        grow(s.m_capacity);
        m_size = s.m_size;
        memcpy((uint8_t*)m_str, (uint8_t*)s.m_str, m_size);
        return *this;
    }

    String& operator=(String&& s)
    {
        if (this != &s) {
            clear();
            m_size = s.m_size;
            m_capacity = s.m_capacity;
            m_str = s.m_str;
            s.m_size = s.m_capacity = 0;
            s.m_str = nullptr;
        }
        return *this;
    }

    String& operator+=(const String& s)
    {
        ensure_capacity(size() + s.size());
        memcpy((uint8_t*)end(), (uint8_t*)s.m_str, s.size());
        m_size = s.size();
        return *this;
    }

    inline void push_back(char c)
    {
        ensure_capacity(size() + 2);
        m_str[m_size++] = c;
        m_str[m_size] = '\0';
    }

    inline const char& at(size_t i) const
    {
        return m_str[i];
    }

    inline char& at(size_t i)
    {
        return m_str[i];
    }

    void clear()
    {
        if (m_str) {
            free(m_str);
            m_str = nullptr;
        }
    }

    inline size_t size() const { return m_size; }

    inline const char& operator[](size_t i) const { return at(i); }
    inline char& operator[](size_t i) { return at(i); }

    inline const char& back() const { return at(size() - 1); }
    inline char& back() { return at(size() - 1); }

    // This function is frozen for now, need to add /0 to the end always
    // inline const char* c_str() { return m_str; } 

private:
    inline void ensure_capacity(size_t new_size)
    {
        if (new_size > m_capacity) {
            grow(new_size + 16);
        }
    }

    void grow(size_t capacity)
    {
        if (capacity < m_capacity) {
            return;
        }

        if (!m_str) {
            m_str = (char*)malloc(capacity);
        } else {
            m_str = (char*)realloc(m_str, capacity);
        }
        m_capacity = capacity;
    }

    inline char* end() { return m_str + m_size; }

    size_t m_size { 0 };
    size_t m_capacity { 0 };
    char* m_str { nullptr };
};
}

using Algo::String;