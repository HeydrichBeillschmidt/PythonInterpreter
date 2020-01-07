#include "BigInteger.h"
using namespace std;

BigInteger::BigInteger(int t) {
    int s = t;
    if (s == 0) sign = 0;
    else if (s < 0) {
        sign = -1;
        s = -s;
    }
    else sign = 1;
    do {
        num.push_back(s % 10);
        s /= 10;
    }while (s > 0);
}

BigInteger::BigInteger(double t) {
    if (t==0) {sign = 0; num.push_back(0);}
    else if (t>0) {
        sign = 1;
        double tmp = floor(t);
        while(tmp!=0) {
            double c = floor(tmp/10);
            num.push_back(int(tmp-c*10));
            tmp = c;
        }
    }
    else {
        sign = -1;
        double tmp = ceil(t);
        while(tmp!=0) {
            double c = ceil(tmp/10);
            num.push_back(int(c*10-tmp));
            tmp = c;
        }
    }
}

BigInteger::BigInteger(const std::string& s) {
    if (s == "0") sign = 0;
    else sign = 1;
    for (int i = s.length()-1; i >= 0; --i) {
        if (s[i] <= '9' && s[i] >= '0') num.push_back(s[i]-'0');
        if (s[i] == '-') sign = -1;
    }
}

int compare_abs(const BigInteger &a, const BigInteger &b) {
    if (!a.sign&&!b.sign) return 0;
    if (!b.sign) return 1;
    if (!a.sign) return -1;
    if (a.num.size()!=b.num.size()) return (a.num.size()>b.num.size()?1:-1);
    else for (int i = a.num.size()-1; i >= 0; --i) {
        if (a.num[i] > b.num[i]) return 1;
        else if (a.num[i] < b.num[i]) return -1;
    }
    return 0;
}

BigInteger Int_minus(const BigInteger &a) {
    BigInteger ans = a;
    ans.sign *= -1;
    return ans;
}

void BigInteger::multi10() {
    num.push_back(0);
    for (int i = num.size()-1; i > 0; --i) num[i] = num[i-1];
    num[0] = 0;
}

BigInteger BigInteger::operator+(const BigInteger &oth) const {
    if (!this->sign) return oth;
    if (!oth.sign) return *this;
    int comp = compare_abs(*this,oth);
    if (comp==-1) return oth.operator+(*this);
    if (!comp && sign*oth.sign==-1) return BigInteger(0);
    BigInteger ans = *this, tmp(oth);
    ans.num.resize(ans.num.size()+1,0);
    if (sign != oth.sign) {
        for (int i = 0, it = oth.num.size(); i < it; ++i) tmp.num[i] *= -1;
    }
    for (int i = 0, it = oth.num.size(); i < it; ++i) {ans.num[i] += tmp.num[i];}
    for (int i = 0, it = ans.num.size(); i < it; ++i) {
        if (ans.num[i] < 0) {ans.num[i] += 10; --ans.num[i+1];}
        if (ans.num[i] >= 10) {int x = ans.num[i] / 10; ans.num[i] %= 10; ans.num[i+1] += x;}
    }
    while (!ans.num.back()) {ans.num.pop_back();}
    return ans;
}

BigInteger BigInteger::operator*(const BigInteger &oth) const {
    if (!this->sign || !oth.sign) return BigInteger(0);
    BigInteger ans(0);
    ans.sign = sign * oth.sign;
    ans.num.resize(num.size()+oth.num.size(),0);
    for (int i = 0, it = num.size(); i < it; ++i) {
        for (int j = 0, jt = oth.num.size(); j < jt; ++j) {
            int tmp = num[i] * oth.num[j];
            ans.num[i+j] += tmp;
        }
    }
    for (int i = 0, it = ans.num.size(); i < it; ++i) {
        if (ans.num[i] >= 10) {int x = ans.num[i] / 10; ans.num[i] %= 10; ans.num[i+1] += x;}
    }
    while (!ans.num.back()) {ans.num.pop_back();}
    return ans;
}

BigInteger BigInteger::operator-(const BigInteger &oth) const {
    if (!oth.sign) return *this;
    return this->operator+(Int_minus(oth));
}

BigInteger BigInteger::operator/(const BigInteger &oth) const {
    if (!sign) return BigInteger(0);
    if (!compare_abs(*this,oth)) return BigInteger(sign * oth.sign);
    BigInteger tmp = *this, tdi = oth, ans(0);
    tmp.sign = tdi.sign = 1;
    while (tmp > tdi) {
        BigInteger di = tdi, t(1);
        while (tmp >= di*BigInteger(10)) {
            di.multi10();
            t.multi10();
        }
        int q = 1;
        while (tmp >= di*BigInteger(q+1)) {++q;}
        tmp -= di*BigInteger(q);
        ans += BigInteger(q) * t;
    }
    ans.sign = sign * oth.sign;
    if (ans.sign == -1 && compare_abs(tmp, BigInteger(0))==1) --ans;
    return ans;
}

BigInteger BigInteger::operator%(const BigInteger &oth) const {
    if (!sign) return BigInteger(0);
    if (compare_abs(*this,oth)==-1) {return (sign*oth.sign==1? *this: *this+oth);}
    return this->operator-(oth.operator*(this->operator/(oth)));
}

bool operator==(const BigInteger &a, const BigInteger &b) {
    if (a.sign != b.sign) return false;
    return !compare_abs(a,b);
}

bool operator!=(const BigInteger &a, const BigInteger &b) {return !(a==b);}

bool operator<(const BigInteger &a, const BigInteger &b) {
    if (a.sign != b.sign) return a.sign < b.sign;
    if (a == b) return false;
    return a.sign != compare_abs(a, b);
}

bool operator>(const BigInteger &a, const BigInteger &b) {
    if (a.sign != b.sign) return a.sign > b.sign;
    if (a == b) return false;
    return a.sign == compare_abs(a, b);
}

bool operator<=(const BigInteger &a, const BigInteger &b) {return !(a > b);}

bool operator>=(const BigInteger &a, const BigInteger &b) {return !(a < b);}

std::ostream &operator<<(std::ostream &os, const BigInteger &obj) {
    if (obj.sign==-1) os << '-';
    for (int i = obj.num.size()-1; i >= 0; --i) os << obj.num[i];
    return os;
}

std::istream &operator>>(std::istream &is, BigInteger &obj) {
    std::string t;
    is >> t;
    obj = BigInteger(t);
    return is;
}

BigInteger::operator double() const {
    double tmp = 0;
    for (int i = num.size()-1; i; --i) {tmp += num[i]; tmp *= 10;}
    tmp += num[0];
    if (sign==-1) tmp = -tmp;
    return tmp;
}

BigInteger::operator int() const {
    int tmp = 0;
    for (int i = num.size()-1; i; --i) {tmp += num[i]; tmp *= 10;}
    tmp += num[0];
    if (sign==-1) tmp = -tmp;
    return tmp;
}

BigInteger::operator bool() const {return bool(sign);}

BigInteger::operator string() const {
    std::string tmp;
    if (sign==-1) tmp.append("-");
    tmp.append(string(num.crbegin(),num.crend()));
    for (auto &c:tmp) if(c != '-') c += '0';
    return tmp;
}