#include "AstTree.h"
#include "Context.h"

void mcool::AstTree::set(mcool::ast::CoolClassList* program) {
  if (tree == nullptr) {
    tree = program;
  } else {
    for (auto* coolClass : program->getData()) {
      tree->add(coolClass);
    }
  }
}

void mcool::AstTree::accept(mcool::ast::Visitor* visitor) {
  assert(this->tree && "tree is not set");
  this->tree->accept(visitor);
}

void mcool::AstTree::addBuildinClasses(ast::CoolClassList* classes, Context* context) {
  assert(context != nullptr);
  auto& mm = context->getMemoryManagement();
  if (classes == nullptr) {
    classes = mm.make<ast::CoolClassList>();
    return;
  }

  auto* objectClassName = mm.make<ast::TypeId>(std::string("Object"));
  auto* boolTypeName = mm.make<ast::TypeId>(std::string("Bool"));
  auto* intTypeName = mm.make<ast::TypeId>(std::string("Int"));
  auto* strTypeName = mm.make<ast::TypeId>(std::string("String"));
  auto* selfTypeName = mm.make<ast::TypeId>(std::string("SELF_TYPE"));
  auto* buildinFileName = mm.make<ast::String>(std::string("<basic class>"));

  auto buildFormalList = [&mm](std::initializer_list<std::pair<std::string, ast::TypeId*>> params) {
    auto* formalList = mm.make<ast::FormalList>();
    for (auto& param : params) {
      auto* idName = mm.make<ast::ObjectId>(param.first);
      auto* formal = mm.make<ast::SingleFormal>(idName, param.second);
      formalList->add(formal);
    }
    return formalList;
  };

  auto* noBody = mm.make<ast::NoExpr>();
  auto* emptyParams = buildFormalList({});

  {
    // construct Object class
    auto* attributeList = mm.make<ast::AttributeList>();

    auto* abort = mm.make<ast::ObjectId>(std::string("abort"));
    auto* abortMethod = mm.make<ast::SingleMethod>(abort, emptyParams, objectClassName, noBody);
    attributeList->add(abortMethod);

    auto* typeName = mm.make<ast::ObjectId>(std::string("type_name"));
    auto* typeNameMethod = mm.make<ast::SingleMethod>(typeName, emptyParams, strTypeName, noBody);
    attributeList->add(typeNameMethod);

    auto* copyName = mm.make<ast::ObjectId>(std::string("copy"));
    auto* copyMethod = mm.make<ast::SingleMethod>(copyName, emptyParams, selfTypeName, noBody);
    attributeList->add(copyMethod);

    auto* object =
        mm.make<ast::CoolClass>(objectClassName, nullptr, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct IO class
    auto* attributeList = mm.make<ast::AttributeList>();

    auto* outStringName = mm.make<ast::ObjectId>(std::string("out_string"));
    auto* outStringParams = buildFormalList({{"str", strTypeName}});
    auto* outStringMethod =
        mm.make<ast::SingleMethod>(outStringName, outStringParams, selfTypeName, noBody);
    attributeList->add(outStringMethod);

    auto* outIntName = mm.make<ast::ObjectId>(std::string("out_int"));
    auto* outIntParams = buildFormalList({{"num", intTypeName}});
    auto* outIntMethod = mm.make<ast::SingleMethod>(outIntName, outIntParams, selfTypeName, noBody);
    attributeList->add(outIntMethod);

    auto* inStringName = mm.make<ast::ObjectId>(std::string("in_string"));
    auto* inStringMethod =
        mm.make<ast::SingleMethod>(inStringName, emptyParams, strTypeName, noBody);
    attributeList->add(inStringMethod);

    auto* inIntName = mm.make<ast::ObjectId>(std::string("in_int"));
    auto* inIntMethod = mm.make<ast::SingleMethod>(inIntName, emptyParams, intTypeName, noBody);
    attributeList->add(inIntMethod);

    auto* ioClassName = mm.make<ast::TypeId>(std::string("IO"));
    auto* object =
        mm.make<ast::CoolClass>(ioClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct Int class
    auto* attributeList = mm.make<ast::AttributeList>();

    auto* valName = mm.make<ast::ObjectId>(std::string("val"));
    auto* valMember = mm.make<ast::SingleMember>(valName, intTypeName, noBody);
    attributeList->add(valMember);

    auto* intClassName = mm.make<ast::TypeId>(std::string("Int"));
    auto* object =
        mm.make<ast::CoolClass>(intClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct Bool class
    auto* attributeList = mm.make<ast::AttributeList>();

    auto* valName = mm.make<ast::ObjectId>(std::string("val"));
    auto* valMember = mm.make<ast::SingleMember>(valName, boolTypeName, noBody);
    attributeList->add(valMember);

    auto* boolClassName = mm.make<ast::TypeId>(std::string("Bool"));
    auto* object =
        mm.make<ast::CoolClass>(boolClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct String class
    auto* attributeList = mm.make<ast::AttributeList>();

    auto* valName = mm.make<ast::ObjectId>(std::string("val"));
    auto* valMember = mm.make<ast::SingleMember>(valName, intTypeName, noBody);
    attributeList->add(valMember);

    auto* strFieldName = mm.make<ast::ObjectId>(std::string("str_field"));
    auto* strFieldMember = mm.make<ast::SingleMember>(strFieldName, strTypeName, noBody);
    attributeList->add(strFieldMember);

    auto* lengthName = mm.make<ast::ObjectId>(std::string("length"));
    auto* lengthMethod = mm.make<ast::SingleMethod>(lengthName, emptyParams, intTypeName, noBody);
    attributeList->add(lengthMethod);

    auto* concatName = mm.make<ast::ObjectId>(std::string("concat"));
    auto* concatParams = buildFormalList({{"arg", strTypeName}});
    auto* concatMethod = mm.make<ast::SingleMethod>(concatName, concatParams, strTypeName, noBody);
    attributeList->add(concatMethod);

    auto* substrName = mm.make<ast::ObjectId>(std::string("substr"));
    auto* substrParams = buildFormalList({{"arg", intTypeName}, {"arg2", intTypeName}});
    auto* substrMethod = mm.make<ast::SingleMethod>(substrName, substrParams, strTypeName, noBody);
    attributeList->add(substrMethod);

    auto* strClassName = mm.make<ast::TypeId>(std::string("String"));
    auto* object =
        mm.make<ast::CoolClass>(strClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }
}