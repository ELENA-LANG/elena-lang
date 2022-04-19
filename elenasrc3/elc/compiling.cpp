//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiling processor body
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiling.h"
#include "cliconst.h"
#include "langcommon.h"
#include "codeimage.h"
#include "modulescope.h"
#include "bcwriter.h"

using namespace elena_lang;

// --- CompilingProcess ---

CompilingProcess :: CompilingProcess(PathString& appPath, PresenterBase* presenter, ErrorProcessor* errorProcessor,
   pos_t codeAlignment,
   JITSettings defaultCoreSettings,
   JITCompilerBase* (*compilerFactory)(LibraryLoaderBase*, PlatformType))
{
   _presenter = presenter;
   _errorProcessor = errorProcessor;
   _jitCompilerFactory = compilerFactory;
   _codeAlignment = codeAlignment;
   _defaultCoreSettings = defaultCoreSettings;

   PathString syntaxPath(*appPath, SYNTAX_FILE);
   FileReader syntax(*syntaxPath, FileRBMode, FileEncoding::Raw, false);
   if (syntax.isOpen()) {
      TerminalMap terminals(
         SyntaxTree::toParseKey(SyntaxKey::eof),
         SyntaxTree::toParseKey(SyntaxKey::identifier),
         SyntaxTree::toParseKey(SyntaxKey::reference),
         SyntaxTree::toParseKey(SyntaxKey::string),
         SyntaxTree::toParseKey(SyntaxKey::integer),
         SyntaxTree::toParseKey(SyntaxKey::hexinteger));

      _parser = new Parser(&syntax, terminals, _presenter);
      _compiler = new Compiler(
         _errorProcessor,
         TemplateProssesor::getInstance(),
         CompilerLogic::getInstance());
   }
   else {
      _errorProcessor->raisePathWarning(wrnSyntaxFileNotFound, *syntaxPath);

      _parser = nullptr;
      _compiler = nullptr;
   }
}

void CompilingProcess :: parseFile(FileIteratorBase& file_it, SyntaxWriterBase* syntaxWriter)
{
   _presenter->printPath(ELC_PARSING_FILE, *file_it);

   // save the path to the current source
   IdentifierString pathStr((*file_it).str());
   syntaxWriter->newNode(SyntaxTree::toParseKey(SyntaxKey::SourcePath), *pathStr);
   syntaxWriter->closeNode();

   TextFileReader source(*file_it, FileEncoding::UTF8, false);
   if (source.isOpen()) {
      try
      {
         _parser->parse(&source, syntaxWriter);
      }
      catch (ParserError& e)
      {
         e.path = *file_it;

         throw e;
      }
   }
   else {
      _errorProcessor->raisePathError(errInvalidFile, *file_it);
   }

}

void CompilingProcess :: parseModule(ModuleIteratorBase& module_it, SyntaxTreeBuilder& builder, ModuleScopeBase& moduleScope)
{
   auto& file_it = module_it.files();
   while (!file_it.eof()) {
      builder.newNode(SyntaxTree::toParseKey(SyntaxKey::Namespace));

      // generating syntax tree
      parseFile(file_it, &builder);

      builder.closeNode();

      ++file_it;
   }
}

void CompilingProcess :: compileModule(ModuleScopeBase& moduleScope, SyntaxTree& source, BuildTree& target)
{
   _compiler->declare(&moduleScope, source);
   _compiler->compile(&moduleScope, source, target);
}

void CompilingProcess :: generateModule(ModuleScopeBase& moduleScope, BuildTree& tree)
{
   ByteCodeWriter bcWriter(&_libraryProvider);
   bcWriter.save(tree, &moduleScope, moduleScope.minimalArgList);

   _libraryProvider.saveModule(moduleScope.module);
   _libraryProvider.saveDebugModule(moduleScope.debugModule);
}

void CompilingProcess :: buildModule(ModuleIteratorBase& module_it, SyntaxTree* syntaxTree,
   ForwardResolverBase* forwardResolver,
   pos_t stackAlingment,
   pos_t rawStackAlingment,
   int minimalArgList,
   bool withDebug)
{
   ModuleScope moduleScope(
      &_libraryProvider,
      forwardResolver,
      _libraryProvider.createModule(module_it.name()),
      withDebug ? _libraryProvider.createDebugModule(module_it.name()) : nullptr,
      stackAlingment, rawStackAlingment, minimalArgList);

   _compiler->prepare(&moduleScope, forwardResolver);

   SyntaxTreeBuilder builder(syntaxTree, _errorProcessor, &moduleScope);
   parseModule(module_it, builder, moduleScope);

   _presenter->print(ELC_COMPILING_MODULE, moduleScope.module->name());

   // generating build tree
   BuildTree buildTree;
   compileModule(moduleScope, *syntaxTree, buildTree);

   // generating byte code
   generateModule(moduleScope, buildTree);
}

void CompilingProcess :: configurate(ProjectBase& project)
{
   project.prepare();

   // load primitives
   auto path_it = project.allocPrimitiveIterator();
   while (!path_it->eof()) {
      IdentifierString key;
      path_it->loadKey(key);

      if ((*key).compare(CORE_ALIAS)) {
         _libraryProvider.addCorePath(**path_it);
      }
      else _libraryProvider.addPrimitivePath(*key, **path_it);

      ++(*path_it);
   }
   freeobj(path_it);

   // load packages
   auto package_it = project.allocPackageIterator();
   while (!package_it->eof()) {
      IdentifierString key;
      package_it->loadKey(key);

      _libraryProvider.addPackage(*key, **package_it);

      ++(*package_it);
   }
   freeobj(package_it);

   // set output paths
   path_t libPath = project.PathSetting(ProjectOption::LibPath);
   if (!libPath.empty())
      _libraryProvider.setRootPath(libPath);

   _libraryProvider.setOutputPath(project.PathSetting(ProjectOption::OutputPath));
   _libraryProvider.setNamespace(project.Namespace());
   _libraryProvider.addPackage(project.Namespace(), project.PathSetting(ProjectOption::OutputPath));
}

