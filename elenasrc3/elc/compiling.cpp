//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiling processor body
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiling.h"
#include "cliconst.h"
#include "langcommon.h"
#include "codeimage.h"
#include "modulescope.h"
#include "bcwriter.h"
#include "separser.h"

using namespace elena_lang;

// --- AddressMapper ---
class AddressMapper : public AddressMapperBase
{
   PresenterBase* presenter;

   Map<Pair<addr_t, mssg_t>, addr_t> addresses;

public:
   void addMethod(addr_t vaddress, mssg_t message, addr_t methodPosition) override
   {
      addresses.add({ vaddress, message }, methodPosition);
   }

   void addSymbol(addr_t vaddress, addr_t position) override
   {
      addresses.add({ vaddress, 0 }, position);
   }

   void output(ReferenceMapper* mapper, LinkResult& result)
   {
      for(auto it = addresses.start(); !it.eof(); ++it) {
         if (it.key().value2) {
            mssg_t message = it.key().value2;
            addr_t address = result.code + *it;
            ref_t signRef = 0;

            IdentifierString fullName(mapper->retrieveReference(it.key().value1, mskVMTRef));
            fullName.append('.');
            fullName.append(mapper->retrieveAction(getAction(message), signRef));
            if (signRef != 0) {
               fullName.append('<');
               ref_t dummy = 0;
               fullName.append(mapper->retrieveAction(signRef | 0x80000000, dummy));
               fullName.append('>');
            }
            fullName.append('[');
            fullName.appendInt(getArgCount(message));
            fullName.append(']');

            fullName.append("=");
            fullName.appendUInt((unsigned int)address, 16);

            presenter->printLine(*fullName);
         }
         else {
            addr_t address = result.code + *it;

            IdentifierString fullName(mapper->retrieveReference(it.key().value1, mskSymbolRef));
            fullName.append("=");
            fullName.appendUInt((unsigned int)address, 16);

            presenter->printLine(*fullName);
         }
      }
   }

   AddressMapper(PresenterBase* presenter)
      : addresses(0u)
   {
      this->presenter = presenter;
   }
};

// --- CompilingProcess::TemplateGenerator ---

CompilingProcess::TemplateGenerator :: TemplateGenerator(CompilingProcess* process)
   : _process(process)
{
}

bool CompilingProcess::TemplateGenerator :: importTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   auto sectionInfo = moduleScope.getSection(
      moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

   if (!sectionInfo.section)
      return false;

   _processor.importTemplate(sectionInfo.section, target, parameters);

   return true;
}

bool CompilingProcess::TemplateGenerator :: importPropertyTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   auto sectionInfo = moduleScope.getSection(
      moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

   if (!sectionInfo.section)
      return false;

   _processor.importInlinePropertyTemplate(sectionInfo.section, target, parameters);

   return true;
}

