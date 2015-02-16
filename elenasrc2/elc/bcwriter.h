//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code writer class.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef bcwriterH
#define bcwriterH 1

#include "bytecode.h"

namespace _ELENA_
{

enum ObjectKind
{
   okUnknown = 0,

   okExternal,
   okInternal,

   okConstant,                     // param - reference, extraparam - class reference
   okLiteralConstant,              // param - reference 
   okCharConstant,                 // param - reference
   okIntConstant,                  // param - reference 
   okLongConstant,                 // param - reference 
   okRealConstant,                 // param - reference 
   okConstantSymbol,               // param - reference 
   okConstantClass,                // param - reference, extraparam - class reference

   okVerbConstant,
   okSignatureConstant,
   okMessageConstant,

   okSymbol,                       // param - reference
//   okSymbolReference,              // param - reference
   okRole,
   okConstantRole,                 // param - role reference
   okField,                         // param - field offset
   okFieldAddress,                  // param - field offset, extraparam - type reference
   okOuter,                         // param - field offset, extraparam - type reference
   okOuterField,                    // param - field offset, extraparam - outer field offset
   okLocal,                         // param - local / out parameter offset, extraparam - type reference
   okParam,                         // param - parameter offset, extraparam - type reference
   okThisParam,                     // param - parameter offset, extraparam - type reference
   okOutputParam,                   // param - target address, extraparam - type reference 
   okSuper,
   okLocalAddress,                  // param - local offset, extraparam - class reference
   okParams,                        // param - local offset
   okBlockLocal,                    // param - local offset, extraparam - type reference    
   okCurrent,                       // param - stack offset
   okAccumulator,                   // param - type class reference, extraparam - type reference
   okAccField,                      // param - field offset
//   okBlockLocalAddress,
////   okBlockOuterField,   
   okIndexAccumulator,
   okExtraRegister,
   okBase,
   okSubject,

   okIdle
};

struct ObjectInfo
{
   ObjectKind kind;
   ref_t      param;
   ref_t      extraparam;

   ObjectInfo()
   {
      this->kind = okUnknown;
      this->param = 0;
      this->extraparam = 0;
   }
   ObjectInfo(ObjectKind kind)
   {
      this->kind = kind;
      this->param = 0;
      this->extraparam = 0;
   }
   ObjectInfo(ObjectKind kind, ObjectInfo copy)
   {
      this->kind = kind;
      this->param = copy.param;
      this->extraparam = copy.extraparam;
   }
   ObjectInfo(ObjectKind kind, ref_t param)
   {
      this->kind = kind;
      this->param = param;
      this->extraparam = 0;
   }
   ObjectInfo(ObjectKind kind, ref_t param, ref_t extraparam)
   {
      this->kind = kind;
      this->param = param;
      this->extraparam = extraparam;
   }
};

// --- ByteCodeWriter class ---
class ByteCodeWriter
{
   struct Scope
   {
      ref_t         sourceRef;
      MemoryWriter* vmt;
      MemoryWriter* code;
      MemoryWriter* debug;
      MemoryWriter* debugStrings;

      Scope()
      {
         vmt = code = NULL;
         debug = debugStrings = NULL;
         sourceRef = 0;
      }
   };

   ByteCode peekNext(ByteCodeIterator it)
   {
      it++;

      return (*it).code;
   }

   ByteCode peekPrevious(ByteCodeIterator it)
   {
      it--;

      return (*it).code;
   }

   void writeNewStatement(MemoryWriter* debug);
   void writeNewBlock(MemoryWriter* debug);
   void writeSelf(Scope& scope, int level, int frameLevel);
   void writeLocal(Scope& scope, const wchar16_t* localName, int level, int frameLevel);
   void writeLocal(Scope& scope, const wchar16_t* localName, int level, DebugSymbol symbol, int frameLevel);
   void writeMessageInfo(Scope& scope, DebugSymbol symbol, ref_t nameRef);
   void writeBreakpoint(ByteCodeIterator& it, MemoryWriter* debug);

