//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler common interfaces.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerCommonH
#define compilerCommonH

#include "syntaxtree.h"
#include "bytecode.h"

namespace _ELENA_
{

constexpr auto V_CATEGORY_MASK   = 0x7FFFFF00u;
constexpr auto V_CATEGORY_MAX    = 0x0000F000u;

// attributes / prmitive types

/// modificator
constexpr auto V_IGNOREDUPLICATE = 0x80006001u;
constexpr auto V_SCRIPTSELFMODE  = 0x80006002u;
//constexpr auto V_AUTOSIZE        = 0x80006003u;
constexpr auto V_NODEBUGINFO     = 0x80006004u;

/// accessors:
constexpr auto V_GETACCESSOR     = 0x80005001u;
constexpr auto V_SETACCESSOR     = 0x80005002u;

/// visibility:
constexpr auto V_PUBLIC          = 0x80004001u;
constexpr auto V_PRIVATE         = 0x80004002u;
constexpr auto V_INTERNAL        = 0x80004003u;
constexpr auto V_PROTECTED       = 0x80004004u;
//constexpr auto V_INLINE          = 0x80004006u;
constexpr auto V_PROPERTY        = 0x80004007u;

/// property:
constexpr auto V_SEALED          = 0x80003001u;
constexpr auto V_ABSTRACT        = 0x80003002u;
constexpr auto V_CLOSED          = 0x80003003u;
constexpr auto V_PREDEFINED      = 0x80003005u;
constexpr auto V_YIELDABLE       = 0x80003006u;

/// scope_prefix:
constexpr auto V_CONST           = 0x80002001u;
constexpr auto V_EMBEDDABLE      = 0x80002002u;
constexpr auto V_WRAPPER         = 0x80002003u;
constexpr auto V_WEAKOP          = 0x80002004u;
constexpr auto V_LOOP            = 0x80002005u;
constexpr auto V_PRELOADED       = 0x80002006u;
constexpr auto V_LAZY            = 0x80002009u;
constexpr auto V_MULTIRETVAL     = 0x8000200Au;

/// scope:
constexpr auto V_CLASS           = 0x80001001u;
constexpr auto V_STRUCT          = 0x80001002u;
constexpr auto V_SYMBOLEXPR      = 0x80001003u;
constexpr auto V_CONSTRUCTOR     = 0x80001004u;
constexpr auto V_EXTENSION       = 0x80001005u;
constexpr auto V_SINGLETON       = 0x80001006u;
constexpr auto V_LIMITED         = 0x80001007u;
constexpr auto V_METHOD          = 0x80001008u;
constexpr auto V_FIELD           = 0x80001009u;
constexpr auto V_TYPETEMPL       = 0x8000100Au;
constexpr auto V_GENERIC         = 0x8000100Bu;
constexpr auto V_FUNCTION        = 0x8000100Cu;     // a closure attribute
constexpr auto V_VARIABLE        = 0x8000100Du;
constexpr auto V_MEMBER          = 0x8000100Eu;
constexpr auto V_STATIC          = 0x8000100Fu;
constexpr auto V_CONVERSION      = 0x80001011u;
constexpr auto V_NEWOP           = 0x80001012u;
constexpr auto V_DISPATCHER      = 0x80001013u;
constexpr auto V_ARGARRAY        = 0x80001014u;
constexpr auto V_EXTERN          = 0x80001015u;
constexpr auto V_INTERN          = 0x80001016u;
constexpr auto V_FORWARD         = 0x80001017u;
constexpr auto V_IMPORT          = 0x80001018u;
constexpr auto V_MIXIN           = 0x80001019u;
constexpr auto V_NOSTRUCT        = 0x8000101Bu;
constexpr auto V_AUTO            = 0x8000101Cu;
constexpr auto V_INITIALIZER     = 0x8000101Du;
constexpr auto V_TEMPLATE        = 0x8000101Eu;
//constexpr auto V_ATTRIBUTE       = 0x8000101Fu;
constexpr auto V_YIELD           = 0x80001020u;
constexpr auto V_NAMESPACE       = 0x80001021u;
constexpr auto V_META            = 0x80001022u;
constexpr auto V_PREVIOUS        = 0x80001023u;
constexpr auto V_TEMPLATEBASED   = 0x80001024u;

/// primitive type attributes
constexpr auto V_STRING          = 0x80000801u;
constexpr auto V_FLOAT           = 0x80000802u;
constexpr auto V_INTBINARY       = 0x80000803u;
constexpr auto V_BINARY          = 0x80000804u;
constexpr auto V_PTRBINARY       = 0x80000805u;
constexpr auto V_MESSAGE         = 0x80000806u;
constexpr auto V_SUBJECT         = 0x80000807u;
constexpr auto V_SYMBOL          = 0x80000808u;
constexpr auto V_INLINEARG       = 0x80000809u;
constexpr auto V_TYPEOF          = 0x8000080Au;

/// primitive types
constexpr auto V_FLAG            = 0x80000001u;
constexpr auto V_NIL             = 0x80000002u;
constexpr auto V_INT32           = 0x80000003u;
constexpr auto V_INT64           = 0x80000004u;
constexpr auto V_DWORD           = 0x80000005u;
constexpr auto V_REAL64          = 0x80000006u;
constexpr auto V_EXTMESSAGE      = 0x80000007u;
constexpr auto V_PTR32           = 0x80000008u;
constexpr auto V_OBJARRAY        = 0x80000009u;
constexpr auto V_INT32ARRAY      = 0x8000000Au;
constexpr auto V_BINARYARRAY     = 0x8000000Bu;
constexpr auto V_INT16ARRAY      = 0x8000000Cu;
constexpr auto V_INT8ARRAY       = 0x8000000Du;
constexpr auto V_OBJECT          = 0x8000000Eu;
constexpr auto V_UNBOXEDARGS     = 0x8000000Fu;
constexpr auto V_PTR64           = 0x80000010u;
constexpr auto V_QWORD           = 0x80000011u;

enum class Visibility
{
   Private,
   Internal,
   Public
};

typedef Map<ident_t, ref_t>      ForwardMap;

enum MethodHint
{
   tpMask         = 0x00000F,

