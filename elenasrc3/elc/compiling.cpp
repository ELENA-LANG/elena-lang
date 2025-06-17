//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiling processor body
//
//                                             (C)2021-2025, by Aleksey Rakov
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
#include "serializer.h"

//#define SHOW_BREAKPOINTS 1

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

bool CompilingProcess::TemplateGenerator :: importExpressionTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   auto sectionInfo = moduleScope.getSection(
      moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

   if (!sectionInfo.section)
      return false;

   _processor.importExpressionTemplate(sectionInfo.section, target, arguments, parameters);

   return true;
}

bool CompilingProcess::TemplateGenerator :: importParameterizedTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
   SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters)
{
   auto sectionInfo = moduleScope.getSection(
      moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

   if (!sectionInfo.section)
      return false;

   _processor.importParameterizedTemplate(sectionInfo.section, target, arguments, parameters);

   return true;
}

bool CompilingProcess::TemplateGenerator :: importTextblock(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target)
{
   auto sectionInfo = moduleScope.getSection(
      moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

   if (!sectionInfo.section)
      return false;

   _processor.importTextblock(sectionInfo.section, target);

   return true;
}

size_t getLengthSkipPostfix(ustr_t name)
{
   size_t len = name.length();

   for (size_t i = len - 1; i != 0; i--) {
      if (name[i] == '\'' || name[i] == '&') {
         break;
      }
      else if (name[i] == '#')
         return i;
   }

   return len;
}

void CompilingProcess::TemplateGenerator :: defineTemplateName(ModuleScopeBase& moduleScope, IdentifierString& name,
   ref_t templateRef, List<SyntaxNode>& parameters)
{
   ModuleBase* module = moduleScope.module;

   ustr_t templateName = module->resolveReference(templateRef);

   if (isWeakReference(templateName)) {
      name.copy(module->name());
      name.append(templateName);
   }
   else name.copy(templateName);

   for (auto it = parameters.start(); !it.eof(); ++it) {
      name.append("&");

      ref_t typeRef = (*it).arg.reference;
      ustr_t param = module->resolveReference(typeRef);

      size_t paramLen = getLengthSkipPostfix(param);

      if (isTemplateWeakReference(param)) {
         // HOTFIX : save template based reference as is
         name.append(param, paramLen);
      }
      else if (isWeakReference(param)) {
         name.append(module->name());
         name.append(param, paramLen);
      }
      else name.append(param, paramLen);

      // NOTE : the names must be different for normal and nullable template argument
      if ((*it).existChild(SyntaxKey::NullableType))
         name.append("#nble");
   }
   name.replaceAll('\'', '@', 0);
}

ref_t CompilingProcess::TemplateGenerator :: generateTemplateName(ModuleScopeBase& moduleScope, Visibility visibility,
   ref_t templateRef, List<SyntaxNode>& parameters, bool& alreadyDeclared)
{
   IdentifierString name;
   defineTemplateName(moduleScope, name, templateRef, parameters);

   return moduleScope.mapTemplateIdentifier(*name, visibility, alreadyDeclared, false);
}

ref_t CompilingProcess::TemplateGenerator :: declareTemplateName(ModuleScopeBase& moduleScope, Visibility visibility,
   ref_t templateRef, List<SyntaxNode>& parameters)
{
   IdentifierString name;
   defineTemplateName(moduleScope, name, templateRef, parameters);

   bool dummy = false;
   return moduleScope.mapTemplateIdentifier(*name, visibility, dummy, true);
}

ref_t CompilingProcess::TemplateGenerator :: generateClassTemplate(ModuleScopeBase& moduleScope,
   ref_t templateRef, List<SyntaxNode>& parameters, bool declarationMode, ExtensionMap* outerExtensionList)
{
   ref_t generatedReference = 0;

   if (declarationMode) {
      generatedReference = declareTemplateName(moduleScope, Visibility::Public, templateRef,
         parameters);
   }
   else {
      auto sectionInfo = moduleScope.getSection(
         moduleScope.module->resolveReference(templateRef), mskSyntaxTreeRef, true);

      SyntaxTree syntaxTree;

      bool alreadyDeclared = false;
      generatedReference = generateTemplateName(moduleScope, Visibility::Public, templateRef,
         parameters, alreadyDeclared);

      if (alreadyDeclared && moduleScope.isDeclared(generatedReference))
         return generatedReference;

      SyntaxTreeWriter writer(syntaxTree);
      writer.newNode(SyntaxKey::Root);
      writer.newNode(SyntaxKey::Namespace);

      _processor.generateClassTemplate(&moduleScope, generatedReference, writer,
         sectionInfo.section, parameters, templateRef);

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

CompilingProcess :: CompilingProcess(path_t appPath, path_t exeExtension,
   path_t modulePrologName, path_t prologName, path_t epilogName,
   PresenterBase* presenter, ErrorProcessor* errorProcessor,
   pos_t codeAlignment,
   ProcessSettings& defaultCoreSettings,
   JITCompilerBase* (*compilerFactory)(PlatformType)
) :
   _appPath(appPath),
   _parser(nullptr),
   _templateGenerator(this),
   _traceMode(false),
   _forwards(nullptr)
{
   _exeExtension = exeExtension;
   _modulePrologName = modulePrologName;
   _prologName = prologName;
   _epilogName = epilogName;

   _presenter = presenter;
   _errorProcessor = errorProcessor;
   _jitCompilerFactory = compilerFactory;
   _codeAlignment = codeAlignment;
   _defaultCoreSettings = defaultCoreSettings;

   _compiler = new Compiler(
      _presenter,
      _errorProcessor,
      &_templateGenerator,
      CompilerLogic::getInstance());

   PathString bcRulesPath(appPath, BC_RULES_FILE);
   FileReader bcRuleReader(*bcRulesPath, FileRBMode, FileEncoding::Raw, false);
   if (bcRuleReader.isOpen()) {
      _bcRules.load(bcRuleReader, bcRuleReader.length());
   }

   PathString btRulesPath(appPath, BT_RULES_FILE);
   FileReader btRuleReader(*btRulesPath, FileRBMode, FileEncoding::Raw, false);
   if (btRuleReader.isOpen()) {
      _btRules.load(btRuleReader, btRuleReader.length());
   }

   PathString btXRulesPath(appPath, BT_XRULES_FILE);
   FileReader btXRuleReader(*btXRulesPath, FileRBMode, FileEncoding::Raw, false);
   if (btRuleReader.isOpen()) {
      _btXRules.load(btXRuleReader, btXRuleReader.length());
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
      SyntaxTree derivationTree;
      parser.parse(path, derivationTree);

      syntaxWriter->saveTree(derivationTree);

      parser.clearStack();
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
   if (!projectPath.empty()) {
      if (!sourceRelativePath.startsWith(projectPath))
         _errorProcessor->raisePathError(errInvalidFile, sourceRelativePath);

      sourceRelativePath += projectPath.length() + 1;
   }      

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
      parseFileTemlate(*env.fileProlog, _prologName, &builder);
      parseFile(*env.projectPath, file_it, &builder, parserTarget);
      parseFileTemlate(*env.fileEpilog, _epilogName, &builder);

      builder.closeNode();

      ++file_it;
   }
}

bool CompilingProcess :: compileModule(ModuleScopeBase& moduleScope, SyntaxTree& source, BuildTree& target,
   ExtensionMap* outerExtensionList)
{
   bool nothingToCompile = _compiler->declare(&moduleScope, source, outerExtensionList);
   if (!nothingToCompile) {
      _compiler->compile(&moduleScope, source, target, outerExtensionList);

      return true;
   }
   return false;
}

void CompilingProcess :: generateModule(ModuleScopeBase& moduleScope, BuildTree& tree, bool savingMode)
{
   ByteCodeWriter bcWriter(&_libraryProvider, true);
   if (_btRules.length() > 0)
      bcWriter.loadBuildTreeRules(&_btRules);
   if (_btXRules.length() > 0)
      bcWriter.loadBuildTreeXRules(&_btXRules);
   if (_bcRules.length() > 0)
      bcWriter.loadByteCodeRules(&_bcRules);

   bcWriter.save(tree, &moduleScope, moduleScope.minimalArgList, moduleScope.ptrSize,
      moduleScope.tapeOptMode);

   if (savingMode) {
      // saving a module
      _presenter->print(ELC_SAVING_MODULE, moduleScope.module->name());

      moduleScope.flush();

      _libraryProvider.saveModule(moduleScope.module);
      _libraryProvider.saveDebugModule(moduleScope.debugModule);

      if (_verbose) {
         PathString path;
         _libraryProvider.retrievePath(moduleScope.module, path);
         _presenter->printPath(ELC_MODULE_TARGET_PATH, *path);
      }
   }
}

inline void printTree(PresenterBase* presenter, SyntaxNode node, List<SyntaxKey>* filters)
{
   DynamicUStr target;

   SyntaxTreeSerializer::save(node, target, filters);

   presenter->print(target.str());
}

inline void printTree(PresenterBase* presenter, BuildNode node, List<BuildKey>* filters)
{
   DynamicUStr target;

   BuildTreeSerializer::save(node, target, filters);

   presenter->print(target.str());
}

void CompilingProcess :: printSyntaxTree(SyntaxTree& syntaxTree)
{
   List<SyntaxKey> filters(SyntaxKey::None);
   if (!_verbose) {
      filters.add(SyntaxKey::Column);
      filters.add(SyntaxKey::Row);
      filters.add(SyntaxKey::SourcePath);
      filters.add(SyntaxKey::InlineTemplate);
   }

   _presenter->print("\nSyntax Tree:");
   printTree(_presenter, syntaxTree.readRoot(), &filters);
}

void CompilingProcess :: printBuildTree(ModuleBase* module, BuildTree& buildTree)
{
   List<BuildKey> filters(BuildKey::None);
   if (!_verbose) {
      filters.add(BuildKey::Path);
#ifndef SHOW_BREAKPOINTS
      filters.add(BuildKey::Breakpoint);
      filters.add(BuildKey::VirtualBreakpoint);
      filters.add(BuildKey::EOPBreakpoint);
      filters.add(BuildKey::OpenStatement);
      filters.add(BuildKey::EndStatement);
#endif
      filters.add(BuildKey::ClassName);
      filters.add(BuildKey::MethodName);
      filters.add(BuildKey::VariableInfo);
      filters.add(BuildKey::ArgumentsInfo);
      filters.add(BuildKey::Idle);
   }

   _presenter->print("\nBuild Tree:");

   BuildNode node = buildTree.readRoot();
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::Symbol:
            _presenter->print("\n@symbol %s", module->resolveReference(current.arg.reference));

            printTree(_presenter, current, &filters);
            break;
         case BuildKey::Class:
            _presenter->print("\n@class %s", module->resolveReference(current.arg.reference));

            printTree(_presenter, current, &filters);
            break;
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }
}

bool CompilingProcess :: buildSyntaxTree(ModuleScopeBase& moduleScope, SyntaxTree* syntaxTree, bool templateMode,
   ExtensionMap* outerExtensionList)
{
   // print the incoming syntax tree if required
   if (_traceMode) {
      printSyntaxTree(*syntaxTree);
   }

   // generating build tree
   BuildTree buildTree;
   bool retVal = compileModule(moduleScope, *syntaxTree, buildTree, outerExtensionList);

   // generating byte code
   generateModule(moduleScope, buildTree, !templateMode);

   // print the outcome bui tree if required
   if (_traceMode) {
      printBuildTree(moduleScope.module, buildTree);
   }

   return retVal;
}

bool CompilingProcess :: buildModule(ProjectEnvironment& env,
   LexicalMap::Iterator& lexical_it,
   ModuleIteratorBase& module_it, SyntaxTree* syntaxTree,
   ForwardResolverBase* forwardResolver,
   VariableResolverBase* variableResolver,
   ModuleSettings& moduleSettings,
   int minimalArgList,
   int ptrSize)
{
   ModuleScope moduleScope(
      &_libraryProvider,
      forwardResolver,
      variableResolver,
      _libraryProvider.createModule(module_it.name()),
      moduleSettings.debugMode ? _libraryProvider.createDebugModule(module_it.name()) : nullptr,
      moduleSettings.stackAlingment,
      moduleSettings.rawStackAlingment,
      moduleSettings.ehTableEntrySize,
      minimalArgList, ptrSize,
      module_it.hints());

   // Validation : standart module must be named "system"
   if (moduleScope.isStandardOne())
      assert(module_it.name().compare(STANDARD_MODULE));

   // loading lexical elements
   while (!lexical_it.eof()) {
      ustr_t name = lexical_it.key();
      ustr_t ns = *lexical_it;

      CompilerLogic::loadMetaData(&moduleScope, name, ns);

      ++lexical_it;
   }

   _compiler->prepare(&moduleScope, forwardResolver, moduleSettings.manifestInfo);

   SyntaxTreeBuilder builder(syntaxTree, _errorProcessor,
      &moduleScope, &_templateGenerator);
   parseModule(env, module_it, builder, moduleScope);

   _presenter->print(ELC_COMPILING_MODULE, moduleScope.module->name());

   return buildSyntaxTree(moduleScope, syntaxTree, false, nullptr);
}

void CompilingProcess :: configurateParser(SyntaxVersion version)
{
   ustr_t syntaxDialect = SYNTAX67_FILE;
   switch (version) {
      case SyntaxVersion::L5:
         syntaxDialect = SYNTAX50_FILE;
         if (_verbose)
            _presenter->printLine("EL5 Dialect");
         break;
      case SyntaxVersion::L6:
         syntaxDialect = SYNTAX60_FILE;
         if (_verbose)
            _presenter->printLine("EL6 Dialect");
         break;
      default:
      case SyntaxVersion::L7:
         syntaxDialect = SYNTAX67_FILE;
         if (_verbose)
            _presenter->printLine("EL7 Dialect");
         break;
   }   

   PathString syntaxPath(_appPath, syntaxDialect);
   FileReader syntax(*syntaxPath, FileRBMode, FileEncoding::Raw, false);
   if (syntax.isOpen()) {
      TerminalMap terminals(
         SyntaxTree::toParseKey(SyntaxKey::eof),
         SyntaxTree::toParseKey(SyntaxKey::identifier),
         SyntaxTree::toParseKey(SyntaxKey::reference),
         SyntaxTree::toParseKey(SyntaxKey::globalreference),
         SyntaxTree::toParseKey(SyntaxKey::string),
         SyntaxTree::toParseKey(SyntaxKey::character),
         SyntaxTree::toParseKey(SyntaxKey::wide),
         SyntaxTree::toParseKey(SyntaxKey::integer),
         SyntaxTree::toParseKey(SyntaxKey::hexinteger),
         SyntaxTree::toParseKey(SyntaxKey::longinteger),
         SyntaxTree::toParseKey(SyntaxKey::real),
         SyntaxTree::toParseKey(SyntaxKey::constant),
         SyntaxTree::toParseKey(SyntaxKey::interpolate));

      _parser = new Parser(&syntax, terminals, _presenter);
   }
   else {
      _errorProcessor->raisePathWarning(wrnSyntaxFileNotFound, *syntaxPath);

      _parser = nullptr;
   }
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

   bool withConditionalBoxing = project.BoolSetting(ProjectOption::ConditionalBoxing, DEFAULT_CONDITIONAL_BOXING);
   _compiler->setConditionalBoxing(withConditionalBoxing);

   bool evalOpFlag = project.BoolSetting(ProjectOption::EvaluateOp, DEFAULT_EVALUATE_OP);
   _compiler->setEvaluateOp(evalOpFlag);

   bool strictTypeFlag = project.BoolSetting(ProjectOption::StrictTypeEnforcing, DEFAULT_STRICT_TYPE_ENFORCING);
   _compiler->setStrictTypeFlag(strictTypeFlag);

   bool nullableTypeWarning = project.BoolSetting(ProjectOption::NullableTypeWarning, DEFAULT_NULLABLE_TYPE_WARNING);
   _compiler->setNullableTypeFlag(nullableTypeWarning);

   // load program forwards
   for (auto it = _forwards.start(); !it.eof(); ++it) {
      ustr_t f = *it;

      size_t index = f.find('=');
      if (index != NOTFOUND_POS) {
         IdentifierString key(f, index);

         project.addForward(*key, f + index + 1);
      }
   }
}

void CompilingProcess :: compile(ProjectBase& project, JITCompilerSettings& jitSettings)
{
   if (_parser == nullptr) {
      _errorProcessor->raiseInternalError(errParserNotInitialized);
   }

   _traceMode = project.BoolSetting(ProjectOption::TracingMode);

   // load the environment
   ProjectEnvironment env;
   project.initEnvironment(env);

   // compile the project
   SyntaxTree syntaxTree;

   bool compiled = false;
   auto module_it = project.allocModuleIterator();
   while (!module_it->eof()) {
      ModuleSettings moduleSettings =
      {
         project.UIntSetting(ProjectOption::StackAlignment, jitSettings.stackAlignment),
         project.UIntSetting(ProjectOption::RawStackAlignment, jitSettings.rawStackAlignment),
         project.UIntSetting(ProjectOption::EHTableEntrySize, jitSettings.ehTableEntrySize),
         project.BoolSetting(ProjectOption::DebugMode, true),
         {
            project.StringSetting(ProjectOption::ManifestName),
            project.StringSetting(ProjectOption::ManifestVersion),
            project.StringSetting(ProjectOption::ManifestAuthor)
         }
      };

      LexicalMap::Iterator lexical_it = project.getLexicalIterator();
      compiled |= buildModule(
         env,
         lexical_it,
         *module_it, &syntaxTree, &project, &project,
         moduleSettings,
         jitSettings.minimalStackLength,
         sizeof(uintptr_t));

      ++(*module_it);
   }

   freeobj(module_it);

   _presenter->print(compiled
      ? ELC_SUCCESSFUL_COMPILATION : ELC_IDLE_COMPILATION);
}

void CompilingProcess :: link(Project& project, LinkerBase& linker, bool withTLS)
{
   // ignore link operation for trace mode
   if (_traceMode)
      return;

   PlatformType uiType = project.UITargetType();

   _presenter->print(ELC_LINKING);

   TargetImageInfo imageInfo;
   imageInfo.type = project.Platform();
   imageInfo.codeAlignment = _codeAlignment;
   imageInfo.autoClassSymbol = project.BoolSetting(ProjectOption::ClassSymbolAutoLoad, _defaultCoreSettings.classSymbolAutoLoad);
   imageInfo.coreSettings.withAlignedJump = project.BoolSetting(ProjectOption::WithJumpAlignment, _defaultCoreSettings.withAlignedJump);
   imageInfo.autoModuleExtension = project.BoolSetting(ProjectOption::ModuleExtensionAutoLoad, false);
   imageInfo.coreSettings.mgSize = project.IntSetting(ProjectOption::GCMGSize, _defaultCoreSettings.mgSize);
   imageInfo.coreSettings.ygSize = project.IntSetting(ProjectOption::GCYGSize, _defaultCoreSettings.ygSize);
   imageInfo.coreSettings.stackReserved = project.IntSetting(ProjectOption::StackReserved, _defaultCoreSettings.stackReserved);
   imageInfo.coreSettings.threadCounter = project.IntSetting(ProjectOption::ThreadCounter, _defaultCoreSettings.threadCounter);
   imageInfo.ns = project.StringSetting(ProjectOption::Namespace);
   imageInfo.withTLS = withTLS;

   AddressMapper* addressMapper = nullptr;
   if (project.BoolSetting(ProjectOption::MappingOutputMode))
      addressMapper = new AddressMapper(_presenter);

   TargetImage code(project.SystemTarget(), &project, &_libraryProvider, _jitCompilerFactory,
      imageInfo, addressMapper);

   // HOTFIX : TLS variable can be used only for MTA
   if (!withTLS && code.getTLSSection()->length() > 0) {
      _errorProcessor->raiseError(errTLSIsNotAllowed);

      return;
   }

   auto result = linker.run(project, code, imageInfo.type, uiType, _exeExtension);

   _presenter->print(ELC_SUCCESSFUL_LINKING);

   if (addressMapper) {
      addressMapper->output(&code, result);
   }

   freeobj(addressMapper);
}

void CompilingProcess :: greeting(PresenterBase* presenter)
{
   // Greetings
   presenter->print(ELC_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELC_REVISION_NUMBER);

#if CROSS_COMPILE_MODE
   presenter->print(ELC_CROSS_COMPILE_GREETING);
#endif
}

void CompilingProcess :: cleanUp(ProjectBase& project)
{
   // clean modules
   auto module_it = project.allocModuleIterator();
   while (!module_it->eof()) {
      PathString path;
      _libraryProvider.resolvePath(module_it->name(), path);

      // remove a module
      if (!PathUtil::removeFile(*path) && _verbose) {
         if (PathUtil::ifExist(*path))
            _presenter->printPath("cannot remove file %s\n", *path);
      }

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
      if (!_exeExtension.empty())
         exePath.changeExtension(_exeExtension);

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
   JITCompilerSettings& jitSettings,
   ustr_t profile)
{
   try
   {
      configurateParser(project.getSyntaxVersion());
      configurate(project);

      if (project.Namespace().empty())
         throw InternalError(errMissingNamespace);

      PlatformType targetType = project.TargetType();

      // Project Greetings
      _presenter->printLine(ELC_STARTING, project.ProjectName(), getPlatformName(project.Platform()),
         getTargetTypeName(targetType, project.SystemTarget()));

      if (!profile.empty())
         _presenter->printLine(ELC_PROFILE_INFO, profile);

      if (_compiler->checkStrictTypeFlag())
         _presenter->printLine(ELC_STRICT_MODE);

      // Cleaning up
      _presenter->printLine(ELC_CLEANING);
      cleanUp(project);

      compile(project, jitSettings);

      // generating target when required
      switch (targetType) {
         case PlatformType::Console:
         case PlatformType::GUI:
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
   catch (InternalStrError& ex) {
      _presenter->print(_presenter->getMessage(ex.messageCode), *ex.arg);

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
