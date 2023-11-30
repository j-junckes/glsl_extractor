#pragma once
#include <parser/GLSLParserBaseVisitor.h>
#include <vector>

enum class GLSLProfile {
  CORE,
  COMPATIBILITY,
  ES
};

struct GLSLVersion {
  int version;
  GLSLProfile profile;
};

enum class StorageQualifier {
  IN,
  OUT,
  UNIFORM
};

enum class InType {
  ATTRIBUTE,
  UNIFORM
};

class LayoutQualifier {
public:
  std::optional<int> location;
};

class TypeQualifier {
public:
  std::optional<int> location;
  std::optional<StorageQualifier> storage;
  std::optional<LayoutQualifier> layout;
};

class FullySpecifiedType {
public:
  TypeQualifier typeQualifier;
  std::string typeSpecifier;
};

class In {
public:
  FullySpecifiedType type;
  std::string name;
  InType inType;
};

class GLSLExporterVisitor : public GLSL::GLSLParserBaseVisitor {
public:
  antlrcpp::Any visitTranslation_unit(GLSL::GLSLParser::Translation_unitContext *ctx) override;

  antlrcpp::Any visitExternal_declaration(GLSL::GLSLParser::External_declarationContext *ctx) override;

  antlrcpp::Any visitInit_declarator_list(GLSL::GLSLParser::Init_declarator_listContext *ctx) override;

  antlrcpp::Any visitSingle_declaration(GLSL::GLSLParser::Single_declarationContext *ctx) override;

  antlrcpp::Any visitFully_specified_type(GLSL::GLSLParser::Fully_specified_typeContext *ctx) override;

  antlrcpp::Any visitType_qualifier(GLSL::GLSLParser::Type_qualifierContext *ctx) override;

  antlrcpp::Any visitSingle_type_qualifier(GLSL::GLSLParser::Single_type_qualifierContext *ctx) override;

  antlrcpp::Any visitLayout_qualifier(GLSL::GLSLParser::Layout_qualifierContext *ctx) override;

  antlrcpp::Any visitLayout_qualifier_id_list(GLSL::GLSLParser::Layout_qualifier_id_listContext *ctx) override;

  antlrcpp::Any visitStorage_qualifier(GLSL::GLSLParser::Storage_qualifierContext *ctx) override;

  antlrcpp::Any visitType_specifier(GLSL::GLSLParser::Type_specifierContext *ctx) override;

  antlrcpp::Any visitType_specifier_nonarray(GLSL::GLSLParser::Type_specifier_nonarrayContext *ctx) override;

  antlrcpp::Any visitTypeless_declaration(GLSL::GLSLParser::Typeless_declarationContext *ctx) override;

  [[nodiscard]] std::vector<In> getUniforms() const;

  [[nodiscard]] std::vector<In> getAttributes() const;

private:
  std::vector<In> _uniforms;
  std::vector<In> _attributes;
};
