/*
 * taken from http://stackoverflow.com/questions/11875045/implementing-boostoptional-in-c11
 */
#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP
#include <exception>
#include <array>
#include <type_traits>
#include <cassert>

template<typename T, class Enable = void>
class optional;

template<typename T>
class optional<T, typename std::enable_if<!std::is_reference<T>::value>::type>
{
private:
    bool valid;

    union {
        T value;
        std::array<char, sizeof(T)> data;
    };
    
    void destruct_if_valid()
    {
        if (valid) {
            value.~T();
        }
    }
    
public:
    // default constructor
    optional() : valid(false) {}
    
    // initializer
    explicit optional(const T& _v)
    {
        valid = true;
        new (&value) T(_v);
    }
    
    // copy constructor
    optional(const optional& other)
    {
        valid = other.valid;
        if (valid) {
            new (&value) T(other.value);
        }
    }
    
    optional& operator=(const optional& other)
    {
        destruct_if_valid();
        valid = other.valid;
        new (&value) T(other.value);
        return *this;
    }
    
    // move constructor
    optional(optional&& other)
    {
        valid = other.valid;
        other.valid = false;
        value = std::move(other);
    }

    ~optional()
    {
        destruct_if_valid();
    }

    bool operator!() const { return !valid; }
    
    operator bool() const { return valid; }

    T& operator*()
    {
        assert(valid);
        if (valid) return value;
        throw std::bad_exception();
    }
    
    const T& operator*() const
    {
        assert(valid);
        if (valid) return value;
        throw std::bad_exception();
    }
    
    T* operator->()
    {
        assert(valid);
        if (valid) return &value;
        throw std::bad_exception();
    }
    
    const T* operator->() const
    {
        assert(valid);
        if (valid) return &value;
        throw std::bad_exception();
    }
    
    void reset(const T& t)
    {
        destruct_if_valid();
        valid = true;
        value = t;
    }
    
    void reset()
    {
        destruct_if_valid();
        valid = false;
    }
};

template<typename T3>
class optional<T3, typename std::enable_if<std::is_reference<T3>::value>::type >
{
private:
    typedef typename std::remove_reference<T3>::type T;
    T* value;

public:
    // default constructor
    optional() : value(nullptr) {}
    
    // initializer
    explicit optional(const T3 _v) : value(&_v) {}
    
    // copy constructor
    optional(const optional& other)
    {
        value = other.value;
    }
    
    optional& operator=(optional other)
    {
        value = other.value;
        return *this;
    }
    
    // move constructor
    optional(optional&& other)
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
	:optional()
    {
    #else
    {
        value = nullptr;
	#endif
        std::swap(value, other.value);
    }

    ~optional()
    {
    }

    bool operator!() const { return !value; }
    
    operator bool() const { return value; }

    T& operator*()
    {
        assert(value);
        if (value) return *value;
        throw std::bad_exception();
    }
    
    const T& operator*() const
    {
        assert(value);
        if (value) return *value;
        throw std::bad_exception();
    }
    
    T* operator->()
    {
        assert(value);
        if (value) return value;
        throw std::bad_exception();
    }
    
    const T* operator->() const
    {
        assert(value);
        if (value) return value;
        throw std::bad_exception();
    }
    
    void reset(T3 t)
    {
        value = &t;
    }
    
    void reset()
    {
        value = nullptr;
    }
};
#endif // OPTIONAL_HPP