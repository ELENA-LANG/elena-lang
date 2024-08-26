//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the compiler common interfaces & types
//
//                                             (C)2021-2024, by Aleksey Rakov
//                                             (C)2024, by ELENA-LANG Org
//---------------------------------------------------------------------------

#ifndef CLICOMMON_H
#define CLICOMMON_H

#include "elena.h"
#include "langcommon.h"
#include "textparser.h"
#include "syntaxtree.h"
#include "projectbase.h"

namespace elena_lang
{

constexpr int MAX_WARNINGS = 200;

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
   MemoryBase* section;
   bool        isAligned;
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
   pos_t           tlsSize;

   addr_t          imageBase;
   pos_t           code;
   pos_t           adata, mdata, mbdata, rdata;
   pos_t           data, stat;
   pos_t           import;
   pos_t           tls;
   pos_t           tlsDirectory;

   pos_t           entryPoint;

   Map<int, pos_t> dictionary;
   RelocationMap   importMapping;

   AddressSpace()
      : dictionary(INVALID_POS), importMapping(0)
   {
      headerSize = codeSize = dataSize = 0;
      import = 0;
      unintDataSize = 0;
      tlsSize = 0;

      importSize = imageSize = 0;
      imageBase = 0;
      code = adata = mdata = mbdata = rdata = 0;
      data = stat = 0;
      tls = 0;
      tlsDirectory = 0xFFFFFFFF;

      entryPoint = 0;
   }
};

class ImageFormatter
{
public:
   virtual void prepareImage(ImageProviderBase& provider, AddressSpace& map, ImageSections& sections,
      pos_t sectionAlignment, pos_t fileAlignment, bool withDebugInfo) = 0;
};

enum class TemplateType
{
   None = 0,
   Inline,
   InlineProperty,
   Class,
   Statement,
   Expression,
   Enumeration,
   ClassBlock
};

enum class Visibility
{
   Private = 1,
   Internal = 2,
   Protected = 3,
   Public = 4,
};

struct BranchingInfo
{
   ref_t typeRef;
   ref_t trueRef;
   ref_t falseRef;

   BranchingInfo()
   {
      typeRef = 0;
      trueRef = falseRef = 0;
   }
};

struct BuiltinReferences
{
   ref_t   superReference, nilValueReference;
   ref_t   intReference, shortReference, uint8Reference;
   ref_t   longReference, realReference;
   ref_t   uintReference, int8Reference, ushortReference;
   ref_t   literalReference;
   ref_t   wideReference;
   ref_t   messageReference, extMessageReference;
   ref_t   messageNameReference;
   ref_t   wrapperTemplateReference;
   ref_t   arrayTemplateReference;
   ref_t   nullableTemplateReference;
   ref_t   argArrayTemplateReference;
   ref_t   closureTemplateReference, tupleTemplateReference;
   ref_t   yielditTemplateReference;
   ref_t   lazyExpressionReference;
   ref_t   pointerReference;

   mssg_t  dispatch_message;
   mssg_t  constructor_message;
   mssg_t  protected_constructor_message;
   mssg_t  invoke_message;
   mssg_t  init_message;
   mssg_t  add_message, sub_message, mul_message, div_message;
   mssg_t  band_message, bor_message, bxor_message;
   mssg_t  and_message, or_message, xor_message;
   mssg_t  refer_message, set_refer_message;
   mssg_t  if_message, iif_message;
   mssg_t  equal_message;
   mssg_t  not_message, negate_message, value_message;
   mssg_t  default_message;
   mssg_t  notequal_message;
   mssg_t  less_message, greater_message;
   mssg_t  notless_message, notgreater_message;
   mssg_t  shl_message, shr_message, bnot_message;

