//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiling processor header
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef COMPLING_H
#define COMPLING_H

#include "clicommon.h"
#include "libman.h"
#include "compiler.h"
#include "parser.h"
#include "derivation.h"

namespace elena_lang
{
   // --- CompilingProcess ---
   class CompilingProcess
   {
      // --- TemplateGenerator ---
      class TemplateGenerator : public TemplateProssesorBase
      {
         CompilingProcess* _process;
         TemplateProssesor _processor;

         ref_t generateTemplateName(ModuleScopeBase& moduleScope, ustr_t ns, Visibility visibility, 
            ref_t templateRef, List<SyntaxNode>& parameters, bool& alreadyDeclared);

      public:
         ref_t generateClassTemplate(ModuleScopeBase& moduleScope, ustr_t ns, ref_t templateRef,
            List<SyntaxNode>& parameters, bool declarationMode) override;

         bool importTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target, 
            List<SyntaxNode>& parameters) override;

         bool importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, 
            SyntaxNode target, List<SyntaxNode>& parameters) override;

         bool importCodeTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target, 
            List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) override;

         TemplateGenerator(CompilingProcess* process);
      };

      path_t              _prologName, _epilogName;

      PresenterBase*      _presenter;
      ErrorProcessor*     _errorProcessor;
      LibraryProvider     _libraryProvider;
      Compiler*           _compiler;
      Parser*             _parser;

      pos_t               _codeAlignment;
      JITSettings         _defaultCoreSettings;

      JITCompilerBase*(*_jitCompilerFactory)(LibraryLoaderBase*, PlatformType);

      TemplateGenerator   _templateGenerator;

      void buildSyntaxTree(ModuleScopeBase& moduleScope, SyntaxTree* syntaxTree, bool templateMode);

      void compileModule(ModuleScopeBase& moduleScope, SyntaxTree& source, BuildTree& target);
      void generateModule(ModuleScopeBase& moduleScope, BuildTree& tree, bool savingMode);
      void parseFileTemlate(ustr_t prolog, path_t name,
         SyntaxWriterBase* syntaxWriter);
      void parseFile(path_t projectPath,
         FileIteratorBase& file_it, 
         SyntaxWriterBase* syntaxWriter);
      void parseModule(path_t projectPath,
         ustr_t fileProlog, ustr_t fileEpilog,
         ModuleIteratorBase& module_it,
         SyntaxTreeBuilder& builder, 
         ModuleScopeBase& moduleScope);
      void buildModule(path_t projectPath,
         ustr_t fileProlog, ustr_t fileEpilog,
         ModuleIteratorBase& module_it, 
         SyntaxTree* syntaxTree, 
         ForwardResolverBase* forwardResolver,
         pos_t stackAlingment,
         pos_t rawStackAlingment,
         pos_t ehTableEntrySize,
         int minimalArgList,
         bool withDebug);

      void configurate(ProjectBase& project);
      void cleanUp(ProjectBase& project);
      void compile(ProjectBase& project, 
         pos_t defaultStackAlignment, 
         pos_t defaultRawStackAlignment,
         pos_t defaultEHTableEntrySize,
         int minimalArgList);
      void link(ProjectBase& project, LinkerBase& linker);

   public:
      void greeting();
      int build(ProjectBase& project, 
         LinkerBase& linker, 
         pos_t defaultStackAlignment, 
         pos_t defaultRawStackAlignment,
         pos_t defaultEHTableEntrySize,
         int minimalArgList);
      int clean(ProjectBase& project);

      CompilingProcess(PathString& appPath, path_t prologName, path_t epilogName,
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
