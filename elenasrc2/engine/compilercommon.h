//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler common interfaces.
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef compilerCommonH
#define compilerCommonH

#include "elena.h"
#include "syntaxtree.h"

#define INVALID_REF      (ref_t)-1

// virtual objects
#define V_FLAG           (ref_t)-03
#define V_NIL            (ref_t)-04

#define V_BINARY         (ref_t)-10
#define V_INT32          (ref_t)-11
#define V_PTR32          (ref_t)-12
#define V_INT64          (ref_t)-13
#define V_REAL64         (ref_t)-14
#define V_SIGNATURE      (ref_t)-18
#define V_MESSAGE        (ref_t)-19
#define V_VERB           (ref_t)-20
#define V_EXTMESSAGE     (ref_t)-21
#define V_SYMBOL         (ref_t)-22

#define V_STRCONSTANT    (ref_t)-23 // used for explicit constant operations

#define V_OBJECT         (ref_t)-28

#define V_OBJARRAY      (ref_t)-30
#define V_INT32ARRAY    (ref_t)-31
#define V_ARGARRAY      (ref_t)-32
#define V_BINARYARRAY   (ref_t)-35
#define V_INT16ARRAY    (ref_t)-38
#define V_INT8ARRAY     (ref_t)-39

#define V_IFBRANCH      (ref_t)-4097
#define V_IFNOTBRANCH   (ref_t)-4098
//#define V_WARNING1    (ref_t)-4099
//#define V_WARNING2    (ref_t)-4100
//#define V_WARNING3    (ref_t)-4101

#define V_STATCKSAFE     (ref_t)-8192
#define V_EMBEDDABLE     (ref_t)-8193
#define V_STATIC         (ref_t)-8194
#define V_SEALED         (ref_t)-8195
#define V_LIMITED        (ref_t)-8196
#define V_STRUCT         (ref_t)-8197
#define V_ENUMLIST       (ref_t)-8198
#define V_DYNAMIC        (ref_t)-8199
#define V_STRING         (ref_t)-8200
#define V_CONST          (ref_t)-8201
#define V_GENERIC        (ref_t)-8202
#define V_EXTENSION      (ref_t)-8203
#define V_NOSTRUCT       (ref_t)-8204
#define V_ACTION         (ref_t)-8205
#define V_GROUP          (ref_t)-8206
#define V_PRELOADED      (ref_t)-8207
#define V_SINGLETON      (ref_t)-8208
#define V_TAPEGROUP      (ref_t)-8209

#define V_CONSTRUCTOR    (ref_t)-16384
#define V_VARIABLE       (ref_t)-16385
#define V_CLASS          (ref_t)-16386
#define V_CONVERSION     (ref_t)-16387
//#define V_EMBEDDABLETMPL (ref_t)-16388
#define V_SYMBOLEXPR     (ref_t)-16389
#define V_TYPETEMPL      (ref_t)-16390
#define V_TEMPLATE       (ref_t)-16391
#define V_FIELD          (ref_t)-16392
#define V_METHOD         (ref_t)-16393
#define V_LOOP           (ref_t)-16394
#define V_IMPORT         (ref_t)-16395
#define V_EXTERN         (ref_t)-16396

namespace _ELENA_
{

//typedef Map<ref_t, ref_t> ClassMap;

enum MethodHint
{
   tpMask        = 0x00F,

   tpUnknown     = 0x000,
   tpSealed      = 0x001,
   tpClosed      = 0x002,
   tpNormal      = 0x003,
//      tpDispatcher = 0x04,
   tpPrivate     = 0x005,
   tpStackSafe   = 0x010,
   tpEmbeddable  = 0x020,
   tpGeneric     = 0x040,
   tpAction      = 0x080,
   tpIfBranch    = 0x100,
   tpIfNotBranch = 0x200,
   tpConstructor = 0x400,
   tpConversion  = 0x800
};

enum DeclarationAttr
{
   daNone     = 0x00,
   daType     = 0x01,
   daClass    = 0x02,
   daTemplate = 0x04,
   daField    = 0x08,
   daMethod   = 0x10, 
   daLoop     = 0x20,
   daImport   = 0x40,
   daExtern   = 0x80
};

// --- _CompileScope ---

struct _CompilerScope
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

