#include <algorithm>
#include <array>
#include <cstring>
#include <functional>
#include <iostream>

template <size_t size>
class StrictNewLineOutputBuffer : public std::streambuf {
 public:
  using Traits = std::function<void(const char*)>;

  StrictNewLineOutputBuffer(Traits t) : traits(t) {
    setp(buffer.begin(), buffer.end());
  }
  virtual ~StrictNewLineOutputBuffer() { sync(); }

  int sync() override {
    std::string content(buffer.begin(), pptr());

    if (auto new_line_pos = content.find_last_of('\n');
        new_line_pos != std::string::npos) {
      traits(content.substr(0, new_line_pos).c_str());

      auto rest = content.substr(new_line_pos + 1);
      std::strncpy(buffer.begin(), rest.data(), rest.length());
      setp(buffer.begin() + rest.length(), buffer.end());
      return 0;
    }

    traits(content.c_str());
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
  Traits traits;
};

class AutoLineBreakStream : public std::ostream {
 public:
  AutoLineBreakStream()
      : std::ostream(&buffer),
        buffer([](const char* content) { std::cout << content << std::endl; }) {
  }

 private:
  StrictNewLineOutputBuffer<13> buffer;
};

int main(void) {
  std::string huge_report;
  for (int i = 0; i < 200; ++i) {
    huge_report += (i % 10) + '0';
  }
  huge_report[45] = '\n';

  std::cout << huge_report << std::endl;
  std::cout << "----------------------------------------------" << std::endl;
  AutoLineBreakStream pout;
  pout << huge_report;
  return 0;
}
