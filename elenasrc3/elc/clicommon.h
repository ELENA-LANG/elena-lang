//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler common interfaces & types
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLICOMMON_H
#define CLICOMMON_H

#include "elena.h"
#include "langcommon.h"
#include "textparser.h"
#include "syntaxtree.h"
#include "errors.h"

namespace elena_lang
{

// --- PresenterBase ----

class PresenterBase
{
public:
   virtual ustr_t getMessage(int code) = 0;

   virtual void print(ustr_t msg) = 0;
   virtual void print(ustr_t msg, ustr_t arg) = 0;
   virtual void print(ustr_t msg, ustr_t arg1, ustr_t arg2) = 0;
   virtual void print(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3) = 0;
   virtual void print(ustr_t msg, int arg1, int arg2) = 0;
   virtual void print(ustr_t msg, int arg1, int arg2, int arg3) = 0;
   virtual void print(ustr_t msg, ustr_t arg1, int arg2, int arg3, ustr_t arg4) = 0;
   virtual void printPath(ustr_t msg, path_t arg) = 0;
   virtual void printPath(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4) = 0;

   virtual ~PresenterBase() = default;
};

// --- ImageFormatter ---

struct ImageSectionHeader
{
   enum class SectionType
   {
      None = 0,
      Text,
      RData,
      Data,
      UninitializedData,
   };

   ustr_t      name;
   pos_t       memorySize;
   pos_t       fileSize;
   pos_t       vaddress;
   SectionType type;

   ImageSectionHeader()
   {
      this->vaddress = this->memorySize = this->fileSize = 0;
      this->type = SectionType::None;
   }
   ImageSectionHeader(const ImageSectionHeader& copy)
   {
      this->name = copy.name;
      this->memorySize = copy.memorySize;
      this->fileSize = copy.fileSize;
      this->vaddress = copy.vaddress;
      this->type = copy.type;
   }
   ImageSectionHeader(ustr_t name, pos_t vaddress, SectionType type, pos_t memorySize, pos_t fileSize)
   {
      this->name = name;
      this->memorySize = memorySize;
      this->fileSize = fileSize;
      this->vaddress = vaddress;
      this->type = type;
   }

   static ImageSectionHeader get(ustr_t name, pos_t vaddress, SectionType type, pos_t memorySize, pos_t fileSize)
   {
      ImageSectionHeader info(name, vaddress, type, memorySize, fileSize);

      return info;
   }
   static ImageSectionHeader get()
   {
      ImageSectionHeader info;

      return info;
   }
};

struct ImageItem
{
   Section* section;
   bool     isAligned;
};

struct ImageSections
{
   List<ImageSectionHeader> headers;
   Map<int, ImageItem>      items;

   ImageSections()
      : headers(ImageSectionHeader()), items({ nullptr, false })
   {
   }
};

struct AddressSpace
{
   pos_t           headerSize;
   pos_t           codeSize, dataSize, unintDataSize, importSize;
   pos_t           imageSize;

   addr_t          imageBase;
   pos_t           code;
   pos_t           mdata, mbdata, rdata;
   pos_t           data;
   pos_t           import;

   pos_t           entryPoint;

   Map<int, pos_t> dictionary;
   RelocationMap   importMapping;

   AddressSpace()
      : dictionary(INVALID_POS), importMapping(0)
   {
      headerSize = codeSize = dataSize = 0;
      import = 0;
      unintDataSize = 0;

      importSize = imageSize = 0;
      imageBase = 0;
      code = data = 0;
      mdata = mbdata = rdata = 0;

      entryPoint = 0;
   }
};

class ImageFormatter
{
public:
   virtual void prepareImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
      pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo) = 0;
};

// --- ProjectBase ---

enum class ProjectOption
{
   None = 0,

   // collections
   Root,
   Files,
   Templates,
   Primitives,
   Forwards,
   Externals,
   References,

   Namespace,

   Module,
   FileKey,
   Forward,
   External,

   TargetPath,
   BasePath,
   OutputPath,
   ProjectPath,
   LibPath,

   ClassSymbolAutoLoad,
   StackAlignment,
   RawStackAlignment,
   GCMGSize,
   GCYGSize,
   DebugMode,

