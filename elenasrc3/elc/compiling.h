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
      PresenterBase*      _presenter;
      ErrorProcessor*     _errorProcessor;
      LibraryProvider     _libraryProvider;
      Compiler*           _compiler;
      Parser*             _parser;

      pos_t               _codeAlignment;
      JITSettings         _defaultCoreSettings;

      JITCompilerBase*(*_jitCompilerFactory)(LibraryLoaderBase*, PlatformType);

      void compileModule(ModuleScopeBase& moduleScope, SyntaxTree& source, BuildTree& target);
      void generateModule(ModuleScopeBase& moduleScope, BuildTree& tree);
      void parseFile(FileIteratorBase& file_it, SyntaxWriterBase* syntaxWriter);
      void parseModule(ModuleIteratorBase& module_it, SyntaxTreeBuilder& builder, ModuleScopeBase& moduleScope);
      void buildModule(ModuleIteratorBase& module_it, 
         SyntaxTree* syntaxTree, 
         ForwardResolverBase* forwardResolver,
         pos_t stackAlingment,
         pos_t rawStackAlingment,
         int minimalArgList,
         bool withDebug);

      void configurate(ProjectBase& project);
      void cleanUp(ProjectBase& project);
      void compile(ProjectBase& project, 
         pos_t defaultStackAlignment, 
         pos_t defaultRawStackAlignment,
         int minimalArgList);
      void link(ProjectBase& project, LinkerBase& linker);

   public:
      void greeting();
      int build(ProjectBase& project, 
         LinkerBase& linker, 
         pos_t defaultStackAlignment, 
         pos_t defaultRawStackAlignment,
         int minimalArgList);

      CompilingProcess(PathString& appPath, PresenterBase* presenter, ErrorProcessor* errorProcessor,
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
