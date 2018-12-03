//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler common interfaces.
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerCommonH
#define compilerCommonH

#include "elena.h"
#include "syntaxtree.h"

// virtual objects
//#define V_PARAMETER      (ref_t)-02
//#define V_FLAG           (ref_t)-03
#define V_NIL            (ref_t)-04
#define V_TYPE           (ref_t)-05

//#define V_BINARY         (ref_t)-10
//#define V_INT8           (ref_t)-11
//#define V_PTR            (ref_t)-12
//#define V_INT32          (ref_t)-13
//#define V_INT64          (ref_t)-14
//#define V_REAL64         (ref_t)-15
//#define V_DWORD          (ref_t)-16
//#define V_SIGNATURE      (ref_t)-18
//#define V_MESSAGE        (ref_t)-19
//#define V_EXTMESSAGE     (ref_t)-21
//#define V_SYMBOL         (ref_t)-22
//
//#define V_STRCONSTANT    (ref_t)-23 // used for explicit constant operations
//
//#define V_OBJECT         (ref_t)-28
//
//#define V_OBJARRAY      (ref_t)-30
//#define V_INT32ARRAY    (ref_t)-31
//#define V_ARGARRAY      (ref_t)-32
//#define V_BINARYARRAY   (ref_t)-35
//#define V_INT16ARRAY    (ref_t)-38
//#define V_INT8ARRAY     (ref_t)-39
//
//#define V_AUTO           (ref_t)-50
//
//#define V_IFBRANCH      (ref_t)-4097
//#define V_IFNOTBRANCH   (ref_t)-4098
////#define V_WARNING1    (ref_t)-4099
////#define V_WARNING2    (ref_t)-4100
////#define V_WARNING3    (ref_t)-4101
//
//#define V_EMBEDDABLE     (ref_t)-8193
//#define V_STATIC         (ref_t)-8194
#define V_SEALED         (ref_t)-8195
//#define V_LIMITED        (ref_t)-8196
//#define V_STRUCT         (ref_t)-8197
//#define V_ENUMLIST       (ref_t)-8198
////#define V_DYNAMIC        (ref_t)-8199
//#define V_STRING         (ref_t)-8200
//#define V_CONST          (ref_t)-8201
//#define V_GENERIC        (ref_t)-8202
//#define V_EXTENSION      (ref_t)-8203
////#define V_NOSTRUCT       (ref_t)-8204
//#define V_ACTION         (ref_t)-8205     // a closure attribute
//#define V_GROUP          (ref_t)-8206
//#define V_PRELOADED      (ref_t)-8207
#define V_SINGLETON      (ref_t)-8208
////#define V_TAPEGROUP      (ref_t)-8209
//#define V_ABSTRACT       (ref_t)-8210
#define V_PUBLIC         (ref_t)-8211
//#define V_PRIVATE        (ref_t)-8212
#define V_INTERNAL       (ref_t)-8213
#define V_CLOSED         (ref_t)-8214
//#define V_PREDEFINED     (ref_t)-8215
#define V_DISPATCHER     (ref_t)-8216

#define V_CONSTRUCTOR    (ref_t)-16384
#define V_VARIABLE       (ref_t)-16385
#define V_CLASS          (ref_t)-16386
#define V_CONVERSION     (ref_t)-16387
//#define V_INITIALIZER    (ref_t)-16388
//#define V_SYMBOLEXPR     (ref_t)-16389
//#define V_TYPETEMPL      (ref_t)-16390
//#define V_TEMPLATE       (ref_t)-16391
//#define V_FIELD          (ref_t)-16392
#define V_METHOD         (ref_t)-16393
//#define V_LOOP           (ref_t)-16394
//#define V_IMPORT         (ref_t)-16395
//#define V_EXTERN         (ref_t)-16396
//#define V_ATTRTEMPLATE   (ref_t)-16398
//#define V_ACCESSOR       (ref_t)-16399
//#define V_BLOCK          (ref_t)-16400
//#define V_NESTEDBLOCK    (ref_t)-16401
//#define V_SET            (ref_t)-16402
//#define V_STACKUNSAFE    (ref_t)-16403
////
////// obsolete
////#define V_MULTI          (ref_t)-16397

namespace _ELENA_
{

typedef Map<ident_t, ref_t>      ForwardMap;

enum MethodHint
{
   tpMask        = 0x00000F,

