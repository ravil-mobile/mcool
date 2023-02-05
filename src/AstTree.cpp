#include "AstTree.h"
#include "MemoryManagement.h"

void mcool::AstTree::set(mcool::ast::CoolClassList *program) {
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

void mcool::AstTree::addBuildinClasses(mcool::ast::CoolClassList* classes) {
  if (classes == nullptr) {
    classes = make<ast::CoolClassList>();
  }

  auto* objectClassName = make<ast::TypeId>(std::string("Object"));
  auto* boolTypeName = make<ast::TypeId>(std::string("Bool"));
  auto* intTypeName = make<ast::TypeId>(std::string("Int"));
  auto* strTypeName = make<ast::TypeId>(std::string("String"));
  auto* selfTypeName = make<ast::TypeId>(std::string("SELF_TYPE"));
  auto* buildinFileName = make<ast::String>(std::string("<basic class>"));

  auto buildFormalList = [](std::initializer_list<std::pair<std::string, ast::TypeId*>> params) {
    auto* formalList = make<ast::FormalList>();
    for (auto& param : params) {
      auto* idName = make<ast::ObjectId>(param.first);
      auto* formal = make<ast::SingleFormal>(idName, param.second);
      formalList->add(formal);
    }
    return formalList;
  };

  auto* noBody = make<ast::NoExpr>();
  auto* emptyParams = buildFormalList({});

  {
    // construct Object class
    auto* attributeList = make<ast::AttributeList>();

    auto* abort = make<ast::ObjectId>(std::string("abort"));
    auto* abortMethod = make<ast::SingleMethod>(abort, emptyParams, objectClassName, noBody);
    attributeList->add(abortMethod);

    auto* typeName = make<ast::ObjectId>(std::string("type_name"));
    auto* typeNameMethod = make<ast::SingleMethod>(typeName, emptyParams, strTypeName, noBody);
    attributeList->add(typeNameMethod);

    auto* copyName = make<ast::ObjectId>(std::string("copy"));
    auto* copyMethod = make<ast::SingleMethod>(copyName, emptyParams, selfTypeName, noBody);
    attributeList->add(copyMethod);

    auto* object = make<ast::CoolClass>(objectClassName, nullptr, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct IO class
    auto* attributeList = make<ast::AttributeList>();

    auto* outStringName = make<ast::ObjectId>(std::string("out_string"));
    auto* outStringParams = buildFormalList({{"str", strTypeName}});
    auto* outStringMethod =
        make<ast::SingleMethod>(outStringName, outStringParams, selfTypeName, noBody);
    attributeList->add(outStringMethod);

    auto* outIntName = make<ast::ObjectId>(std::string("out_int"));
    auto* outIntParams = buildFormalList({{"num", intTypeName}});
    auto* outIntMethod = make<ast::SingleMethod>(outIntName, outIntParams, selfTypeName, noBody);
    attributeList->add(outIntMethod);

    auto* inStringName = make<ast::ObjectId>(std::string("in_string"));
    auto* inStringMethod = make<ast::SingleMethod>(inStringName, emptyParams, strTypeName, noBody);
    attributeList->add(inStringMethod);

    auto* inIntName = make<ast::ObjectId>(std::string("in_int"));
    auto* inIntMethod = make<ast::SingleMethod>(inIntName, emptyParams, intTypeName, noBody);
    attributeList->add(inIntMethod);

    auto* ioClassName = make<ast::TypeId>(std::string("IO"));
    auto* object =
        make<ast::CoolClass>(ioClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct Int class
    auto* attributeList = make<ast::AttributeList>();

    auto* valName = make<ast::ObjectId>(std::string("val"));
    auto* valMember = make<ast::SingleMember>(valName, intTypeName, noBody);
    attributeList->add(valMember);

    auto* intClassName = make<ast::TypeId>(std::string("Int"));
    auto* object =
        make<ast::CoolClass>(intClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct Bool class
    auto* attributeList = make<ast::AttributeList>();

    auto* valName = make<ast::ObjectId>(std::string("val"));
    auto* valMember = make<ast::SingleMember>(valName, boolTypeName, noBody);
    attributeList->add(valMember);

    auto* boolClassName = make<ast::TypeId>(std::string("Bool"));
    auto* object =
        make<ast::CoolClass>(boolClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }

  {
    // construct String class
    auto* attributeList = make<ast::AttributeList>();

    auto* valName = make<ast::ObjectId>(std::string("val"));
    auto* valMember = make<ast::SingleMember>(valName, intTypeName, noBody);
    attributeList->add(valMember);

    auto* strFieldName = make<ast::ObjectId>(std::string("str_field"));
    auto* strFieldMember = make<ast::SingleMember>(strFieldName, strTypeName, noBody);
    attributeList->add(strFieldMember);

    auto* lengthName = make<ast::ObjectId>(std::string("length"));
    auto* lengthMethod = make<ast::SingleMethod>(lengthName, emptyParams, intTypeName, noBody);
    attributeList->add(lengthMethod);

    auto* concatName = make<ast::ObjectId>(std::string("concat"));
    auto* concatParams = buildFormalList({{"arg", strTypeName}});
    auto* concatMethod = make<ast::SingleMethod>(concatName, concatParams, strTypeName, noBody);
    attributeList->add(concatMethod);

    auto* substrName = make<ast::ObjectId>(std::string("substr"));
    auto* substrParams = buildFormalList({{"arg", intTypeName}, {"arg2", intTypeName}});
    auto* substrMethod = make<ast::SingleMethod>(substrName, substrParams, strTypeName, noBody);
    attributeList->add(substrMethod);

    auto* strClassName = make<ast::TypeId>(std::string("String"));
    auto* object =
        make<ast::CoolClass>(strClassName, objectClassName, attributeList, buildinFileName);
    classes->add(object);
  }
}