   BuiltinReferences()
   {
      superReference = intReference = 0;
      nilValueReference = 0;
      shortReference = uint8Reference = 0;
      uintReference = 0;
      int8Reference = ushortReference = 0;
      longReference = realReference = 0;
      literalReference = wideReference = 0;
      messageReference = extMessageReference = 0;
      messageNameReference = 0;
      wrapperTemplateReference = 0;
      arrayTemplateReference = argArrayTemplateReference = 0;
      nullableTemplateReference = 0;
      closureTemplateReference = lazyExpressionReference = tupleTemplateReference = 0;
      yielditTemplateReference = 0;
      pointerReference = 0;

      dispatch_message = constructor_message = 0;
      protected_constructor_message = 0;
      invoke_message = init_message = 0;
      add_message = sub_message = mul_message = div_message = 0;
      band_message = bor_message = bxor_message = 0;
      and_message = or_message = xor_message = 0;
      refer_message = set_refer_message = 0;
      if_message = iif_message = 0;
      equal_message = 0;
      not_message = negate_message = value_message = 0;
      default_message = 0;
      notequal_message = 0;
      greater_message = less_message = 0;
      notgreater_message = notless_message = 0;
      bnot_message = shl_message = shr_message = 0;
   }
};

// --- ManifestInfo ---
struct ManifestInfo
{
   ustr_t maninfestName;
   ustr_t maninfestVersion;
   ustr_t maninfestAuthor;
};

// --- SizeInfo ---
struct SizeInfo
{
   int  size;
   bool readOnly;
};

// --- ExternalType ---
enum class ExternalType
{
   Standard = 0,
   WinApi
};

// --- ExternalInfo ---
struct ExternalInfo
{
   ExternalType type;
   ref_t        reference;
};

// --- ModuleScopeBase ---
class ModuleScopeBase : public SectionScopeBase
{
public:
   ReferenceMap         predefined;
   ReferenceMap         attributes;
   ReferenceMap         aliases;
   ReferenceMap         operations;
   BuiltinReferences    buildins;
   BranchingInfo        branchingInfo;

   IdentifierString     selfVar;
   IdentifierString     declVar;
   IdentifierString     superVar;
   IdentifierString     receivedVar;

   pos_t                stackAlingment, rawStackAlingment;
   pos_t                ehTableEntrySize;
   int                  minimalArgList;
   int                  ptrSize;

   bool                 tapeOptMode;

   Map<ref_t, SizeInfo> cachedSizes;
   Map<ref_t, ref_t>    cachedClassReferences;
   Map<ref_t, bool>     cachedEmbeddableReadonlys;
   Map<ref_t, bool>     cachedEmbeddables;
   Map<ref_t, bool>     cachedEmbeddableStructs;
   Map<ref_t, bool>     cachedEmbeddableArrays;
   Map<ref_t, bool>     cachedStacksafeArgs;
   Map<ref_t, bool>     cachedWrappers; 

   virtual bool isStandardOne() = 0;
   virtual bool withValidation() = 0;

   virtual bool isDeclared(ref_t reference) = 0;
   virtual bool isSymbolDeclared(ref_t reference) = 0;

   virtual bool isInternalOp(ref_t reference)
   {
      ustr_t referenceName = resolveFullName(reference);
      if (isWeakReference(referenceName)) {
         return true;
      }
      else {
         auto refInfo = getModule(referenceName, true);

         return refInfo.module == module;
      }
   }

   virtual ref_t mapAnonymous(ustr_t prefix = nullptr) = 0;

   virtual ref_t mapFullReference(ustr_t referenceName, bool existing = false) = 0;
   virtual ref_t mapWeakReference(ustr_t referenceName, bool existing = false) = 0;