bool CompilingProcess::TemplateGenerator :: importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   SyntaxNode target, List<SyntaxNode>& parameters)
{
   auto sectionInfo = moduleScope.getSection(
      moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

   if (!sectionInfo.section)
      return false;

   _processor.importInlineTemplate(sectionInfo.section, target, parameters);

   return true;
}

bool CompilingProcess::TemplateGenerator :: importCodeTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   auto sectionInfo = moduleScope.getSection(
      moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

   if (!sectionInfo.section)
      return false;

   _processor.importCodeTemplate(sectionInfo.section, target, arguments, parameters);

   return true;
}

ref_t CompilingProcess::TemplateGenerator :: generateTemplateName(ModuleScopeBase& moduleScope, ustr_t ns, Visibility visibility,
   ref_t templateRef, List<SyntaxNode>& parameters, bool& alreadyDeclared)
{
   ModuleBase* module = moduleScope.module;

   ustr_t templateName = module->resolveReference(templateRef);

   IdentifierString name;
   if (isWeakReference(templateName)) {
      name.copy(module->name());
      name.append(templateName);
   }
   else name.copy(templateName);

   for(auto it = parameters.start(); !it.eof(); ++it) {
      name.append("&");

      ref_t typeRef = (*it).arg.reference;
      ustr_t param = module->resolveReference(typeRef);
      if (isWeakReference(param)) {
         name.append(module->name());
         name.append(param);
      }
      else name.append(param);
   }
   name.replaceAll('\'', '@', 0);

   return moduleScope.mapTemplateIdentifier(ns, *name, visibility, alreadyDeclared, false);
}

ref_t CompilingProcess::TemplateGenerator :: declareTemplateName(ModuleScopeBase& moduleScope, ustr_t ns, Visibility visibility, 
   ref_t templateRef, List<SyntaxNode>& parameters)
{
   ModuleBase* module = moduleScope.module;

   ustr_t templateName = module->resolveReference(templateRef);
   IdentifierString name;
   if (isWeakReference(templateName)) {
      name.copy(module->name());
      name.append(templateName);
   }
   else name.copy(templateName);

   for (auto it = parameters.start(); !it.eof(); ++it) {
      name.append("&");

      ref_t typeRef = (*it).arg.reference;
      ustr_t param = module->resolveReference(typeRef);
      if (isWeakReference(param)) {
         name.append(module->name());
         name.append(param);
      }
      else name.append(param);
   }
   name.replaceAll('\'', '@', 0);

   bool dummy = false;
   return moduleScope.mapTemplateIdentifier(ns, *name, visibility, dummy, true);
}

ref_t CompilingProcess::TemplateGenerator :: generateClassTemplate(ModuleScopeBase& moduleScope, ustr_t ns,
   ref_t templateRef, List<SyntaxNode>& parameters, bool declarationMode, ExtensionMap* outerExtensionList)
{
   ref_t generatedReference = 0;

   if (declarationMode) {
      generatedReference = declareTemplateName(moduleScope, ns, Visibility::Public, templateRef,
         parameters);
   }
   else {
      auto sectionInfo = moduleScope.getSection(
         moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

      SyntaxTree syntaxTree;

      bool alreadyDeclared = false;
      generatedReference = generateTemplateName(moduleScope, ns, Visibility::Public, templateRef,
         parameters, alreadyDeclared);

      if (alreadyDeclared && moduleScope.isDeclared(generatedReference))
         return generatedReference;

      SyntaxTreeWriter writer(syntaxTree);
      writer.newNode(SyntaxKey::Root);
      writer.newNode(SyntaxKey::Namespace, ns);

      _processor.generateClassTemplate(&moduleScope, generatedReference, writer,
         sectionInfo.section, parameters);

      writer.closeNode();
      writer.closeNode();

      if (_process->_verbose) {
         ustr_t templateName = moduleScope.module->resolveReference(generatedReference);

         _process->_presenter->print(ELC_COMPILING_TEMPLATE, templateName);
      }

      _process->buildSyntaxTree(moduleScope, &syntaxTree, true, outerExtensionList);
   }

   return generatedReference;
}

// --- CompilingProcess ---

CompilingProcess :: CompilingProcess(PathString& appPath, path_t prologName, path_t epilogName,
   PresenterBase* presenter, ErrorProcessor* errorProcessor,
   pos_t codeAlignment,
   JITSettings defaultCoreSettings,
   JITCompilerBase* (*compilerFactory)(LibraryLoaderBase*, PlatformType)
) :
   _templateGenerator(this)
{
   _prologName = prologName;
   _epilogName = epilogName;

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
         SyntaxTree::toParseKey(SyntaxKey::character),
         SyntaxTree::toParseKey(SyntaxKey::wide),
         SyntaxTree::toParseKey(SyntaxKey::integer),
         SyntaxTree::toParseKey(SyntaxKey::hexinteger),
         SyntaxTree::toParseKey(SyntaxKey::longinteger),
         SyntaxTree::toParseKey(SyntaxKey::real),
         SyntaxTree::toParseKey(SyntaxKey::constant));

      _parser = new Parser(&syntax, terminals, _presenter);
      _compiler = new Compiler(
         _presenter,
         _errorProcessor,
         &_templateGenerator,
         CompilerLogic::getInstance());
   }
   else {
      _errorProcessor->raisePathWarning(wrnSyntaxFileNotFound, *syntaxPath);

      _parser = nullptr;
      _compiler = nullptr;
   }

   PathString bcRulesPath(*appPath, BC_RULES_FILE);
   FileReader bcRuleReader(*bcRulesPath, FileRBMode, FileEncoding::Raw, false);
   if (bcRuleReader.isOpen()) {
      _bcRules.load(bcRuleReader, bcRuleReader.length());
   }

   PathString btRulesPath(*appPath, BT_RULES_FILE);
   FileReader btRuleReader(*btRulesPath, FileRBMode, FileEncoding::Raw, false);
   if (btRuleReader.isOpen()) {
      _btRules.load(btRuleReader, btRuleReader.length());
   }

   _verbose = false;
}

