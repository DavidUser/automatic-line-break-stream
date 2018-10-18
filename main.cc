#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>

template <size_t size>
class StrictNewLineOutputBuffer : public std::streambuf {
 public:
  StrictNewLineOutputBuffer() { setp(buffer.begin(), buffer.end()); }
  virtual ~StrictNewLineOutputBuffer() { sync(); }

  int sync() override {
    std::string cotent(buffer.begin(), pptr());

    if (auto new_line_pos = cotent.find_last_of('\n');
        new_line_pos != std::string::npos) {
      std::cout << cotent.substr(0, new_line_pos + 1);

      auto rest = cotent.substr(new_line_pos + 1);
      std::strncpy(buffer.begin(), rest.data(), rest.length());
      setp(buffer.begin() + rest.length(), buffer.end());
      return 0;
    }

    std::cout << cotent << std::endl;
    setp(buffer.begin(), buffer.end());
    return 0;
  }

 protected:
  int_type overflow(int_type character) override {
    sync();
    sputc(character);
    return 0;
  }

 private:
  std::array<char, size> buffer;
};

class Printer : public std::ostream {
 public:
  Printer() : std::ostream(&buffer) {}

 private:
  StrictNewLineOutputBuffer<13> buffer;
};

int main(void) {
  std::string huge_report;
  for (int i = 0; i < 200; ++i) {
    huge_report += (i % 10) + '0';
  }
  huge_report[50] = '\n';

  std::cout << huge_report << std::endl;
  std::cout << "----------------------------------------------" << std::endl;
  Printer pout;
  pout << huge_report;
  return 0;
}