   tpUnknown      = 0x0000000,
   tpSealed       = 0x0000001,
   tpClosed       = 0x0000002,
   tpNormal       = 0x0000003,
   tpDispatcher   = 0x0000004,
   tpPrivate      = 0x0000005,

   tpStackSafe    = 0x0000010,
   tpEmbeddable   = 0x0000020,
   tpGeneric      = 0x0000040,
   tpFunction     = 0x0000080,
   tpTargetSelf   = 0x0000100, // used for script generated classes (self refers to __target)
   tpConstructor  = 0x0200400,
   tpConversion   = 0x0200800,
   tpMultimethod  = 0x0001000,
   tpStatic       = 0x0004000,
   tpGetAccessor  = 0x0008000,
   tpMixin        = 0x0010000,
   tpAbstract     = 0x0020000,
   tpInternal     = 0x0040000,
   tpPredefined   = 0x0080000, // virtual class declaration
//   tpDynamic     = 0x0100000, // indicates that the method does not accept stack allocated parameters
   tpInitializer  = 0x0200000,
   tpSetAccessor  = 0x0400000,
   tpCast         = 0x0800000,
   tpYieldable    = 0x1000000,
   tpConstant     = 0x2000000,
   tpProtected    = 0x4000000,
   tpMultiRetVal  = 0x8000000,
};

enum ConversionResult
{
   crUncompatible = 0,
   crCompatible,
   crConverted,
   crBoxingRequired
};

struct ConversionInfo
{
   ConversionResult result;
   mssg_t           message;
   ref_t            classRef;
   int              stackSafeAttr;
   bool             embeddable;

   ConversionInfo()
   {
      result = ConversionResult::crUncompatible;
      message = 0;
      classRef = 0;
      stackSafeAttr = 0;
      embeddable = false;
   }
   ConversionInfo(ConversionResult result)
   {
      this->result = result;
      message = 0;
      classRef = 0;
      stackSafeAttr = 0;
      embeddable = false;
   }
   ConversionInfo(ConversionResult result, mssg_t message, ref_t classRef, int stackSafeAttr, bool embeddable)
   {
      this->result = result;
      this->message = message;
      this->classRef = classRef;
      this->stackSafeAttr = stackSafeAttr;
      this->embeddable = embeddable;
   }
};

// --- _Project ---

class _ProjectManager
{
public:
   virtual ident_t getManinfestName() = 0;
   virtual ident_t getManinfestVersion() = 0;
   virtual ident_t getManinfestAuthor() = 0;

