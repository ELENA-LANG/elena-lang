//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Common implementation
//                                             (C)2024-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "pch.h"
// --------------------------------------------------------------------------
#include "tests_common.h"
#include "module.h"

#include "serializer.h"

#include <windows.h>

using namespace elena_lang;

#ifdef _M_IX86

constexpr auto DEFAULT_STACKALIGNMENT = 1;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 4;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 16;

constexpr int MINIMAL_ARG_LIST = 1;

#elif _M_X64

constexpr auto DEFAULT_STACKALIGNMENT = 2;
constexpr auto DEFAULT_RAW_STACKALIGNMENT = 16;
constexpr auto DEFAULT_EHTABLE_ENTRY_SIZE = 32;

constexpr int MINIMAL_ARG_LIST = 2;

#endif

// --- TestModuleScope ---

TestModuleScope::TestModuleScope(bool tapeOptMode)
   : ModuleScopeBase(new Module(), nullptr, DEFAULT_STACKALIGNMENT, DEFAULT_RAW_STACKALIGNMENT, 
      DEFAULT_EHTABLE_ENTRY_SIZE, MINIMAL_ARG_LIST, sizeof(uintptr_t), tapeOptMode)
{
   _anonymousRef = 0x100;
}

bool TestModuleScope :: isStandardOne()
{
   return false;
}

bool TestModuleScope :: withValidation()
{
   return false;
}

ref_t TestModuleScope :: mapAnonymous(ustr_t prefix)
{
   IdentifierString name("'", prefix, INLINE_CLASSNAME);
   name.appendInt(_anonymousRef++);

   return module->mapReference(*name);
}

ref_t TestModuleScope :: mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility)
{
   ReferenceName fullName(ns, identifier);

   return module->mapReference(*fullName);
}

ref_t TestModuleScope :: mapTemplateIdentifier(ustr_t identifier, Visibility visibility, bool& alreadyDeclared,
   bool declarationMode)
{
   return 0;
}

ref_t TestModuleScope :: mapFullReference(ustr_t referenceName, bool existing)
{
   if (emptystr(referenceName))
      return 0;

   return module->mapReference(referenceName, existing);
}

ref_t TestModuleScope :: mapWeakReference(ustr_t referenceName, bool existing)
{
   return 0;
}

ExternalInfo TestModuleScope :: mapExternal(ustr_t dllAlias, ustr_t functionName)
{
   return {};
}

inline ref_t mapExistingIdentifier(ModuleBase* module, ustr_t identifier, Visibility visibility)
{
   ustr_t prefix = CompilerLogic::getVisibilityPrefix(visibility);

   IdentifierString name(prefix, identifier);

   return module->mapReference(*name, true);
}

ref_t TestModuleScope :: resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility)
{
   if (!ns.empty()) {
      ReferenceName fullName(ns, identifier);

      return ::mapExistingIdentifier(module, *fullName, visibility);
   }
   else return ::mapExistingIdentifier(module, identifier, visibility);
}

ref_t TestModuleScope :: resolveImportedIdentifier(ustr_t identifier, IdentifierList* importedNs)
{
   return 0;
}

ref_t TestModuleScope :: resolveWeakTemplateReferenceID(ref_t reference)
{
   return 0;
}

SectionInfo TestModuleScope :: getSection(ustr_t referenceName, ref_t mask, bool silentMode)
{
   SectionInfo info = {};

   if (isWeakReference(referenceName)) {
      info.module = module;
      info.reference = module->mapReference(referenceName, true);
      info.section = module->mapSection(info.reference | mask, true);
   }

   return info;
}

MemoryBase* TestModuleScope :: mapSection(ref_t reference, bool existing)
{
   return module->mapSection(reference, existing);
}

ModuleInfo TestModuleScope :: getModule(ustr_t referenceName, bool silentMode)
{
   if (isWeakReference(referenceName)) {
      return { module, module->mapReference(referenceName) };
   }
   return {};
}

ModuleInfo TestModuleScope :: getWeakModule(ustr_t referenceName, bool silentMode)
{
   return {};
}