   _Module* module;

//   // cached references
   ref_t superReference;
   ref_t intReference;
   ref_t longReference;
   ref_t realReference;
   ref_t signatureReference;
   ref_t messageReference;
   ref_t verbReference;
   ref_t boolReference;
   ref_t literalReference;
   ref_t wideReference;
   ref_t charReference;
   ref_t arrayReference;
   ref_t paramsSubj;

   // list of typified classes which may need get&type message
   SubjectMap  subjectHints;

   // cached bool values
   BranchingInfo branchingInfo;

   virtual ref_t loadClassInfo(ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;
   virtual _Module* loadReferenceModule(ref_t& reference) = 0;

   _CompilerScope()
   {
      module = NULL;
      intReference = boolReference = superReference = 0;
      signatureReference = verbReference = messageReference = 0;
      longReference = literalReference = wideReference = 0;
      arrayReference = charReference = realReference = 0;
      paramsSubj = 0;
   }
};

// --- _Compiler ---

class _Compiler
{
public:
////   virtual void injectVirtualReturningMethod(SyntaxWriter& writer, ref_t messagRef, LexicalType type, int argument) = 0;
   virtual void injectBoxing(SyntaxWriter& writer, _CompilerScope& scope, LexicalType boxingType, int argument, ref_t targetClassRef) = 0;
   virtual void injectConverting(SyntaxWriter& writer, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef, bool stacksafe) = 0;
   virtual void injectFieldExpression(SyntaxWriter& writer) = 0;

   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject) = 0;
   virtual void injectEmbeddableOp(SNode assignNode, SNode callNode, ref_t subject, int paramCount, int verb) = 0;

   virtual void injectLocalBoxing(SNode node, int size) = 0;
//   //virtual int injectTempLocal(SNode node) = 0;

   virtual void generateEnumListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef) = 0;

   virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader) = 0;
};

// --- _CompilerLogic ---

class _CompilerLogic
{
public:
   struct ChechMethodInfo
   {
      bool  found;
      bool  withCustomDispatcher;
      bool  closed;
      bool  stackSafe;
      bool  embeddable;
      bool  withOpenArgDispatcher;
      ref_t outputReference;

      ChechMethodInfo()
      {
         embeddable = closed = found = false;
         outputReference = 0;
         withCustomDispatcher = false;
         stackSafe = false;
         withOpenArgDispatcher = false;
      }
   };

   virtual int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, ChechMethodInfo& result) = 0;

   // retrieve the class info / size
   virtual bool defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;
   virtual int defineStructSize(_CompilerScope& scope, ref_t reference, ref_t type = 0, bool embeddableOnly = false) = 0;
   virtual int defineStructSize(ClassInfo& info, bool embeddableOnly = false) = 0;

   virtual ref_t definePrimitiveArray(_CompilerScope& scope, ref_t elementRef) = 0;

   // retrieve the call type
   virtual int resolveCallType(_CompilerScope& scope, ref_t& classReference, ref_t message, ChechMethodInfo& result) = 0;

   // retrieve the operation type
   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t& result) = 0;
   virtual int resolveOperationType(_CompilerScope& scope, int operatorId, ref_t loperand, ref_t roperand, ref_t roperand2, ref_t& result) = 0;
   virtual int resolveNewOperationType(_CompilerScope& scope, ref_t loperand, ref_t roperand, ref_t& result) = 0;

   // retrieve the branching operation type
   virtual bool resolveBranchOperation(_CompilerScope& scope, _Compiler& compiler, int operatorId, ref_t loperand, ref_t& reference) = 0;

   virtual ref_t resolvePrimitiveReference(_CompilerScope& scope, ref_t reference) = 0;
   virtual ref_t retrievePrimitiveReference(_CompilerScope& scope, ClassInfo& info) = 0;

   // check if the classes is compatible
   virtual bool isCompatible(_CompilerScope& scope, ref_t targetRef, ref_t sourceRef) = 0;
   virtual bool isCompatibleWithType(_CompilerScope& scope, ref_t targetRef, ref_t type) = 0;

   virtual bool isVariable(_CompilerScope& scope, ref_t targetRef) = 0;

   virtual bool isEmbeddableArray(ClassInfo& info) = 0;
   virtual bool isVariable(ClassInfo& info) = 0;
   virtual bool isEmbeddable(ClassInfo& info) = 0;
   virtual bool isEmbeddable(_CompilerScope& scope, ref_t reference) = 0;
   virtual bool isMethodStacksafe(ClassInfo& info, ref_t message) = 0;
   virtual bool isMethodGeneric(ClassInfo& info, ref_t message) = 0;

   // class is considered to be a role if it cannot be initiated
   virtual bool isRole(ClassInfo& info) = 0;          

   virtual bool isPrimitiveRef(ref_t reference) = 0;