   void writeFieldDebugInfo(ClassInfo& info, MemoryWriter* writer, MemoryWriter* debugStrings);
   void writeClassDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, const wchar16_t* className, int flags);
   void writeSymbolDebugInfo(_Module* debugModule, MemoryWriter* debug, MemoryWriter* debugStrings, const wchar16_t* symbolName);
   void writeProcedureDebugInfo(MemoryWriter* writer, ref_t sourceNameRef);
   void writeDebugInfoStopper(MemoryWriter* debug);

   void compileProcedure(ByteCodeIterator& it, Scope& scope);
   void compileVMT(size_t classPosition, ByteCodeIterator& it, Scope& scope);
//   void writeAction(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule);
   void compileSymbol(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef);
//   void writeClassHandler(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule);
   void compileClass(ref_t reference, ByteCodeIterator& it, _Module* module, _Module* debugModule, ref_t sourceRef);

public:
   ref_t writeSourcePath(_Module* debugModule, const tchar_t* path);
   ref_t writeMessage(_Module* debugModule, _Module* module, MessageMap& verbs, ref_t message);

   void declareClass(CommandTape& tape, ref_t reference);
   void declareSymbol(CommandTape& tape, ref_t reference);
   void declareStaticSymbol(CommandTape& tape, ref_t staticReference);
   void declareIdleMethod(CommandTape& tape, ref_t message);
   void declareMethod(CommandTape& tape, ref_t message, bool withPresavedMessage, bool withNewFrame = true);
   void declareExternalBlock(CommandTape& tape);
   void declareVariable(CommandTape& tape, ref_t nilReference);
   void declarePrimitiveVariable(CommandTape& tape, int value);
   void declareArgumentList(CommandTape& tape, int count);
   int declareLoop(CommandTape& tape/*, bool threadFriendly*/);  // thread friendly means the loop contains safe point
   void declareThenBlock(CommandTape& tape, bool withStackControl = true);
   void declareThenElseBlock(CommandTape& tape);
   void declareElseBlock(CommandTape& tape);
   void declareSwitchBlock(CommandTape& tape);
   void declareSwitchOption(CommandTape& tape);
   void declareTry(CommandTape& tape);
   void declareCatch(CommandTape& tape);
   void declareAlt(CommandTape& tape);
   void declarePrimitiveCatch(CommandTape& tape);

   void declareLocalInfo(CommandTape& tape, const wchar16_t* localName, int level);
   void declareLocalIntInfo(CommandTape& tape, const wchar16_t* localName, int level);
   void declareLocalLongInfo(CommandTape& tape, const wchar16_t* localName, int level);
   void declareLocalRealInfo(CommandTape& tape, const wchar16_t* localName, int level);
   void declareLocalByteArrayInfo(CommandTape& tape, const wchar16_t* localName, int level);
   void declareLocalShortArrayInfo(CommandTape& tape, const wchar16_t* localName, int level);
   void declareLocalParamsInfo(CommandTape& tape, const wchar16_t* localName, int level);
   void declareSelfInfo(CommandTape& tape, int level);
   void declareMessageInfo(CommandTape& tape, ref_t nameRef);
   void declareBreakpoint(CommandTape& tape, int row, int disp, int length, int stepType);
   void declareStatement(CommandTape& tape);
   void declareBlock(CommandTape& tape);

   void newFrame(CommandTape& tape);
   void newStructure(CommandTape& tape, int size, ref_t reference);
   void newDynamicStructure(CommandTape& tape);
   void newDynamicWStructure(CommandTape& tape);
   void newDynamicNStructure(CommandTape& tape);

   void newObject(CommandTape& tape, int fieldCount, ref_t reference);
   void newDynamicObject(CommandTape& tape);

   void loadObject(CommandTape& tape, ObjectInfo object);
   void pushObject(CommandTape& tape, ObjectInfo object);
   void saveObject(CommandTape& tape, ObjectInfo object);
   void popObject(CommandTape& tape, ObjectInfo object);

   void loadBase(CommandTape& tape, ObjectInfo object);
   void initBase(CommandTape& tape, int fieldCount);
   void saveBase(CommandTape& tape, ObjectInfo object, int fieldOffset);