void CompilingProcess :: parseFileTemlate(ustr_t prolog, path_t name,
   SyntaxWriterBase* syntaxWriter)
{
   if (!prolog)
      return;

   StringTextReader<char> reader(prolog);
   try
   {
      _parser->parse(&reader, syntaxWriter);
   }
   catch (ParserError& e)
   {
      e.path = name;

      throw e;
   }

}

void CompilingProcess :: parseFileStandart(SyntaxWriterBase* syntaxWriter, path_t path)
{
   TextFileReader source(path, FileEncoding::UTF8, false);
   if (source.isOpen()) {
      try
      {
         _parser->parse(&source, syntaxWriter);
      }
      catch (ParserError& e)
      {
         e.path = path;

         throw e;
      }
   }
   else {
      _errorProcessor->raisePathError(errInvalidFile, path);
   }
}

void CompilingProcess :: parseFileUserDefinedGrammar(SyntaxWriterBase* syntaxWriter, path_t path,
   ProjectTarget* parserTarget, path_t projectPath)
{
   ScriptParser parser;

   // loading target options
   size_t i = 0;
   IdentifierString option;
   do {
      size_t index = (*parserTarget->options).findSub(i, '\n', parserTarget->options.length());
      option.copy(*parserTarget->options + i, index - i);

      parser.setOption(*option, projectPath);

      i = index + 1;
   } while (i < parserTarget->options.length());

   try {
      // based on the target type generate the syntax tree for the file
      PathString fullPath(projectPath);
      fullPath.combine(path);

      SyntaxTree derivationTree;
      parser.parse(*fullPath, derivationTree);

      syntaxWriter->saveTree(derivationTree);
   }
   catch (ParserError& e)
   {
      e.path = path;

      throw e;
   }
}

void CompilingProcess :: parseFile(path_t projectPath,
   FileIteratorBase& file_it,
   SyntaxWriterBase* syntaxWriter,
   ProjectTarget* parserTarget)
{
   // save the path to the current source
   path_t sourceRelativePath = (*file_it).str();
   if (!projectPath.empty())
      sourceRelativePath += projectPath.length() + 1;

   _presenter->printPathLine(ELC_PARSING_FILE, sourceRelativePath);

   IdentifierString pathStr(sourceRelativePath);
   syntaxWriter->newNode(SyntaxTree::toParseKey(SyntaxKey::SourcePath), *pathStr);
   syntaxWriter->closeNode();

   int type = parserTarget ? parserTarget->type : 1;

   switch (type) {
      case 1:
         parseFileStandart(syntaxWriter, *file_it);
         break;
      case 2:
         parseFileUserDefinedGrammar(syntaxWriter, *file_it, parserTarget, projectPath);
         break;
      default:
      {
         IdentifierString typeStr;
         typeStr.appendInt(type);

         _errorProcessor->raiseError(errInvalidParserTargetType, *typeStr);
         break;
      }
   }
}

void CompilingProcess :: parseModule(ProjectEnvironment& env,
   ModuleIteratorBase& module_it,
   SyntaxTreeBuilder& builder,
   ModuleScopeBase& moduleScope)
{
   IdentifierString target;

   auto& file_it = module_it.files();
   while (!file_it.eof()) {
      builder.newNode(SyntaxTree::toParseKey(SyntaxKey::Namespace));

      ProjectTarget* parserTarget = nullptr;
      if (file_it.loadTarget(target)) {
         parserTarget = env.targets.get(target.str());
         if (!parserTarget)
            _errorProcessor->raiseError(errInvalidParserTarget, target.str());
      }

      // generating syntax tree
      parseFileTemlate(*env.fileProlog, _prologName, &builder); // !! temporal explicit prolog
      parseFile(*env.projectPath, file_it, &builder, parserTarget);
      parseFileTemlate(*env.fileEpilog, _epilogName, &builder);

      builder.closeNode();

      ++file_it;
   }
}

