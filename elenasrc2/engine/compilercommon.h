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
#define V_PARAMETER      (ref_t)-02
#define V_FLAG           (ref_t)-03
#define V_NIL            (ref_t)-04

#define V_BINARY         (ref_t)-10
#define V_INT8           (ref_t)-11
//#define V_PTR            (ref_t)-12
#define V_INT32          (ref_t)-13
#define V_INT64          (ref_t)-14
#define V_REAL64         (ref_t)-15
//#define V_SIGNATURE      (ref_t)-18
//#define V_MESSAGE        (ref_t)-19
//#define V_EXTMESSAGE     (ref_t)-21
//#define V_SYMBOL         (ref_t)-22

//#define V_STRCONSTANT    (ref_t)-23 // used for explicit constant operations

#define V_OBJECT         (ref_t)-28

#define V_OBJARRAY      (ref_t)-30
#define V_INT32ARRAY    (ref_t)-31
#define V_ARGARRAY      (ref_t)-32
#define V_BINARYARRAY   (ref_t)-35
#define V_INT16ARRAY    (ref_t)-38
#define V_INT8ARRAY     (ref_t)-39

#define V_AUTO           (ref_t)-50

//#define V_IFBRANCH      (ref_t)-4097
//#define V_IFNOTBRANCH   (ref_t)-4098
////#define V_WARNING1    (ref_t)-4099
////#define V_WARNING2    (ref_t)-4100
////#define V_WARNING3    (ref_t)-4101
//
//#define V_STATCKSAFE     (ref_t)-8192
//#define V_EMBEDDABLE     (ref_t)-8193
#define V_STATIC         (ref_t)-8194
#define V_SEALED         (ref_t)-8195
//#define V_LIMITED        (ref_t)-8196
#define V_STRUCT         (ref_t)-8197
#define V_ENUMLIST       (ref_t)-8198
//#define V_DYNAMIC        (ref_t)-8199
#define V_STRING         (ref_t)-8200
#define V_CONST          (ref_t)-8201
//#define V_GENERIC        (ref_t)-8202
//#define V_EXTENSION      (ref_t)-8203
//#define V_NOSTRUCT       (ref_t)-8204
//#define V_ACTION         (ref_t)-8205
//#define V_GROUP          (ref_t)-8206
//#define V_PRELOADED      (ref_t)-8207
#define V_SINGLETON      (ref_t)-8208
//#define V_TAPEGROUP      (ref_t)-8209
#define V_ABSTRACT       (ref_t)-8210
#define V_PUBLIC         (ref_t)-8211
#define V_PRIVATE        (ref_t)-8212

#define V_CONSTRUCTOR    (ref_t)-16384
#define V_VARIABLE       (ref_t)-16385
#define V_CLASS          (ref_t)-16386
#define V_CONVERSION     (ref_t)-16387
#define V_IMPLICIT       (ref_t)-16388
#define V_SYMBOLEXPR     (ref_t)-16389
#define V_TYPETEMPL      (ref_t)-16390
#define V_TEMPLATE       (ref_t)-16391
#define V_FIELD          (ref_t)-16392
#define V_METHOD         (ref_t)-16393
//#define V_LOOP           (ref_t)-16394
//#define V_IMPORT         (ref_t)-16395
//#define V_EXTERN         (ref_t)-16396
#define V_ATTRTEMPLATE   (ref_t)-16398
//#define V_ACCESSOR       (ref_t)-16399
//#define V_BLOCK          (ref_t)-16400
//#define V_NESTEDBLOCK    (ref_t)-16401
#define V_SET            (ref_t)-16402
//
//// obsolete
//#define V_MULTI          (ref_t)-16397

namespace _ELENA_
{

typedef Map<ident_t, ref_t> ForwardMap;

enum MethodHint
{
   tpMask        = 0x0000F,

