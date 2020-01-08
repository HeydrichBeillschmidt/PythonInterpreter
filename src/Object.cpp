#include "Object.h"
using namespace std;

void Object::upper_type() {
    if (type==BOOL) {type = INT; data_INT = BigInteger(data_BOOL); return;}
    else if (type==INT) {type = FLOAT; data_FLOAT = double(data_INT); return;}
    else if (type==FLOAT) {type = STRING; data_STRING = to_string(data_FLOAT); return;}
    else ;
}

Object Object::operator+(const Object &oth) const {
    Object a(*this), b(oth);
    if (a.type<b.type) swap(a,b);
    while (b.type<a.type) b.upper_type();
    if (a.type==STRING) {a.data_STRING = a.data_STRING + b.data_STRING; return a;}
    else if (a.type==FLOAT) {a.data_FLOAT = a.data_FLOAT + b.data_FLOAT; return a;}
    else if (a.type==INT) {a.data_INT = a.data_INT + b.data_INT; return a;}
    else {a.type=INT; a.data_INT = BigInteger(a.data_BOOL + b.data_BOOL); return a;}
}

Object Object::operator-(const Object &oth) const {
    Object a(*this), b(oth);
    typeT fix_type = fit_type(a, b);
    while (a.type<fix_type) a.upper_type();
    while (b.type<fix_type) b.upper_type();
    if (fix_type==FLOAT) {a.data_FLOAT = a.data_FLOAT - b.data_FLOAT; return a;}
    else if (fix_type==INT) {a.data_INT = a.data_INT - b.data_INT; return a;}
    else {a.type=INT; a.data_INT = BigInteger(a.data_BOOL) - BigInteger(b.data_BOOL); return a;}
}

Object Object::operator*(const Object &oth) const {
    Object a(*this), b(oth);
    if (a.type<b.type) swap(a,b);
    if (a.type==STRING) {
        if (b.type==INT) {
            int it = int(b.data_INT);
            if (it<=0) {a.data_STRING = ""; return a;}
            else {
                string tmp = a.data_STRING;
                for (int i = 1; i < it; ++i) {a.data_STRING = a.data_STRING + tmp;}
                return a;
            }
        }
        else ;
    }
    else {
        while (b.type<a.type) b.upper_type();
        if (a.type==FLOAT) {a.data_FLOAT = a.data_FLOAT * b.data_FLOAT; return a;}
        else if (a.type==INT) {a.data_INT = a.data_INT * b.data_INT; return a;}
        else {a.type=INT; a.data_INT = BigInteger(a.data_BOOL && b.data_BOOL); return a;}
    }
    return a;
}

Object Object::operator/(const Object &oth) const {
    Object a(*this), b(oth);
    return(Object(BigInteger(a) / BigInteger(b)));
}

Object Object::operator%(const Object &oth) const {
    Object a(*this), b(oth);
    return(Object(BigInteger(a) % BigInteger(b)));
}

Object float_devision(const Object &a, const Object &b) {
    return Object(double(a) / double(b));
}

Object Object::self_devision(const Object &oth) {
    return *this = float_devision(*this, oth);
}

bool operator==(const Object &a, const Object &b) {
    if (a.type!=b.type) {
        if (a.type==STRING || b.type==STRING) return false;
        else return (double(a)==double(b));
    }
    if (a.type==STRING) return a.data_STRING==b.data_STRING;
    else if (a.type==FLOAT) return a.data_FLOAT==b.data_FLOAT;
    else if (a.type==INT) return a.data_INT==b.data_INT;
    else return a.data_BOOL==b.data_BOOL;
}

bool operator!=(const Object &a, const Object &b) {return !(a==b);}

bool operator<(const Object &a, const Object &b) {
    if (a==b) return false;
    typeT fix_type = fit_type(a, b);
    if (fix_type==STRING) {return a.data_STRING<b.data_STRING;}
    else if (fix_type == FLOAT) {
        Object c(a), d(b);
        fix_type = FLOAT;
        while (c.type<fix_type) c.upper_type();
        while (d.type<fix_type) d.upper_type();
        return c.data_FLOAT<d.data_FLOAT;
    }
    else {
        Object c(a), d(b);
        fix_type = INT;
        while (c.type<fix_type) c.upper_type();
        while (d.type<fix_type) d.upper_type();
        return c.data_INT<d.data_INT;
    }
}

bool operator>(const Object &a, const Object &b) {
    if (a==b) return false;
    typeT fix_type = fit_type(a, b);
    if (fix_type==STRING) {return a.data_STRING>b.data_STRING;}
    else if (fix_type == FLOAT) {
        Object c(a), d(b);
        fix_type = FLOAT;
        while (c.type<fix_type) c.upper_type();
        while (d.type<fix_type) d.upper_type();
        return c.data_FLOAT>d.data_FLOAT;
    }
    else {
        Object c(a), d(b);
        fix_type = INT;
        while (c.type<fix_type) c.upper_type();
        while (d.type<fix_type) d.upper_type();
        return c.data_INT>d.data_INT;
    }
}

bool operator<=(const Object &a, const Object &b) {return !(a > b);}

bool operator>=(const Object &a, const Object &b) {return !(a < b);}

std::ostream &operator<<(std::ostream &os, const Object &obj) {
    if (obj.type==STRING) os << obj.data_STRING;
    else if (obj.type==FLOAT) os << obj.data_FLOAT;
    else if (obj.type==INT) os << obj.data_INT;
    else if (obj.type==BOOL) os << (obj.data_BOOL? "True": "False");
    else os << "None";
    return os;
}

Object::operator bool() const {
    if (type==STRING) return !data_STRING.empty();
    else if (type==INT) return bool(data_INT);
    else if (type==FLOAT) return data_FLOAT != 0;
    else return data_BOOL;
}

Object::operator string() const {
    if (type==STRING) return data_STRING;
    else if (type==INT) return string(data_INT);
    else if (type==FLOAT) {return to_string(data_FLOAT);}
    else {
        string tmp = (data_BOOL? "True": "False");
        return tmp;
    }
}

Object::operator BigInteger() const {
    if (type==FLOAT) return BigInteger(data_FLOAT);
    else if (type==INT) return data_INT;
    else return BigInteger(data_BOOL);
}

Object::operator double() const {
    if (type==INT) return double(data_INT);
    else if (type==FLOAT) return data_FLOAT;
    else return double(data_BOOL);
}