void CompilingProcess :: compileModule(ModuleScopeBase& moduleScope, SyntaxTree& source, BuildTree& target, 
   ExtensionMap* outerExtensionList)
{
   _compiler->declare(&moduleScope, source, outerExtensionList);
   _compiler->compile(&moduleScope, source, target, outerExtensionList);
}

void CompilingProcess :: generateModule(ModuleScopeBase& moduleScope, BuildTree& tree, bool savingMode)
{
   ByteCodeWriter bcWriter(&_libraryProvider);
   if (_btRules.length() > 0)
      bcWriter.loadBuildTreeRules(&_btRules);
   if (_bcRules.length() > 0)
      bcWriter.loadByteCodeRules(&_bcRules);

   bcWriter.save(tree, &moduleScope, moduleScope.minimalArgList, moduleScope.tapeOptMode);

   if (savingMode) {
      // saving a module
      _presenter->print(ELC_SAVING_MODULE, moduleScope.module->name());

      _libraryProvider.saveModule(moduleScope.module);
      _libraryProvider.saveDebugModule(moduleScope.debugModule);
   }
}

void CompilingProcess :: buildSyntaxTree(ModuleScopeBase& moduleScope, SyntaxTree* syntaxTree, bool templateMode, 
   ExtensionMap* outerExtensionList)
{
   // generating build tree
   BuildTree buildTree;
   compileModule(moduleScope, *syntaxTree, buildTree, outerExtensionList);

   // generating byte code
   generateModule(moduleScope, buildTree, !templateMode);
}

void CompilingProcess :: buildModule(ProjectEnvironment& env,
   ModuleIteratorBase& module_it, SyntaxTree* syntaxTree,
   ForwardResolverBase* forwardResolver,
   pos_t stackAlingment,
   pos_t rawStackAlingment,
   pos_t ehTableEntrySize,
   int minimalArgList,
   int ptrSize,
   bool withDebug)
{
   ModuleScope moduleScope(
      &_libraryProvider,
      forwardResolver,
      _libraryProvider.createModule(module_it.name()),
      withDebug ? _libraryProvider.createDebugModule(module_it.name()) : nullptr,
      stackAlingment, rawStackAlingment, ehTableEntrySize, minimalArgList, ptrSize);

   _compiler->prepare(&moduleScope, forwardResolver);

   SyntaxTreeBuilder builder(syntaxTree, _errorProcessor,
      &moduleScope, &_templateGenerator);
   parseModule(env, module_it, builder, moduleScope);

   _presenter->print(ELC_COMPILING_MODULE, moduleScope.module->name());

   buildSyntaxTree(moduleScope, syntaxTree, false, nullptr);
}

void CompilingProcess :: configurate(Project& project)
{
   project.prepare();

   project.initLoader(_libraryProvider);

   _libraryProvider.setOutputPath(project.PathSetting(ProjectOption::ProjectPath));
   _libraryProvider.setNamespace(project.Namespace());
   _libraryProvider.addPackage(project.Namespace(), project.PathSetting(ProjectOption::OutputPath));

   int optMode = project.IntSetting(ProjectOption::OptimizationMode, optMiddle);
   _compiler->setOptimizationMode(optMode);

   bool withMethodParamInfo = project.BoolSetting(ProjectOption::GenerateParamNameInfo, true);
   _compiler->setMethodParamInfo(withMethodParamInfo);
}

void CompilingProcess :: compile(ProjectBase& project,
   pos_t defaultStackAlignment,
   pos_t defaultRawStackAlignment,
   pos_t defaultEHTableEntrySize,
   int minimalArgList)
{
   if (_parser == nullptr) {
      _errorProcessor->raiseInternalError(errParserNotInitialized);
   }

   // load the environment
   ProjectEnvironment env;
   project.initEnvironment(env);

   // compile the project
   SyntaxTree syntaxTree;

   auto module_it = project.allocModuleIterator();
   while (!module_it->eof()) {
      buildModule(
         env, *module_it, &syntaxTree, &project,
         project.IntSetting(ProjectOption::StackAlignment, defaultStackAlignment),
         project.IntSetting(ProjectOption::RawStackAlignment, defaultRawStackAlignment),
         project.IntSetting(ProjectOption::EHTableEntrySize, defaultEHTableEntrySize),
         minimalArgList,
         sizeof(uintptr_t),
         project.BoolSetting(ProjectOption::DebugMode, true));

      ++(*module_it);
   }

   freeobj(module_it);

   _presenter->print(ELC_SUCCESSFUL_COMPILATION);
}

