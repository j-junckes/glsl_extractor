#ifndef GLSL_EXTRACTOR_GLSL_EXTRACTOR_HPP
#define GLSL_EXTRACTOR_GLSL_EXTRACTOR_HPP

#include <string>

#ifdef GLSL_EXTRACTOR_WITH_NLOHMANN_JSON
#ifndef EOF
#define EOF (-1)
#endif
#include <nlohmann/json.hpp>
#endif

namespace GLSLExtractor {
  std::string getShadersAsJsonString(const std::string &vertexShader, const std::string &fragmentShader);

#ifdef GLSL_EXTRACTOR_WITH_NLOHMANN_JSON
  nlohmann::json getShadersAsJson(const std::string &vertexShader, const std::string &fragmentShader);
#endif
}// namespace GLSLExtractor

#endif
