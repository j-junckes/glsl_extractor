#include "GLSLExporterVisitor.hpp"
#include <GLSLExtractor.hpp>
#include <parser/GLSLLexer.h>
#include <parser/GLSLParser.h>
#ifndef EOF
#define EOF (-1)
#endif
#include <nlohmann/json.hpp>

namespace GLSLExtractor {
  nlohmann::json getShadersAsJson(const std::string &vertexShader, const std::string &fragmentShader);
}

GLSLExporterVisitor extractGlsl(const std::string &contents) {
  antlr4::ANTLRInputStream input(contents);
  GLSL::GLSLLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);
  GLSL::GLSLParser parser(&tokens);

  antlr4::tree::ParseTree *tree = parser.translation_unit();

  GLSLExporterVisitor glslExporterVisitor;
  glslExporterVisitor.visit(tree);

  return glslExporterVisitor;
}

void processAttributes(const std::vector<In> &attributes, nlohmann::json &j, const std::string &type) {
  for (const auto &attribute: attributes) {
    nlohmann::json innerJson;

    innerJson["name"] = attribute.name;
    innerJson["type"] = attribute.type.typeSpecifier;
    if (attribute.type.typeQualifier.layout.has_value() && attribute.type.typeQualifier.layout->location.has_value()) {
      innerJson["location"] = attribute.type.typeQualifier.layout->location.value();
    }

    j[type]["attributes"].push_back(innerJson);
  }
}

void processUniforms(const std::vector<In> &uniforms, nlohmann::json &j, const std::string &type) {
  for (const auto &uniform: uniforms) {
    nlohmann::json innerJson;

    innerJson["name"] = uniform.name;
    innerJson["type"] = uniform.type.typeSpecifier;

    j[type]["uniforms"].push_back(innerJson);
  }
}

std::string GLSLExtractor::getShadersAsJsonString(const std::string &vertexShader, const std::string &fragmentShader) {
  nlohmann::json j;

  auto vertexVisitor = extractGlsl(vertexShader);

  auto vertexAttributes = vertexVisitor.getAttributes();
  auto vertexUniforms = vertexVisitor.getUniforms();

  processAttributes(vertexAttributes, j, "vertex");
  processUniforms(vertexUniforms, j, "vertex");

  auto fragmentVisitor = extractGlsl(fragmentShader);

  auto fragmentAttributes = fragmentVisitor.getAttributes();
  auto fragmentUniforms = fragmentVisitor.getUniforms();

  processAttributes(fragmentAttributes, j, "fragment");
  processUniforms(fragmentUniforms, j, "fragment");

  return j.dump(2);
}

nlohmann::json GLSLExtractor::getShadersAsJson(const std::string &vertexShader, const std::string &fragmentShader) {
  nlohmann::json j;

  auto vertexVisitor = extractGlsl(vertexShader);

  auto vertexAttributes = vertexVisitor.getAttributes();
  auto vertexUniforms = vertexVisitor.getUniforms();

  processAttributes(vertexAttributes, j, "vertex");
  processUniforms(vertexUniforms, j, "vertex");

  auto fragmentVisitor = extractGlsl(fragmentShader);

  auto fragmentAttributes = fragmentVisitor.getAttributes();
  auto fragmentUniforms = fragmentVisitor.getUniforms();

  processAttributes(fragmentAttributes, j, "fragment");
  processUniforms(fragmentUniforms, j, "fragment");

  return j;
}
