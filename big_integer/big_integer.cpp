#include "big_integer.hpp"
#include <bits/stdc++.h>

BigInt::BigInt() : digits_("") {};
BigInt::BigInt(std::string number) { *this = number; }
BigInt::BigInt(int64_t number) { *this = std::to_string(number);
}

std::ostream& operator<<(std::ostream& os, const BigInt& big_int) {
  if (big_int.sign_ < 0) {
    os << '-';
  }
  int size = big_int.digits_.size() - 1;
  while (size >= 0) {
    os << big_int.digits_[size] - '0';
    --size;
  }
  return os;
}

std::istream& operator>>(std::istream& is, BigInt& big_int) {
  std::string temp_str;
  is >> temp_str;
  BigInt temp(temp_str);
  big_int = temp;
  return is;
}
BigInt BigInt::operator-() {
  if (!this->digits_.empty() && this->digits_[0] != '0') {
    if (sign_ == 1) {
      sign_ = -1;
    } else {
      sign_ = 1;
    }
    return *this;
  }
  sign_ = 1;
  return *this;
}

BigInt BigInt::Negative() {
  BigInt x = *this;
  x.sign_ *= -1;
  return x;
}


BigInt& BigInt::operator--() { return *this = *this - 1; }
BigInt& BigInt::operator++() { return *this = *this + 1; }
BigInt BigInt::operator++(int) {
  BigInt cp(*this);
  ++*this;
  return cp;
}
BigInt BigInt::operator--(int) {
  BigInt cp(*this);
  --*this;
  return cp;
}

BigInt BigInt::Normalize(size_t newSign) {
  for (size_t i = digits_.size() - 1; i > 0 && digits_[i] == '0'; i--)
    digits_.erase(digits_.begin() + i);
  if (digits_.size() == 1 && digits_[0] == '0') {
    sign_ = 1;
  } else {
    sign_ = newSign;
  };
  return *this;
}

std::string BigInt::Reverse(const std::string& str) {
  std::string cp = str;
  std::string res;
  while (!cp.empty()) {
    res.push_back(cp.back());
    cp.pop_back();
  }
  return res;
}

BigInt& BigInt::operator=(std::string& str) {
  int newSign = 0;
  if (!str.empty()) {
    if (str[0] == '-') {
      newSign = -1;
    } else {
      newSign = 1;
    }
  }
  if  (newSign == -1) {
    digits_ = str.substr(1);
  } else {
    digits_ = str;
  }
  digits_ = Reverse(digits_);
  this->Normalize(newSign);
  return *this;
}

bool BigInt::operator==(const BigInt& x) const {
  return (digits_ == x.digits_ && sign_ == x.sign_);
}

bool BigInt::operator!=(const BigInt& big_int) const {
  return !(*this == big_int);
}

bool BigInt::operator<(const BigInt& x) const {
  if (sign_ != x.sign_) { return sign_ < x.sign_; }
  if (digits_.size() != x.digits_.size()) {
    return (sign_ == 1 ? digits_.size() < x.digits_.size() : digits_.size() > x.digits_.size());
  }
  for (int i = digits_.size() - 1; i >= 0; i--)
    if (digits_[i] != x.digits_[i]) {
      if (sign_ == 1) {
        return digits_[i] < x.digits_[i];
      }
      return digits_[i] > x.digits_[i];
    }
  return false;
}

bool BigInt::operator<=(const BigInt& big_int) const {
  return (*this < big_int || *this == big_int);
}

bool BigInt::operator>(const BigInt& big_int) const {
  return (!(*this < big_int) && !(*this == big_int));
}

bool BigInt::operator>=(const BigInt& big_int) const {
  return (*this > big_int || *this == big_int);
}

BigInt BigInt::operator+(BigInt big_int) {
  BigInt curr = *this;
  if (curr.sign_ != big_int.sign_) { return curr - big_int.Negative(); }
  BigInt res;
  size_t carry = 0;
  for (size_t a = 0; a < digits_.size() || a < big_int.digits_.size() || carry; a++) {
    carry += (a < curr.digits_.size() ? curr.digits_[a] - '0' : 0) +
             (a < big_int.digits_.size() ? big_int.digits_[a] - '0' : 0);
    res.digits_ += (carry % 10 + '0');
    carry /= 10;
  }
  return res.Normalize(sign_);
}

BigInt BigInt::operator-(BigInt big_int) {
  BigInt curr = *this;
  if (curr.sign_ != big_int.sign_) return curr + big_int.Negative();
  size_t realSign = curr.sign_;
  curr.sign_ = big_int.sign_ = 1;
  if (curr < big_int) return ((big_int - curr).Negative()).Normalize(-realSign);
  BigInt res;
  for (int i = 0, borrow = 0; i < int(digits_.size()); i++) {
    borrow = (curr.digits_[i] - borrow - (i < int(big_int.digits_.size()) ? big_int.digits_[i] : '0'));
    if (borrow >= 0) {
      res.digits_ += borrow + '0';
    } else {
      res.digits_ += borrow + '0' + 10;
    }
    borrow = (borrow >= 0 ? 0 : 1);
  }
  return res.Normalize(realSign);
}

BigInt BigInt::operator*(BigInt big_int) {
  BigInt res("0");
  for (size_t a = 0, b = digits_[a] - '0'; a < digits_.size(); a++, b = digits_[a] - '0') {
    while (b--) res = (res + big_int);
    big_int.digits_.insert(big_int.digits_.begin(), '0');
  }
  return res.Normalize(sign_ * big_int.sign_);
}

BigInt BigInt::operator/(BigInt big_int) {
  if (big_int.digits_.size() == 1 && big_int.digits_[0] == '0')
    big_int.digits_[0] /= (big_int.digits_[0] - '0');
  BigInt temp("0"), res;
  for (size_t a = 0; a < digits_.size(); a++) res.digits_ += "0";
  size_t newSign = sign_ * big_int.sign_;
  big_int.sign_ = 1;
  for (int a = digits_.size() - 1; a >= 0; a--) {
    temp.digits_.insert(temp.digits_.begin(), '0');
    temp = temp + digits_.substr(a, 1);
    while (!(temp < big_int)) {
      temp = temp - big_int;
      res.digits_[a]++;
    }
  }
  return res.Normalize(newSign);
}

BigInt BigInt::operator%(BigInt big_int) {
  if (big_int.digits_.size() == 1 && big_int.digits_[0] == '0')
    big_int.digits_[0] /= char(big_int.digits_[0] - '0');
  BigInt res("0");
  big_int.sign_ = 1;
  for (int a = digits_.size() - 1; a >= 0; a--) {
    res.digits_.insert(res.digits_.begin(), '0');
    res = res + digits_.substr(a, 1);
    while (!(res < big_int)) res = res - big_int;
  }
  return res.Normalize(sign_);
}

std::string BigInt::toString() const {
  std::string ret = Reverse(digits_);
  return (sign_ == -1 ? "-" : "") + ret;
}