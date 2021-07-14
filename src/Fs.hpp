#ifndef FS_HPP
#define FS_HPP

#include <test/Test.hpp>

class Fs : public test::Test {
public:
  Fs() : test::Test("Fs") {}

  bool execute_class_api_case();

private:
  bool open_case();
};

#endif // FS_HPP