   tpUnknown     = 0x000000,
   tpSealed      = 0x000001,
   tpClosed      = 0x000002,
   tpNormal      = 0x000003,
   tpDispatcher  = 0x000004,
//   tpPrivate     = 0x000005,
//   tpStackSafe   = 0x000010,
//   tpEmbeddable  = 0x000020,
//   tpGeneric     = 0x000040,
//   tpAction      = 0x000080,
//   tpIfBranch    = 0x000100,
//   tpIfNotBranch = 0x000200,
   tpConstructor = 0x200400,
   tpConversion  = 0x200800,
   tpMultimethod = 0x001000,
//   tpArgDispatcher=0x003000,
//   tpStatic      = 0x004000,
//   tpAccessor    = 0x008000,
//   tpSpecial     = 0x010000,
//   tpAbstract    = 0x020000,
//   tpInternal    = 0x040000,
//   tpPredefined  = 0x080000, // virtual class declaration
//   tpDynamic     = 0x100000, // indicates that the method does not accept stack allocated parameters
//   tpInitializer = 0x200000,
};

// --- _Project ---

class _ProjectManager
{
public:
//   virtual ident_t Namespace() const = 0;
//
//   virtual int getDefaultEncoding() = 0; // !! obsolete!?
//   virtual int getTabSize() = 0; // !! obsolete!?
//
//   //   virtual bool HasWarnings() const = 0;     // !! obsolete
//   //   virtual bool WarnOnWeakUnresolved() const = 0;
//
//   virtual ident_t getManinfestName() = 0;
//   virtual ident_t getManinfestVersion() = 0;
//   virtual ident_t getManinfestAuthor() = 0;

   virtual void printInfo(const char* msg, ident_t value) = 0;

   virtual void raiseError(ident_t msg) = 0;
   virtual void raiseError(ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;
   virtual void raiseError(ident_t msg, ident_t value) = 0;

//   virtual void raiseErrorIf(bool throwExecption, ident_t msg, ident_t identifier) = 0;

   virtual void raiseWarning(int level, ident_t msg, ident_t path, int row, int column, ident_t terminal = NULL) = 0;
//   virtual void raiseWarning(int level, ident_t msg, ident_t path) = 0;

   virtual _Module* createModule(ident_t name) = 0;
   virtual _Module* createDebugModule(ident_t name) = 0;

   virtual _Module* loadModule(ident_t package, bool silentMode) = 0;
//   //   virtual void saveModule(_Module* module, ident_t extension) = 0; // !! obsolete

   virtual _Module* resolveModule(ident_t referenceName, ref_t& reference, bool silentMode = false) = 0;
   virtual _Module* resolveWeakModule(ident_t weakReferenceName, ref_t& reference, bool silentMode = false) = 0;

   virtual ident_t resolveForward(ident_t forward) = 0;

   virtual bool addForward(ident_t forward, ident_t reference) = 0;

//   virtual ident_t resolveExternalAlias(ident_t alias, bool& stdCall) = 0;
};

//// class forward declaration
//class _Compiler;

// --- _ModuleScope ---

struct _ModuleScope
{
//   struct BranchingInfo
//   {
//      ref_t reference;
//      ref_t trueRef;
//      ref_t falseRef;
//
//      BranchingInfo()
//      {
//         reference = 0;
//         trueRef = falseRef = 0;
//      }
//   };

   _ProjectManager*  project;

   _Module*          module;
   _Module*          debugModule;

   // cached references
   ref_t             superReference;
//   ref_t             intReference;
//   ref_t             longReference;
//   ref_t             realReference;
//   ref_t             signatureReference;
//   ref_t             messageReference;
//   ref_t             extMessageReference;
//   ref_t             boolReference;
//   ref_t             literalReference;
//   ref_t             wideReference;
//   ref_t             charReference;
//   ref_t             arrayReference;
//   ref_t             refTemplateReference;
//   ref_t             arrayTemplateReference;
//   ref_t             closureTemplateReference;
//   ref_t             lazyExprReference;
//
   // cached messages
   ref_t             dispatch_message;
   ref_t             newobject_message;

//   // cached bool values
//   BranchingInfo     branchingInfo;

   // cached paths
   SymbolMap         savedPaths;

   MessageMap        attributes;

   virtual ref_t mapAnonymous(ident_t prefix) = 0;

////   virtual ref_t mapAttribute(SNode terminal) = 0;
////   virtual ref_t mapTerminal(SNode terminal, bool existing = false) = 0;
//
//   virtual SubjectList* getAutogerenatedExtensions(ref_t attr) = 0;
   virtual void saveAttribute(ident_t typeName, ref_t classReference) = 0;
//   virtual void saveAutogerenatedExtension(ref_t attr, ref_t extension) = 0;

   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;
   virtual ref_t loadClassInfo(ClassInfo& info, ident_t vmtName, bool headerOnly = false) = 0;
//   virtual ref_t loadSymbolExpressionInfo(SymbolExpressionInfo& info, ident_t symbolName) = 0;

