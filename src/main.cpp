
#include <cstdio>

#include <SosAPI/sos/Appfs.hpp>
#include <chrono.hpp>
#include <printer/JsonPrinter.hpp>
#include <sys/Cli.hpp>
#include <sys/Version.hpp>

#include "UnistdTest.hpp"

int main(int argc, char *argv[]) {

  sys::Cli cli(argc, argv);
  printer::Printer printer;

  sos::Appfs::Info info = sos::Appfs().get_info(argv[0]);
  test::Test::initialize(
      test::Test::Initialize()
          .set_git_hash(SOS_GIT_HASH)
          .set_name("Analyze")
          .set_version(sys::Version::from_u16(info.version()).string_view())
          .set_printer(&printer));

  if (cli.get_option("unistd") == "true") {
    UnistdTest().execute(cli);
  }

  test::Test::finalize();

  return 0;
}