   virtual void printInfo(const char* msg, ident_t value) = 0;

   virtual void raiseError(ident_t msg) = 0;
   virtual void raiseError(ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;
   virtual void raiseError(ident_t msg, ident_t value) = 0;

   virtual void raiseWarning(int level, ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;

   virtual _Module* createModule(ident_t name) = 0;
   virtual _Module* createDebugModule(ident_t name) = 0;

   virtual _Module* loadModule(ident_t package, bool silentMode) = 0;

   virtual _Module* resolveModule(ident_t referenceName, ref_t& reference, bool silentMode = false) = 0;
   virtual _Module* resolveWeakModule(ident_t weakReferenceName, ref_t& reference, bool silentMode = false) = 0;

   virtual ident_t resolveForward(ident_t forward) = 0;

   virtual bool addForward(ident_t forward, ident_t reference) = 0;

   virtual ident_t resolveExternalAlias(ident_t alias, bool& stdCall) = 0;
};

// --- _ModuleScope ---

struct _ModuleScope
{
   struct BranchingInfo
   {
      ref_t reference;
      ref_t trueRef;
      ref_t falseRef;

      BranchingInfo()
      {
         reference = 0;
         trueRef = falseRef = 0;
      }
   };

   _ProjectManager*  project;

   _Module*          module;
   _Module*          debugModule;

   // cached references
   ref_t             superReference;
   ref_t             intReference;
   ref_t             longReference;
   ref_t             realReference;
   ref_t             messageNameReference;
   ref_t             messageReference;
   ref_t             extMessageReference;
   ref_t             literalReference;
   ref_t             wideReference;
   ref_t             charReference;
   ref_t             refTemplateReference;
   ref_t             arrayTemplateReference;
   ref_t             argArrayTemplateReference;
   ref_t             closureTemplateReference;
   ref_t             lazyExprReference;
//   ref_t             wrapReference;

   // cached messages
   ref_t             dispatch_message;
   ref_t             init_message;
   ref_t             constructor_message;
   ref_t             protected_constructor_message;

   // cached bool values
   BranchingInfo     branchingInfo;

   // cached paths
   SymbolMap         savedPaths;

   // cached requests
   Map<ref_t, Pair<int, bool>> cachedSizes;

   // compiler options
   int               stackAlignment;

   AttributeMap      attributes;

   virtual ref_t mapAnonymous(ident_t prefix = nullptr) = 0;

   virtual void saveAttribute(ident_t typeName, ref_t classReference) = 0;

   virtual ref_t __FASTCALL loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;
   virtual ref_t __FASTCALL loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false) = 0;
   virtual ref_t __FASTCALL loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbolName) = 0;

   virtual _Module* loadReferenceModule(ident_t referenceName, ref_t& reference) = 0;
   virtual _Module* loadReferenceModule(ref_t reference, ref_t& moduleReference) = 0;

   bool __FASTCALL isDeclared(ref_t reference)
   {
      if (!reference) {
         return false;
      }
      else return mapSection(reference | mskMetaRDataRef, true) != nullptr;
   }

   virtual _Memory* mapSection(ref_t reference, bool existing) = 0;
   virtual ref_t mapTemplateClass(ident_t ns, ident_t templateName, bool& alreadyDeclared) = 0;

   virtual void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly, bool inheritMode,
                                 bool ignoreFields) = 0;

   virtual ref_t resolveClosure(ref_t closureMessage, ref_t outputRef, ident_t ns) = 0;

   virtual ref_t mapNewIdentifier(ident_t ns, ident_t identifier, Visibility visibility) = 0;
   virtual ref_t mapFullReference(ident_t referenceName, bool existing = false) = 0;
   virtual ref_t mapWeakReference(ident_t referenceName, bool existing = false) = 0;

   virtual ref_t resolveImplicitIdentifier(ident_t ns, ident_t identifier, Visibility visibility) = 0;
   virtual ref_t resolveImportedIdentifier(ident_t identifier, IdentifierList* importedNs) = 0;
   virtual ident_t resolveFullName(ref_t reference) = 0;
   virtual ident_t resolveFullName(ident_t referenceName) = 0;

   virtual ref_t resolveWeakTemplateReferenceID(ref_t reference) = 0;