   virtual ref_t mapNewIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) = 0;

   virtual ref_t mapTemplateIdentifier(ustr_t identifier, Visibility visibility, 
      bool& alreadyDeclared, bool declarationMode) = 0;

   virtual ref_t resolveImplicitIdentifier(ustr_t ns, ustr_t identifier, Visibility visibility) = 0;
   virtual ref_t resolveImportedIdentifier(ustr_t identifier, IdentifierList* importedNs) = 0;

   virtual ref_t resolveWeakTemplateReferenceID(ref_t reference) = 0;

   virtual SectionInfo getSection(ustr_t referenceName, ref_t mask, bool silentMode) = 0;

   virtual ModuleInfo getModule(ustr_t referenceName, bool silentMode) = 0;
   virtual ModuleInfo getWeakModule(ustr_t referenceName, bool silentMode) = 0;

   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false, bool fieldsOnly = false) = 0;
   virtual ref_t loadClassInfo(ClassInfo& info, ustr_t referenceName, bool headerOnly = false, bool fieldsOnly = false) = 0;

   virtual ref_t loadSymbolInfo(SymbolInfo& info, ref_t reference) = 0;
   virtual ref_t loadSymbolInfo(SymbolInfo& info, ustr_t referenceName) = 0;

   virtual void newNamespace(ustr_t name) = 0;
   virtual bool includeNamespace(IdentifierList& importedNs, ustr_t name, bool& duplicateInclusion) = 0;

   virtual ExternalInfo mapExternal(ustr_t dllAlias, ustr_t functionName) = 0;

   virtual Visibility retrieveVisibility(ref_t reference) = 0;

   ModuleScopeBase(ModuleBase* module,
      ModuleBase* debugModule,
      pos_t stackAlingment, 
      pos_t rawStackAlingment,
      pos_t ehTableEntrySize,
      int minimalArgList,
      int ptrSize,
      bool tapeOptMode
   ) :
      predefined(0),
      attributes(0),
      aliases(0),
      operations(0),
      cachedSizes({}),
      cachedClassReferences(0),
      cachedEmbeddableReadonlys(false),
      cachedEmbeddables(false), 
      cachedEmbeddableStructs(false),
      cachedEmbeddableArrays(false),
      cachedStacksafeArgs(false),
      cachedWrappers(false)
   {
      this->module = module;
      this->debugModule = debugModule;
      this->stackAlingment = stackAlingment;
      this->rawStackAlingment = rawStackAlingment;
      this->ehTableEntrySize = ehTableEntrySize;
      this->minimalArgList = minimalArgList;
      this->ptrSize = ptrSize;
      this->tapeOptMode = tapeOptMode;
   }
};

// --- ExpressionAttributes ---

enum class ExpressionAttribute : pos64_t
{
   None                 = 0x00000000000,
   Meta                 = 0x00000000001,
   NestedNs             = 0x00000000002,
   Forward              = 0x00000000004,
   Weak                 = 0x00000000008,
   NoTypeAllowed        = 0x00000000010,
   Intern               = 0x00000000020,
   Parameter            = 0x00000000040,
   NewVariable          = 0x00000000080,
   Local                = 0x00000000100,
   NewOp                = 0x00000000200,
   StrongResolved       = 0x00000000400,
   RootSymbol           = 0x00000000800,
   Root                 = 0x00000001000,
   CastOp               = 0x00000002000,
   IgnoreDuplicate      = 0x00000004000,
   RefOp                = 0x00000008000,
   NoPrimitives         = 0x00000010000,
   MssgLiteral          = 0x00000020000,
   MssgNameLiteral      = 0x00000040000,
   Extern               = 0x00000080000,
   Member               = 0x00000100000,
   ProbeMode            = 0x00000200000,
   AllowPrivateCall     = 0x00000400000,
   InitializerScope     = 0x00000800000,
   NestedDecl           = 0x00001000000,
   ConstantExpr         = 0x00002000000,
   Variadic             = 0x00004000000,
   WithVariadicArg      = 0x00008000000,
   RetrievingType       = 0x00010000000,
   RetValExpected       = 0x00020000000,
   CheckShortCircle     = 0x00040000000,
   LookaheadExprMode    = 0x00080000000,   
   Class                = 0x00100000000,
   Nillable             = 0x00200000000,
   AllowGenericSignature= 0x00400000000,
   OutRefOp             = 0x01000000000,
   WithVariadicArgCast  = 0x02008000000,
   DistributedForward   = 0x04000000000,
   DynamicObject        = 0x08000000000,
   Superior             = 0x10000000000,
   Lookahead            = 0x20000000000,
   NoDebugInfo          = 0x40000000000,
   NoExtension          = 0x80000000000,
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