   tpUnknown     = 0x00000,
   tpSealed      = 0x00001,
   tpClosed      = 0x00002,
   tpNormal      = 0x00003,
////      tpDispatcher = 0x04,
   tpPrivate     = 0x00005,
//   tpStackSafe   = 0x0010,
   tpEmbeddable  = 0x0020,
   tpGeneric     = 0x0040,
   tpAction      = 0x0080,
//   tpIfBranch    = 0x0100,
//   tpIfNotBranch = 0x0200,
   tpConstructor = 0x00400,
   tpConversion  = 0x00800,
   tpMultimethod = 0x01000,
   tpArgDispatcher = 0x3000,
   tpStatic      = 0x04000,
   tpAccessor    = 0x08000,
   tpSpecial     = 0x10000,
};

// --- _CompileScope ---

struct _CompilerScope
{
////   struct BranchingInfo
////   {
////      ref_t reference;
////      ref_t trueRef;
////      ref_t falseRef;
////
////      BranchingInfo()
////      {
////         reference = 0;
////         trueRef = falseRef = 0;
////      }
////   };
//
//   ident_t  sourcePath;
//   ref_t    sourcePathRef;

   _Module* module;
   _Module* debugModule;

   // cached references
   ref_t superReference;
   ref_t intReference;
   ref_t longReference;
   ref_t realReference;
//   ref_t signatureReference;
//   ref_t messageReference;
//   ref_t extMessageReference;
//   ref_t boolReference;
   ref_t literalReference;
   ref_t wideReference;
   ref_t charReference;
   ref_t arrayReference;

////   // cached bool values
////   BranchingInfo branchingInfo;
//
   virtual void raiseError(const char* message, ident_t sourcePath, SNode terminal) = 0;
   //virtual void raiseWarning(int level, const char* message, ident_t sourcePath, SNode terminal) = 0;

//   virtual ref_t mapAttribute(SNode terminal) = 0;
//   virtual ref_t mapTerminal(SNode terminal, bool existing = false) = 0;
////   virtual ref_t mapReference(ident_t reference, bool existing = false) = 0;
////   virtual ref_t mapTemplateClass(ident_t templateName, bool& alreadyDeclared) = 0;
////   virtual ref_t mapAnonymous() = 0;
//
//   virtual bool saveAttribute(ident_t name, ref_t attr/*, bool internalAttr*/) = 0;

   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;

   virtual _Module* loadReferenceModule(ident_t referenceName, ref_t& reference) = 0;
   virtual _Module* loadReferenceModule(ref_t reference, ref_t& moduleReference) = 0;

   virtual _Memory* mapSection(ref_t reference, bool existing) = 0;

////   virtual bool includeModule(ident_t name, bool& duplicateExtensions, bool& duplicateAttributes, bool& duplicateInclusion) = 0;
////
////   virtual void validateReference(SNode terminal, ref_t reference) = 0;
////
////   virtual void saveAutogerenatedExtension(ref_t attr, ref_t extension) = 0;
////   virtual SubjectList* getAutogerenatedExtensions(ref_t attr) = 0;

   _CompilerScope()
//      : attributes(0)
   {
////      sourcePath = NULL;
////      sourcePathRef = 0;
      debugModule = module = NULL;
      intReference = /*boolReference = */superReference = 0;
////      signatureReference = messageReference = 0;
      longReference = literalReference = wideReference = 0;
      arrayReference = charReference = realReference = 0;
////      extMessageReference = 0;
   }
};

// --- _Compiler ---

class _Compiler
{
public:
   virtual void injectBoxing(SyntaxWriter& writer, _CompilerScope& scope, LexicalType boxingType, int argument, ref_t targetClassRef, bool arrayMode = false) = 0;
   virtual void injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef, bool stacksafe) = 0;
//   virtual void injectFieldExpression(SyntaxWriter& writer) = 0;

//   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject) = 0;
//   virtual void injectEmbeddableOp(SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb) = 0;
//   virtual void injectEmbeddableConstructor(SNode classNode, ref_t message, ref_t privateRef) = 0;
   virtual void injectVirtualMultimethod(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType, ref_t parentRef = 0) = 0;
   virtual void injectVirtualArgDispatcher(_CompilerScope& scope, SNode classNode, ref_t message, LexicalType methodType) = 0;
   virtual void injectVirtualReturningMethod(_CompilerScope& scope, SNode classNode, ref_t message, ident_t variable) = 0;