void CompilingProcess :: compile(ProjectBase& project,
   pos_t defaultStackAlignment,
   pos_t defaultRawStackAlignment,
   int minimalArgList)
{
   if (_parser == nullptr) {
      _errorProcessor->raiseInternalError(errParserNotInitialized);
   }

   SyntaxTree syntaxTree;

   auto module_it = project.allocModuleIterator();
   while (!module_it->eof()) {
      buildModule(*module_it, &syntaxTree, &project,
         project.IntSetting(ProjectOption::StackAlignment, defaultStackAlignment),
         project.IntSetting(ProjectOption::RawStackAlignment, defaultRawStackAlignment),
         minimalArgList,
         project.BoolSetting(ProjectOption::DebugMode, true));

      ++(*module_it);
   }

   freeobj(module_it);

   _presenter->print(ELC_SUCCESSFUL_COMPILATION);
}

void CompilingProcess :: link(ProjectBase& project, LinkerBase& linker)
{
   _presenter->print(ELC_LINKING);

   TargetImageInfo imageInfo;
   imageInfo.type = project.Platform();
   imageInfo.codeAlignment = _codeAlignment;
   imageInfo.autoClassSymbol = project.BoolSetting(ProjectOption::ClassSymbolAutoLoad);
   imageInfo.coreSettings.mgSize = project.IntSetting(ProjectOption::GCMGSize, _defaultCoreSettings.mgSize);
   imageInfo.coreSettings.ygSize = project.IntSetting(ProjectOption::GCYGSize, _defaultCoreSettings.ygSize);
   imageInfo.ns = project.StringSetting(ProjectOption::Namespace);

   TargetImage code(&project, &_libraryProvider, _jitCompilerFactory,
      imageInfo);

   linker.run(project, code);

   _presenter->print(ELC_SUCCESSFUL_LINKING);
}

void CompilingProcess :: greeting()
{
   // Greetings
   _presenter->print(ELC_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELC_REVISION_NUMBER);
}

void CompilingProcess :: cleanUp(ProjectBase& project)
{
   // clean modules
   auto module_it = project.allocModuleIterator();
   while (!module_it->eof()) {
      PathString path;
      _libraryProvider.resolvePath(module_it->name(), path);

      // remove a module
      PathUtil::removeFile(*path);

      // remove a debug module
      path.changeExtension("dnl");
      PathUtil::removeFile(*path);

      ++(*module_it);
   }

   freeobj(module_it);
   // remove executable
   path_t output = project.PathSetting(ProjectOption::TargetPath);
   if (!output.empty()) {
      PathString exePath(output);
      exePath.changeExtension("exe");

      PathUtil::removeFile(*exePath);

      // remove debug module
      exePath.changeExtension("dn");
      PathUtil::removeFile(*exePath);
   }
}

int CompilingProcess :: build(ProjectBase& project,
   LinkerBase& linker,
   pos_t defaultStackAlignment,
   pos_t defaultRawStackAlignment,
   int minimalArgList)
{
   try
   {
      configurate(project);

      PlatformType targetType = project.TargetType();

      // Project Greetings
      _presenter->print(ELC_STARTING, project.ProjectName(), getPlatformName(project.Platform()), getTargetTypeName(targetType));

      // Cleaning up
      _presenter->print(ELC_CLEANING);
      cleanUp(project);

      compile(project, defaultStackAlignment, defaultRawStackAlignment, minimalArgList);

      // generating target when required
      switch (targetType) {
         case PlatformType::Console:
            link(project, linker);
            break;
         case PlatformType::Library:
            //do nothing
            break;
         default:
            // to make compiler happy
            break;
      }

      return 0;
   }
   //catch (LinkerException e)
   //{
   //
   //}
   catch (ParserError e)
   {
      _presenter->printPath(e.message, e.path, e.lineInfo.row, e.lineInfo.column, e.token);

      _presenter->print(ELC_UNSUCCESSFUL);
      return -2;
   }
   catch (InvalidChar& e) {
      _presenter->print("(%d,%d): Invalid char %c\n", e.lineInfo.row, e.lineInfo.column, e.ch);

      _presenter->print(ELC_UNSUCCESSFUL);
      return -2;
   }
   catch (JITUnresolvedException& ex)
   {
      _presenter->print(_presenter->getMessage(errUnresovableLink), ex.referenceInfo.referenceName);

      _presenter->print(ELC_UNSUCCESSFUL);
      return -2;
   }
   catch (InternalError& ex) {
      _presenter->print(_presenter->getMessage(ex.messageCode));

      _presenter->print(ELC_UNSUCCESSFUL);
      return -2;
   }
   catch(AbortError&) {
      _presenter->print(ELC_UNSUCCESSFUL);
      return -2;
   }
   catch (...)
   {
      _presenter->print(_presenter->getMessage(errFatalError));
      _presenter->print(ELC_UNSUCCESSFUL);

      return -2;
   }
}