   bool test(ExpressionAttribute mask)
   {
      return test(attrs, mask);
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

// --- FieldAttributes ---
struct FieldAttributes
{
   TypeInfo typeInfo;
   int      size;
   bool     isConstant;
   bool     isStatic;
   bool     isEmbeddable;
   bool     isReadonly;
   bool     inlineArray;
   bool     fieldArray;
   bool     overrideMode;
   bool     autogenerated;
   bool     privateOne;
};

// --- CompilerBase ---
typedef Map<ustr_t, ref_t, allocUStr, freeUStr> ForwardMap;

class CompilerBase
{
public:
   virtual void generateOverloadListMember(ModuleScopeBase& scope, ref_t listRef, ref_t classRef, 
      mssg_t messageRef, MethodHint targetType) = 0;

   virtual ref_t resolvePrimitiveType(ModuleScopeBase& moduleScope, 
      TypeInfo typeInfo, bool declarationMode = false) = 0;

   virtual ref_t generateExtensionTemplate(ModuleScopeBase& scope, ref_t templateRef, size_t argumentLen,
      ref_t* arguments, ustr_t ns, ExtensionMap* outerExtensionList) = 0;

   virtual ~CompilerBase() = default;

};

// --- TemplateProssesorBase ---

class TemplateProssesorBase
{
public:
   virtual ref_t generateClassTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
      List<SyntaxNode>& parameters, bool declarationMode, ExtensionMap* outerExtensionList) = 0;

   virtual bool importTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
      List<SyntaxNode>& parameters) = 0;
   virtual bool importInlineTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
      List<SyntaxNode>& parameters) = 0;
   virtual bool importPropertyTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
      List<SyntaxNode>& parameters) = 0;
   virtual bool importCodeTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
      List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) = 0;
   virtual bool importExpressionTemplate(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target,
      List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) = 0;
   virtual bool importEnumTemplate(ModuleScopeBase& moduleScope, ref_t templateRef,
      SyntaxNode target, List<SyntaxNode>& arguments, List<SyntaxNode>& parameters) = 0;
   virtual bool importTextblock(ModuleScopeBase& moduleScope, ref_t templateRef, SyntaxNode target) = 0;
};

// --- SyntaxWriterBase ---
class SyntaxWriterBase
{
public:
   virtual void newNode(parse_key_t key) = 0;
   virtual void newNode(parse_key_t key, ustr_t arg) = 0;
   virtual void injectNode(parse_key_t key) = 0;
   virtual void renameNode(parse_key_t key) = 0;

   virtual void appendTerminal(parse_key_t key, ustr_t value, LineInfo lineInfo) = 0;

   virtual void closeNode() = 0;

   virtual void saveTree(SyntaxTree& tree) = 0;
};

// --- ErrorProcessor ---

class CLIException : public ExceptionBase {};

enum class WarningLevel
{
   None,
   Level0,
   Level1,
   Level2,
   Level3
};

class ErrorProcessor : public ErrorProcessorBase
{
   PresenterBase* _presenter;
   int            _warningMasks;
   int            _numberOfWarnings;

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
      if (terminal != SyntaxKey::None) {
         SyntaxNode col = terminal.findChild(SyntaxKey::Column);
         SyntaxNode row = terminal.findChild(SyntaxKey::Row);

         _presenter->print(_presenter->getMessage(code), pathArg, col.arg.value, row.arg.value, terminal.identifier());
      }
      else {
         _presenter->print(_presenter->getMessage(code), pathArg, 0, 0, "<unknown>");
      }
   }

