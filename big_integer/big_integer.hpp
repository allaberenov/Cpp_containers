
#include <string>

class BigInt {
private:
  int sign_;
  std::string digits_;

public:
  BigInt();
  BigInt(std::string);
  BigInt(int64_t);

  BigInt operator-();

  BigInt& operator--();
  BigInt& operator++();
  BigInt operator++(int);
  BigInt operator--(int);

  BigInt Negative();
  BigInt Normalize(size_t new_sign);
  static std::string Reverse(const std::string& str) ;
  std::string ToString() const;

  BigInt& operator=(std::string&);
  BigInt operator+(BigInt);
  BigInt operator-(BigInt big_int);
  BigInt operator*(BigInt big_int);
  BigInt operator/(BigInt big_int);
  BigInt operator%(BigInt big_int);

  bool operator==(const BigInt&) const;
  bool operator!=(const BigInt&) const;
  bool operator<(const BigInt&) const;
  bool operator<=(const BigInt&) const;
  bool operator>(const BigInt&) const;
  bool operator>=(const BigInt&) const;

  friend std::ostream& operator<<(std::ostream&, const BigInt&);
  friend std::istream& operator>>(std::istream&, BigInt&);
};
