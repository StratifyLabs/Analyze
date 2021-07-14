#include <fcntl.h>
#include <unistd.h>

#include <fs.hpp>
#include <printer.hpp>
#include <var.hpp>

#include "UnistdTest.hpp"

#define EXEC_PATH "/app/ram/Analyze"
#define TEST_FILE_PATH "/home/test.txt"

UnistdTest::UnistdTest() : Test("UnistdTest") {}

bool UnistdTest::execute_class_api_case() {
  TEST_ASSERT_RESULT(execute_api_access_case());
  TEST_ASSERT_RESULT(execute_api_file_case());

  return true;
}

bool UnistdTest::execute_api_access_case() {
  Printer::Object po(printer(), "access");

  TEST_ASSERT(access("/nomount/some/file", F_OK) < 0);
  TEST_ASSERT(errno == ENOENT);
  TEST_ASSERT(access("/nomount", F_OK) < 0);
  TEST_ASSERT(errno == ENOENT);

  TEST_ASSERT(access("/012345678901234567890123456789012345678901234"
                     "5678901234567890123456789",
                     F_OK) < 0);
  TEST_ASSERT(errno == ENAMETOOLONG);

  TEST_ASSERT(access("/0123456789/01234567890/123456789/0123/"
                     "4567890/123456789012345/67890123/456/789",
                     F_OK) < 0);
  TEST_ASSERT(errno == ENAMETOOLONG);

  // create a file in home
  int fd;
  TEST_ASSERT((fd = open(TEST_FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, 0666)) >=
              0);
  TEST_ASSERT(write(fd, "test", 4) == 4);
  TEST_ASSERT(close(fd) >= 0);

  TEST_ASSERT(access(TEST_FILE_PATH, F_OK) == 0);
  TEST_ASSERT(access(TEST_FILE_PATH, W_OK) == 0);
  TEST_ASSERT(access(TEST_FILE_PATH, R_OK) == 0);

  TEST_ASSERT(access(EXEC_PATH, F_OK) == 0);
  TEST_ASSERT(access(EXEC_PATH, R_OK) == 0);
  TEST_ASSERT(access(EXEC_PATH, X_OK) == 0);
  TEST_ASSERT(access(EXEC_PATH, W_OK) < 0);
  TEST_ASSERT(errno == EACCES);

  TEST_ASSERT(access("/", R_OK) == 0);
  TEST_ASSERT(access("/", X_OK) == 0);
  TEST_ASSERT(access("/", W_OK) < 0);
  TEST_ASSERT(errno == EACCES);

  TEST_ASSERT(access("/app/.install", W_OK) == 0);
  TEST_ASSERT(access("/app/.install", R_OK) < 0);
  TEST_ASSERT(errno == EACCES);
  TEST_ASSERT(access("/app/.install", X_OK) < 0);
  TEST_ASSERT(errno == EACCES);

  printer().key("accessCase", "complete");
  return true;
}

bool UnistdTest::execute_api_sleep_case() {

  printer().key("sleepCase", "complete");
  return true;
}

bool UnistdTest::execute_api_directory_case() {

  // ENOENT
  // ENAMETOOLONG
  // ENOTSUP
  // EINVAL

  return case_result();
}

bool UnistdTest::execute_api_file_case() {
  Printer::Object po(printer(), "file");
  TEST_ASSERT(open("/home/1234567890123456789012345678901234567890", O_RDONLY) <
              0);
  TEST_ASSERT(errno == ENAMETOOLONG);
  TEST_ASSERT(open("/1234567890123456789012345678901234567890", O_RDONLY) < 0);
  TEST_ASSERT(errno == ENAMETOOLONG);

  TEST_ASSERT(open("/home/nofile", O_RDONLY) < 0);
  TEST_ASSERT(errno == ENOENT);

  TEST_ASSERT(open("/nodir/nofile", O_RDONLY) < 0);
  TEST_ASSERT(errno == ENOTSUP);

  int fd;
  TEST_ASSERT(fd = open(TEST_FILE_PATH, O_RDONLY) >= 0);
  struct stat st;
  TEST_ASSERT(fstat(fd, &st) == 0);
  TEST_EXPECT((st.st_mode & S_IFMT) == S_IFREG);

  int flags = 0;
  TEST_ASSERT((flags = fcntl(fd, F_GETFL)) >= 0);
  TEST_EXPECT(flags & O_ACCMODE == O_RDONLY);

  TEST_ASSERT(rename("/home/noexists", "/home/test_rename.txt") < 0);
  TEST_EXPECT(errno == ENOENT);
  TEST_ASSERT(rename(TEST_FILE_PATH, "/home/test_rename.txt") == 0);
  TEST_ASSERT(rename("/home/test_rename.txt", TEST_FILE_PATH) == 0);

  const char *test_copy = "/home/test_copy.txt";
  unlink(test_copy);
  errno = 0;
  TEST_EXPECT(remove(test_copy) < 0);
  TEST_EXPECT(errno == ENOENT);

  API_RESET_ERROR();
  // make a copy
  File(File::IsOverwrite::yes, test_copy).write(File(TEST_FILE_PATH));
  TEST_ASSERT(is_success());

  TEST_ASSERT(remove(test_copy) == 0);

  TEST_ASSERT(FileSystem().exists(test_copy) == false);

  API_RESET_ERROR();
  // make a copy
  File(File::IsOverwrite::yes, test_copy).write(File(TEST_FILE_PATH));
  TEST_ASSERT(is_success());

  TEST_ASSERT(unlink(test_copy) == 0);
  TEST_ASSERT(FileSystem().exists(test_copy) == false);

  TEST_ASSERT(unlink(TEST_FILE_PATH) == 0);

  const StringView hello_world = "Hello World\n";
  TEST_ASSERT(File(File::IsOverwrite::yes, TEST_FILE_PATH)
                  .write(hello_world)
                  .is_success());
  TEST_ASSERT(FileSystem().exists(TEST_FILE_PATH) == true);

  TEST_ASSERT(
      DataFile().write(File(TEST_FILE_PATH)).data().add_null_terminator() ==
      hello_world);

  printer().key("fileCase", "complete");
  return true;
}