   virtual _Module* loadReferenceModule(ident_t referenceName, ref_t& reference) = 0;
   virtual _Module* loadReferenceModule(ref_t reference, ref_t& moduleReference) = 0;

   virtual _Memory* mapSection(ref_t reference, bool existing) = 0;
//   virtual ref_t mapTemplateClass(ident_t ns, ident_t templateName, bool& alreadyDeclared) = 0;

   virtual void importClassInfo(ClassInfo& copy, ClassInfo& target, _Module* exporter, bool headerOnly, bool inheritMode) = 0;

//   virtual ref_t resolveClosure(_Compiler& compiler, ref_t closureMessage, ref_t outputRef, ExtensionMap* extensionsToExport) = 0;

   virtual ref_t mapNewIdentifier(ident_t ns, ident_t identifier, bool privateOne) = 0;
   virtual ref_t mapFullReference(ident_t referenceName, bool existing = false) = 0;

   virtual ref_t resolveImplicitIdentifier(ident_t ns, ident_t identifier, bool referenceOne, IdentifierList* importedNs) = 0;
   virtual ident_t resolveFullName(ref_t reference) = 0;
   virtual ident_t resolveFullName(ident_t referenceName) = 0;

   void raiseError(const char* message, ident_t sourcePath, SNode node)
   {
      SNode terminal = SyntaxTree::findTerminalInfo(node);

      int col = terminal.findChild(lxCol).argument;
      int row = terminal.findChild(lxRow).argument;
      ident_t identifier = terminal.identifier();
      if (emptystr(identifier))
         identifier = terminal.identifier();

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

//   void raiseWarning(int level, const char* message, ident_t sourcePath, ident_t identifier)
//   {
//      project->raiseWarning(level, message, sourcePath, 0, 0, identifier);
//   }
//
//   void raiseWarning(int level, const char* message, ident_t sourcePath)
//   {
//      project->raiseWarning(level, message, sourcePath);
//   }
//
//   virtual ref_t generateTemplate(_Compiler& compiler, ref_t reference, List<ref_t>& parameters, ExtensionMap* extensionsToExport) = 0;
//
//   virtual bool includeNamespace(IdentifierList& importedNs, ident_t name, bool& duplicateInclusion) = 0;

   _ModuleScope()
      : attributes(0), savedPaths(-1)
   {
      project = nullptr;
      debugModule = module = nullptr;
      /*intReference = boolReference = */superReference = 0;
//      signatureReference = messageReference = 0;
//      longReference = literalReference = wideReference = 0;
//      arrayReference = charReference = realReference = 0;
//      closureTemplateReference = refTemplateReference = 0;
//      lazyExprReference = extMessageReference = 0;
//      arrayTemplateReference = 0;

      newobject_message = dispatch_message = 0;
   }
};

// --- _Compiler ---

class _Compiler
{
public:
//   virtual void injectBoxing(SyntaxWriter& writer, _CompilerScope& scope, LexicalType boxingType, int argument, ref_t targetClassRef, bool arrayMode = false) = 0;
//   virtual void injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef, 
//      ref_t targetRef, int stacksafeAttr) = 0;
////   virtual void injectFieldExpression(SyntaxWriter& writer) = 0;
//
//   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject) = 0;
//   virtual void injectEmbeddableOp(_CompilerScope& scope, SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb) = 0;
//   virtual void injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t privateRef, ref_t genericMessage) = 0;
   virtual void injectVirtualMultimethod(_ModuleScope& scope, SNode classNode, ref_t message, LexicalType methodType) = 0;
//   virtual void injectVirtualArgDispatcher(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType) = 0;
   virtual void injectVirtualReturningMethod(_ModuleScope& scope, SNode classNode, ref_t message, ident_t variable, ref_t outputRef) = 0;
//   virtual void injectVirtualDispatchMethod(SNode classNode, ref_t message, LexicalType type, ident_t argument) = 0;
//   virtual void injectDirectMethodCall(SyntaxWriter& writer, ref_t targetRef, ref_t message) = 0;
//
//   virtual void injectLocalBoxing(SNode node, int size) = 0;
////   //virtual int injectTempLocal(SNode node) = 0;
//
//   virtual void injectVirtualStaticConstField(_CompilerScope& scope, SNode classNode, ident_t fieldName, ref_t fieldRef) = 0;
//
//   virtual void generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef) = 0;
   virtual void generateOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef) = 0;
   virtual void generateClosedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef) = 0;
   virtual void generateSealedOverloadListMember(_ModuleScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef) = 0;