   void raiseError(const char* message, ident_t sourcePath, SNode node)
   {
      SNode terminal = SyntaxTree::findTerminalInfo(node);

      int col = terminal.findChild(lxCol).argument;
      int row = terminal.findChild(lxRow).argument;
      ident_t identifier = terminal.identifier();
      //if (emptystr(identifier))
      //   identifier = terminal.identifier();

      project->raiseError(message, sourcePath, row, col, identifier);
   }

   void raiseWarning(int level, const char* message, ident_t sourcePath, SNode node)
   {
      SNode terminal = SyntaxTree::findTerminalInfo(node);

      int col = terminal.findChild(lxCol).argument;
      int row = terminal.findChild(lxRow).argument;
      ident_t identifier = terminal.identifier();

      project->raiseWarning(level, message, sourcePath, row, col, identifier);
   }

   void printMessageInfo(const char* info, ref_t message)
   {
      IdentifierString messageName;
      ByteCodeCompiler::resolveMessageName(messageName, module, message);

      project->printInfo(info, messageName.ident());
   }

   void printInfo(const char* message, const char* arg)
   {
      project->printInfo(message, arg);
   }

//   //   void raiseWarning(int level, const char* message, ident_t sourcePath, ident_t identifier)
////   {
////      project->raiseWarning(level, message, sourcePath, 0, 0, identifier);
////   }
////
////   void raiseWarning(int level, const char* message, ident_t sourcePath)
////   {
////      project->raiseWarning(level, message, sourcePath);
////   }

   virtual ref_t generateTemplate(ref_t reference, List<SNode>& parameters, ident_t ns, bool declarationMode,
      ExtensionMap* outerExtensionList) = 0;
   virtual void generateStatementCode(SyntaxWriter& writer, ref_t reference, List<SNode>& parameters) = 0;
   virtual void generateTemplateProperty(SyntaxWriter& writer, ref_t reference, List<SNode>& parameters,
      int bookmark, bool inlineMode) = 0;
   virtual void importClassTemplate(SyntaxWriter& writer, ref_t reference, List<SNode>& parameters) = 0;

   virtual void declareNamespace(ident_t name) = 0;
   virtual bool includeNamespace(IdentifierList& importedNs, ident_t name, bool& duplicateInclusion) = 0;

   bool __FASTCALL isInteralOp(ref_t reference)
   {
      ident_t identName = resolveFullName(reference);
      if (isWeakReference(identName)) {
         return true;
      }
      else {
         ref_t ownerRef = 0;
         _Module* ownerModule = project->resolveModule(identName, ownerRef, true);

         return ownerModule == module;
      }
   }

   _ModuleScope()
      : attributes(0), savedPaths(INVALID_REF), cachedSizes(Pair<int,bool>(0, false))
   {
      project = nullptr;
      debugModule = module = nullptr;
      intReference = superReference = 0;
      messageNameReference = messageReference = 0;
      longReference = literalReference = wideReference = 0;
      charReference = realReference = 0;
      closureTemplateReference = refTemplateReference = 0;
      lazyExprReference = extMessageReference = 0;
      arrayTemplateReference = 0;
      /*wrapReference = */argArrayTemplateReference = 0;

      init_message = dispatch_message = 0;
      protected_constructor_message = constructor_message = 0;

      // default compiler settings
      stackAlignment = 4;
   }
};

// --- _CompileScope ---

struct _CompileScope
{
   _ModuleScope* moduleScope;
   ident_t       ns;
};

// --- _Compiler ---

class _Compiler
{
public:
   virtual ref_t resolvePrimitiveReference(_CompileScope& scope, ref_t argRef, ref_t elementRef, bool declarationMode) = 0;

   //virtual void injectConverting(SNode& node, LexicalType convertOp, int convertArg, LexicalType targetOp, int targetArg, ref_t targetClassRef,
   //   int stacksafeAttr, bool embeddableAttr) = 0;

   virtual bool injectEmbeddableOp(_ModuleScope& scope, SNode assignNode, SNode callNode, SNode copyNode, ref_t subject, int paramCount) = 0;
   virtual void injectEmbeddableConstructor(SNode classNode, mssg_t message, mssg_t privateRef) = 0;
   virtual void injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, mssg_t message, LexicalType methodType,
      ClassInfo& info) = 0;
   virtual void injectVirtualReturningMethod(_ModuleScope& scope, SNode classNode, mssg_t message, 
      ident_t variable, ref_t outputRef) = 0;
   virtual void injectVirtualDispatchMethod(_ModuleScope& scope, SNode classNode, mssg_t message, 
      LexicalType type, ident_t argument) = 0;