   virtual void injectLocalBoxing(SNode node, int size) = 0;
//   //virtual int injectTempLocal(SNode node) = 0;

   virtual void injectVirtualStaticConstField(_CompilerScope& scope, SNode classNode, ident_t fieldName, ref_t fieldRef) = 0;

   virtual void generateListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef) = 0;
   virtual void generateOverloadListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef) = 0;
   virtual void generateClosedOverloadListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef) = 0;
   virtual void generateSealedOverloadListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef, ref_t classRef) = 0;

//   virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader) = 0;
};

// --- _CompilerLogic ---

class _CompilerLogic
{
public:
   struct ChechMethodInfo
   {
      bool  found;
      bool  directResolved;
      bool  withCustomDispatcher;
//      //bool  closed;
      bool  stackSafe;
      bool  embeddable;
      bool  withOpenArgDispatcher;
      bool  withOpenArg1Dispatcher;
      bool  withOpenArg2Dispatcher;
      bool  closure;
      ref_t outputReference;

      ChechMethodInfo()
      {
         directResolved = false;
         embeddable = /*closed = */found = false;
         outputReference = 0;
         withCustomDispatcher = false;
         stackSafe = false;
         withOpenArgDispatcher = false;
         withOpenArg1Dispatcher = false;
         withOpenArg2Dispatcher = false;
         closure = false;
      }
   };

   virtual int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, ChechMethodInfo& result) = 0;
   virtual int checkMethod(ClassInfo& info, ref_t message, ChechMethodInfo& result) = 0;

   // retrieve the class info / size
   virtual bool defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;

   virtual int defineStructSizeVariable(_CompilerScope& scope, ref_t reference, ref_t elementRef, bool& variable) = 0;
   virtual int defineStructSize(_CompilerScope& scope, ref_t reference, ref_t elementRef) = 0;
   virtual int defineStructSize(ClassInfo& info, bool& variable) = 0;

   virtual ref_t definePrimitiveArray(_CompilerScope& scope, ref_t elementRef) = 0;

   // retrieve the call type
   virtual int resolveCallType(_CompilerScope& scope, ref_t& classReference, ref_t message, ChechMethodInfo& result) = 0;

   // retrieve the operation type
   virtual int resolveOperationType(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t roperand, ref_t& result) = 0;
   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result) = 0;
   virtual int resolveNewOperationType(_CompilerScope& scope, ref_t loperand, ref_t roperand, ref_t& result) = 0;

//   // retrieve the branching operation type
//   virtual bool resolveBranchOperation(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t& reference) = 0;

   virtual ref_t resolvePrimitiveReference(_CompilerScope& scope, ref_t reference) = 0;
   virtual ref_t retrievePrimitiveReference(_CompilerScope& scope, ClassInfo& info) = 0;

   // check if the classes is compatible
   virtual bool isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef) = 0;

//   virtual bool isVariable(_CompilerScope& scope, ref_t targetRef) = 0;

   virtual bool isEmbeddableArray(ClassInfo& info) = 0;
//   virtual bool isVariable(ClassInfo& info) = 0;
   virtual bool isEmbeddable(ClassInfo& info) = 0;
   virtual bool isEmbeddable(_CompilerScope& scope, ref_t reference) = 0;
   virtual bool isMethodStacksafe(ClassInfo& info, ref_t message) = 0;
   virtual bool isMethodGeneric(ClassInfo& info, ref_t message) = 0;
   virtual bool isMultiMethod(ClassInfo& info, ref_t message) = 0;
   virtual bool isClosure(ClassInfo& info, ref_t message) = 0;

   // class is considered to be a role if it cannot be initiated
   virtual bool isRole(ClassInfo& info) = 0;          
   virtual bool isAbstract(ClassInfo& info) = 0;