   virtual bool declareModule(SyntaxTree& tree, _ModuleScope& scope/*, ident_t path, ident_t ns, IdentifierList* imported*/, bool& repeatMode/*, ExtensionMap* extensionsToExport*/) = 0;
   virtual void compileModule(SyntaxTree& syntaxTree, _ModuleScope& scope/*, ident_t path, ident_t ns, IdentifierList* imported*//*, Unresolveds& unresolveds*/) = 0;

////   virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader) = 0;
};

// --- _CompilerLogic ---

class _CompilerLogic
{
public:
   struct ChechMethodInfo
   {
      bool  found;
      bool  directResolved;
//      bool  withCustomDispatcher;
////      //bool  closed;
//      bool  stackSafe;
//      bool  embeddable;
//      bool  withOpenArgDispatcher;
//      bool  withOpenArg1Dispatcher;
//      bool  withOpenArg2Dispatcher;
//      bool  closure;
//      bool  dynamicRequired;
      ref_t outputReference;

      ChechMethodInfo()
      {
         directResolved = false;
         /*embeddable = *//*closed = */found = false;
         outputReference = 0;
//         withCustomDispatcher = false;
//         stackSafe = false;
//         withOpenArgDispatcher = false;
//         withOpenArg1Dispatcher = false;
//         withOpenArg2Dispatcher = false;
//         closure = false;
//         dynamicRequired = false;
      }
   };

//   virtual int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, ChechMethodInfo& result) = 0;
//   virtual int checkMethod(ClassInfo& info, ref_t message, ChechMethodInfo& result) = 0;

