#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace stl {

int strlen(const char* str) {
  int i{};
  while (str[i] != '\0') i++;
  return i;
}

void memcpy(void* dest, const void* src, size_t size) {
  char* curr_byte = (char*)dest;
  for (size_t i = 0; i < size; i++) {
    curr_byte[i] = *((char*)src + i);
  }
}

void memset(void* dest, char value, size_t size) {
  for (size_t i = 0; i < size; i++) {
    *(((char*)dest) + i) = value;
  }
}

void strcpy(char* dest, const char* src) { memcpy(dest, src, strlen(src) + 1); }

void strcat(char* dest, const char* src) {
  memcpy(&dest[strlen(dest)], src, strlen(src) + 1);
}

bool strcmp(const char* str_one, const char* str_two) {
  for (int i = 0; i < strlen(str_one); i++) {
    if (str_one[i] != str_two[i]) return false;
  }
  return true;
}

void chrcat(char* dest, char src) {
  dest[strlen(dest)] = src;
  dest[strlen(dest) + 1] = '\0';
}

inline void reverse(char* str) {
  // reverse string without temp variable
  int i = 0, ndx = strlen(str) - 1;
  while (i < ndx) {
    str[i] = str[ndx] ^ str[i];
    str[ndx] = str[ndx] ^ str[i];
    str[i] = str[ndx] ^ str[i];
    i++;
    ndx--;
  }
}

void itos(int i, char* dest) {
  int ndx{};
  // loop through int backward, getting char for each number
  for (; i > 0; ndx++) {
    dest[ndx] = (i % 10) + '0';
    i = i / 10;
  }

  // add null terminator
  dest[ndx] = '\0';
  reverse(dest);
}

template <class T>
class Iterator {
  // iterator tags
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

 private:
  pointer m_ptr;

 public:
  Iterator(pointer ptr) : m_ptr(ptr) {}

  reference operator*() const { return *m_ptr; }
  pointer operator->() { return m_ptr; }

  // prefix increment
  Iterator& operator++() {
    m_ptr++;
    return *this;
  }

  // postfix increment
  Iterator operator++(int) {
    Iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  friend bool operator==(const Iterator& a, const Iterator& b) {
    return a.m_ptr == b.m_ptr;
  }

  friend bool operator!=(const Iterator& a, const Iterator& b) {
    return a.m_ptr != b.m_ptr;
  }
};

// slice of a string
class slice {
 private:
  char* data;
  int length;

 public:
  slice(char* data, int len) : data(data), length(len) {}

  int len() { return this->length; }

  /// replace slice with another string. must be same size
  void replace(const char* str) {
    if (strlen(str) != length) {
      throw std::out_of_range("string provided bigger than string slice");
    }
  }

  void reverse() { stl::reverse(data); }

  // operator methods
  friend std::ostream& operator<<(std::ostream& os, const slice& s) {
    for (int i = 0; i < s.length; i++) os << s.data[i];
    return os;
  }

  // iterator methods
  Iterator<char> begin() { return Iterator(&data[0]); }
  Iterator<char> end() { return Iterator(&data[this->length]); }
};

// todo
class str {
  char* value{};
  size_t length{};
  size_t allocd{};
  bool ref{};

 public:
  str(const char* str) {
    std::cout << "cstr constructor\n";
    this->length = strlen(str);
    this->allocd = length + 1;
    value = (char*)malloc(this->allocd);
    strcpy(value, str);
  }

  str() {
    std::cout << "empty constructor\n";
    this->length = 0;
    this->allocd = 1;
    value = (char*)malloc(this->allocd);
    value[0] = '\0';
  }

  str(str& s) {
    std::cout << "str& constructor\n";
    this->length = s.len();
    this->allocd = s.capacity();
    value = s.data();
    ref = true;
  }

  str(size_t initial_size) {
    std::cout << "size constructor\n";
    this->length = 0;
    this->allocd = initial_size;
    value = (char*)malloc(this->allocd);
    // zero all values
    memset(value, '\0', initial_size);
  }

  ~str() {
    if (value != nullptr && !ref) free(value);
  }

  size_t len() { return this->length; }
  const char* c_str() { return value; }
  void reverse() { stl::reverse(value); }
  str sub(int start, int len) {
    str s(len);
    for (int i = start; i < start + len; i++) {
      s += value[i];
    }
    return s;
  }

  stl::slice slice(int start, int len) {
    if (start + len > static_cast<int>(this->length)) {
      throw std::out_of_range("slice exceeds length of string");
    }
    return stl::slice(&value[start], len);
  }

  char& front() { return value[0]; }
  char& back() { return value[this->length-1]; }
  char* data() { return value; }
  bool empty() { return this->length == 0; }
  size_t capacity() { return this->allocd; }

  void reserve(size_t res) {
    if (res > this->allocd) {
      this->allocd = res;
      value = (char*)realloc(value, this->allocd);
    }
  }

  //  operators
  str& operator+=(int i) {
    char buf[11]{};
    itos(i, buf);
    this->length += strlen(buf);
    // check for room
    if (this->allocd <= this->length) {
      this->allocd += strlen(buf);
      value = (char*)realloc(value, this->allocd);
    }
    strcat(value, buf);
    return *this;
  }

  str& operator+=(char c) {
    this->length += 1;

    // check for room
    if (this->allocd <= this->length) {
      this->allocd += 1;
      value = (char*)realloc(value, this->allocd);
    }

    value[this->length - 1] = c;
    value[this->length] = '\0';
    return *this;
  }

  str& operator+=(const char* another) {
    this->length += strlen(another);

    // check for room
    if (this->allocd <= this->length) {
      this->allocd = this->length + 1;
      value = (char*)realloc(value, this->allocd);
    }

    strcat(value, another);
    return *this;
  }

  str& operator=(const char* str_val) {
    this->length = strlen(str_val);

    if (this->length >= this->allocd) {
      this->allocd = this->length + 1;
      value = (char*)realloc(value, this->allocd);
    }

    strcpy(value, str_val);
    return *this;
  }

  str& operator=(stl::str str_val) {
    this->operator=(str_val.c_str());
    return *this;
  }

  bool operator==(str& two) { return strcmp(value, two.c_str()); }
  bool operator==(const char* two) { return strcmp(value, two); }

  char& operator[](size_t indx) {
    if (indx >= this->length) {
      throw std::out_of_range("out of bounds str access");
    }
    return value[indx];
  }

  friend std::ostream& operator<<(std::ostream& os, str& obj) {
    return os << obj.value;
  }
  
  // iterator methods
  Iterator<char> begin() { return Iterator(&value[0]); }
  Iterator<char> end() { return Iterator(&value[this->length]); }
};

}  // namespace stl

stl::str getname(int id) {
  if (id == 1) {
    return "kai";
  } else {
    return "gary";
 }
}

inline void changename(stl::str& name, const char* newname) {
  name = newname;
}

int main() {
  auto name = getname(1);
  std::cout << name << "\n";

  changename(name, "Frederic Nietzsche");

  auto sl = name.slice(0,5);

  for (auto& ch : sl) {
    std::cout << "test : " << ch << "\n";
  }
}

