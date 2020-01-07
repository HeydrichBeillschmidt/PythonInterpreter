#ifndef PYTHON_BIGINTEGER
#define PYTHON_BIGINTEGER

#include <vector>
#include <string>
#include <iostream>
#include <cmath>
using namespace std;

class BigInteger {
    friend int compare_abs(const BigInteger &a, const BigInteger &b);
    friend BigInteger Int_minus(const BigInteger &a);
    friend bool operator==(const BigInteger &a, const BigInteger &b);
    friend bool operator!=(const BigInteger &a, const BigInteger &b);
    friend bool operator<(const BigInteger &a, const BigInteger &b);
    friend bool operator>(const BigInteger &a, const BigInteger &b);
    friend bool operator<=(const BigInteger &a, const BigInteger &b);
    friend bool operator>=(const BigInteger &a, const BigInteger &b);
    friend std::ostream &operator<<(std::ostream &os, const BigInteger &obj);
    friend std::istream &operator>>(std::istream &is, BigInteger &obj);
    private:
        std::vector<int> num;
        int sign;
    public:
        explicit BigInteger(int t = 0);
        explicit BigInteger(const std::string& s);
        explicit BigInteger(double t);
        BigInteger(const BigInteger &oth) = default;
        BigInteger &operator=(const BigInteger &oth) = default;
        void multi10();
        BigInteger operator+(const BigInteger &oth) const;
        BigInteger operator*(const BigInteger &oth) const;
        BigInteger operator-(const BigInteger &oth) const;
        BigInteger operator/(const BigInteger &oth) const;
        BigInteger operator%(const BigInteger &oth) const;
        BigInteger operator+=(const BigInteger &oth) {return *this = *this + oth;}
        BigInteger operator-=(const BigInteger &oth) {return *this = *this - oth;}
        BigInteger operator*=(const BigInteger &oth) {return *this = *this * oth;}
        BigInteger operator/=(const BigInteger &oth) {return *this = *this / oth;}
        BigInteger operator%=(const BigInteger &oth) {return *this = *this % oth;}
        BigInteger &operator++() {return *this = *this + BigInteger(1);}
        BigInteger operator++(int x) {BigInteger tmp = *this; ++*this; return tmp;}
        BigInteger &operator--() {return *this = *this - BigInteger(1);}
        BigInteger operator--(int x) {BigInteger tmp = *this; --*this; return tmp;}
        explicit operator double() const;
        explicit operator bool() const;
        explicit operator int() const;
        explicit operator string() const;
        ~BigInteger() {num.clear();}
};

#endif