public:
   void info(int code, ustr_t arg) override
   {
      _presenter->print(_presenter->getMessage(code), arg);
   }

   void info(int code, ustr_t arg, ustr_t arg2) override
   {
      _presenter->print(_presenter->getMessage(code), arg, arg2);
   }

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

   virtual void raiseTerminalError(int code, ustr_t pathArg, SyntaxNode node)
   {
      printTerminalInfo(code, pathArg, node);

      throw AbortError();
   }

   void raiseTerminalWarning(int level, int code, ustr_t pathArg, SyntaxNode node)  
   {
      if (!test(_warningMasks, level))
         return;

      if (_numberOfWarnings < MAX_WARNINGS) {
         _numberOfWarnings++;

         printTerminalInfo(code, pathArg, node);
      }
   }

   bool hasWarnings() { return _numberOfWarnings > 0; }

   void setWarningLevel(WarningLevel level)
   {
      switch (level) {
         case WarningLevel::Level0:
            _warningMasks = WARNING_MASK_0;
            break;
         case WarningLevel::Level1:
            _warningMasks = WARNING_MASK_1;
            break;
         case WarningLevel::Level2:
            _warningMasks = WARNING_MASK_2;
            break;
         case WarningLevel::Level3:
            _warningMasks = WARNING_MASK_3;
            break;
         default:
            break;
      }
   }

   WarningLevel getWarningLevel()
   {
      switch (_warningMasks) {
         case WARNING_MASK_1:
            return WarningLevel::Level1;
         case WARNING_MASK_2:
            return WarningLevel::Level2;
         case WARNING_MASK_3:
            return WarningLevel::Level3;
         case WARNING_MASK_0:
         default:
            return WarningLevel::Level0;
      }
   }

   ErrorProcessor(PresenterBase* presenter)
   {
      _presenter = presenter;
      _warningMasks = WARNING_MASK_2;
      _numberOfWarnings = 0;
   }
};

// --- LinkResult ---

struct LinkResult
{
   addr_t code;
   addr_t rdata;
};

// --- LinkerBaser ---
class LinkerBase
{
protected:
   ErrorProcessorBase* _errorProcessor;

public:
   virtual LinkResult run(ProjectBase& project, ImageProviderBase& provider, 
      PlatformType uiType, path_t exeExtension) = 0;

   LinkerBase(ErrorProcessorBase* errorProcessor)
   {
      _errorProcessor = errorProcessor;
   }
};

// --- ConversionResult ---
enum class ConversionResult
{
   None = 0,
   BoxingRequired,
   VariadicBoxingRequired,
   Conversion,
   NativeConversion,
   DynamicConversion
};

// --- ConversionRoutine ---
struct ConversionRoutine
{
   ConversionResult result;
   union {
      mssg_t conversionMssg;
      ref_t  operationKey;
   };
   
   int              stackSafeAttrs;
};

// --- SysLibraryLoaderBase ---
class SysLibraryLoaderBase
{
public:
   virtual void* loadFunction(const char* name) = 0;

   virtual ~SysLibraryLoaderBase() = default;
};

// --- VirtualMethodList ---
enum class VirtualType : int
{
   None = 0,
   Multimethod,
   EmbeddableWrapper,
   AbstractEmbeddableWrapper
};

struct VirtualMethod
{
   mssg_t      message;
   VirtualType type;
   int         nillableArgs;

   VirtualMethod()
   {
      message = 0;
      type = VirtualType::None;
      nillableArgs = 0;
   }
   VirtualMethod(mssg_t message, VirtualType type, int nillableArgs)
   {
      this->message = message;
      this->type = type;
      this->nillableArgs = nillableArgs;
   }
};

typedef List<VirtualMethod>   VirtualMethodList;

}

#endif // CLICOMMON_H
