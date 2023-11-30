#include <GLSLExtractor.hpp>
#include <argparse/argparse.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

constexpr std::string_view versionString = std::string_view(GLSL_EXTRACTOR_VERSION "-" GLSL_EXTRACTOR_COMMIT "-" GLSL_EXTRACTOR_BRANCH "-" GLSL_EXTRACTOR_COMPILER);

std::string getFileContents(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) {
    throw std::runtime_error("File '" + path.string() + "' does not exist");
  }

  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open file '" + path.string() + "'");
  }

  std::string fileContent((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());

  return fileContent;
}

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("glslextractor", versionString.data());

  program.add_argument("vertex_file")
      .help("Vertex shader file")
      .action([](const std::string &value) { return std::filesystem::path(value); });

  program.add_argument("fragment_file")
      .help("Fragment shader file")
      .action([](const std::string &value) { return std::filesystem::path(value); });

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    std::cout << program;
    exit(0);
  }

  auto vertexFile = program.get<std::filesystem::path>("vertex_file");
  auto fragmentFile = program.get<std::filesystem::path>("fragment_file");

  std::string vertexContents, fragmentContents;

  try {
    vertexContents = getFileContents(vertexFile);
    fragmentContents = getFileContents(fragmentFile);
  } catch (const std::runtime_error &err) {
    std::cout << err.what() << std::endl;
    exit(0);
  }

  auto json = GLSLExtractor::getShadersAsJson(vertexContents, fragmentContents);

  std::cout << json.dump(2) << std::endl;

  return 0;
}
