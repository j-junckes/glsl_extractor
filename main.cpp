#include <iostream>
#include <argparse/argparse.hpp>

constexpr std::string_view versionString = std::string_view(GLSL_TO_CPP_VERSION "-"
                                                            GLSL_TO_CPP_COMMIT "-"
                                                            GLSL_TO_CPP_BRANCH "-"
                                                            GLSL_TO_CPP_COMPILER);

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("glsl_to_cpp", versionString.data());

  program.add_argument("vertex_file")
      .help("Vertex shader file")
      .action([](const std::string &value) { return value; });

  program.add_argument("fragment_file")
      .help("Fragment shader file")
      .action([](const std::string &value) { return value; });

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }

  return 0;
}