//   virtual void injectDefaultConstructor(_ModuleScope& scope, SNode classNode, ref_t classRef, bool protectedOne) = 0;
   virtual void injectExprOperation(SNode& node, int size, int tempLocal, LexicalType op, int opArg) = 0;

//   virtual SNode injectTempLocal(SNode node, int size, bool boxingMode) = 0;
//
//   virtual void injectVirtualField(SNode classNode, LexicalType sourceType, ref_t sourceArg, int postfixIndex) = 0;

   virtual void generateOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef) = 0;
   virtual void generateClosedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef) = 0;
   virtual void generateSealedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef) = 0;
   virtual ref_t generateExtensionTemplate(_ModuleScope& scope, ref_t templateRef, size_t argumentLen,
      ref_t* arguments, ident_t ns, ExtensionMap* outerExtensionList) = 0;

   virtual void declareModuleIdentifiers(SyntaxTree& tree, _ModuleScope& scope) = 0;
   virtual bool declareModule(SyntaxTree& tree, _ModuleScope& scope, bool forcedDeclaration,
      bool& repeatMode, ExtensionMap* outerExtensionList) = 0;
   virtual void compileModule(SyntaxTree& syntaxTree, _ModuleScope& scope,
      ident_t greeting, ExtensionMap* outerExtensionList) = 0;
};

// --- _CompilerLogic ---

class _CompilerLogic
{
public:
   struct FieldAttributes
   {
      ref_t fieldRef;
      ref_t elementRef;
      int   size;
      bool  isStaticField;
      bool  isEmbeddable;
      bool  isConstAttr;
//      bool  isSealedAttr;
//      bool  isClassAttr;
      bool  isArray;

//      // if the field should be mapped to the message
//      ref_t messageRef;
//      ref_t messageAttr;

      FieldAttributes()
      {
         elementRef = fieldRef = 0;
         size = 0;
         /*isClassAttr = */isStaticField = isEmbeddable = isConstAttr = /*isSealedAttr = */false;
         isArray = false;

//         messageRef = messageAttr = 0;
      }
   };

   enum class ExpressionAttribute : uint64_t
   {
      eaNone               = 0x00000000000,
      eaNoDebugInfo        = 0x00000000001,
      eaNestedNs           = 0x00000000002,
      eaIntern             = 0x00000000004,
      eaModuleScope        = 0x00000000008,
      eaNewOp              = 0x00000000010,
      eaSilent             = 0x00000000020,
      eaRootExpr           = 0x00000000040,
      eaRootSymbol         = 0x00000000080,
      eaRoot               = 0x00000000100,
      eaCast               = 0x00000000200,
      eaNoPrimitives       = 0x00000000400,
      eaDynamicObject      = 0x00000000800,
      eaInlineExpr         = 0x00000001000,
//      eaNoUnboxing         = 0x00000002000,
//      eaNoBoxing           = 0x00000003000,
      eaMember             = 0x00000004000,
      eaRef                = 0x00000008000,
      eaPropExpr           = 0x00000010000,
      eaMetaField          = 0x00000020000,
      eaLoop               = 0x00000040000,
      eaExtern             = 0x00000080000,
      eaForward            = 0x00000100000,
      eaParams             = 0x00000200000,
      eaInitializerScope   = 0x00000400000,
      eaSwitch             = 0x00000800000,
      eaClass              = 0x00001000000,
//      eaYieldExpr          = 0x00002000000,
      eaIgnoreDuplicates   = 0x00004000000,
      eaMssg               = 0x00008000000,
//      eaVirtualExpr        = 0x00010000000,
      eaSubj               = 0x00020000000,
      eaDirectCall         = 0x00040000000,
      eaParameter          = 0x00080000000,
      eaLazy               = 0x00100000000,
//      eaInlineArg          = 0x00200000000,
      eaConstExpr          = 0x00400000000,
//      eaCallOp             = 0x00800000000,
      eaRefExpr            = 0x01000000000,
      eaPreviousScope      = 0x02000000000,
      eaConversionOp       = 0x04000000000,
      eaTarget             = 0x08080000000,
      eaTargetExpr         = 0x08000000000,
      eaAssignTarget       = 0x10000000000,
      eaTypeOfOp           = 0x20000000000,

