#include <unistd.h>

#include "Fs.hpp"

#define EXEC_PATH "/app/flash/posix"

bool Fs::execute_class_api_case() {

  TEST_ASSERT_RESULT(open_case());

  return true;
}

bool Fs::open_case() {
  printer::Printer::Object po(printer(), "open");
  TEST_ASSERT(access(EXEC_PATH, F_OK) < 0);
  TEST_ASSERT(errno == ENOENT);

  return true;
}
