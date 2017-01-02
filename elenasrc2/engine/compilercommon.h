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

// virtual objects
#define V_FLAG        (size_t)-03
#define V_NIL         (size_t)-04

#define V_BINARY      (size_t)-10
#define V_INT32       (size_t)-11
#define V_PTR32       (size_t)-12
#define V_INT64       (size_t)-13
#define V_REAL64      (size_t)-14
#define V_SIGNATURE   (size_t)-18
#define V_MESSAGE     (size_t)-19
#define V_VERB        (size_t)-20
#define V_EXTMESSAGE  (size_t)-21
#define V_SYMBOL      (size_t)-22

#define V_OBJECT      (size_t)-28

#define V_OBJARRAY    (size_t)-30
#define V_INT32ARRAY  (size_t)-31
#define V_ARGARRAY    (size_t)-32
#define V_BINARYARRAY (size_t)-35
#define V_INT16ARRAY  (size_t)-38
#define V_INT8ARRAY   (size_t)-39

#define V_IFBRANCH    (size_t)-4097
#define V_IFNOTBRANCH (size_t)-4098
#define V_WARNING1    (size_t)-4099
#define V_WARNING2    (size_t)-4100
#define V_WARNING3    (size_t)-4101

#define V_STATCKSAFE  (size_t)-8192
#define V_EMBEDDABLE  (size_t)-8193
#define V_STATIC      (size_t)-8194
#define V_SEALED      (size_t)-8195
#define V_LIMITED     (size_t)-8196
#define V_STRUCT      (size_t)-8197
#define V_ENUMLIST    (size_t)-8198
#define V_DYNAMIC     (size_t)-8199
#define V_STRING      (size_t)-8200
#define V_CONST       (size_t)-8201
#define V_GENERIC     (size_t)-8202
#define V_EXTENSION   (size_t)-8203
#define V_NOSTRUCT    (size_t)-8204
#define V_ACTION      (size_t)-8205
#define V_GROUP       (size_t)-8206
#define V_PRELOADED   (size_t)-8207

namespace _ELENA_
{

typedef Map<ref_t, ref_t> ClassMap;

enum MethodHint
{
   tpMask        = 0x0F,

   tpUnknown     = 0x00,
   tpSealed      = 0x01,
   tpClosed      = 0x02,
   tpNormal      = 0x03,
//      tpDispatcher = 0x04,
   tpPrivate     = 0x05,
   tpStackSafe   = 0x10,
   tpEmbeddable  = 0x20,
   tpGeneric     = 0x40,
   tpAction      = 0x80,
   tpIfBranch    = 0x100,
   tpIfNotBranch = 0x200,
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

   // cached references
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
   ref_t paramsReference;

   // list of typified classes which may need get&type message
   ClassMap    typifiedClasses;
   SubjectMap  attributeHints;

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
      paramsReference = 0;
   }
};

// --- _Compiler ---

class _Compiler
{
public:
   virtual void injectVirtualReturningMethod(SNode node, ident_t variable) = 0;
   virtual void injectBoxing(_CompilerScope& scope, SNode node, LexicalType boxingType, int argument, ref_t targetClassRef) = 0;
   virtual void injectConverting(SNode node, LexicalType convertOp, int convertArg, LexicalType createOp, int createArg, ref_t targetClassRef) = 0;
   virtual void injectFieldExpression(SNode node) = 0;

   virtual void injectEmbeddableGet(SNode assignNode, SNode callNode, ref_t subject) = 0;

   virtual void injectLocalBoxing(SNode node, int size) = 0;

   virtual void generateEnumListMember(_CompilerScope& scope, ref_t enumRef, ref_t memberRef) = 0;

   virtual ref_t readEnumListMember(_CompilerScope& scope, _Module* extModule, MemoryReader& reader) = 0;
};

// --- _CompilerLogic ---

class _CompilerLogic
{
public:
   virtual int checkMethod(_CompilerScope& scope, ref_t reference, ref_t message, bool& found, ref_t& outputType) = 0;

   // retrieve the class info / size
   virtual bool defineClassInfo(_CompilerScope& scope, ClassInfo& info, ref_t reference, bool headerOnly = false) = 0;
   virtual int defineStructSize(_CompilerScope& scope, ref_t reference, ref_t type = 0, bool embeddableOnly = false) = 0;
   virtual int defineStructSize(ClassInfo& info, bool embeddableOnly = false) = 0;

   virtual ref_t definePrimitiveArray(_CompilerScope& scope, ref_t elementRef) = 0;

   // retrieve the call type
   virtual int resolveCallType(_CompilerScope& scope, ref_t& classReference, ref_t message, bool& classFound, ref_t& outputType) = 0;

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

   // auto generate virtual methods / fields
   virtual void injectVirtualCode(SNode node, _CompilerScope& scope, ClassInfo& info, _Compiler& compiler) = 0;
   virtual void injectOperation(SNode node, _CompilerScope& scope, _Compiler& compiler, int operatorId, int operation, ref_t& reference, ref_t type) = 0;
   virtual bool injectImplicitConversion(SNode node, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t sourceRef, ref_t sourceType) = 0;
   virtual void injectNewOperation(SNode node, _CompilerScope& scope, int operation, ref_t elementType, ref_t targetRef) = 0;
   virtual void injectVariableAssigning(SNode node, _CompilerScope& scope, _Compiler& compiler, ref_t targetRef, ref_t& type, bool paramMode) = 0;

   // auto generate class flags
   virtual void tweakClassFlags(_CompilerScope& scope, ref_t classRef, ClassInfo& info) = 0;
   virtual bool tweakPrimitiveClassFlags(LexicalType attr, ClassInfo& info) = 0;

   virtual bool validateClassFlag(ClassInfo& info, int flag) = 0;

   // attribute validations
   virtual bool validateClassAttribute(int& attrValue) = 0;
   virtual bool validateMethodAttribute(int& attrValue) = 0;
   virtual bool validateFieldAttribute(int& attrValue) = 0;
   virtual bool validateLocalAttribute(int& attrValue) = 0;
   virtual bool validateSymbolAttribute(int& attrValue) = 0;
   virtual bool validateWarningAttribute(int& attrValue) = 0;

   virtual bool isDefaultConstructorEnabled(ClassInfo& info) = 0;

   // optimization
   virtual void optimizeEmbeddableBoxing(_CompilerScope& scope, _Compiler& compiler, SNode node, ref_t targetRef, bool assingingMode) = 0;
   virtual bool recognizeEmbeddableGet(_CompilerScope& scope, SNode node, ref_t returningType, ref_t& subject) = 0;
   virtual bool recognizeEmbeddableIdle(SNode node) = 0;

   virtual bool optimizeEmbeddableGet(_CompilerScope& scope, _Compiler& compiler, SNode node) = 0;
};
   
}  // _ELENA_

#endif // compilerCommonH