      eaScopeMask          = /*0x0300041400A*/eaNestedNs | eaAssignTarget | eaTypeOfOp,
//      eaObjectMask         = 0x008A821B2F4,
   };

   struct ExpressionAttributes
   {
      ExpressionAttribute attrs;

      bool test(ExpressionAttribute mask)
      {
         return ((uint64_t)attrs & (uint64_t)mask) == (uint64_t)mask;
      }

      bool testAndExclude(ExpressionAttribute mask)
      {
         if (test(mask)) {
            exclude(mask);

            return true;
         }
         else return false;
      }

      bool testany(ExpressionAttribute mask)
      {
         return ((uint64_t)attrs & (uint64_t)mask) != (uint64_t)0ul;
      }

      void exclude(ExpressionAttribute mask)
      {
         attrs = (ExpressionAttribute)((uint64_t)attrs & ~(uint64_t)mask);
      }

      void include(ExpressionAttribute mask)
      {
         attrs = (ExpressionAttribute)((uint64_t)attrs | (uint64_t)mask);
      }

      operator const ExpressionAttribute () const { return attrs; }

      static bool __FASTCALL test(ExpressionAttribute attrs, ExpressionAttribute mask)
      {
         return ((uint64_t)attrs & (uint64_t)mask) == (uint64_t)mask;
      }

      static bool __FASTCALL testany(ExpressionAttribute attrs, ExpressionAttribute mask)
      {
         return ((uint64_t)attrs & (uint64_t)mask) != (uint64_t)0;
      }

      static ExpressionAttribute __FASTCALL exclude(ExpressionAttribute attrs, ExpressionAttribute mask)
      {
         return (ExpressionAttribute)((uint64_t)attrs & ~(uint64_t)mask);
      }

      ExpressionAttributes()
      {
         attrs = ExpressionAttribute::eaNone;
      }

      ExpressionAttributes(ExpressionAttribute attrs)
      {
         this->attrs = attrs;
      }
      ExpressionAttributes(ExpressionAttribute attrs, ExpressionAttribute excludeMask)
         : ExpressionAttributes(attrs)
      {
         exclude(excludeMask);
      }
//      ExpressionAttributes(ExpressionAttributes attrs, ExpressionAttribute excludeMask)
//         : ExpressionAttributes(attrs.attrs)
//      {
//         exclude(excludeMask);
//      }
   };

   struct ChechMethodInfo
   {
      bool  found;
      bool  directResolved;
      bool  withCustomDispatcher;
      bool  withVariadicDispatcher;
      bool  stackSafe;
      bool  embeddable;
      bool  withEmbeddableRet;
//      bool  function;
//      bool  dynamicRequired;
      ref_t outputReference;
      ref_t constRef;
      ref_t protectedRef;

      ChechMethodInfo()
      {
         directResolved = false;
         embeddable = found = false;
         outputReference = 0;
         constRef = 0;
         protectedRef = 0;
         withVariadicDispatcher = withCustomDispatcher = false;
         stackSafe = false;
         withEmbeddableRet = false;
//         function = false;
//         dynamicRequired = false;
      }
   };

   virtual int __FASTCALL defineStackSafeAttrs(_ModuleScope& scope, mssg_t message) = 0;

   virtual int checkMethod(_ModuleScope& scope, ref_t reference, mssg_t message,
      ChechMethodInfo& result, bool resolveProtected) = 0;
   virtual int checkMethod(ClassInfo& info, mssg_t message, ChechMethodInfo& result,
      bool resolveProtected) = 0;

