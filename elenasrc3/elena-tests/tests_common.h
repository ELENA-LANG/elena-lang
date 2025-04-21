//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Common declarations
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include "pch.h"
#include "compiler.h"

namespace elena_lang 
{
   class TestException
   {
   public:
      int code;

      TestException(int code)
         : code(code)
      {
      }
   };

   class TestErrorProcessor : public ErrorProcessor
   {
      TestErrorProcessor()
         : ErrorProcessor(nullptr)
      {
         setWarningLevel(WarningLevel::Level0);
      }

   public:
      void info(int code, ustr_t arg) override
      {
      }

      void info(int code, ustr_t arg, ustr_t arg2) override
      {
      }

      void raiseError(int code, ustr_t arg) override
      {
         throw TestException(code);
      }

      void raisePathError(int code, path_t arg) override
      {
         throw TestException(code);
      }

      void raisePathWarning(int code, path_t arg) override
      {
         throw TestException(code);
      }

      void raiseInternalError(int code) override
      {
         throw TestException(code);
      }

      void raiseTerminalError(int code, ustr_t pathArg, SyntaxNode node) override
      {
         throw TestException(code);
      }

      void raiseTerminalWarning(int level, int code, ustr_t pathArg, SyntaxNode node)
      {
      }

      static ErrorProcessor* getInstance()
      {
         static TestErrorProcessor instance;

         return &instance;
      }

   };

   // --- TestModuleScope ---
   class TestModuleScope : public ModuleScopeBase
   {
      ref_t _anonymousRef;

   public:
      bool isStandardOne() override;
      bool withValidation() override;

      ref_t mapAnonymous(ustr_t prefix) override;

      ref_t mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) override;
      ref_t mapTemplateIdentifier(ustr_t identifier, Visibility visibility, bool& alreadyDeclared,
         bool declarationMode) override;

      ref_t mapFullReference(ustr_t referenceName, bool existing) override;
      ref_t mapWeakReference(ustr_t referenceName, bool existing) override;

      ExternalInfo mapExternal(ustr_t dllAlias, ustr_t functionName) override;