//   virtual bool isPrimitiveArray(ref_t reference) = 0;
//
//   // auto generate virtual methods / fields
   virtual void injectVirtualCode(_CompilerScope& scope, ref_t classRef, ClassInfo& info, _Compiler& compiler) = 0;
   virtual void injectOperation(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, int operatorId, int operation, ref_t& reference, ref_t type) = 0;
   virtual bool injectImplicitConversion(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, ref_t sourceType) = 0;
   virtual void injectNewOperation(SyntaxWriter& writer, _CompilerScope& scope, int operation, ref_t elementType, ref_t targetRef) = 0;
//   virtual void injectVariableAssigning(SyntaxWriter& writer, _CompilerScope& scope, _Compiler& compiler, ref_t& targetRef, ref_t& type, int& operand, bool paramMode) = 0;

   // auto generate class flags
   virtual void tweakClassFlags(_CompilerScope& scope, ref_t classRef, ClassInfo& info, bool classClassMode) = 0;
   virtual bool tweakPrimitiveClassFlags(ref_t classRef, ClassInfo& info) = 0;

////   virtual bool validateClassFlag(ClassInfo& info, int flag) = 0;

   // attribute validations
   virtual bool validateClassAttribute(int& attrValue) = 0;
   virtual bool validateMethodAttribute(int& attrValue) = 0;
   virtual bool validateFieldAttribute(int& attrValue) = 0;
   virtual bool validateLocalAttribute(int& attrValue) = 0;
   virtual bool validateSymbolAttribute(int attrValue, bool& constant, bool& staticOne, bool& preloadedOne) = 0;
   virtual bool validateDeclarationAttribute(int attrValue, DeclarationAttr& declType) = 0;
//   virtual bool validateWarningAttribute(int& attrValue) = 0;
   virtual bool validateMessage(ref_t message, bool isClassClass) = 0;

   virtual bool isDefaultConstructorEnabled(ClassInfo& info) = 0;

   virtual ref_t defineOperatorMessage(_CompilerScope& scope, ref_t operatorId, int paramCount, ref_t loperand, ref_t roperand, ref_t roperand2) = 0;

   // optimization
   virtual bool validateBoxing(_CompilerScope& scope, _Compiler& compiler, SNode& node, ref_t targetRef, ref_t sourceRef, bool assingingMode) = 0;
   virtual bool recognizeEmbeddableGet(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject) = 0;
   virtual bool recognizeEmbeddableGetAt(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject) = 0;
   virtual bool recognizeEmbeddableGetAt2(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject) = 0;
   virtual bool recognizeEmbeddableEval(_CompilerScope& scope, SNode node, ref_t extensionRef, ref_t returningType, ref_t& subject) = 0;
   virtual bool recognizeEmbeddableEval2(_CompilerScope& scope, SNode root, ref_t extensionRef, ref_t returningType, ref_t& subject) = 0;
   virtual bool recognizeEmbeddableIdle(SNode node, bool extensionOne) = 0;
   virtual bool optimizeEmbeddable(SNode node, _CompilerScope& scope) = 0;

////   virtual void optimizeDuplicateBoxing(SNode node) = 0;

   virtual bool optimizeEmbeddableGet(_CompilerScope& scope, _Compiler& compiler, SNode node) = 0;
   virtual bool optimizeEmbeddableOp(_CompilerScope& scope, _Compiler& compiler, SNode node/*, int verb, int attribte, int paramCount*/) = 0;

////   virtual bool recognizeNestedScope(SNode& node) = 0;
////   virtual bool recognizeScope(SNode& node) = 0;
////   virtual bool recognizeNewLocal(SNode& node) = 0;
////   virtual bool recognizeNewField(SNode& node) = 0;
};
   
}  // _ELENA_

#endif // compilerCommonH