   // retrieve the class info / size
   virtual bool __FASTCALL defineClassInfo(_ModuleScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;

   virtual int defineStructSizeVariable(_ModuleScope& scope, ref_t reference, ref_t elementRef, bool& variable) = 0;
   virtual int __FASTCALL defineStructSize(_ModuleScope& scope, ref_t reference, ref_t elementRef) = 0;
   virtual int __FASTCALL defineStructSize(ClassInfo& info, bool& variable) = 0;

   virtual ref_t __FASTCALL definePrimitiveArray(_ModuleScope& scope, ref_t elementRef, bool structOne) = 0;

   // retrieve the call type
   virtual int __FASTCALL resolveCallType(_ModuleScope& scope, ref_t& classReference, mssg_t message, ChechMethodInfo& result) = 0;

   // retrieve the operation type
   virtual int resolveOperationType(_ModuleScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result) = 0;
   virtual int resolveNewOperationType(_ModuleScope& scope, ref_t loperand, ref_t roperand) = 0;

   // retrieve the branching operation type
   virtual bool resolveBranchOperation(_ModuleScope& scope, int operatorId, ref_t loperand, ref_t& reference) = 0;

//   virtual ref_t resolveArrayElement(_ModuleScope& scope, ref_t reference) = 0;

   // check if the classes is compatible
   virtual bool __FASTCALL isCompatible(_ModuleScope& scope, ref_t targetRef, ref_t sourceRef, bool ignoreNils) = 0;

   virtual bool __FASTCALL isVariable(_ModuleScope& scope, ref_t targetRef) = 0;
   virtual bool __FASTCALL isValidType(_ModuleScope& scope, ref_t targetRef, bool ignoreUndeclared, bool allowRole) = 0;
   virtual bool __FASTCALL doesClassExist(_ModuleScope& scope, ref_t targetRef) = 0;
//   virtual bool isArray(_ModuleScope& scope, ref_t targetRef) = 0;
//   virtual bool isSealedOrClosed(_ModuleScope& scope, ref_t targetRef) = 0;
//
////   virtual bool isWrapper(ClassInfo& info) = 0;
////   virtual ref_t resolvePrimitive(ClassInfo& info, ref_t& element) = 0;
////   // check if the class can be used as a fixed-size embeddable array
   virtual bool __FASTCALL isEmbeddableArray(ClassInfo& info) = 0;
   virtual bool __FASTCALL isEmbeddable(ClassInfo& info) = 0;
   virtual bool __FASTCALL isEmbeddable(_ModuleScope& scope, ref_t reference) = 0;
   virtual bool __FASTCALL isStacksafeArg(ClassInfo& info) = 0;
   virtual bool __FASTCALL isStacksafeArg(_ModuleScope& scope, ref_t reference) = 0;
   virtual bool __FASTCALL isMethodAbstract(ClassInfo& info, mssg_t message) = 0;
//   virtual bool isMethodYieldable(ClassInfo& info, mssg_t message) = 0;
   virtual bool __FASTCALL isMethodGeneric(ClassInfo& info, mssg_t message) = 0;
   virtual bool __FASTCALL isMixinMethod(ClassInfo& info, mssg_t message) = 0;
   virtual bool __FASTCALL isMultiMethod(ClassInfo& info, mssg_t message) = 0;
////   virtual bool isFunction(ClassInfo& info, ref_t message) = 0;
////   virtual bool isMethodEmbeddable(ClassInfo& info, ref_t message) = 0;
////   //   virtual bool isDispatcher(ClassInfo& info, ref_t message) = 0;

   // class is considered to be a role if it cannot be initiated
   virtual bool __FASTCALL isRole(ClassInfo& info) = 0;
   virtual bool __FASTCALL isAbstract(ClassInfo& info) = 0;
   virtual bool __FASTCALL validateAutoType(_ModuleScope& scope, ref_t& reference) = 0;

   virtual bool isWithEmbeddableDispatcher(_ModuleScope& scope, SNode node) = 0;

   // auto generate virtual methods / fields
   virtual void injectVirtualCode(_ModuleScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler, bool closed) = 0;
   virtual void injectVirtualFields(_ModuleScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler) = 0;
   virtual ref_t generateOverloadList(_ModuleScope& scope, _Compiler& compiler, mssg_t message,
      ClassInfo::CategoryInfoMap& list, void* param, ref_t(*resolve)(void*, ref_t), int flags) = 0;
   virtual void injectVirtualMultimethods(_ModuleScope& scope, SNode node, _Compiler& compiler,
      List<mssg_t>& implicitMultimethods, LexicalType methodType, ClassInfo& info) = 0;
   virtual void verifyMultimethods(_ModuleScope& scope, SNode node, ClassInfo& info, List<mssg_t>& implicitMultimethods) = 0;
   virtual void injectOperation(SNode& node, _CompileScope& scope, _Compiler& compiler, int operatorId, int operation, ref_t& reference,
      ref_t elementRef, int tempLocal) = 0;
   virtual ConversionInfo injectImplicitConversion(_CompileScope& scope, _Compiler& compiler, ref_t targetRef,
      ref_t sourceRef, ref_t elementRef/*, bool noUnboxing, int fixedArraySize*/) = 0;
   ////   virtual ref_t resolveImplicitConstructor(_ModuleScope& scope, ref_t targetRef, ref_t signRef, int paramCount, int& stackSafeAttr, bool ignoreMultimethod) = 0;
//   virtual void injectNewOperation(SNode& node, _ModuleScope& scope, int operation, ref_t targetRef, ref_t elementRef) = 0;
   virtual void injectInterfaceDispatch(_ModuleScope& scope, _Compiler& compiler, SNode node, ref_t parentRef) = 0;
//   virtual bool injectConstantConstructor(SNode& node, _ModuleScope& scope, _Compiler& compiler, ref_t targetRef, mssg_t messageRef) = 0;

   // auto generate class flags
   virtual void tweakClassFlags(_ModuleScope& scope, _Compiler& compiler, ref_t classRef, ClassInfo& info, bool classClassMode) = 0;
   virtual void tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info) = 0;

