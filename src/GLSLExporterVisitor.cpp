#include "GLSLExporterVisitor.hpp"

antlrcpp::Any GLSLExporterVisitor::visitTranslation_unit(GLSL::GLSLParser::Translation_unitContext *ctx) {
  for (auto externalDeclaration: ctx->external_declaration()) {
    visit(externalDeclaration);
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitExternal_declaration(GLSL::GLSLParser::External_declarationContext *ctx) {
  if (ctx->declaration()) {
    return visit(ctx->declaration());
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitInit_declarator_list(GLSL::GLSLParser::Init_declarator_listContext *ctx) {
  if (ctx->single_declaration()) {
    return visit(ctx->single_declaration());
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitSingle_declaration(GLSL::GLSLParser::Single_declarationContext *ctx) {
  if (ctx->fully_specified_type() && ctx->typeless_declaration()) {
    auto type = any_cast<FullySpecifiedType>(visit(ctx->fully_specified_type()));
    auto name = any_cast<std::string>(visit(ctx->typeless_declaration()));

    if (type.typeQualifier.storage == StorageQualifier::IN && type.typeQualifier.layout.has_value()) {
      _attributes.push_back({type, name, InType::ATTRIBUTE});
    } else if (type.typeQualifier.storage == StorageQualifier::UNIFORM) {
      _uniforms.push_back({type, name, InType::UNIFORM});
    }
  } else if (ctx->typeless_declaration()) {
    auto name = any_cast<std::string>(visit(ctx->typeless_declaration()));
    //_uniforms.push_back({name, InType::UNIFORM});
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitFully_specified_type(GLSL::GLSLParser::Fully_specified_typeContext *ctx) {
  if (ctx->type_qualifier() && ctx->type_specifier()) {
    auto typeQualifier = any_cast<TypeQualifier>(visit(ctx->type_qualifier()));
    auto typeSpecifier = any_cast<std::string>(visit(ctx->type_specifier()));

    return FullySpecifiedType{typeQualifier, typeSpecifier};
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitType_qualifier(GLSL::GLSLParser::Type_qualifierContext *ctx) {
  TypeQualifier typeQualifier;

  for (auto singleTypeQualifier: ctx->single_type_qualifier()) {
    auto singleTypeQualifierResult = visit(singleTypeQualifier);

    try {
      auto storageQualifier = any_cast<StorageQualifier>(singleTypeQualifierResult);
      typeQualifier.storage = storageQualifier;
    } catch (const std::bad_any_cast &e) {
      try {
        auto layoutQualifier = any_cast<LayoutQualifier>(singleTypeQualifierResult);
        typeQualifier.layout = layoutQualifier;
      } catch (const std::bad_any_cast &e) {
        return nullptr;
      }
    }
  }

  return typeQualifier;
}

antlrcpp::Any GLSLExporterVisitor::visitSingle_type_qualifier(GLSL::GLSLParser::Single_type_qualifierContext *ctx) {
  if (ctx->storage_qualifier()) {
    return visit(ctx->storage_qualifier());
  } else if (ctx->layout_qualifier()) {
    return visit(ctx->layout_qualifier());
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitLayout_qualifier(GLSL::GLSLParser::Layout_qualifierContext *ctx) {
  if (ctx->layout_qualifier_id_list()) {
    return visit(ctx->layout_qualifier_id_list());
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitLayout_qualifier_id_list(GLSL::GLSLParser::Layout_qualifier_id_listContext *ctx) {
  for (auto layoutQualifierId: ctx->layout_qualifier_id()) {
    if (layoutQualifierId->IDENTIFIER()) {
      auto identifier = layoutQualifierId->IDENTIFIER()->getText();
      if (identifier == "location") {
        auto location = std::stoi(layoutQualifierId->constant_expression()->getText());
        return LayoutQualifier{location};
      }
    }
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitStorage_qualifier(GLSL::GLSLParser::Storage_qualifierContext *ctx) {
  if (ctx->IN()) {
    return StorageQualifier::IN;
  } else if (ctx->OUT()) {
    return StorageQualifier::OUT;
  } else if (ctx->UNIFORM()) {
    return StorageQualifier::UNIFORM;
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitType_specifier(GLSL::GLSLParser::Type_specifierContext *ctx) {
  if (ctx->type_specifier_nonarray()) {
    return visit(ctx->type_specifier_nonarray());
  }

  return nullptr;
}

antlrcpp::Any GLSLExporterVisitor::visitType_specifier_nonarray(GLSL::GLSLParser::Type_specifier_nonarrayContext *ctx) {
  return ctx->getText();
}

antlrcpp::Any GLSLExporterVisitor::visitTypeless_declaration(GLSL::GLSLParser::Typeless_declarationContext *ctx) {
  return ctx->IDENTIFIER()->getText();
}

std::vector<In> GLSLExporterVisitor::getUniforms() const {
  return _uniforms;
}

std::vector<In> GLSLExporterVisitor::getAttributes() const {
  return _attributes;
}