      ref_t resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) override;
      ref_t resolveImportedIdentifier(ustr_t identifier, IdentifierList* importedNs) override;

      ref_t resolveWeakTemplateReferenceID(ref_t reference) override;

      ustr_t resolveFullName(ref_t reference) override
      {
         ustr_t referenceName = module->resolveReference(reference);
         /*if (isTemplateWeakReference(referenceName)) {
            return resolveWeakTemplateReference(referenceName + TEMPLATE_PREFIX_NS_LEN);
         }
         else */return referenceName;
      }

      SectionInfo getSection(ustr_t referenceName, ref_t mask, bool silentMode) override;
      MemoryBase* mapSection(ref_t reference, bool existing) override;

      ModuleInfo getModule(ustr_t referenceName, bool silentMode) override;
      ModuleInfo getWeakModule(ustr_t referenceName, bool silentMode) override;

      ref_t loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly, bool fieldsOnly) override;
      ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly, bool fieldsOnly) override
      {
         return loadClassInfo(info, module->resolveReference(reference), headerOnly, fieldsOnly);
      }

      ref_t loadSymbolInfo(SymbolInfo& info, ustr_t referenceName) override;
      ref_t loadSymbolInfo(SymbolInfo& info, ref_t reference) override
      {
         return loadSymbolInfo(info, module->resolveReference(reference));
      }

      void newNamespace(ustr_t name) override;
      bool includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion) override;

      bool isDeclared(ref_t reference) override;
      bool isSymbolDeclared(ref_t reference) override;

      Visibility retrieveVisibility(ref_t reference) override;

      void flush() override
      {
      }

      bool declareImport(ustr_t, ustr_t) override
      {
         return false;
      }

      ustr_t resolveImport(ustr_t alias) override
      {
         return alias;
      }

      bool checkVariable(ustr_t) override
      {
         return false;
      }

      TestModuleScope(bool tapeOptMode);
   };

   class TestTemplateProssesor : public TemplateProssesorBase
   {
      Map<Pair<ref_t, ref_t>, ref_t> _mapping;

      TestTemplateProssesor()
         : _mapping(0)
      {

      }

   public:
      ref_t generateClassTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
         List<SyntaxNode>& parameters, bool declarationMode, ExtensionMap* outerExtensionList) override;

      bool importTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& parameters) override;
      bool importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& parameters) override;
      bool importPropertyTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& parameters) override;
      bool importCodeTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) override;
      bool importExpressionTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
         List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) override;
      bool importEnumTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
         SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) override;
      bool importTextblock(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target) override;

      static TemplateProssesorBase* getInstance(Map<Pair<ref_t, ref_t>, ref_t>* mapping = nullptr)
      {
         static TestTemplateProssesor instance;
         if (mapping) {
            instance._mapping.clear();
            for (auto it = mapping->start(); !it.eof(); ++it) {
               instance._mapping.add(it.key(), *it);
            }
         }

         return &instance;
      }
   };

   // --- CompilerEnvironment ---
   class CompilerEnvironment
   {
      Map<Pair<ref_t, ref_t>, ref_t> _templateMapping;

   public:
      void initializeOperators(ModuleScopeBase* scope);

      ModuleScopeBase* createModuleScope(bool tapeOptMode, bool withAttributes = false);

      void setUpTemplateMockup(ref_t templateRef, ref_t elementRef, ref_t reference);

      Compiler* createCompiler();

      CompilerEnvironment();
   };

   // --- BaseFixture ---
   class BaseFixture : public testing::Test
   {
   protected:
      SyntaxTree syntaxTree;

      SyntaxNode declarationNode;

      void LoadDeclarationScenario(ustr_t common, ustr_t descr);
      void LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2);
      void LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2, ustr_t descr3);
      void LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2, ustr_t descr3, ustr_t descr4);
      void LoadDeclarationScenario(ustr_t common, ustr_t descr1, ustr_t descr2, ustr_t descr3, ustr_t descr4, ustr_t descr5);

   public:
      CompilerEnvironment env;
   };

   // --- ScenarioTest --- 
   class ScenarioTest : public BaseFixture
   {
   protected:
      BuildTree  buildTree;

      BuildNode  controlOutputNode;

      ref_t      targetRef;

      virtual BuildNode getExpectedOutput(BuildNode node, int)
      {
         return node;
      }

      virtual SyntaxNode findClassNode();
      virtual SyntaxNode findTargetNode(int scenario);

      void SetUp() override;
   };

   // --- ScenarioTest --- 
   class CompileTest : public BaseFixture
   {
   protected:
      void SetUp() override;
   };

   // --- ExprTest ---
   class ExprTest : public BaseFixture
   {
   protected:
      BuildTree  buildTree;

      SyntaxNode exprNode;

      BuildNode  buildNode;

   protected:
      void SetUp() override;

   public:
      void runBuildTest(bool declareDefaultMessages = false, bool declareOperators = false, ref_t funcRef = 0);
   };

   // --- MethodScenarioTest ---
   class MethodScenarioTest : public ScenarioTest
   {
   protected:
      bool       checkTargetMessage;

      ref_t      argArrayTemplateRef;
      ref_t      byRefTemplateRef;

      ref_t      genericVargRef;
      ref_t      targetVargRef;
      ref_t      intNumberRef;
      ref_t      intByRefRef;

      SyntaxNode findAutoGenerated(SyntaxNode classNode);

   public:
      void runTest(bool withProtectedConstructor = false, bool withAttributes = false, int syntaxScenario = 0, int buildScrenario = 0);

      MethodScenarioTest()
      {
         checkTargetMessage = false;

         argArrayTemplateRef = INVALID_REF;
         byRefTemplateRef = INVALID_REF;

         genericVargRef = targetVargRef = intNumberRef = 0;
         intByRefRef = 0;
      }
   };

   void getAppPath(PathString& appPath);
}

#endif // TESTS_COMMON_H