ref_t TestModuleScope :: loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly, bool fieldsOnly)
{
   ModuleInfo moduleInfo = { module, module->mapReference(referenceName)};

   return CompilerLogic::loadClassInfo(info, moduleInfo, module, headerOnly, fieldsOnly);
}

ref_t TestModuleScope :: loadSymbolInfo(SymbolInfo& info, ustr_t referenceName)
{
   return 0;
}

void TestModuleScope :: newNamespace(ustr_t name)
{
}

bool TestModuleScope :: includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion)
{
   return false;
}

bool TestModuleScope :: isDeclared(ref_t reference)
{
   return false;
}

bool TestModuleScope :: isSymbolDeclared(ref_t reference)
{
   return false;
}

Visibility TestModuleScope :: retrieveVisibility(ref_t reference)
{
   return Visibility::Private;
}

// --- TestTemplateProssesor ---

ref_t TestTemplateProssesor :: generateClassTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   List<SyntaxNode>& parameters, bool declarationMode, ExtensionMap* outerExtensionList)
{
   ref_t mapping = _mapping.get({ templateRef, parameters.get(1).arg.reference});
   if (mapping)
      return mapping;

   return templateRef;
}

bool TestTemplateProssesor :: importTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
   List<SyntaxNode>& parameters)
{
   return false;
}

bool TestTemplateProssesor :: importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
   List<SyntaxNode>& parameters)
{
   return false;
}

bool TestTemplateProssesor :: importPropertyTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
   List<SyntaxNode>& parameters)
{
   return false;
}

bool TestTemplateProssesor :: importCodeTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
   List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   return false;
}

bool TestTemplateProssesor :: importExpressionTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
   List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   return false;
}

bool TestTemplateProssesor :: importEnumTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   return false;
}

bool TestTemplateProssesor :: importTextblock(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target)
{
   return false;
}

// --- CompilerEnvironment ---

CompilerEnvironment :: CompilerEnvironment()
   : _templateMapping(0)
{

}

ModuleScopeBase* CompilerEnvironment :: createModuleScope(bool tapeOptMode, bool withAttributes)
{
   auto scope = new TestModuleScope(tapeOptMode);

   if (withAttributes) {
      scope->attributes.add("dispatch", V_DISPATCHER);
      scope->attributes.add("public", V_PUBLIC);
      scope->attributes.add("var", V_VARIABLE);
      scope->attributes.add("new", V_NEWOP);
      scope->attributes.add("constructor", V_CONSTRUCTOR);
   }

   return scope;
}

void CompilerEnvironment :: initializeOperators(ModuleScopeBase* scope)
{
   scope->buildins.refer_message =
      encodeMessage(scope->module->mapAction(REFER_MESSAGE, 0, false),
         2, 0);
   scope->buildins.value_message =
      encodeMessage(scope->module->mapAction(VALUE_MESSAGE, 0, false),
         1, PROPERTY_MESSAGE);
}

void CompilerEnvironment :: setUpTemplateMockup(ref_t templateRef, ref_t elementRef, ref_t reference)
{
   _templateMapping.add({ templateRef, elementRef }, reference);
}

Compiler* CompilerEnvironment :: createCompiler()
{
   auto compiler = new Compiler(nullptr, TestErrorProcessor::getInstance(), TestTemplateProssesor::getInstance(&_templateMapping), CompilerLogic::getInstance());

   compiler->setNoValidation();
   compiler->setDebugMode(false);

   return compiler;
}

// --- BaseFixture ---

void BaseFixture :: LoadDeclarationScenario(ustr_t common, ustr_t descr)
{
   DynamicUStr syntax(common);

   size_t index = common.findStr("$1");
   if (index != NOTFOUND_POS) {
      syntax.cut(index, 2);
      syntax.insert(descr, index);
   }

   SyntaxTreeSerializer::load(syntax.str(), declarationNode);
}

void BaseFixture :: LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2)
{
   DynamicUStr syntax(common);

   size_t index = common.findStr("$1");
   if (index != NOTFOUND_POS) {
      syntax.cut(index, 2);

      syntax.insert(descr2, index);
      syntax.insert(" ", index);
      syntax.insert(descr1, index);
   }

   SyntaxTreeSerializer::load(syntax.str(), declarationNode);
}

