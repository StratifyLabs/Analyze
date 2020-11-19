
#include <cstdio>

#include <chrono.hpp>
#include <printer/JsonPrinter.hpp>
#include <sys/Cli.hpp>

#include "Fs.hpp"

int main(int argc, char *argv[]) {

  sys::Cli cli(argc, argv);
  printer::Printer printer;

  test::Test::initialize(test::Test::Initialize()
                             .set_git_hash(SOS_GIT_HASH)
                             .set_name("Analyze")
                             .set_printer(&printer));

  if (cli.get_option("fs") == "true") {
    Fs().execute(cli);
  }

  test::Test::finalize();

  return 0;
}