   Key,
   Value,
};

class FileIteratorBase
{
protected:
   virtual void next() = 0;
   virtual path_t path() = 0;

public:
   virtual bool eof() = 0;

   virtual bool loadKey(IdentifierString& retVal) = 0;

   path_t operator*()
   {
      return path();
   }

   FileIteratorBase& operator ++()
   {
      next();

      return *this;
   }
   virtual ~FileIteratorBase() = default;
};

class ModuleIteratorBase
{
protected:
   virtual void next() = 0;

public:
   virtual ustr_t name() = 0;

   virtual bool eof() = 0;

   virtual FileIteratorBase& files() = 0;

   ModuleIteratorBase& operator ++()
   {
      next();

      return *this;
   }
   virtual ~ModuleIteratorBase() = default;
};

class ProjectBase : public ForwardResolverBase
{
public:
   virtual ModuleIteratorBase* allocModuleIterator() = 0;

   virtual FileIteratorBase* allocPrimitiveIterator() = 0;
   virtual FileIteratorBase* allocPackageIterator() = 0;

   virtual PlatformType TargetType() = 0;
   virtual PlatformType Platform() = 0;
   virtual ustr_t ProjectName() = 0;
   virtual ustr_t Namespace() = 0;

   virtual path_t PathSetting(ProjectOption option) const = 0;
   virtual ustr_t StringSetting(ProjectOption option) const = 0;
   virtual bool BoolSetting(ProjectOption option, bool defValue = false) const = 0;
   virtual int IntSetting(ProjectOption option, int defValue = 0) const = 0;

   virtual void prepare() = 0;

   virtual ~ProjectBase() = default;
};

enum class TemplateType
{
   None = 0,
   Inline
};

enum class Visibility
{
   Private,
   Internal,
   Public
};

struct BuiltinReferences
{
   ref_t   superReference;

   mssg_t  dispatch_message;
   mssg_t  constructor_message;

   BuiltinReferences()
   {
      superReference = 0;

      dispatch_message = constructor_message = 0;
   }
};

// --- ModuleScopeBase ---
class ModuleScopeBase : public SectionScopeBase
{
public:
   ReferenceMap       predefined;
   ReferenceMap       attributes;
   BuiltinReferences  buildins;

   IdentifierString   selfVar;

   pos_t              stackAlingment, rawStackAlingment;

   virtual bool isStandardOne() = 0;

   virtual ref_t mapFullReference(ustr_t referenceName, bool existing = false) = 0;
   virtual ref_t mapWeakReference(ustr_t referenceName, bool existing = false) = 0;

   virtual ref_t mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) = 0;

   virtual ref_t resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) = 0;

   virtual SectionInfo getSection(ustr_t referenceName, ref_t mask, bool silentMode) = 0;

   virtual ModuleInfo getModule(ustr_t referenceName, bool silentMode) = 0;
   virtual ModuleInfo getWeakModule(ustr_t referenceName, bool silentMode) = 0;

   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;
   virtual ref_t loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly = false) = 0;

   virtual void importClassInfo(ClassInfo& copy, ClassInfo& target, ModuleBase* exporter, bool headerOnly, bool inheritMode/*,
      bool ignoreFields*/) = 0;

   ModuleScopeBase(ModuleBase* module,
      pos_t stackAlingment, 
      pos_t rawStackAlingment)
      : predefined(0), attributes(0)
   {
      this->module = module;
      this->stackAlingment = stackAlingment;
      this->rawStackAlingment = rawStackAlingment;
   }
};

// --- ExpressionAttributes ---

enum class ExpressionAttribute : pos64_t
{
   None              = 0x00000000,
   Meta              = 0x00000001,
   NestedNs          = 0x00000002,
   Forward           = 0x00000004,
   Weak              = 0x00000008,
   NoTypeAllowed     = 0x00000010,
   Intern            = 0x00000020,
   Parameter         = 0x00000040,
};

struct ExpressionAttributes
{
   ExpressionAttribute attrs;

   static bool testAndExclude(ExpressionAttribute& attrs, ExpressionAttribute mask)
   {
      if (test(attrs, mask)) {
         attrs = exclude(attrs, mask);

         return true;
      }
      else return false;
   }

