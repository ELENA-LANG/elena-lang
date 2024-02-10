//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiling processor header
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMPLING_H
#define COMPLING_H

#include "clicommon.h"
#include "libman.h"
#include "compiler.h"
#include "parser.h"
#include "derivation.h"
#include "project.h"

namespace elena_lang
{
   // --- CompilingProcess ---
   class CompilingProcess
   {
      struct ModuleSettings
      {
         pos_t          stackAlingment;
         pos_t          rawStackAlingment;
         pos_t          ehTableEntrySize;
         bool           debugMode;
         bool           multiThreadMode;
         ManifestInfo   manifestInfo;
      };

      // --- TemplateGenerator ---
      class TemplateGenerator : public TemplateProssesorBase
      {
         CompilingProcess* _process;
         TemplateProssesor _processor;

         ref_t declareTemplateName(ModuleScopeBase& moduleScope, ustr_t ns, Visibility visibility,
            ref_t templateRef, List<SyntaxNode>& parameters);
         ref_t generateTemplateName(ModuleScopeBase& moduleScope, ustr_t ns, Visibility visibility,
            ref_t templateRef, List<SyntaxNode>& parameters, bool& alreadyDeclared);

      public:
         ref_t generateClassTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t templateRef,
            List<SyntaxNode>& parameters, bool declarationMode, ExtensionMap* outerExtensionList) override;

         bool importTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target, 
            List<SyntaxNode>& parameters) override;

         bool importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, 
            SyntaxNode target, List<SyntaxNode>& parameters) override;

         bool importPropertyTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, 
            SyntaxNode target, List<SyntaxNode>& parameters) override;

         bool importCodeTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target, 
            List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) override;

         TemplateGenerator(CompilingProcess* process);
      };

      path_t              _modulePrologName, _prologName, _epilogName;

      PresenterBase*      _presenter;
      ErrorProcessor*     _errorProcessor;
      LibraryProvider     _libraryProvider;
      Compiler*           _compiler;
      Parser*             _parser;

      pos_t               _codeAlignment;
      JITSettings         _defaultCoreSettings;

      JITCompilerBase*(*_jitCompilerFactory)(LibraryLoaderBase*, PlatformType);

      TemplateGenerator   _templateGenerator;

      MemoryDump          _bcRules;
      MemoryDump          _btRules;

      bool                _verbose;

      IdentifierList      _forwards;

      void buildSyntaxTree(ModuleScopeBase& moduleScope, SyntaxTree* syntaxTree, bool templateMode, 
         ExtensionMap* outerExtensionList);

      void parseFileStandart(SyntaxWriterBase* syntaxWriter, path_t path);
      void parseFileUserDefinedGrammar(SyntaxWriterBase* syntaxWriter, path_t path,
         ProjectTarget* parserTarget, path_t projectPath);

      void compileModule(ModuleScopeBase& moduleScope, SyntaxTree& source, BuildTree& target, 
         ExtensionMap* outerExtensionList);
      void generateModule(ModuleScopeBase& moduleScope, BuildTree& tree, bool savingMode);
      void parseFileTemlate(ustr_t prolog, path_t name,
         SyntaxWriterBase* syntaxWriter);
      void parseFile(path_t projectPath,
         FileIteratorBase& file_it, 
         SyntaxWriterBase* syntaxWriter,
         ProjectTarget* parserTarget);
      void parseModule(ProjectEnvironment& env,
         ModuleIteratorBase& module_it,
         SyntaxTreeBuilder& builder, 
         ModuleScopeBase& moduleScope);
      void buildModule(ProjectEnvironment& env,
         ModuleIteratorBase& module_it, 
         SyntaxTree* syntaxTree, 
         ForwardResolverBase* forwardResolver,
         ModuleSettings& moduleSettings,
         int minimalArgList,
         int ptrSize);

      void configurate(Project& project);
      void cleanUp(ProjectBase& project);
      void compile(ProjectBase& project, 
         pos_t defaultStackAlignment, 
         pos_t defaultRawStackAlignment,
         pos_t defaultEHTableEntrySize,
         int minimalArgList,
         bool multiThreadMode);
      void link(Project& project, LinkerBase& linker, bool withTLS);

   public:
      void addForward(ustr_t f)
      {
         _forwards.add(f.clone());
      }

      void greeting();
      int build(Project& project, 
         LinkerBase& linker, 
         pos_t defaultStackAlignment, 
         pos_t defaultRawStackAlignment,
         pos_t defaultEHTableEntrySize,
         int minimalArgList);
      int clean(Project& project);

      void setVerboseOn()
      {
         _verbose = true;
         _compiler->setVerboseOn();
      }

      CompilingProcess(PathString& appPath, path_t modulePrologName, path_t prologName, path_t epilogName,
         PresenterBase* presenter, ErrorProcessor* errorProcessor,
         pos_t codeAlignment,
         JITSettings defaultCoreSettings,
         JITCompilerBase* (*compilerFactory)(LibraryLoaderBase*, PlatformType));

      virtual ~CompilingProcess()
      {
         freeobj(_parser);
         freeobj(_compiler);
      }
   };
}

#endif // COMPLING_H
