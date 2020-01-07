#ifndef PYTHON_OBJECT
#define PYTHON_OBJECT

#include <iostream>
#include <iomanip>
#include <utility>
#include "BigInteger.h"
using namespace std;

enum typeT {NONE, BOOL, INT, FLOAT, STRING};

class Object{
    friend void swap(Object &a, Object &b) {Object c; c = a; a = b; b = c;}
    friend typeT fit_type(const Object &a, const Object &b) {return a.type>=b.type? a.type: b.type;}
    friend Object float_devision(const Object &a, const Object &b);
    friend bool operator==(const Object &a, const Object &b);
    friend bool operator!=(const Object &a, const Object &b);
    friend bool operator<(const Object &a, const Object &b);
    friend bool operator>(const Object &a, const Object &b);
    friend bool operator<=(const Object &a, const Object &b);
    friend bool operator>=(const Object &a, const Object &b);
    friend std::ostream &operator<<(std::ostream &os, const Object &obj);
    private:
        bool data_BOOL;
        BigInteger data_INT;
        double data_FLOAT;
        string data_STRING;
    public:
        typeT type;
        explicit Object(const BigInteger& int_val): type(INT), data_INT(int_val) {}
        explicit Object(double float_val): type(FLOAT), data_FLOAT(float_val) {}
        explicit Object(bool bool_val): type(BOOL), data_BOOL(bool_val) {}
        explicit Object(std::string str_val): type(STRING), data_STRING(std::move(str_val)) {}
        Object(): type(NONE) {}
        Object(const Object &oth) = default;
        Object(Object &&oth) = default;
        Object &operator=(const Object &oth) = default;
        Object &operator=(Object &&oth) = default;
        void upper_type();
        Object operator+(const Object &oth) const;
        Object operator-(const Object &oth) const;
        Object operator*(const Object &oth) const;
        Object operator/(const Object &oth) const;
        Object operator%(const Object &oth) const;
        Object operator+=(const Object &oth) {return *this = *this + oth;}
        Object operator-=(const Object &oth) {return *this = *this - oth;}
        Object operator*=(const Object &oth) {return *this = *this * oth;}
        Object operator/=(const Object &oth) {return *this = *this / oth;}
        Object operator%=(const Object &oth) {return *this = *this % oth;}
        Object self_devision(const Object &oth);
        explicit operator bool() const;
        explicit operator string() const;
        explicit operator BigInteger() const;
        explicit operator double() const;
};

#endif