   void boxObject(CommandTape& tape, int size, ref_t vmtReference, bool alwaysBoxing = false);
   void boxArgList(CommandTape& tape, ref_t vmtReference);
   void unboxArgList(CommandTape& tape);

   void releaseObject(CommandTape& tape, int count = 1);
   void releaseArgList(CommandTape& tape);

   void setMessage(CommandTape& tape, ref_t message);
   void setSubject(CommandTape& tape, ref_t subject);

   void callMethod(CommandTape& tape, int vmtOffset, int paramCount);
   void callRoleMessage(CommandTape& tape, int paramCount);
   void callResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);
   void callVMTResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);
   void typecast(CommandTape& tape);

   void doGenericHandler(CommandTape& tape);
   void resend(CommandTape& tape);
   void resend(CommandTape& tape, ObjectInfo object, int dispatchIndex = 0);
   void resendResolvedMethod(CommandTape& tape, ref_t reference, ref_t message);
   void callExternal(CommandTape& tape, ref_t functionReference, int paramCount);

//   int declareLabel(CommandTape& tape);
   void jumpIfEqual(CommandTape& tape, ref_t ref);
   void jumpIfNotEqual(CommandTape& tape, ref_t ref);
//   void jumpIfNotEqualN(CommandTape& tape, int value);
   void jump(CommandTape& tape, bool previousLabel = false);

   void throwCurrent(CommandTape& tape);

   void gotoEnd(CommandTape& tape, PseudoArg label);

   void selectConstant(CommandTape& tape, ref_t r1, ref_t r2);

   void freeVirtualStack(CommandTape& tape, int count);

   void insertStackAlloc(ByteCodeIterator it, CommandTape& tape, int size);
   void updateStackAlloc(ByteCodeIterator it, CommandTape& tape, int size);

//   void setLabel(CommandTape& tape);
   void endCatch(CommandTape& tape);
   void endAlt(CommandTape& tape);
   void endPrimitiveCatch(CommandTape& tape);
   void endThenBlock(CommandTape& tape, bool withStackContro = true);
   void endLoop(CommandTape& tape);
   void endLoop(CommandTape& tape, ref_t comparingRef);
   void endExternalBlock(CommandTape& tape);
   void exitMethod(CommandTape& tape, int count, int reserved, bool withFrame = true);
   void endMethod(CommandTape& tape, int paramCount, int reserved, bool withFrame = true);
   void endIdleMethod(CommandTape& tape);
   void endClass(CommandTape& tape);
   void endSymbol(CommandTape& tape);
   void exitStaticSymbol(CommandTape& tape, ref_t staticReference);
   void endSwitchOption(CommandTape& tape);
   void endSwitchBlock(CommandTape& tape);

   void assignInt(CommandTape& tape, ObjectInfo target);
   void assignLong(CommandTape& tape, ObjectInfo target);
   void assignShort(CommandTape& tape, ObjectInfo target);
   void assignByte(CommandTape& tape, ObjectInfo target);
   void saveInt(CommandTape& tape, ObjectInfo target);
   void copyInt(CommandTape& tape, int offset);
   void copyShort(CommandTape& tape, int offset);
   void copyStructure(CommandTape& tape, int offset, int size);
   void copySubject(CommandTape& tape);
   void loadSymbolReference(CommandTape& tape, ref_t reference);
   void saveIntConstant(CommandTape& tape, int value);
   void invertBool(CommandTape& tape, ref_t trueRef, ref_t falseRef);
   void doIntOperation(CommandTape& tape, int operator_id);
   void doLongOperation(CommandTape& tape, int operator_id);
   void doRealOperation(CommandTape& tape, int operator_id);
   void doLiteralOperation(CommandTape& tape, int operator_id);
   void doArrayOperation(CommandTape& tape, int operator_id);

   void compile(CommandTape& tape, _Module* module, _Module* debugModule, ref_t sourceRef);
};

} // _ELENA_

#endif // bcwriterH
