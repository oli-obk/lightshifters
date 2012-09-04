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
        if (other.valid) {
            valid = true;
            new (&value) T(other.value);
        }
        else valid = false;
    }
    
    optional& operator=(const optional& other)
    {
        valid = other.valid;
        std::copy(std::begin(other.data), std::end(other.data), std::begin(data));
        return *this;
    }
    
    // move constructor
    optional(optional&& other)
	#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)
	:optional()
    {
    #else
    {
        valid = false;
	#endif
        valid = other.valid;
        other.valid = false;
        std::copy(std::begin(other.data), std::end(other.data), std::begin(data));
    }

    ~optional()
    {
        if (valid)
        value.~T();
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
    
    void reset(T t)
    {
        *this = optional<T>(t);
    }
    
    void reset()
    {
        *this = optional<T>();
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