void BaseFixture :: LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2, ustr_t descr3)
{
   DynamicUStr syntax(common);

   size_t index = common.findStr("$1");
   if (index != NOTFOUND_POS) {
      syntax.cut(index, 2);

      syntax.insert(descr3, index);
      syntax.insert(" ", index);
      syntax.insert(descr2, index);
      syntax.insert(" ", index);
      syntax.insert(descr1, index);
   }

   SyntaxTreeSerializer::load(syntax.str(), declarationNode);
}

void BaseFixture :: LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2, ustr_t descr3, ustr_t descr4)
{
   DynamicUStr syntax(common);

   size_t index = common.findStr("$1");
   if (index != NOTFOUND_POS) {
      syntax.cut(index, 2);

      syntax.insert(descr4, index);
      syntax.insert(" ", index);
      syntax.insert(descr3, index);
      syntax.insert(" ", index);
      syntax.insert(descr2, index);
      syntax.insert(" ", index);
      syntax.insert(descr1, index);
   }

   SyntaxTreeSerializer::load(syntax.str(), declarationNode);
}

void BaseFixture :: LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2, ustr_t descr3, ustr_t descr4, ustr_t descr5)
{
   DynamicUStr syntax(common);

   size_t index = common.findStr("$1");
   if (index != NOTFOUND_POS) {
      syntax.cut(index, 2);

      syntax.insert(descr5, index);
      syntax.insert(" ", index);
      syntax.insert(descr4, index);
      syntax.insert(" ", index);
      syntax.insert(descr3, index);
      syntax.insert(" ", index);
      syntax.insert(descr2, index);
      syntax.insert(" ", index);
      syntax.insert(descr1, index);
   }

   SyntaxTreeSerializer::load(syntax.str(), declarationNode);
}

void elena_lang::getAppPath(PathString& appPath)
{
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(nullptr, path, MAX_PATH);

   appPath.copySubPath(path, false);
   appPath.lower();
}

// --- ScenarioTest ---

SyntaxNode ScenarioTest::findTargetNode(int)
{
   return findClassNode().findChild(SyntaxKey::Method);
}

SyntaxNode ScenarioTest::findClassNode()
{
   return SyntaxTree::gotoChild(declarationNode.firstChild(), SyntaxKey::Class, targetRef);
}

void ScenarioTest::SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   BuildTreeWriter buildWriter(buildTree);
   buildWriter.appendNode(BuildKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);

   controlOutputNode = buildTree.readRoot().appendChild(BuildKey::Tape);
}

void ScenarioTest :: run(ModuleScopeBase* moduleScope, int scenario)
{
   moduleScope->predefined.add("nil", V_NIL);

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::Namespace nsScope(compiler, moduleScope, TestErrorProcessor::getInstance(), nullptr, nullptr);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   Compiler::Class classHelper(nsScope, targetRef, Visibility::Public);
   classHelper.load();
   Compiler::Method methodHelper(classHelper);

   SyntaxNode methodNode = findTargetNode(scenario);
   if (methodNode != SyntaxKey::None)
      methodHelper.compile(writer, methodNode);

   // Assess
   bool matched = BuildTree::compare(output.readRoot(), controlOutputNode, true);
   EXPECT_TRUE(matched);

   freeobj(compiler);
   freeobj(moduleScope);
}

// --- CompileTest ---

void CompileTest :: SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
}

// --- MethodScenarioTest ---

