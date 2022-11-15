#include "big_integer.hpp"
#include <bits/stdc++.h>

const int kKten = 10;

BigInt::BigInt() : digits_("") {}
BigInt::BigInt(std::string number) { *this = number; }
BigInt::BigInt(int64_t number) { *this = std::to_string(number); }

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
  BigInt big_int = *this;
  big_int.sign_ *= -1;
  return big_int;
}

BigInt& BigInt::operator--() { return *this = *this - 1; }
BigInt& BigInt::operator++() { return *this = *this + 1; }
BigInt BigInt::operator++(int) {
  BigInt copy(*this);
  ++*this;
  return copy;
}
BigInt BigInt::operator--(int) {
  BigInt copy(*this);
  --*this;
  return copy;
}

BigInt BigInt::Normalize(size_t new_sign) {
  for (size_t i = digits_.size() - 1; i > 0 && digits_[i] == '0'; i--) {
    digits_.erase(digits_.begin() + i);
  }
  if (digits_.size() == 1 && digits_[0] == '0') {
    sign_ = 1;
  } else {
    sign_ = new_sign;
  }
  return *this;
}

std::string BigInt::Reverse(const std::string& str) {
  std::string copy = str;
  std::string res;
  while (!copy.empty()) {
    res.push_back(copy.back());
    copy.pop_back();
  }
  return res;
}

BigInt& BigInt::operator=(std::string& str) {
  int new_sign = 0;
  if (!str.empty()) {
    if (str[0] == '-') {
      new_sign = -1;
    } else {
      new_sign = 1;
    }
  }
  if (new_sign == -1) {
    digits_ = str.substr(1);
  } else {
    digits_ = str;
  }
  digits_ = Reverse(digits_);
  this->Normalize(new_sign);
  return *this;
}

bool BigInt::operator==(const BigInt& big_int) const {
  return (digits_ == big_int.digits_ && sign_ == big_int.sign_);
}

bool BigInt::operator!=(const BigInt& big_int) const {
  return !(*this == big_int);
}

bool BigInt::operator<(const BigInt& big_int) const {
  if (sign_ != big_int.sign_) { return sign_ < big_int.sign_; }
  if (digits_.size() != big_int.digits_.size()) {
    return (sign_ == 1 ? digits_.size() < big_int.digits_.size()
                       : digits_.size() > big_int.digits_.size());
  }
  for (int i = digits_.size() - 1; i >= 0; i--) {
    if (digits_[i] != big_int.digits_[i]) {
      if (sign_ == 1) {
        return digits_[i] < big_int.digits_[i];
      }
      return digits_[i] > big_int.digits_[i];
    }
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
  for (size_t i = 0;
       i < digits_.size() || i < big_int.digits_.size() || carry != 0; i++) {
    carry += (i < curr.digits_.size() ? curr.digits_[i] - '0' : 0) +
             (i < big_int.digits_.size() ? big_int.digits_[i] - '0' : 0);
    res.digits_ += (carry % kKten + '0');
    carry /= kKten;
  }
  return res.Normalize(sign_);
}

BigInt BigInt::operator-(BigInt big_int) {
  BigInt curr = *this;
  if (curr.sign_ != big_int.sign_) {
    return curr + big_int.Negative();
  }
  size_t real_sign = curr.sign_;
  curr.sign_ = big_int.sign_ = 1;
  if (curr < big_int) {
    return ((big_int - curr).Negative()).Normalize(-real_sign);
  }
  BigInt res;
  for (int i = 0, borrow = 0; i < int(digits_.size()); i++) {
    borrow = (curr.digits_[i] - borrow -
              (i < int(big_int.digits_.size()) ? big_int.digits_[i] : '0'));
    if (borrow >= 0) {
      res.digits_ += borrow + '0';
    } else {
      res.digits_ += borrow + '0' + kKten;
    }
    borrow = (borrow >= 0 ? 0 : 1);
  }
  return res.Normalize(real_sign);
}

BigInt BigInt::operator*(BigInt big_int) {
  BigInt res("0");
  for (size_t i = 0, b = digits_[i] - '0'; i < digits_.size();
       i++, b = digits_[i] - '0') {
    while (b-- >= 0) {
      res = (res + big_int);
    }
    big_int.digits_.insert(big_int.digits_.begin(), '0');
  }
  return res.Normalize(sign_ * big_int.sign_);
}

BigInt BigInt::operator/(BigInt big_int) {
  if (big_int.digits_.size() == 1 && big_int.digits_[0] == '0') {
    big_int.digits_[0] /= (big_int.digits_[0] - '0');
  }
  BigInt temp("0");
  BigInt res;
  for (size_t i = 0; i < digits_.size(); i++) {
    res.digits_ += "0";
  }
  size_t new_sign = sign_ * big_int.sign_;
  big_int.sign_ = 1;
  for (int i = digits_.size() - 1; i >= 0; i--) {
    temp.digits_.insert(temp.digits_.begin(), '0');
    temp = temp + digits_.substr(i, 1);
    while (!(temp < big_int)) {
      temp = temp - big_int;
      res.digits_[i]++;
    }
  }
  return res.Normalize(new_sign);
}

BigInt BigInt::operator%(BigInt big_int) {
  if (big_int.digits_.size() == 1 && big_int.digits_[0] == '0') {
    big_int.digits_[0] /= char(big_int.digits_[0] - '0');
  }
  BigInt res("0");
  big_int.sign_ = 1;
  for (int i = digits_.size() - 1; i >= 0; i--) {
    res.digits_.insert(res.digits_.begin(), '0');
    res = res + digits_.substr(i, 1);
    while (!(res < big_int)) {
      res = res - big_int;
    }
  }
  return res.Normalize(sign_);
}

std::string BigInt::ToString() const {
  std::string ret = Reverse(digits_);
  return (sign_ == -1 ? "-" : "") + ret;
}