//   virtual bool isPrimitiveRef(ref_t reference) = 0;
//   virtual bool isPrimitiveArray(ref_t reference) = 0;

   // auto generate virtual methods / fields
   virtual void injectVirtualCode(_CompilerScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler, bool closed) = 0;
   virtual void injectVirtualFields(_CompilerScope& scope, SNode node, ref_t classRef, ClassInfo& info, _Compiler& compiler) = 0;
   virtual void injectVirtualMultimethods(_CompilerScope& scope, SNode node, ClassInfo& info, _Compiler& compiler, List<ref_t>& implicitMultimethods, LexicalType methodType) = 0;
   virtual void verifyMultimethods(_CompilerScope& scope, SNode node, ClassInfo& info, List<ref_t>& implicitMultimethods) = 0;
   virtual void injectOperation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, int operatorId, int operation, ref_t& reference, ref_t elementRef) = 0;
   virtual bool injectImplicitConversion(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, ref_t elementRef) = 0;
//   virtual bool injectImplicitConstructor(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t actionRef, int paramCount) = 0;
   virtual bool injectImplicitCreation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef) = 0;
   virtual void injectNewOperation(SyntaxWriter& writer, _CompilerScope& scope, int operation, ref_t targetRef, ref_t elementRef) = 0;
////   virtual void injectVariableAssigning(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t& targetRef, ref_t& type, int& operand, bool paramMode) = 0;
//   virtual void injectOverloadList(_CompilerScope& scope, ClassInfo& info, _Compiler& compiler, ref_t classRef) = 0;

   // auto generate class flags
   virtual void tweakClassFlags(_CompilerScope& scope, _Compiler& compiler, ref_t classRef, ClassInfo& info, bool classClassMode) = 0;
   virtual bool tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info) = 0;

////   virtual bool validateClassFlag(ClassInfo& info, int flag) = 0;

   // attribute validations
   virtual bool validateClassAttribute(int& attrValue) = 0;
   virtual bool validateMethodAttribute(int& attrValue) = 0;
   virtual bool validateFieldAttribute(int& attrValue, bool& isSealed, bool& isConstant) = 0;
   virtual bool validateLocalAttribute(int& attrValue) = 0;
//   virtual bool validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne) = 0;
//////   virtual bool validateWarningAttribute(int& attrValue) = 0;
   virtual bool validateMessage(ref_t message, bool isClassClass) = 0;

   virtual bool isDefaultConstructorEnabled(ClassInfo& info) = 0;

   // optimization
   virtual bool validateBoxing(_CompilerScope& scope, _Compiler& compiler, SNode& node, ref_t targetRef, ref_t sourceRef, bool unboxingExpected) = 0;
//   virtual bool recognizeEmbeddableGet(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableGetAt(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableGetAt2(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableEval(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableEval2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningRef, ref_t& subject) = 0;
//   virtual bool recognizeEmbeddableIdle(SNode node, bool extensionOne) = 0;
//   virtual bool recognizeEmbeddableMessageCall(SNode node, ref_t& messageRef) = 0;
   virtual bool optimizeEmbeddable(SNode node, _CompilerScope& scope) = 0;

   virtual bool optimizeEmbeddableGet(_CompilerScope& scope, _Compiler& compiler, SNode node) = 0;
   virtual bool optimizeEmbeddableOp(_CompilerScope& scope, _Compiler& compiler, SNode node/*, int verb, int attribte, int paramCount*/) = 0;
   virtual void optimizeBranchingOp(_CompilerScope& scope, SNode node) = 0;

   virtual ref_t resolveMultimethod(_CompilerScope& scope, ref_t multiMessage, ref_t targetRef, ref_t implicitSignatureRef) = 0;
};

}  // _ELENA_

#endif // compilerCommonH