void MethodScenarioTest :: runTest(bool withProtectedConstructor, bool withAttributes, int syntaxScenario, int buildScrenario)
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true, withAttributes);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.intReference = intNumberRef;

   if (argArrayTemplateRef != INVALID_REF) {
      moduleScope->buildins.argArrayTemplateReference = argArrayTemplateRef;

      env.setUpTemplateMockup(argArrayTemplateRef, 1, genericVargRef);
      env.setUpTemplateMockup(argArrayTemplateRef, 2, targetVargRef);
   }
   if (byRefTemplateRef != INVALID_REF) {
      moduleScope->buildins.wrapperTemplateReference = byRefTemplateRef;

      env.setUpTemplateMockup(byRefTemplateRef, intNumberRef, intByRefRef);
   }

   moduleScope->buildins.constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
         0, FUNCTION_MESSAGE);
   if (withProtectedConstructor)
      moduleScope->buildins.protected_constructor_message =
      encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE2, 0, false),
         0, FUNCTION_MESSAGE);

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   writer.newNode(BuildKey::Root);
   Compiler::Namespace nsScope(compiler, moduleScope, TestErrorProcessor::getInstance(), nullptr, nullptr);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   Compiler::Class classHelper(nsScope, targetRef, Visibility::Public);
   classHelper.load();
   Compiler::Method methodHelper(classHelper);

   SyntaxNode methodNode = findTargetNode(syntaxScenario);
   if (methodNode == SyntaxKey::Method) {
      methodHelper.compile(writer, methodNode);
   }
   else if (methodNode == SyntaxKey::Constructor) {
      Compiler::ClassClass classClassHelper(classHelper);
      classClassHelper.load();

      methodHelper.compileConstructor(writer, methodNode, classClassHelper);
   }      

   writer.closeNode();

   // Assess
   bool matched = BuildTree::compare(getExpectedOutput(output.readRoot().firstChild(), buildScrenario),
      getExpectedOutput(controlOutputNode, buildScrenario), !checkTargetMessage);
   EXPECT_TRUE(matched);

   freeobj(compiler);
   freeobj(moduleScope);
}

SyntaxNode MethodScenarioTest :: findAutoGenerated(SyntaxNode classNode)
{
   SyntaxNode current = classNode.firstChild();
   while (current != SyntaxKey::None) {
      if (current == SyntaxKey::Method && current.existChild(SyntaxKey::Autogenerated))
         return current;

      current = current.nextNode();
   }

   return {};
}

// --- ExprTest ---

void ExprTest::SetUp()
{
   SyntaxTreeWriter writer(syntaxTree);
   writer.appendNode(SyntaxKey::Root);

   BuildTreeWriter buildWriter(buildTree);
   buildWriter.appendNode(BuildKey::Root);

   declarationNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 1);
   exprNode = syntaxTree.readRoot().appendChild(SyntaxKey::Idle, 2);

   buildNode = buildTree.readRoot().appendChild(BuildKey::Tape);
}

void ExprTest :: runBuildTest(bool declareDefaultMessages, bool declareOperators, ref_t funcRef)
{
   // Arrange
   ModuleScopeBase* moduleScope = env.createModuleScope(true, declareDefaultMessages);
   moduleScope->buildins.superReference = 1;
   moduleScope->buildins.intReference = 2;
   moduleScope->buildins.wrapperTemplateReference = 3;
   moduleScope->buildins.closureTemplateReference = funcRef;

   if (declareDefaultMessages) {
      moduleScope->buildins.dispatch_message = encodeMessage(
         moduleScope->module->mapAction(DISPATCH_MESSAGE, 0, false), 1, 0);
      moduleScope->buildins.constructor_message =
         encodeMessage(moduleScope->module->mapAction(CONSTRUCTOR_MESSAGE, 0, false),
            0, FUNCTION_MESSAGE);
   }

   Compiler* compiler = env.createCompiler();

   BuildTree output;
   BuildTreeWriter writer(output);
   Compiler::Namespace nsScope(compiler, moduleScope, nullptr, nullptr, nullptr);

   Compiler::Class cls(nsScope, 0, Visibility::Internal);
   Compiler::Method method(cls);
   Compiler::Code code(method);

   // Act
   nsScope.declare(declarationNode.firstChild(), true);

   if (declareOperators)
      env.initializeOperators(moduleScope);

   writer.newNode(BuildKey::Root);
   writer.newNode(BuildKey::Tape);
   Compiler::Expression expression(code, writer);
   expression.compileRoot(exprNode.firstChild(), ExpressionAttribute::NoDebugInfo);
   writer.closeNode();
   writer.closeNode();

   // Assess
   bool matched = BuildTree::compare(buildNode, output.readRoot().firstChild(), true);
   EXPECT_TRUE(matched);

   freeobj(compiler);
   freeobj(moduleScope);
}
