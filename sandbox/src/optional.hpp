/*
 * taken from http://stackoverflow.com/questions/11875045/implementing-boostoptional-in-c11
 */
#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP
#include <exception>
#include <array>
#include <type_traits>

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
    optional(const T& _v)
    {
        valid = true;
        new (&value) T(_v);
    }
    
	#ifdef WIN32
	template<class A>
	optional(A a)
	{
		valid = true;
        new(&value) T(a);
	}
	#else
    template<typename... Args>
    optional(Args... args)
    {
        valid = true;
        new(&value) T(args...);
    }
	#endif
    
    // copy constructor
    optional(const optional& other)
    {
        if (other.valid) {
            valid = true;
            new (&value) T(other.value);
        }
        else valid = false;
    }
    
    optional& operator=(optional other)
    {
        std::swap(*this, other);
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
        std::swap(*this, other);
    }
    
    friend void swap(optional& first, optional& second)
    {
        std::swap(first.valid, second.valid);
        std::swap(first.data, second.data);
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
        if (valid) return value;
        throw std::bad_exception();
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
    optional(const T3 _v) : value(&_v) {}
    
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
        std::swap(*this, other);
    }
    
    friend void swap(optional& first, optional& second)
    {
        std::swap(first.value, second.value);
    }

    ~optional()
    {
    }

    bool operator!() const { return !value; }
    
    operator bool() const { return value; }

    T& operator*()
    {
        if (value) return *value;
        throw std::bad_exception();
    }
};
#endif // OPTIONAL_HPP