   static bool test(ExpressionAttribute attrs, ExpressionAttribute mask)
   {
      return ((pos64_t)attrs & (pos64_t)mask) == (pos64_t)mask;
   }

   static ExpressionAttribute exclude(ExpressionAttribute attrs, ExpressionAttribute mask)
   {
      return (ExpressionAttribute)((pos64_t)attrs & ~(pos64_t)mask);
   }

   ExpressionAttributes& operator |=(ExpressionAttribute value)
   {
      attrs = static_cast<ExpressionAttribute>((pos64_t)attrs | (pos64_t)value);

      return *this;
   }

   ExpressionAttributes()
   {
      attrs = ExpressionAttribute::None;
   }

   ExpressionAttributes(ExpressionAttribute attrs)
   {
      this->attrs = attrs;
   }
};

// --- CompilerBase ---
typedef Map<ustr_t, ref_t, allocUStr, freeUStr> ForwardMap;

class CompilerBase
{
public:   
};

// --- TemplateProssesorBase ---

class TemplateProssesorBase
{
public:
   virtual bool importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
      List<SyntaxNode>& parameters) = 0;
};

// --- SyntaxWriterBase ---
class SyntaxWriterBase
{
public:
   virtual void newNode(parse_key_t key) = 0;
   virtual void newNode(parse_key_t key, ustr_t arg) = 0;
   virtual void injectNode(parse_key_t key) = 0;

   virtual void appendTerminal(parse_key_t key, ustr_t value, LineInfo lineInfo) = 0;

   virtual void closeNode() = 0;
};

// --- ErrorProcessor ---

class CLIException : public ExceptionBase {};

class ErrorProcessor : public ErrorProcessorBase
{
   PresenterBase* _presenter;
   int            _warningMasks;

   static SyntaxNode findTerminal(SyntaxNode node)
   {
      if (node.existChild(SyntaxKey::Row))
         return node;

      SyntaxNode current = node.firstChild();
      while (current != SyntaxKey::None) {
         SyntaxNode terminalNode = findTerminal(current);
         if (terminalNode != SyntaxKey::None)
            return terminalNode;

         current = current.nextNode();
      }

      return current;
   }

   void printTerminalInfo(int code, ustr_t pathArg, SyntaxNode node)
   {
      SyntaxNode terminal = findTerminal(node);
      SyntaxNode col = terminal.findChild(SyntaxKey::Column);
      SyntaxNode row = terminal.findChild(SyntaxKey::Row);

      _presenter->print(_presenter->getMessage(code), pathArg, col.arg.value, row.arg.value, terminal.identifier());
   }

public:
   void raiseError(int code, ustr_t arg) override
   {
      _presenter->print(_presenter->getMessage(code), arg);

      throw CLIException();
   }

   void raisePathError(int code, path_t arg) override
   {
      _presenter->printPath(_presenter->getMessage(code), arg);

      throw CLIException();
   }

   void raisePathWarning(int code, path_t arg) override
   {
      _presenter->printPath(_presenter->getMessage(code), arg);
   }

   void raiseInternalError(int code) override
   {
      _presenter->print(_presenter->getMessage(code));

      throw CLIException();
   }

   void raiseTerminalError(int code, ustr_t pathArg, SyntaxNode node)
   {
      printTerminalInfo(code, pathArg, node);

      throw AbortError();
   }

   void raiseTerminalWarning(int level, int code, ustr_t pathArg, SyntaxNode node)  
   {
      if (!test(_warningMasks, level))
         return;

      printTerminalInfo(code, pathArg, node);
   }

   ErrorProcessor(PresenterBase* presenter)
   {
      _presenter = presenter;
      _warningMasks = WARNING_MASK_2;
   }
};

// --- LinkerBaser ---

class LinkerBase
{
protected:
   ErrorProcessorBase* _errorProcessor;

public:
   virtual void run(ProjectBase& project, ImageProviderBase& provider) = 0;

   LinkerBase(ErrorProcessorBase* errorProcessor)
   {
      _errorProcessor = errorProcessor;
   }
};

}

#endif // CLICOMMON_H