void CompilingProcess :: link(Project& project, LinkerBase& linker, bool withTLS)
{
   _presenter->print(ELC_LINKING);

   TargetImageInfo imageInfo;
   imageInfo.type = project.Platform();
   imageInfo.codeAlignment = _codeAlignment;
   imageInfo.autoClassSymbol = project.BoolSetting(ProjectOption::ClassSymbolAutoLoad, _defaultCoreSettings.classSymbolAutoLoad);
   imageInfo.coreSettings.mgSize = project.IntSetting(ProjectOption::GCMGSize, _defaultCoreSettings.mgSize);
   imageInfo.coreSettings.ygSize = project.IntSetting(ProjectOption::GCYGSize, _defaultCoreSettings.ygSize);
   imageInfo.coreSettings.threadCounter = project.IntSetting(ProjectOption::ThreadCounter, _defaultCoreSettings.threadCounter);
   imageInfo.ns = project.StringSetting(ProjectOption::Namespace);
   imageInfo.withTLS = withTLS;

   AddressMapper* addressMapper = nullptr;
   if (project.BoolSetting(ProjectOption::MappingOutputMode))
      addressMapper = new AddressMapper(_presenter);

   TargetImage code(project.SystemTarget(), &project, &_libraryProvider, _jitCompilerFactory,
      imageInfo, addressMapper);

   auto result = linker.run(project, code);

   _presenter->print(ELC_SUCCESSFUL_LINKING);

   if (addressMapper) {
      addressMapper->output(&code, result);
   }

   freeobj(addressMapper);
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

int CompilingProcess :: clean(Project& project)
{
   configurate(project);

   cleanUp(project);

   return 0;
}

int CompilingProcess :: build(Project& project,
   LinkerBase& linker,
   pos_t defaultStackAlignment,
   pos_t defaultRawStackAlignment,
   pos_t defaultEHTableEntrySize,
   int minimalArgList)
{
   try
   {
      configurate(project);

      PlatformType targetType = project.TargetType();

      // Project Greetings
      _presenter->printLine(ELC_STARTING, project.ProjectName(), getPlatformName(project.Platform()),
         getTargetTypeName(targetType, project.SystemTarget()));

      // Cleaning up
      _presenter->printLine(ELC_CLEANING);
      cleanUp(project);

      compile(project, defaultStackAlignment, defaultRawStackAlignment, defaultEHTableEntrySize, minimalArgList);

      // generating target when required
      switch (targetType) {
         case PlatformType::Console:
            link(project, linker, false);
            break;
         case PlatformType::MTA_Console:
            link(project, linker, true);
            break;
         case PlatformType::Library:
            //do nothing
            break;
         default:
            // to make compiler happy
            break;
      }

      return _errorProcessor->hasWarnings() ? WARNING_RET_CODE : 0;
   }
   //catch (LinkerException e)
   //{
   //
   //}
   catch (ParserError e)
   {
      _presenter->printPath(e.message, e.path, e.lineInfo.row, e.lineInfo.column, e.token);

      _presenter->print(ELC_UNSUCCESSFUL);
      return ERROR_RET_CODE;
   }
   catch (InvalidChar& e) {
      _presenter->print("(%d,%d): Invalid char %c\n", e.lineInfo.row, e.lineInfo.column, e.ch);

      _presenter->print(ELC_UNSUCCESSFUL);
      return ERROR_RET_CODE;
   }
   catch (JITUnresolvedException& ex)
   {
      _presenter->print(_presenter->getMessage(errUnresovableLink), ex.referenceInfo.referenceName);

      _presenter->print(ELC_UNSUCCESSFUL);
      return ERROR_RET_CODE;
   }
   catch (InternalError& ex) {
      _presenter->print(_presenter->getMessage(ex.messageCode), ex.arg);

      _presenter->print(ELC_UNSUCCESSFUL);
      return ERROR_RET_CODE;
   }
   catch(AbortError&) {
      _presenter->print(ELC_UNSUCCESSFUL);
      return ERROR_RET_CODE;
   }
   catch (...)
   {
      _presenter->print(_presenter->getMessage(errFatalError));
      _presenter->print(ELC_UNSUCCESSFUL);

      return ERROR_RET_CODE;
   }
}