   virtual void validateClassDeclaration(_ModuleScope& scope, ClassInfo& info, bool& withAbstractMethods,
      bool& disptacherNotAllowed, bool& emptyStructure) = 0;

   // attribute validations
   virtual bool __FASTCALL validateNsAttribute(int attrValue, Visibility& visibility) = 0;
   virtual bool __FASTCALL validateClassAttribute(int& attrValue, Visibility& visibility) = 0;
   virtual bool __FASTCALL validateMethodAttribute(int& attrValue, bool& explicitMode) = 0;
   virtual bool __FASTCALL validateImplicitMethodAttribute(int& attrValue, bool complexName) = 0;
   virtual bool validateFieldAttribute(int& attrValue, FieldAttributes& attrs) = 0;
   virtual bool __FASTCALL validateExpressionAttribute(ref_t attrValue, ExpressionAttributes& attributes, bool& newVariable) = 0;
   virtual bool validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne,
      Visibility& visibility) = 0;
   virtual bool validateMessage(_ModuleScope& scope, mssg_t message, int hints) = 0;
   virtual bool validateArgumentAttribute(int attrValue, bool& byRefArg, bool& paramsArg) = 0;

   virtual bool isSignatureCompatible(_ModuleScope& scope, mssg_t targetMessage, mssg_t sourceMessage) = 0;
   virtual bool isMessageCompatibleWithSignature(_ModuleScope& scope, ref_t targetRef, mssg_t targetMessage,
      ref_t* sourceSignatures, size_t len, int& stackSafeAttr) = 0;

   virtual mssg_t resolveSingleMultiDisp(_ModuleScope& scope, ref_t reference, mssg_t message) = 0;

   virtual mssg_t resolveEmbeddableRetMessage(_CompileScope& scope, _Compiler& compiler, ref_t target,
      mssg_t message, ref_t expectedRef) = 0 ;

   // optimization
   virtual bool recognizeEmbeddableIdle(SNode node, bool extensionOne) = 0;
   virtual bool recognizeEmbeddableMessageCall(SNode node, mssg_t& messageRef) = 0;
   virtual bool optimizeEmbeddable(SNode node, _ModuleScope& scope) = 0;

////   virtual bool optimizeReturningStructure(_ModuleScope& scope, _Compiler& compiler, SNode node, bool argMode) = 0;
   virtual bool optimizeEmbeddableOp(_ModuleScope& scope, _Compiler& compiler, SNode node) = 0;
   virtual bool optimizeBranchingOp(_ModuleScope& scope, SNode node) = 0;

   virtual mssg_t resolveMultimethod(_ModuleScope& scope, mssg_t multiMessage, ref_t targetRef, ref_t implicitSignatureRef,
      int& stackSafeAttr, bool selfCall) = 0;
   virtual ref_t resolveExtensionTemplate(_ModuleScope& scope, _Compiler& compiler, ident_t pattern,
      ref_t signatureRef, ident_t ns, ExtensionMap* outerExtensionList) = 0;
};

typedef _CompilerLogic::ExpressionAttributes EAttrs;
typedef _CompilerLogic::ExpressionAttribute EAttr;

}  // _ELENA_

#endif // compilerCommonH