   // retrieve the class info / size
   virtual bool defineClassInfo(_ModuleScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;

//   virtual int defineStructSizeVariable(_CompilerScope& scope, ref_t reference, ref_t elementRef, bool& variable) = 0;
//   virtual int defineStructSize(_CompilerScope& scope, ref_t reference, ref_t elementRef) = 0;
//   virtual int defineStructSize(ClassInfo& info, bool& variable) = 0;
//
//   virtual ref_t definePrimitiveArray(_CompilerScope& scope, ref_t elementRef) = 0;

   // retrieve the call type
   virtual int resolveCallType(_ModuleScope& scope, ref_t& classReference, ref_t message, ChechMethodInfo& result) = 0;

//   // retrieve the operation type
//   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result) = 0;
//   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result) = 0;
//   virtual int resolveNewOperationType(_CompilerScope& scope, ref_t loperand, ref_t roperand, ref_t& result) = 0;
//
//   // retrieve the branching operation type
//   virtual bool resolveBranchOperation(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t& reference) = 0;
//
//   virtual ref_t resolvePrimitiveReference(_CompilerScope& scope, ref_t reference) = 0;
//   virtual ref_t retrievePrimitiveReference(_CompilerScope& scope, ClassInfo& info) = 0;

   // check if the classes is compatible
   virtual bool isCompatible(_ModuleScope& scope, ref_t targetRef, ref_t sourceRef) = 0;

//   virtual bool isVariable(_CompilerScope& scope, ref_t targetRef) = 0;
//
//   virtual bool isEmbeddableArray(ClassInfo& info) = 0;
//   virtual bool isVariable(ClassInfo& info) = 0;
//   virtual bool isEmbeddable(ClassInfo& info) = 0;
//   virtual bool isEmbeddable(_CompilerScope& scope, ref_t reference) = 0;
//   virtual bool isMethodStacksafe(ClassInfo& info, ref_t message) = 0;
//   virtual bool isMethodAbstract(ClassInfo& info, ref_t message) = 0;
//   virtual bool isMethodGeneric(ClassInfo& info, ref_t message) = 0;
   virtual bool isMultiMethod(ClassInfo& info, ref_t message) = 0;
//   virtual bool isClosure(ClassInfo& info, ref_t message) = 0;
//   virtual bool isDispatcher(ClassInfo& info, ref_t message) = 0;

   // class is considered to be a role if it cannot be initiated
   virtual bool isRole(ClassInfo& info) = 0;          
//   virtual bool isAbstract(ClassInfo& info) = 0;
//
//   virtual bool isWithEmbeddableDispatcher(_CompilerScope& scope, SNode node) = 0;
//
////   virtual bool isPrimitiveRef(ref_t reference) = 0;
////   virtual bool isPrimitiveArray(ref_t reference) = 0;

   // auto generate virtual methods / fields
   virtual void injectVirtualCode(_ModuleScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler/*, bool closed*/) = 0;
//   virtual void injectVirtualFields(_CompilerScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler) = 0;
   virtual void injectVirtualMultimethods(_ModuleScope& scope, SNode node, ClassInfo& info, _Compiler& compiler, List<ref_t>& implicitMultimethods, LexicalType methodType) = 0;
   virtual void verifyMultimethods(_ModuleScope& scope, SNode node, ClassInfo& info, List<ref_t>& implicitMultimethods) = 0;
//   virtual void injectOperation(SyntaxWriter& writer, _CompilerScope& scope, int operatorId, int operation, ref_t& reference, ref_t elementRef) = 0;
   virtual bool injectImplicitConversion(SyntaxWriter& writer, _ModuleScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef/*, ref_t elementRef*/) = 0;
//   virtual bool injectImplicitConstructor(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t signRef) = 0;
//   virtual bool injectImplicitCreation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef) = 0;
//   virtual bool injectDefaultCreation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t classClassRef) = 0;
//   virtual void injectNewOperation(SyntaxWriter& writer, _CompilerScope& scope, int operation, ref_t targetRef, ref_t elementRef) = 0;
//////   virtual void injectVariableAssigning(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t& targetRef, ref_t& type, int& operand, bool paramMode) = 0;
////   virtual void injectOverloadList(_CompilerScope& scope, ClassInfo& info, _Compiler& compiler, ref_t classRef) = 0;
//   virtual void injectInterfaceDisaptch(_CompilerScope& scope, _Compiler& compiler, SNode node, ref_t parentRef) = 0;

   // auto generate class flags
   virtual void tweakClassFlags(_ModuleScope& scope, _Compiler& compiler, ref_t classRef, ClassInfo& info, bool classClassMode) = 0;
//   virtual bool tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info) = 0;
//
//   virtual void validateClassDeclaration(ClassInfo& info, bool& withAbstractMethods, bool& disptacherNotAllowed, bool& emptyStructure) = 0;

   // attribute validations
   virtual bool validateClassAttribute(int& attrValue) = 0;
   virtual bool validateMethodAttribute(int& attrValue, bool& explicitMode) = 0;
   virtual bool validateImplicitMethodAttribute(int& attrValue) = 0;
   virtual bool validateFieldAttribute(int& attrValue/*, bool& isSealed, bool& isConstant*/) = 0;
   virtual bool validateExpressionAttribute(int& attrValue, bool& typeAttr, bool& castAttr) = 0;
//   virtual bool validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne) = 0;
   virtual bool validateMessage(_ModuleScope& scope, ref_t message, bool isClassClass) = 0;
   virtual bool validateArgumentAttribute(int attrValue) = 0;

   virtual bool isDefaultConstructorEnabled(ClassInfo& info) = 0;

//   // optimization
//   virtual bool validateBoxing(_CompilerScope& scope, _Compiler& compiler, SNode& node, ref_t targetRef, ref_t sourceRef, bool unboxingExpected, bool dynamicRequired) = 0;
//   virtual bool recognizeEmbeddableGet(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableGetAt(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableGetAt2(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableEval(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableEval2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableIdle(SNode node, bool extensionOne) = 0;
//   virtual bool recognizeEmbeddableMessageCall(SNode node, ref_t& messageRef) = 0;
//   virtual bool optimizeEmbeddable(SNode node, _CompilerScope& scope) = 0;
//
//   virtual bool optimizeEmbeddableGet(_CompilerScope& scope, _Compiler& compiler, SNode node) = 0;
//   virtual bool optimizeEmbeddableOp(_CompilerScope& scope, _Compiler& compiler, SNode node/*, int verb, int attribte, int paramCount*/) = 0;
//   virtual void optimizeBranchingOp(_CompilerScope& scope, SNode node) = 0;

   virtual ref_t resolveMultimethod(_ModuleScope& scope, ref_t multiMessage, ref_t targetRef, ref_t implicitSignatureRef/*, int& stackSafeAttr*/) = 0;
};

}  // _ELENA_

#endif // compilerCommonH
