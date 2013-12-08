//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA compiler class implementation.
//
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "compiler.h"
#include "errors.h"
#include <errno.h>

using namespace _ELENA_;

// --- Mode constants ---
#define CTRL_MASK             0xFFC00000
#define CTRL_ROOT             0x80000000
#define CTRL_LOOP             0x20000000
#define CTRL_TRY_MODE         0x10000000
#define CTRL_CATCH_MODE       0x08000000
#define CTRL_TYPED_MODE       0x04000000
#define CTRL_PREV_EXIT_MODE   0x02000000     // used for if-else statement to indicate that the exit label is not the last one
#define CTRL_DIRECT_PARAM     0x01000000     // indictates that the parameter should be stored directly in reverse order
#define CTRL_GETPROP_MODE     0x00800000     // used in GET PROPERTY expression
#define CTRL_TYPEDOUT_MODE    0x00400000

// --- Method optimization masks ---
#define MTH_FRAME_USED        0x00000001
#define MTH_OPENARG_USED      0x00000002

// --- Auxiliary routines ---

inline bool isCollection(DNode node)
{
   return (node == nsExpression && node.nextNode()==nsExpression);
}

inline ref_t importMessage(_Module* exporter, ref_t exportRef, _Module* importer)
{
   int verbId = 0;
   ref_t signRef = 0;
   int paramCount = 0;

   decodeMessage(exportRef, signRef, verbId, paramCount);

   // if it is generic message
   if (signRef == 0) {
      return exportRef;
   }

   // otherwise signature and custom verb should be imported
   if (signRef != 0) {
      const wchar16_t* subject = exporter->resolveSubject(signRef);

      signRef = importer->mapSubject(subject, false);
   }
   return encodeMessage(signRef, verbId, paramCount);
}

inline ref_t importReference(_Module* exporter, ref_t exportRef, _Module* importer)
{
   const wchar16_t* reference = exporter->resolveReference(exportRef);

   return importer->mapReference(reference);
}

inline void findUninqueName(_Module* module, ReferenceNs& name)
{
   size_t pos = getlength(name);
   int   index = 0;
   ref_t ref = 0;
   do {
      name[pos] = 0;
      name.appendHex(index++);

      ref = module->mapReference(name, true);
   } while (ref != 0);
}

// skip the hints and return the first hint node or none
inline DNode skipHints(DNode& node)
{
   DNode hints;
   if (node == nsHint)
      hints = node;

   while (node == nsHint)
      node = node.nextNode();

   return hints;
}

inline int countSymbol(DNode node, Symbol symbol)
{
   int counter = 0;
   while (node != nsNone) {
      if (node == symbol)
         counter++;

      node = node.nextNode();
   }
   return counter;
}

inline bool findSymbol(DNode node, Symbol symbol)
{
   while (node != nsNone) {
      if (node==symbol)
         return true;

      node = node.nextNode();
   }
   return false;
}

inline DNode goToSymbol(DNode node, Symbol symbol)
{
   while (node != nsNone) {
      if (node==symbol)
         return node;

      node = node.nextNode();
   }
   return node;
}

inline bool IsPrimitiveArray(ObjectType type)
{
   switch(type) {
      case otLiteral:
      case otByteArray:
      case otParams:
         return true;
      default:
         return false;
   }
}

inline bool IsPrimitiveOperand(ObjectType type)
{
   switch(type) {
      case otRef:
      case otShort:
      case otInt:
      case otIntVar:
      case otByte:
      case otLong:
      case otLongVar:
      case otReal:
      case otRealVar:
      case otArray:
      case otLiteral:
      case otIndex:
      case otLength:
      case otByteArray:
      case otParams:
         return true;
      default:
         return false;
   }
}

inline bool IsVarOperand(ObjectInfo operand)
{
   if (operand.kind == okField) {
      return true;
   }
   else if (operand.kind == okLocal) {
      switch(operand.type) {
         case otIntVar:
         case otLongVar:
         case otRealVar:
            return true;
         default:
            return false;
      }
   }
   else return false;
}

inline bool IsPrimitiveDataOperand(ObjectType type)
{
   switch(type) {
      case otRef:
      case otShort:
      case otInt:
      case otByte:
      case otLong:
      case otReal:
      case otIndex:
      case otLength:
         return true;
      default:
         return false;
   }
}

inline bool IsExprOperator(int operator_id)
{
   switch(operator_id) {
      case ADD_MESSAGE_ID:
      case SUB_MESSAGE_ID:
      case MUL_MESSAGE_ID:
      case DIV_MESSAGE_ID:
      case AND_MESSAGE_ID:
      case OR_MESSAGE_ID:
      case XOR_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsVarOperator(int operator_id)
{
   switch(operator_id) {
      case WRITE_MESSAGE_ID:
      case APPEND_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
      case SEPARATE_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsBranchOperator(int operator_id)
{
   switch(operator_id) {
      case EQUAL_MESSAGE_ID:
      case LESS_MESSAGE_ID:
      case NOTEQUAL_MESSAGE_ID:
      case NOTLESS_MESSAGE_ID:
      case GREATER_MESSAGE_ID:
      case NOTGREATER_MESSAGE_ID:
         return true;
      default:
         return false;
   }
}

inline bool IsPrimitiveOperation(int operator_id, ObjectInfo loperand, ObjectInfo roperand, int mode)
{
   switch (operator_id) {
      case ADD_MESSAGE_ID:
      case SUB_MESSAGE_ID:
      case MUL_MESSAGE_ID:
      case DIV_MESSAGE_ID:
      case AND_MESSAGE_ID:
      case OR_MESSAGE_ID:
      case XOR_MESSAGE_ID:
         return (IsPrimitiveDataOperand(loperand.type)) && (IsPrimitiveDataOperand(roperand.type) && 
            (test(mode, CTRL_TYPEDOUT_MODE) || test(mode, CTRL_TYPED_MODE)));
      case EQUAL_MESSAGE_ID:
      case LESS_MESSAGE_ID:
      case NOTEQUAL_MESSAGE_ID:
      case NOTLESS_MESSAGE_ID:
      case GREATER_MESSAGE_ID:
      case NOTGREATER_MESSAGE_ID:
      case REFER_MESSAGE_ID:
      case SET_REFER_MESSAGE_ID:
         return (IsPrimitiveOperand(loperand.type)) && (IsPrimitiveOperand(roperand.type));
      case WRITE_MESSAGE_ID:
      case APPEND_MESSAGE_ID:
      case REDUCE_MESSAGE_ID:
      case INCREASE_MESSAGE_ID:
      case SEPARATE_MESSAGE_ID:
         return (IsVarOperand(loperand) && IsPrimitiveOperand(loperand.type)) && (IsPrimitiveOperand(roperand.type));
      default:
         return false;
   }   
}

inline int definePrimitiveExprPrefix(ObjectType type)
{
   switch(type) {
      case otInt:
      case otIntVar:
      case otIndex:
         return CTRL_TYPED_MODE | TypeCode::tInt;
      case otLong:
      case otLongVar:
         return CTRL_TYPED_MODE | TypeCode::tLong;
      case otReal:
      case otRealVar:
         return CTRL_TYPED_MODE | TypeCode::tReal;
      case otLength:
         return CTRL_TYPED_MODE | TypeCode::tLen;
      case otShort:
         return CTRL_TYPED_MODE | TypeCode::tShort;
      case otLiteral:
         return CTRL_TYPED_MODE | TypeCode::tStr;
      case otByteArray:
         return CTRL_TYPED_MODE | TypeCode::tBytes;
      case otParams:
         return CTRL_TYPED_MODE | TypeCode::tParams;
      default:
         return 0;
   }
}

inline int definePrimitiveExprPrefix(ObjectType type, int mode)
{
   if (!test(mode, CTRL_TYPED_MODE)) {
      return (mode & CTRL_MASK) | definePrimitiveExprPrefix(type);
   }
   else return mode;
}

inline int getTypedOutMode(int mode)
{
   if (test(mode, CTRL_TYPEDOUT_MODE)) {
      return mode & CTRL_TYPEDOUT_MODE | (mode & ~CTRL_MASK);
   }
   else return 0;   
}

inline ObjectType ModeToType(int mode)
{  
   int type = mode & ~CTRL_MASK;
   switch(type) {
      case TypeCode::tInt:
         return otInt;
      case TypeCode::tLong:
         return otLong;
      case TypeCode::tReal:
         return otReal;
      case TypeCode::tShort:
         return otShort;
      case TypeCode::tLen:
         return otLength;
      case TypeCode::tStr:
         return otLiteral;
      default:
         if (mode >= TypeCode::tBytes) {
            return otByteArray;
         }
         else return otNone;
   }
}

inline TypeCode MapTypeCode(ObjectType type)
{  
   switch(type) {
      case otInt:
      case otIndex:
      case otIntVar:
      case otLength:
         return TypeCode::tInt;
      case otLong:
      case otLongVar:
         return TypeCode::tLong;
      case otReal:
      case otRealVar:
         return TypeCode::tReal;
      case otParams:
         return TypeCode::tParams;
      default:
         return TypeCode::tRef;
   }
}

// --- Compiler::ModuleScope ---

Compiler::ModuleScope :: ModuleScope(Project* project, Unresolveds* forwardsUnresolved)
   : symbolHints(okUnknown)
{
   this->project = project;
   this->sourcePath = NULL;
   this->forwardsUnresolved = forwardsUnresolved;

   warnOnUnresolved = project->BoolSetting(opWarnOnUnresolved);
   warnOnWeakUnresolved = project->BoolSetting(opWarnOnWeakUnresolved);
}

void Compiler::ModuleScope :: init(_Module* module, _Module* debugModule, const _path_t* sourcePath)
{
   this->module = module;
   this->debugModule = debugModule;
   this->sourcePath = sourcePath;

//   // cache the frequently used references
   nilReference = mapConstantReference(NIL_CLASS);
   trueReference = mapConstantReference(TRUE_CLASS);
   falseReference = mapConstantReference(FALSE_CLASS);
   controlReference = mapConstantReference(CONTROL_CLASS);

   shortSubject = mapSubject(SHORT_SUBJECT);
   intSubject = mapSubject(INT_SUBJECT);
   longSubject = mapSubject(LONG_SUBJECT);
   realSubject = mapSubject(REAL_SUBJECT);
   referenceSubject = mapSubject(REFERENCE_SUBJECT);
   wideStrSubject = mapSubject(WSTR_SUBJECT);
   dumpSubject = mapSubject(DUMP_SUBJECT);
   handleSubject = mapSubject(HANDLE_SUBJECT);
   lengthSubject = mapSubject(LENGTH_SUBJECT);
   lengthOutSubject = mapSubject(OUT_LENGTH_SUBJECT);
   indexSubject = mapSubject(INDEX_SUBJECT);
   arraySubject = mapSubject(ARRAY_SUBJECT);
   byteSubject = mapSubject(BYTE_SUBJECT);

   whileSignRef = mapSubject(WHILE_SIGNATURE);
   untilSignRef = mapSubject(UNTIL_SIGNATURE);

   defaultNs.add(module->Name());
}

ObjectType Compiler::ModuleScope :: mapSubjectType(TerminalInfo identifier, bool& out)
{
   ref_t subjRef = mapSubject(identifier, out);
   ObjectType type = mapSubjectType(subjRef);

   if (out) {
      switch(type) {
         case otInt:
            return otIntVar;
         case otLong:
            return otLongVar;
         case otReal:
            return otRealVar;
      }
   }
   return type;
}

ObjectType Compiler::ModuleScope :: mapSubjectType(TerminalInfo identifier)
{
   ref_t subjRef = mapSubject(identifier);
   return mapSubjectType(subjRef);
}

ObjectType Compiler::ModuleScope :: mapSubjectType(ref_t subjRef)
{
   if (subjRef == referenceSubject) {
      return otRef;
   }
   else if (subjRef == intSubject) {
      return otInt;
   }
   else if (subjRef == longSubject) {
      return otLong;
   }
   else if (subjRef == realSubject) {
      return otReal;
   }
   else if (subjRef == arraySubject) {
      return otArray;
   }
   else if (subjRef == wideStrSubject) {
      return otLiteral;
   }
   else if (subjRef == lengthSubject) {
      return otLength;
   }
   else if (subjRef == shortSubject) {
      return otShort;
   }
   else if (subjRef == indexSubject) {
      return otIndex;
   }
   else if (subjRef == dumpSubject) {
      return otByteArray;
   }
   else if (subjRef == byteSubject) {
      return otByte;
   }
   else return otNone;         
}

ObjectInfo Compiler::ModuleScope :: mapObject(TerminalInfo identifier)
{
   if (identifier==tsReference) {
      return mapReference(identifier, false);
   }
   else if (identifier==tsPrivate) {
      return defineObjectInfo(mapTerminal(identifier, true));
   }
   else if (identifier==tsIdentifier) {
      return defineObjectInfo(mapTerminal(identifier, true), true );
   }
   else return ObjectInfo();
}

ref_t Compiler::ModuleScope :: resolveIdentifier(const wchar16_t* identifier)
{
   List<const wchar16_t*>::Iterator it = defaultNs.start();
   while (!it.Eof()) {
      ReferenceNs name(*it, identifier);

      if (checkGlobalReference(name))
         return module->mapReference(name);

      it++;
   }
   return 0;
}

ref_t Compiler::ModuleScope :: mapTerminal(TerminalInfo terminal, bool existing)
{
   if (terminal == tsIdentifier) {
      ref_t reference = forwards.get(terminal);
      if (reference == 0) {
         if (!existing) {
            ReferenceNs name(module->Name(), terminal);

            return module->mapReference(name);
         }
         else return resolveIdentifier(terminal);
      }
      else return reference;
   }
   else if (terminal == tsPrivate) {
      ReferenceNs name(module->Name(), terminal);

      return mapReference(name, existing).reference;
   }
   else return mapReference(terminal, existing).reference;
}

bool Compiler::ModuleScope :: checkGlobalReference(const wchar16_t* referenceName)
{
   ref_t moduleRef = 0;
   _Module* module = project->resolveModule(referenceName, moduleRef, true);

   if (module == NULL || moduleRef == 0)
      return false;

   return module->mapReference(referenceName, true);
}

ObjectInfo Compiler::ModuleScope :: defineObjectInfo(ref_t reference, bool checkState)
{
   // if reference is zero the symbol is unknown
   if (reference == 0) {
      return ObjectInfo();
   }
   else if (reference == controlReference) {
      return ObjectInfo(okConstant, otControl, reference);
   }
   // check if symbol should be treated like constant one
   else if (symbolHints.exist(reference, okConstant)) {
      return ObjectInfo(okConstant, reference);
   }
   else if (checkState) {
      ClassInfo info;
      ref_t r = loadClassInfo(info, module->resolveReference(reference));
      if (r) {
         // if it is a stateless class
         if (test(info.header.flags, elStateless)) {
            defineConstant(reference);

            return ObjectInfo(okConstant, reference);
         }
         // if it is a normal class
         // then the symbol is reference to the class class
         else if (test(info.header.flags, elStandartVMT) && info.classClassRef != 0) {
            return ObjectInfo(okConstant, otClass, reference, info.classClassRef);
         }
      }
   }
   // otherwise it is a normal one
   return ObjectInfo(okSymbol, reference);
}

ObjectInfo Compiler::ModuleScope :: mapReference(const wchar16_t* reference, bool existing)
{
   ref_t referenceID = 0;

   if (!isWeakReference(reference)) {
      // if it is external link
      if (ConstantIdentifier::compare(reference, EXTERNAL_MODULE, strlen(EXTERNAL_MODULE)) && reference[strlen(EXTERNAL_MODULE)]=='\'') {
         return ObjectInfo(okExternal);
      }
      else if (existing) {
         // check if the reference does exist
         ref_t moduleRef = 0;
         _Module* argModule = project->resolveModule(reference, moduleRef);

         if (argModule != NULL && moduleRef != 0)
            referenceID = module->mapReference(reference);
      }
      else referenceID = module->mapReference(reference, existing);
   }
   else referenceID = module->mapReference(reference, existing);

   return defineObjectInfo(referenceID);
}

ref_t Compiler::ModuleScope :: loadClassInfo(ClassInfo& info, const wchar16_t* vmtName)
{
   if (emptystr(vmtName))
      return 0;

   // load class meta data
   ref_t moduleRef = 0;
   _Module* argModule = project->resolveModule(vmtName, moduleRef);

   if (argModule == NULL || moduleRef == 0)
      return 0;

   // load argument VMT meta data
   _Memory* metaData = argModule->mapSection(moduleRef | mskMetaRDataRef, true);
   if (metaData == NULL)
      return 0;

   MemoryReader reader(metaData);

   info.load(&reader);

   if (argModule != module) {
      // import class class reference
      if (info.classClassRef != 0)
         info.classClassRef = importReference(argModule, info.classClassRef, module);

      // import reference
      importReference(argModule, moduleRef, module);
   }
   return moduleRef;   
}

void Compiler::ModuleScope :: validateReference(TerminalInfo terminal, ref_t reference)
{
   // check if the reference may be resolved
   bool found = false;

   if (warnOnUnresolved && (warnOnWeakUnresolved || !isWeakReference(terminal))) {
      int   mask = reference & mskAnyRef;
      reference &= ~mskAnyRef;

      ref_t    ref = 0;
      _Module* refModule = project->resolveModule(module->resolveReference(reference), ref, true);

      if (refModule != NULL) {
         found = (refModule->mapSection(ref | mask, true)!=NULL);
      }
      if (!found) {
         if (!refModule || refModule == module) {
            forwardsUnresolved->add(Unresolved(sourcePath, reference | mask, module, terminal.Row(), terminal.Col()));
         }
         else raiseWarning(wrnUnresovableLink, terminal);
      }
   }
}

void Compiler::ModuleScope :: raiseError(const char* message, TerminalInfo terminal)
{
   project->raiseError(message, sourcePath, terminal.Row(), terminal.Col(), terminal.value);
}

void Compiler::ModuleScope :: raiseWarning(const char* message, TerminalInfo terminal)
{
   project->raiseWarning(message, sourcePath, terminal.Row(), terminal.Col(), terminal.value);
}

void Compiler::ModuleScope :: compileForwardHints(DNode hints, bool& constant)
{
   constant = false;

   while (hints == nsHint) {
      if (ConstIdentifier::compare(hints.Terminal(), HINT_CONSTANT)) {
         constant = true;
      }
      else raiseWarning(wrnUnknownHint, hints.Terminal());

      hints = hints.nextNode();
   }
}

// --- Compiler::SourceScope ---

//Compiler::SourceScope :: SourceScope(Scope* parent)
//   : Scope(parent)
//{
//   this->reference = 0;
//}

Compiler::SourceScope :: SourceScope(ModuleScope* moduleScope, ref_t reference)
   : Scope(moduleScope)
{
   this->reference = reference;
}

// --- Compiler::SymbolScope ---

Compiler::SymbolScope :: SymbolScope(ModuleScope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
//   param = NULL;
}

//void Compiler::SymbolScope :: compileHints(DNode hints)
//{
//   while (hints == nsHint) {
//      raiseWarning(wrnUnknownHint, hints.Terminal());
//
//      hints = hints.nextNode();
//   }
//}

ObjectInfo Compiler::SymbolScope :: mapObject(TerminalInfo identifier)
{
   /*if (StringHelper::compare(identifier, param) || ConstIdentifier::compare(identifier, PARAM_VAR)) {
      return ObjectInfo(otLocal, -1);
   }
   else */return Scope::mapObject(identifier);
}

// --- Compiler::ClassScope ---

Compiler::ClassScope :: ClassScope(ModuleScope* parent, ref_t reference)
   : SourceScope(parent, reference)
{
   info.header.parentRef = moduleScope->mapConstantReference(SUPER_CLASS);
   info.header.flags = elStandartVMT;
   info.size = 0;
   info.classClassRef = 0;
}

ObjectInfo Compiler::ClassScope :: mapObject(TerminalInfo identifier)
{
   if (ConstIdentifier::compare(identifier, SUPER_VAR)) {
      return ObjectInfo(okSuper, info.header.parentRef);
   }
   else if (ConstIdentifier::compare(identifier, SELF_VAR)) {
      return ObjectInfo(okVSelf, -1);
   }
   else {
      int reference = info.fields.get(identifier);
      if (reference != -1) {
         // if it is data field
         if (test(info.header.flags, elStructureRole)) {
            int type = getClassType();
            switch (type) {
               case elDebugDWORD:
                  return ObjectInfo(okField, otInt, -1);
               case elDebugQWORD:
                  return ObjectInfo(okField, otLong, -1);
               case elDebugReal64:
                  return ObjectInfo(okField, otReal, -1);
               case elDebugLiteral:
                  return ObjectInfo(okField, otLiteral, -1);
               case elDebugBytes:
                  return ObjectInfo(okField, otByteArray, -1);
               default:
                  return ObjectInfo(okUnknown);
            }
         }
         else if (test(info.header.flags, elDynamicRole)) {
            int type = getClassType();
            if (type == elDebugArray) {
               return ObjectInfo(okField, otArray, -1);
            }
            else return ObjectInfo(okUnknown);
         }
         // otherwise it is a normal field
         else return ObjectInfo(okField, reference);
      }
      else return Scope::mapObject(identifier);
   }
}

void Compiler::ClassScope :: compileHints(DNode hints)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (ConstIdentifier::compare(terminal, HINT_GROUP)) {
         info.header.flags |= elGroup;
      }
      else if (ConstIdentifier::compare(terminal, HINT_MESSAGE)) {
         info.header.flags |= elMessage;
      }
      else if (ConstIdentifier::compare(terminal, HINT_SIGNATURE)) {
         info.header.flags |= elSignature;
      }
      else if (ConstIdentifier::compare(terminal, HINT_ROLE)) {
         info.header.flags |= elRole;
      }
      else if (ConstIdentifier::compare(terminal, HINT_SEALED)) {
         info.header.flags |= elSealed;
      }
      else raiseWarning(wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

int Compiler::ClassScope :: getFieldSizeHint()
{
   switch (info.header.flags & elDebugMask) {
      case elDebugDWORD:
         return 4;
      case elDebugReal64:
      case elDebugQWORD:
         return 8;
      case elDebugArray:
         return (size_t)-4;
      case elDebugLiteral:
         return (size_t)-2;
      case elDebugBytes:
         return (size_t)-1;
      default:
         return 0;
   }
}

int Compiler::ClassScope :: getFieldSizeHint(TerminalInfo terminal)
{
   if (terminal.symbol == tsInteger) {
      return StringHelper::strToInt(terminal);
   }
   else if (terminal.symbol == tsHexInteger) {
      return StringHelper::strToLong(terminal, 16);
   }
   else {
      raiseWarning(wrnUnknownHint, terminal);

      return 0;
   }
}

void Compiler::ClassScope :: compileFieldSizeHint(DNode hints, size_t& size)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (ConstIdentifier::compare(terminal, HINT_TYPE)) {
         DNode value = hints.select(nsHintValue);

         if (value!=nsNone) {
            setTypeHints(value);

            size = getFieldSizeHint();
         }
         else raiseWarning(wrnInvalidHint, terminal);
      }
      else if (ConstIdentifier::compare(terminal, HINT_SIZE)) {
         DNode value = hints.select(nsHintValue);

         // if size is defined it is static byte array
         if (value!=nsNone) {
            info.header.flags |= elDebugBytes;
            info.header.flags |= elStructureRole;

            size = getFieldSizeHint(value.Terminal());
         }
         else raiseWarning(wrnInvalidHint, terminal);
      }

      hints = hints.nextNode();
   }
}

void Compiler::ClassScope :: compileFieldHints(DNode hints, int offset)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (ConstIdentifier::compare(terminal, HINT_TYPE)) {
         // ignore type hint;
         // it is compiled by compileFieldSizeHint
      }
      else if (ConstIdentifier::compare(terminal, HINT_SIZE)) {
         // ignore size hint;
         // it is compiled by compileFieldSizeHint
      }
      else raiseWarning(wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

void Compiler::ClassScope :: setTypeHints(DNode hintValue)
{
   TerminalInfo terminal = hintValue.Terminal();

   if (ConstIdentifier::compare(terminal, HINT_INT)) {
      info.header.flags |= elDebugDWORD;
      info.header.flags |= elStructureRole;
   }
   else if (ConstIdentifier::compare(terminal, HINT_LONG)) {
      info.header.flags |= elDebugQWORD;
      info.header.flags |= elStructureRole;
   }
   else if (ConstIdentifier::compare(terminal, HINT_REAL)) {
      info.header.flags |= elDebugReal64;
      info.header.flags |= elStructureRole;
   }
   else if (ConstIdentifier::compare(terminal, HINT_LITERAL)) {
      info.header.flags |= elDebugLiteral;
      info.header.flags |= elStructureRole;
      info.header.flags |= elDynamicRole;
   }
   else if (ConstIdentifier::compare(terminal, HINT_ARRAY)) {
      info.header.flags |= elDebugArray;
      info.header.flags |= elDynamicRole;
   }
   //else if (ConstIdentifier::compare(terminal, HINT_MESSAGE)) {
   //   info.header.flags |= elMessage;
   //   info.header.flags |= elRole;
   //   info.header.flags |= elStructureRole;
   //}
   else if (ConstIdentifier::compare(terminal, HINT_BYTEARRAY)) {
      info.header.flags |= elDebugBytes;
      info.header.flags |= elDynamicRole;
      info.header.flags |= elStructureRole;
   }
   //else if (ConstantIdentifier::compare(terminal, HINT_GROUP)) {
   //   info.header.flags |= elGroup;
   //   info.header.flags |= elStructureRole;
   //}
   //else if (ConstantIdentifier::compare(terminal, HINT_ROLE)) {
   //   info.header.flags |= elRole;
   //}
   else raiseWarning(wrnUnknownHintValue, terminal);
}

// --- Compiler::MetodScope ---

Compiler::MethodScope :: MethodScope(ClassScope* parent)
   : Scope(parent), parameters(Parameter())
{
   this->message = 0;
   this->withBreakHandler = false;
   this->withCustomVerb = false;
   this->reserved = 0;
   this->masks = 0;
}

void Compiler::MethodScope :: include()
{
   ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   ClassInfo::MethodMap::Iterator it = classScope->info.methods.getIt(message);
   if (it.Eof()) {
      classScope->info.methods.add(message, true);
   }
   else (*it) = true;
}

//int Compiler::MethodScope :: compileHints(DNode hints)
//{
//   int mode = 0;
//
//   while (hints == nsHint) {
//      TerminalInfo terminal = hints.Terminal();
//
//      //if (ConstIdentifier::compare(terminal, HINT_PRIMITIVE)) {
//      //   if (mode != 0) {
//      //      raiseWarning(wrnInvalidHint, terminal);
//      //   }
//      //   else mode |= CTRL_HINT_PRIMITIVE;
//      //}
//      //else if (ConstIdentifier::compare(terminal, HINT_INTERNAL)) {
//      //   ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);
//      //   if (!test(classScope->info.header.flags, elStructureRole) || mode != 0) {
//      //      raiseWarning(wrnInvalidHint, terminal);
//      //   }
//      //   else {
//      //      mode = mapInternalReference(hints.select(nsHintValue).Terminal());
//
//      //      mode |= CTRL_HINT_INTERNAL;
//      //   }
//      //}
//      //else if (ConstIdentifier::compare(terminal, HINT_EXTERNAL)) {
//      //   ClassScope* classScope = (ClassScope*)getScope(Scope::slClass);
//      //   if (!test(classScope->info.header.flags, elStructureRole) || mode != 0) {
//      //      raiseWarning(wrnInvalidHint, terminal);
//      //   }
//      //   else {
//      //      mode = mapExternalReference(hints.select(nsHintValue).Terminal());
//
//      //      mode |= CTRL_HINT_EXTERNAL;
//      //   }
//      //}
////      else if (ConstIdentifier::compare(terminal, HINT_LOCK)) {
////         mode |= modLock;
////      }
//      /*else */raiseWarning(wrnUnknownHint, hints.Terminal());
//
//      hints = hints.nextNode();
//   }
//
//   return mode;
//}

ObjectInfo Compiler::MethodScope :: mapObject(TerminalInfo identifier)
{
   if (ConstIdentifier::compare(identifier, THIS_VAR)) {
      return ObjectInfo(okSelf, 0);
   }
   else if (ConstIdentifier::compare(identifier, SELF_VAR)) {
      ObjectInfo retVal = parent->mapObject(identifier);
      // overriden to set FRAME USED flag
      if (retVal.kind == okVSelf) {
         masks |= MTH_FRAME_USED;
      }

      return retVal;
   }
   else {
      Parameter param = parameters.get(identifier);

      int local = param.reference;
      if (local >= 0) {
         masks |= MTH_FRAME_USED;

         return ObjectInfo(okLocal, param.type, -1 - local);
      }
      else return Scope::mapObject(identifier); 
   }
//   else if (ConstIdentifier::compare(identifier, NEXT_VAR)) {
//      return ObjectInfo(otVNext, 0);
//   }
}

// --- Compiler::CodeScope ---

Compiler::CodeScope :: CodeScope(SourceScope* parent)
   : Scope(parent), locals(Parameter(0, otNone))
{
   this->tape = &parent->tape;
   this->level = 0;
   this->saved = this->reserved = 0;
   this->breakLabel = 0;
}

//Compiler::CodeScope :: CodeScope(MethodScope* parent, CodeType type)
//   : Scope(parent)
//{
//   this->tape = &((ClassScope*)parent->getScope(slClass))->tape;
//   this->level = 0;
//}

Compiler::CodeScope :: CodeScope(MethodScope* parent)
   : Scope(parent), locals(Parameter(0, otNone))
{
   this->tape = &((ClassScope*)parent->getScope(slClass))->tape;
   this->level = 0;
   this->saved = this->reserved = 0;
   this->breakLabel = -1;
}

Compiler::CodeScope :: CodeScope(CodeScope* parent)
   : Scope(parent), locals(Parameter(0, otNone))
{
   this->tape = parent->tape;
   this->level = parent->level;
   this->saved = this->reserved = 0;
   this->breakLabel = parent->breakLabel;
}

ObjectInfo Compiler::CodeScope :: mapObject(TerminalInfo identifier)
{
   Parameter local = locals.get(identifier);
   if (local.reference) {
      if (local.type != otNone) {
         return ObjectInfo(okLocal, local.type, local.reference);
      }
      else return ObjectInfo(okLocal, local.reference);
   }
   else return Scope::mapObject(identifier);
}

// Note: size modificator is used only for bytearray
//       in all other cases it should be zero
void Compiler::CodeScope :: compileLocalHints(DNode hints, ObjectType& type, int& size)
{
   while (hints == nsHint) {
      TerminalInfo terminal = hints.Terminal();

      if (ConstIdentifier::compare(terminal, HINT_TYPE)) {
         TerminalInfo typeValue = hints.firstChild().Terminal();
         if (ConstIdentifier::compare(typeValue, HINT_INT)) {
            type = otInt;
         }
         else if (ConstIdentifier::compare(typeValue, HINT_SHORT)) {
            type = otShort;
         }
         else if (ConstIdentifier::compare(typeValue, HINT_LONG)) {
            type = otLong;
         }
         else if (ConstIdentifier::compare(typeValue, HINT_REAL)) {
            type = otReal;
         }
         else if (ConstIdentifier::compare(typeValue, HINT_BYTEARRAY)) {
            type = otByteArray;
         }
         else raiseWarning(wrnUnknownHint, terminal);
      }
      else if (ConstIdentifier::compare(terminal, HINT_SIZE)) {
         TerminalInfo sizeValue = hints.firstChild().Terminal();
         if (type == otByteArray && sizeValue.symbol == tsInteger) {
            size = StringHelper::strToInt(sizeValue.value);
         }
         else raiseWarning(wrnUnknownHint, terminal);
      }
      else raiseWarning(wrnUnknownHint, terminal);

      hints = hints.nextNode();
   }
}

// --- Compiler::InlineClassScope ---

Compiler::InlineClassScope :: InlineClassScope(CodeScope* owner, ref_t reference)
   : ClassScope(owner->moduleScope, reference), outers(Outer())
{
   this->parent = owner;
   info.header.flags |= elNestedClass;
}

Compiler::InlineClassScope::Outer Compiler::InlineClassScope :: mapSelf()
{
   String<wchar16_t, 10> thisVar(THIS_VAR);

   Outer owner = outers.get(thisVar);
   // if owner reference is not yet mapped, add it
   if (owner.outerObject.kind == okUnknown) {
      owner.reference = info.fields.Count();
      owner.outerObject.kind = okSelf;

      outers.add(thisVar, owner);
      mapKey(info.fields, thisVar, owner.reference);
   }
   return owner;
}

ObjectInfo Compiler::InlineClassScope :: mapObject(TerminalInfo identifier)
{
   if (ConstIdentifier::compare(identifier, THIS_VAR)) {
      return ObjectInfo(okSelf, 0);
   }
   else {
      Outer outer = outers.get(identifier);

	  // if object already mapped
      if (outer.reference!=-1) {
         if (outer.outerObject.kind == okSuper) {
            return ObjectInfo(okSuper, outer.reference);
         }
         else return ObjectInfo(okOuter, outer.reference);
      }
      else {
         outer.outerObject = parent->mapObject(identifier);
         // handle outer fields in a special way: save only self
         if (outer.outerObject.kind==okField) {
            Outer owner = mapSelf();

            // map as an outer field (reference to outer object and outer object field index)
            return ObjectInfo(okOuterField, owner.reference, outer.outerObject.reference);
         }
         // map if the object is outer one
         else if (outer.outerObject.kind==okLocal || outer.outerObject.kind==okField || outer.outerObject.kind==okOuter
            || outer.outerObject.kind==okVSelf || outer.outerObject.kind==okSuper || outer.outerObject.kind==okOuterField)
         {
            outer.reference = info.fields.Count();

            outers.add(identifier, outer);
            mapKey(info.fields, identifier.value, outer.reference);

            return ObjectInfo(okOuter, outer.reference);
         }
         // if inline symbol declared in symbol it treats self variable in a special way
         else if (ConstIdentifier::compare(identifier, SELF_VAR)) {
            return ObjectInfo(okVSelf, -1);
         }
         else if (outer.outerObject.kind == okUnknown) {
            // check if there is inherited fields
            outer.reference = info.fields.get(identifier);
            if (outer.reference != -1) {
               return ObjectInfo(okField, outer.reference);
            }
            else return outer.outerObject;
         }
         else return outer.outerObject;
      }
   }
}

// --- Compiler ---

Compiler :: Compiler(StreamReader* syntax)
   : _parser(syntax), _verbs(0), _unresolveds(Unresolved(), NULL)
{
   loadVerbs(_verbs);
   loadOperators(_operators);

//   // default settings
//   _optFlag = 0;
}

void Compiler :: loadOperators(MessageMap& operators)
{
   addVerb(operators, ADD_OPERATOR, ADD_MESSAGE_ID);
   addVerb(operators, SUB_OPERATOR, SUB_MESSAGE_ID);
   addVerb(operators, MUL_OPERATOR, MUL_MESSAGE_ID);
   addVerb(operators, DIV_OPERATOR, DIV_MESSAGE_ID);
   addVerb(operators, IF_OPERATOR, IF_MESSAGE_ID);
   addVerb(operators, IFNOT_OPERATOR, IFNOT_MESSAGE_ID);
   addVerb(operators, EQUAL_OPERATOR, EQUAL_MESSAGE_ID);
   addVerb(operators, NOTEQUAL_OPERATOR, NOTEQUAL_MESSAGE_ID);
   addVerb(operators, LESS_OPERATOR, LESS_MESSAGE_ID);
   addVerb(operators, GREATER_OPERATOR, GREATER_MESSAGE_ID);
   addVerb(operators, NOTLESS_OPERATOR, NOTLESS_MESSAGE_ID);
   addVerb(operators, NOTGREATER_OPERATOR, NOTGREATER_MESSAGE_ID);
   addVerb(operators, AND_OPERATOR, AND_MESSAGE_ID);
   addVerb(operators, OR_OPERATOR, OR_MESSAGE_ID);
   addVerb(operators, XOR_OPERATOR, XOR_MESSAGE_ID);
   addVerb(operators, REFER_OPERATOR, REFER_MESSAGE_ID);
   addVerb(operators, APPEND_OPERATOR, APPEND_MESSAGE_ID);
   addVerb(operators, REDUCE_OPERATOR, REDUCE_MESSAGE_ID);
   addVerb(operators, INCREASE_OPERATOR, INCREASE_MESSAGE_ID);
   addVerb(operators, SEPARATE_OPERATOR, SEPARATE_MESSAGE_ID);
   addVerb(operators, WRITE_OPERATOR, WRITE_MESSAGE_ID);
   addVerb(operators, READ_OPERATOR, READ_MESSAGE_ID);
}

void Compiler :: loadRules(StreamReader* optimization)
{
   _rules.load(optimization);
}

bool Compiler :: optimizeJumps(CommandTape& tape)
{
   //if (!test(_optFlag, optJumps))
   //   return false;

   return CommandTape::optimizeJumps(tape);
}

bool Compiler :: applyRules(CommandTape& tape)
{
   if (!_rules.loaded)
      return false;

   if (_rules.apply(tape)) {
      while (_rules.apply(tape));

      return true;
   }
   else return false;
}

void Compiler :: optimizeTape(CommandTape& tape)
{
   // optimize the code
   while (applyRules(tape));

   // optimize unsued and idle jumps
   while (optimizeJumps(tape));
}

ref_t Compiler :: mapNestedExpression(CodeScope& scope, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // if it is a root inline expression we could try to name it after the symbol
   if (test(mode, CTRL_ROOT)) {
      // check if the inline symbol is declared in the symbol
      SymbolScope* symbol = (SymbolScope*)scope.getScope(Scope::slSymbol);
      if (symbol != NULL) {
         return symbol->reference;
      }
   }

   // otherwise auto generate the name
   ReferenceNs name(moduleScope->module->Name(), INLINE_POSTFIX);

   findUninqueName(moduleScope->module, name);

   return moduleScope->module->mapReference(name);
}

void Compiler :: declareParameterDebugInfo(MethodScope& scope, CommandTape* tape, bool withSelf)
{
   // declare method parameter debug info
   LocalMap::Iterator it = scope.parameters.start();
   while (!it.Eof()) {
      switch ((*it).type) {
         case otInt:
         case otIntVar:
            _writer.declareLocalIntInfo(*tape, it.key(), -2 - (*it).reference);
            break;
         case otLong:
         case otLongVar:
            _writer.declareLocalLongInfo(*tape, it.key(), -2 - (*it).reference);
            break;
         case otReal:
         case otRealVar:
            _writer.declareLocalRealInfo(*tape, it.key(), -2 - (*it).reference);
            break;
         default:
            _writer.declareLocalInfo(*tape, it.key(), -2 - (*it).reference);
      }

      it++;
   }
   if (withSelf)
      _writer.declareSelfInfo(*tape, -2);
}

void Compiler :: importCode(DNode node, ModuleScope& scope, CommandTape* tape, const wchar16_t* referenceName)
{
   ByteCodeIterator it = tape->end();

   ref_t reference = 0;
   _Module* api = scope.project->resolveModule(referenceName, reference);

   _Memory* section = api->mapSection(reference | mskCodeRef, true);
   if (section == NULL) {
      scope.raiseError(errInvalidLink, node.Terminal());
   }
   else tape->import(section);

   // import references
   while (!it.Eof()) {
      CommandTape::import(*it, api, scope.module);
      it++;
   }
}

Compiler::InheritResult Compiler :: inheritClass(ClassScope& scope, ref_t parentRef)
{
   ModuleScope* moduleScope = scope.moduleScope;

   size_t flagCopy = scope.info.header.flags;
   size_t classClassCopy = scope.info.classClassRef;

   // get module reference
   ref_t moduleRef = 0;
   _Module* module = moduleScope->project->resolveModule(moduleScope->module->resolveReference(parentRef), moduleRef);

   if (module == NULL || moduleRef == 0)
      return irUnsuccessfull;

   // load parent meta data
   _Memory* metaData = module->mapSection(moduleRef | mskMetaRDataRef, true);
   if (metaData != NULL) {
      MemoryReader reader(metaData);
      // import references if we inheriting class from another module
      if (moduleScope->module != module) {
         ClassInfo copy;
         copy.load(&reader);

         scope.info.header = copy.header;
         scope.info.size = copy.size;
         // import method references and mark them as inherited
         ClassInfo::MethodMap::Iterator it = copy.methods.start();
         while (!it.Eof()) {
            scope.info.methods.add(importMessage(module, it.key(), moduleScope->module), false);

            it++;
         }

         // copy fields
         scope.info.fields.add(copy.fields);
      }
      else {
         scope.info.load(&reader);

         // mark all methods as inherited
         ClassInfo::MethodMap::Iterator it = scope.info.methods.start();
         while (!it.Eof()) {
            (*it) = false;

            it++;
         }
      }

      if (test(scope.info.header.flags, elSealed))
         return irSealed;

      // restore parent and flags
      scope.info.header.parentRef = parentRef;
      scope.info.classClassRef = classClassCopy;
      scope.info.header.flags |= flagCopy;

      return irSuccessfull;
   }
   else return irUnsuccessfull;
}

void Compiler :: compileParentDeclaration(DNode node, ClassScope& scope)
{
   // base class system'object must not to have a parent
   ref_t parentRef = scope.info.header.parentRef;
   if (scope.info.header.parentRef == scope.reference) {
      if (node.Terminal() != nsNone)
         scope.raiseError(errInvalidSyntax, node.Terminal());

      parentRef = 0;
   }
   // if the class has an implicit parent
   else if (node.Terminal() != nsNone) {
      TerminalInfo identifier = node.Terminal();
      if (identifier == tsIdentifier || identifier == tsPrivate) {
         parentRef = scope.moduleScope->mapTerminal(node.Terminal(), true);
      }
      else parentRef = scope.moduleScope->mapReference(identifier).reference;

      if (parentRef == 0)
         scope.raiseError(errUnknownClass, node.Terminal());
   }
   InheritResult res = compileParentDeclaration(parentRef, scope);
   //if (res == irObsolete) {
   //   scope.raiseWarning(wrnObsoleteClass, node.Terminal());
   //}
   if (res == irInvalid) {
      scope.raiseError(errInvalidParent, node.Terminal());
   }
   if (res == irSealed) {
      scope.raiseError(errSealedParent, node.Terminal());
   }
   else if (res == irUnsuccessfull)
      scope.raiseError(node != nsNone ? errUnknownClass : errUnknownBaseClass, node.Terminal());
}

Compiler::InheritResult Compiler :: compileParentDeclaration(ref_t parentRef, ClassScope& scope)
{
   scope.info.header.parentRef = parentRef;
   if (scope.info.header.parentRef != 0) {
      return inheritClass(scope, scope.info.header.parentRef);
   }
   else return irSuccessfull;
}

void Compiler :: compileSwitch(DNode node, CodeScope& scope, ObjectInfo switchValue)
{
   _writer.declareSwitchBlock(*scope.tape);

   if (switchValue.kind == okRegister) {
      switchValue.kind = okCurrent;
      switchValue.reference = 0;

      _writer.pushObject(*scope.tape, switchValue);
   }

   DNode option = node.firstChild();
   while (option == nsSwitchOption || option == nsBiggerSwitchOption || option == nsLessSwitchOption)  {
      _writer.declareSwitchBlock(*scope.tape);

      int operator_id = EQUAL_MESSAGE_ID;
      if (option == nsBiggerSwitchOption) {
         operator_id = GREATER_MESSAGE_ID;
      }
      else if (option == nsLessSwitchOption) {
         operator_id = LESS_MESSAGE_ID;
      }

      ObjectInfo optionValue = compileObject(option.firstChild(), scope, 0);
      _writer.loadObject(*scope.tape, optionValue);
      boxObject(scope, optionValue, 0);
      _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
      _writer.pushObject(*scope.tape, switchValue);

      _writer.setMessage(*scope.tape, encodeMessage(0, operator_id, 1));
      _writer.callDispatcher(*scope.tape, 0, 1);

      _writer.jumpIfEqual(*scope.tape, scope.moduleScope->trueReference);

      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild().nextNode();

      if (thenCode.firstChild().nextNode() != nsNone) {
         compileCode(thenCode, subScope, CTRL_PREV_EXIT_MODE);
      }
      // if it is inline action
      else compileRetExpression(thenCode.firstChild(), scope, 0);

      _writer.endSwitchOption(*scope.tape);

      option = option.nextNode();
   }
   if (option == nsLastSwitchOption) {
      CodeScope subScope(&scope);
      DNode thenCode = option.firstChild();
      if (thenCode.firstChild().nextNode() != nsNone) {
         compileCode(thenCode, subScope);
      }
      // if it is inline action
      else compileRetExpression(thenCode.firstChild(), scope, 0);
   }

   _writer.endSwitchBlock(*scope.tape);
}

void Compiler :: compileAssignment(DNode node, CodeScope& scope, ObjectInfo object)
{
   if (object.type == otNone && (object.kind == okLocal || object.kind == okField)) {
      _writer.saveObject(*scope.tape, object);
   }
   //else if ((object.type == otOuterField)) {
   //   _writer.assignObject(*scope.tape, object);
   //}
   else if (object.kind == okUnknown) {
      scope.raiseError(errUnknownObject, node.Terminal());
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());
}

void Compiler :: compileStackAssignment(DNode node, CodeScope& scope, ObjectInfo variableInfo, ObjectInfo object)
{
   if (variableInfo.type == otInt && (object.type == otInt || object.type == otIndex)) {
      _writer.copyInt(*scope.tape, variableInfo);
   }
   else if (variableInfo.type == otLong && (object.type == otLong)) {
      _writer.copyLong(*scope.tape, variableInfo);
   }
   else if (variableInfo.type == otReal && (object.type == otReal)) {
      _writer.copyLong(*scope.tape, variableInfo);
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());
}

void Compiler :: compileVariable(DNode node, CodeScope& scope, DNode hints)
{
   if (!scope.locals.exist(node.Terminal())) {
      ObjectType type = otNone;
      int size = 0;
      scope.compileLocalHints(hints, type, size);

      int level = scope.newLocal();
      int mode = definePrimitiveExprPrefix(type, size);

      if (type != otNone) {
         ObjectInfo primitive;;

         // Note: size modificator is used only for bytearray
         //       in all other cases it should be zero
         if (type == otByteArray && size > 0) {
            mode += size;

            // byte array should start with size field
            allocatePrimitiveObject(scope, TypeCode::tInt, primitive);
            _writer.declarePrimitiveVariable(*scope.tape, size);
            _writer.loadObject(*scope.tape, ObjectInfo(okLocal, primitive.reference));
            _writer.popObject(*scope.tape, ObjectInfo(okRegisterField));
         }
   
         allocatePrimitiveObject(scope, mode & ~CTRL_MASK, primitive);

         // make the reservation permanent
         scope.saved = scope.reserved;

         _writer.pushObject(*scope.tape, primitive);
         switch (type)
         {
            case otInt:
            case otShort:
               _writer.declareLocalIntInfo(*scope.tape, node.Terminal(), level);
               break;
            case otLong:
               _writer.declareLocalLongInfo(*scope.tape, node.Terminal(), level);
               break;
            case otReal:
               _writer.declareLocalRealInfo(*scope.tape, node.Terminal(), level);
               break;
         }         
      }
      else {
         _writer.declareVariable(*scope.tape, scope.moduleScope->nilReference);
         _writer.declareLocalInfo(*scope.tape, node.Terminal(), level);
      }

      DNode assigning = node.firstChild();
      if (assigning != nsNone) {
         // if it is stack allocated variable
         if (type != otNone) {
            DNode expr = assigning.firstChild();

            ObjectInfo info = compileExpression(expr, scope, mode | CTRL_TYPEDOUT_MODE);

            scope.mapLocal(node.Terminal(), level, type);

            // if the expression was out-assigning, no need to assign the variable once again
            if (info.kind != okIdle)  {
               _writer.loadObject(*scope.tape, info);
               compileStackAssignment(node, scope, scope.mapObject(node.Terminal()), info);
            }
         }
         // otherwise, use normal routine
         else {
            ObjectInfo info = compileExpression(assigning.firstChild(), scope, mode);
            _writer.loadObject(*scope.tape, info);

            scope.mapLocal(node.Terminal(), level, type);

            info = boxObject(scope, info, 0);

            compileAssignment(node, scope, scope.mapObject(node.Terminal()));
         }
      }      
   }
   else scope.raiseError(errDuplicatedLocal, node.Terminal());
}

ObjectInfo Compiler :: compileTerminal(DNode node, CodeScope& scope, int mode)
{
   TerminalInfo terminal = node.Terminal();

   recordStep(scope, terminal, dsStep);

   ObjectInfo object;
   if (terminal==tsLiteral) {
      object = ObjectInfo(okConstant, otLiteral, scope.moduleScope->module->mapConstant(terminal));
   }
   else if (terminal == tsInteger) {
      int integer = StringHelper::strToInt(terminal.value);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      object = ObjectInfo(okConstant, otInt, scope.moduleScope->module->mapConstant(terminal));
   }
   else if (terminal == tsLong) {
      String<wchar16_t, 30> s("_"); // special mark to tell apart from integer constant
      s.append(terminal.value, getlength(terminal.value) - 1);

      long long integer = StringHelper::strToLongLong(s + 1, 10);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      object = ObjectInfo(okConstant, otLong, scope.moduleScope->module->mapConstant(s));
   }
   else if (terminal==tsHexInteger) {
      String<wchar16_t, 20> s(terminal.value, getlength(terminal.value) - 1);

      long integer = s.toLong(16);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      // convert back to string as a decimal integer
      s.clear();
      s.appendLong(integer);

      object = ObjectInfo(okConstant, otInt, scope.moduleScope->module->mapConstant(s));
   }
   else if (terminal == tsReal) {
      String<wchar16_t, 30> s(terminal.value, getlength(terminal.value) - 1);
      double number = StringHelper::strToDouble(s);
      if (errno == ERANGE)
         scope.raiseError(errInvalidIntNumber, terminal);

      object = ObjectInfo(okConstant, otReal, scope.moduleScope->module->mapConstant(s));
   }
   else if (!emptystr(terminal)) {
      object = scope.mapObject(terminal);
   }

   switch (object.kind) {
      case okUnknown:
         scope.raiseError(errUnknownObject, terminal);
      case okSymbol:
         scope.moduleScope->validateReference(terminal, object.reference | mskSymbolRef);
   }

   return object;
}

ObjectInfo Compiler :: compileObject(DNode objectNode, CodeScope& scope, int mode)
{
   ObjectInfo result;

   DNode member = objectNode.firstChild();
   switch (member)
   {
//      case nsRetStatement:
      case nsNestedClass:
         if (objectNode.Terminal() != nsNone) {
            result = compileNestedExpression(objectNode, scope, mode);
            break;
         }
      case nsSubCode:
      case nsInlineExpression:
         result = compileNestedExpression(member, scope, mode);
         break;
      case nsExpression:
         if (isCollection(member)) {
            TerminalInfo parentInfo = objectNode.Terminal();
            // if the parent class is defined
            if (parentInfo == tsIdentifier || parentInfo == tsReference || parentInfo == tsPrivate) {
               ref_t vmtReference = scope.moduleScope->mapTerminal(parentInfo, true);
               if (vmtReference == 0)
                  scope.raiseError(errUnknownObject, parentInfo);

               result = compileCollection(member, scope, mode & ~CTRL_ROOT, vmtReference);
            }
            else result = compileCollection(member, scope, mode & ~CTRL_ROOT);
         }
         else result = compileExpression(member, scope, mode & ~CTRL_ROOT);
         break;
      case nsSubjectArg:
         if (member.nextNode() == nsExpression) {
            return compileGetProperty(member, scope, mode | CTRL_GETPROP_MODE);
         }
         if (_verbs.exist(member.Terminal())) {
            result = compileMessageReference(member, scope, mode);
         }
         else result = compileSignatureReference(member, scope, mode);
         break;
      case nsSignatureReference:
         result = compileSignatureReference(member, scope, mode);
         break;
      case nsMessageReference:
         result = compileMessageReference(member, scope, mode);
         break;
      default:
         result = compileTerminal(objectNode, scope, mode);
   }

   return result;
}

ObjectInfo Compiler :: compileSignatureReference(DNode objectNode, CodeScope& scope, int mode)
{
   IdentifierString message;
   ref_t sign_id = 0;

   // reserve place for param counter
   message.append('0');

   // place dummy verb
   message.append('#');
   message.append(0x20);

   if (objectNode == nsSubjectArg) {
      TerminalInfo subject = objectNode.Terminal();

      message.append('&');
      message.append(subject);
   }
   else {
      DNode arg = objectNode.firstChild();
      while (arg == nsSubjectArg) {
         TerminalInfo subject = arg.Terminal();

         message.append('&');
         message.append(subject);

         arg = arg.nextNode();
      }
   }

   return ObjectInfo(okConstant, otSignature, scope.moduleScope->module->mapReference(message));
}

ObjectInfo Compiler :: compileMessageReference(DNode objectNode, CodeScope& scope, int mode)
{
   IdentifierString message;
   TerminalInfo verb = objectNode.Terminal();

   // reserve place for param counter
   message.append('0');

   DNode arg = objectNode.firstChild();
   ref_t verb_id = _verbs.get(verb.value);
   ref_t sign_id = 0;
   int count = 0;

   if (verb_id == 0) {
      // if followed by argument list - it is a custom verb
      if (arg == nsSubjectArg || verb == tsPrivate) {
         message.append('#');
         message.append(EVAL_MESSAGE_ID + 0x20);
         message.append('&');
         message.append(verb);
         count++;
      }
      // otherwise it is GET message
      else {
         message.append('#');
         message.append(GET_MESSAGE_ID + 0x20);
         message.append('&');
         message.append(verb);
      }
   }
   else {
      message.append('#');
      message.append(verb_id + 0x20);
   }

   // if it is a generic verb, make sure no parameters are provided
   if ((verb_id == SEND_MESSAGE_ID || verb_id == DISPATCH_MESSAGE_ID) && objectNode.firstChild() == nsSubjectArg) {
      scope.raiseError(errInvalidOperation, verb);
   }

   // if method has argument list
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      message.append('&');

      message.append(subject);

      count++;
      arg = arg.nextNode();
   }

   if (objectNode.nextNode() == nsSizeValue) {
      TerminalInfo size = objectNode.nextNode().Terminal();
      if (size == tsInteger) {
         count = StringHelper::strToInt(size.value);
      }
      else scope.raiseError(errInvalidOperation, size);
   }

   // define the number of parameters
   message[0] = message[0] + count;

   return ObjectInfo(okConstant, otMessage, scope.moduleScope->module->mapReference(message));
}

ObjectInfo Compiler :: saveObject(CodeScope& scope, ObjectInfo object, int mode)
{
   _writer.pushObject(*scope.tape, object);
   object.kind = okCurrent;

   return object;
}

inline bool checkIfBoxingRequired(ObjectInfo object)
{
   if (object.kind == okLocal || object.kind == okLocalAddress || object.kind == okRegister) {
      switch(object.type) {
         case otInt:         
         case otIntVar:
         case otLength:
         case otIndex:
         case otByte:
         case otLong:
         case otReal:
         case otShort:
         case otParams:
            return true;
      }
   }
   return false;
}

ObjectInfo Compiler :: boxObject(CodeScope& scope, ObjectInfo object, int mode)
{
   // boxing should be applied only for type local parameter
   if (object.kind == okLocal || object.kind == okRegister || object.kind == okCurrent || object.kind == okLocalAddress) {
      switch(object.type) {
         case otInt:
         case otLength:
         case otIndex:
         case otByte:
            _writer.boxObject(*scope.tape, 4, scope.moduleScope->mapConstantReference(INT_CLASS) | mskVMTRef, true);
            break;
         case otLong:
            _writer.boxObject(*scope.tape, 8, scope.moduleScope->mapConstantReference(LONG_CLASS) | mskVMTRef, true);
            break;
         case otReal:
            _writer.boxObject(*scope.tape, 8, scope.moduleScope->mapConstantReference(REAL_CLASS) | mskVMTRef, true);
            break;
         case otParams:
            _writer.boxArray(*scope.tape, scope.moduleScope->mapConstantReference(ARRAY_CLASS) | mskVMTRef);
            break;
      }
   }

   return object;
}

ObjectInfo Compiler :: compilePrimitiveLength(CodeScope& scope, ObjectInfo objectInfo, int mode)
{
   // if literal object is passed as a length argument
   if (objectInfo.type == otLiteral && mode == TypeCode::tInt) {
      ObjectInfo target;
      allocatePrimitiveObject(scope, mode, target);

      _writer.loadObject(*scope.tape, objectInfo);
      _writer.loadLiteralLength(*scope.tape, target);

      return ObjectInfo(okLocalAddress, target.type, target.reference);
   }
   // if bytearray object is passed as a length argument
   else if (objectInfo.type == otByteArray && mode == TypeCode::tInt) {
      ObjectInfo target;
      allocatePrimitiveObject(scope, mode, target);

      _writer.loadObject(*scope.tape, objectInfo);
      _writer.loadByteArrayLength(*scope.tape, target);

      return ObjectInfo(okLocalAddress, target.type, target.reference);
   }
   // if open arg is passed as a length argument
   else if (objectInfo.type == otParams && mode == TypeCode::tInt) {
      ObjectInfo target;
      allocatePrimitiveObject(scope, mode, target);

      _writer.loadObject(*scope.tape, objectInfo);
      _writer.loadParamsLength(*scope.tape, target);

      return ObjectInfo(okLocalAddress, target.type, target.reference);
   }
   else return ObjectInfo(okUnknown);
}

ObjectInfo Compiler :: compilePrimitiveLengthOut(CodeScope& scope, ObjectInfo objectInfo, int mode)
{
   // if literal object is passed as a length argument
   if (objectInfo.type == otLiteral && mode == TypeCode::tInt) {
      _writer.loadObject(*scope.tape, objectInfo);
      _writer.loadLiteralLength(*scope.tape, ObjectInfo(okCurrent, 0));

      return ObjectInfo(okIdle);
   }
   // if bytearray object is passed as a length argument
   else if (objectInfo.type == otByteArray && mode == TypeCode::tInt) {
      _writer.loadObject(*scope.tape, objectInfo);
      _writer.loadByteArrayLength(*scope.tape, ObjectInfo(okCurrent, 0));

      return ObjectInfo(okIdle);
   }
   else if (objectInfo.type == otParams && mode == TypeCode::tInt) {
      _writer.loadObject(*scope.tape, objectInfo);
      _writer.loadParamsLength(*scope.tape, ObjectInfo(okCurrent, 0));

      return ObjectInfo(okIdle);
   }
   else return ObjectInfo(okUnknown);
}

void Compiler :: compileMessageParameter(DNode& arg, CodeScope& scope, const wchar16_t* subject, int mode, size_t& count)
{
   if (arg == nsMessageParameter) {
      count++;

      ObjectInfo param = compileObject(arg.firstChild(), scope, mode & ~CTRL_DIRECT_PARAM);

      _writer.loadObject(*scope.tape, param);

      // box the object if required
      if (checkIfBoxingRequired(param) && !test(mode, CTRL_TYPED_MODE)) {
         boxObject(scope, param, mode);
      }
      if (test(mode, CTRL_DIRECT_PARAM)) {
         _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
      }
      else _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

      arg = arg.nextNode();
   }
   else if (arg == nsTypedMessageParameter) {
      count++;

      ObjectInfo param = compileObject(arg.firstChild(), scope, mode & ~CTRL_DIRECT_PARAM);
      if (IsPrimitiveArray(param.type) && test(mode, CTRL_TYPED_MODE) && (mode & ~CTRL_MASK) == TypeCode::tLen) {
         // if the length can be calculated directly
         param = compilePrimitiveLength(scope, param, TypeCode::tInt);
         if (param.kind != okUnknown) {
            _writer.loadObject(*scope.tape, param);

            if (test(mode, CTRL_DIRECT_PARAM)) {
               _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
            }
            else _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

            return;
         }
      }

      _writer.loadObject(*scope.tape, param);

      // box the object if required
      if (checkIfBoxingRequired(param) && !test(mode, CTRL_TYPED_MODE)) {
         boxObject(scope, param, mode);
      }

      _writer.pushObject(*scope.tape, ObjectInfo(okRegister));

      // send the subject to the parameter
      _writer.setMessage(*scope.tape, encodeMessage(scope.moduleScope->mapSubject(subject), GET_MESSAGE_ID, 0));
      _writer.callDispatcher(*scope.tape, 0, 0);
      if (test(mode, CTRL_DIRECT_PARAM)) {
         _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
      }
      else _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

      arg = arg.nextNode();
   }
}

ref_t Compiler :: mapMessage(DNode node, CodeScope& scope, size_t& count, int& mode)
{
   bool             simpleParameters = true;

   TerminalInfo     verb = node.Terminal();
   IdentifierString signature;
   bool             first = true;

   ref_t            verb_id = _verbs.get(verb.value);
   int              paramCount = 0;
   DNode            arg = node.firstChild();

   // if it is a dispatch operation
   if (arg == nsTypedMessageParameter && verb_id != 0) {
      count = 1;

      if (arg.firstChild().firstChild() == nsNone)
         mode |= CTRL_DIRECT_PARAM;

      return encodeMessage(0, verb_id, 0);
   }

   if (verb_id == 0) {
      signature.append(verb);

      // if followed by argument list - it is EVAL verb
      if (arg != nsNone) {
         verb_id = EVAL_MESSAGE_ID;

         first = false;
      }
      // otherwise it is GET message
      else verb_id = GET_MESSAGE_ID;
   }

   // if it is open argument list
   if (arg == nsMessageOpenParameter) {
      simpleParameters = false;

      // terminator
      count = 1;

      arg = arg.firstChild();
      while (arg != nsNone) {
         count++;

         arg = arg.nextNode();
      }

      paramCount = OPEN_ARG_COUNT;
   }
   else {
      // if message has generic argument list
      while (arg == nsMessageParameter) {
         count++;

         if (arg.firstChild().firstChild() != nsNone)
            simpleParameters = false;

         arg = arg.nextNode();
      }

      // if message has named argument list
      while (arg == nsSubjectArg) {
         int modePrefix = 0;
         TerminalInfo subject = arg.Terminal();

         if (!first) {
            signature.append('&');
         }
         else first = false;

         signature.append(subject);

         if (subject == tsReference) {
            modePrefix = definePrimitiveExprPrefix(scope.moduleScope->mapSubjectType(subject));
         }

         arg = arg.nextNode();

         // skip an argument
         if (arg == nsMessageParameter || arg == nsTypedMessageParameter) {
            count++;

            if (arg.firstChild().firstChild() != nsNone)
               simpleParameters = false;

            arg = arg.nextNode();
         }
      }
      paramCount = count;
   }

   // if it is out-assigning
   // out'type'xxx postfix should be added
   if (test(mode, CTRL_TYPEDOUT_MODE)) {
      switch(ModeToType(mode)) {
         case otShort:
            signature.append(ConstantIdentifier("&out'type'short"));
            break;
         case otInt:
            signature.append(ConstantIdentifier("&out'type'int"));
            break;
         case otLong:
            signature.append(ConstantIdentifier("&out'type'long"));
            break;
         case otReal:
            signature.append(ConstantIdentifier("&out'type'real"));
            break;
         default:
            // if it was not recognized
            // turn the mode off
            mode &= ~CTRL_TYPEDOUT_MODE;
            break;
      }
   }

   // if signature is presented
   ref_t sign_id = 0;
   if (!emptystr(signature)) {
      sign_id = scope.moduleScope->module->mapSubject(signature, false);
   }

   if (simpleParameters)
      mode |= CTRL_DIRECT_PARAM;

   // create a message id
   return encodeMessage(sign_id, verb_id, paramCount);
}

void Compiler :: compileDirectMessageParameters(DNode arg, CodeScope& scope, int mode)
{
   // if it is a dispatch operation
   if (arg == nsTypedMessageParameter) {
      _writer.pushObject(*scope.tape, compileObject(arg.firstChild(), scope, mode));
   }
   else if (arg == nsMessageParameter) {
      compileDirectMessageParameters(arg.nextNode(), scope, mode);

      ObjectInfo param = compileObject(arg.firstChild(), scope, mode);
      
      // box the object if required
      if (checkIfBoxingRequired(param) && !test(mode, CTRL_TYPED_MODE)) {
         _writer.loadObject(*scope.tape, param);

         boxObject(scope, param, mode);

         _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
      }
      else _writer.pushObject(*scope.tape, param);
   }
   else if (arg == nsSubjectArg) {
      int modePrefix = mode;
      TerminalInfo subject = arg.Terminal();

      if (subject == tsReference) {
         modePrefix = definePrimitiveExprPrefix(scope.moduleScope->mapSubjectType(subject), mode);
      }

      arg = arg.nextNode();

      compileDirectMessageParameters(arg.nextNode(), scope, mode);

      size_t dummy = 0;
      compileMessageParameter(arg, scope, subject, modePrefix, dummy);
   }
}

void Compiler :: compilePresavedMessageParameters(DNode arg, CodeScope& scope, int& mode)
{
   // if it is a dispatch operation
   if (arg == nsTypedMessageParameter) {
      _writer.loadObject(*scope.tape, compileObject(arg.firstChild(), scope, mode));
      _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 1));
   }
   else {
      size_t count = 0;

//      // if message has open argument list
      if (arg == nsMessageOpenParameter) {
         arg = arg.firstChild();

         while (arg != nsNone) {
            count++;

            ObjectInfo retVal = compileExpression(arg, scope, 0);
            _writer.loadObject(*scope.tape, retVal);
            boxObject(scope, retVal, mode);
            _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

            arg = arg.nextNode();
         }

         mode = CTRL_TYPED_MODE | TypeCode::tParams;
      }
      else {
         // if message has generic argument list
         while (arg == nsMessageParameter) {
            count++;

            ObjectInfo param = compileObject(arg.firstChild(), scope, mode);
            _writer.loadObject(*scope.tape, param);

            // box the object if required
            if (checkIfBoxingRequired(param) && !test(mode, CTRL_TYPED_MODE)) {
               boxObject(scope, param, mode);
            }

            _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, count));

            arg = arg.nextNode();
         }

         // if message has named argument list
         while (arg == nsSubjectArg) {
            int modePrefix = mode;
            TerminalInfo subject = arg.Terminal();

            if (subject == tsReference) {
               modePrefix = definePrimitiveExprPrefix(scope.moduleScope->mapSubjectType(subject), mode);
            }

            arg = arg.nextNode();

            compileMessageParameter(arg, scope, subject, modePrefix, count);
         }
      }
   }
}

ref_t Compiler :: compileMessageParameters(DNode node, CodeScope& scope, ObjectInfo object, int mode, size_t& spaceToRelease)
{
   size_t count = 0;
   ref_t messageRef = mapMessage(node, scope, count, mode);

   // if the target is in register or is a symbol, direct message compilation is not possible
   if (object.kind == okRegister || object.kind == okSymbol) {
      mode &= ~CTRL_DIRECT_PARAM;
   }
   else if (count == 1) {
      mode |= CTRL_DIRECT_PARAM;
   }

   // if it is primtive get length operation - do nothing
   if (IsPrimitiveArray(object.type)/* && test(mode, CTRL_PRIMITIVE_MODE) && ifAny(mode & ~CTRL_MASK, CTRL_INT_EXPR, CTRL_LEN_EXPR )*/
      && (messageRef == encodeMessage(scope.moduleScope->lengthSubject, GET_MESSAGE_ID, 0)
      || (messageRef == encodeMessage(scope.moduleScope->lengthOutSubject, GET_MESSAGE_ID, 0)))) 
   {
   }
   // if only simple arguments are used we could directly save parameters
   else if (test(mode, CTRL_DIRECT_PARAM)) {
      compileDirectMessageParameters(node.firstChild(), scope, mode);

      _writer.loadObject(*scope.tape, object);
      object = boxObject(scope, object, mode);

      _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
   }
   // otherwise the space should be allocated first, 
   // to garantee the correct order of parameter evaluation
   else {
      _writer.declareArgumentList(*scope.tape, count + 1);
      _writer.loadObject(*scope.tape, object);

      object = boxObject(scope, object, mode);

      _writer.saveObject(*scope.tape, ObjectInfo(okCurrent));

      compilePresavedMessageParameters(node.firstChild(), scope, mode);
   }

   // if open argument list was used indicate that extra space should be released after the message call
   if ((mode & ~CTRL_MASK) == TypeCode::tParams) {
      spaceToRelease =  count;
   }

   return messageRef;
}

ObjectInfo Compiler :: compileBranchingOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode, int operator_id)
{
   _writer.loadObject(*scope.tape, object);

   DNode elsePart = node.select(nsElseOperation);
   if (elsePart != nsNone) {
      _writer.declareThenElseBlock(*scope.tape);
      compileBranching(node, scope, operator_id, CTRL_PREV_EXIT_MODE);
      _writer.declareElseBlock(*scope.tape); 
      compileBranching(elsePart, scope, 0, 0); // for optimization, the condition is checked only once
      _writer.endThenBlock(*scope.tape);
   }
   else if (test(mode, CTRL_LOOP)) {
      compileBranching(node, scope, operator_id, CTRL_PREV_EXIT_MODE);
      _writer.jump(*scope.tape, true);
   }
   else {
      _writer.declareThenBlock(*scope.tape);
      compileBranching(node, scope, operator_id, 0);
      _writer.endThenBlock(*scope.tape);
   }

   return ObjectInfo(okRegister, 0);
}

ObjectInfo Compiler :: compileOperator(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   TerminalInfo operator_name = node.Terminal();
   int operator_id = _operators.get(operator_name);

   // if it is branching operators
   if (operator_id == IF_MESSAGE_ID || operator_id == IFNOT_MESSAGE_ID) {
      return compileBranchingOperator(node, scope, object, mode, operator_id);
   }
   // others
   else {
      // if the left operand is a result of operation / symbol, use the normal routine
      if (object.kind == okRegister || object.kind == okSymbol) {
         _writer.declareArgumentList(*scope.tape, 2);
         _writer.loadObject(*scope.tape, object);

         _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 0));
      }

      // if the loperand is primtive data
      // pass a note to external / primitive routine
      ObjectInfo operand = compileExpression(node, scope, definePrimitiveExprPrefix(object.type, mode & ~CTRL_TYPEDOUT_MODE));

      recordStep(scope, node.Terminal(), dsProcedureStep);

      // if it is primitive operations
      if (IsPrimitiveOperation(operator_id, object, operand, mode)) {
         return compilePrimitiveOperator(node, scope, operator_id, object, operand, mode);
      }
      else {
         // if the left operand is a result of operation / symbol, use the normal routine
         if (object.kind == okRegister || object.kind == okSymbol) {
            _writer.loadObject(*scope.tape, operand);

            operand = boxObject(scope, operand, mode);

            _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 1));

            // if we are unlucky load and check if the operator target should be boxed
            if (checkIfBoxingRequired(object)) {
               _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 0));
               object = boxObject(scope, ObjectInfo(okRegister, object), mode);
               _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 0));
            }
         }
         // otherwise use simplified algorithm
         else {
            if (checkIfBoxingRequired(operand)) {
               _writer.loadObject(*scope.tape, operand);

               operand = boxObject(scope, operand, mode);

               _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
            }
            else _writer.pushObject(*scope.tape, operand);

            if (checkIfBoxingRequired(object)) {
               _writer.loadObject(*scope.tape, object);
               object = boxObject(scope, object, mode);
               _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
            }
            else _writer.pushObject(*scope.tape, object);
         }

         // if it is an operation with nil
         if ((object.kind == okConstant || object.kind == okSymbol) && object.reference == scope.moduleScope->nilReference) {
            if (operator_id == EQUAL_MESSAGE_ID) {
               _writer.compare(*scope.tape, scope.moduleScope->trueReference, scope.moduleScope->falseReference);

               return ObjectInfo(okRegister, 0);
            }
            else if (operator_id == NOTEQUAL_MESSAGE_ID) {
               _writer.compare(*scope.tape, scope.moduleScope->falseReference, scope.moduleScope->trueReference);

               return ObjectInfo(okRegister, 0);
            }
         }

         int message_id = encodeMessage(0, operator_id, 1);

         _writer.setMessage(*scope.tape, message_id);
         _writer.callDispatcher(*scope.tape, 0, 1);
      }
   }

   return ObjectInfo(okRegister, 0);
}

ObjectInfo Compiler :: compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int messageRef, int mode)
{
   if (messageRef == 0)
      scope.raiseError(errUnknownMessage, node.Terminal());

   int signRef = getSignature(messageRef);
   int paramCount = getParamCount(messageRef);
   bool catchMode = test(mode, CTRL_CATCH_MODE);

   // if it is generic dispatch
   if (paramCount == 0 && node.firstChild() == nsTypedMessageParameter) {
      _writer.dispatchVerb(*scope.tape, messageRef, 1, 0);
   }
   // if it could be replaced with direct type'length operation
   else if (IsPrimitiveArray(object.type) && messageRef == encodeMessage(scope.moduleScope->lengthSubject, GET_MESSAGE_ID, 0))
   {
      object = compilePrimitiveLength(scope, object, TypeCode::tInt);
      if (object.kind == okUnknown) {
         scope.raiseError(errInvalidOperation, node.Terminal());
      }
      else return object;
   }
   // if it could be replaced with direct out type'length operation
   else if (IsPrimitiveArray(object.type) && messageRef == encodeMessage(scope.moduleScope->lengthOutSubject, GET_MESSAGE_ID, 0))
   {
      object = compilePrimitiveLengthOut(scope, object, TypeCode::tInt);
      if (object.kind == okUnknown) {
         scope.raiseError(errInvalidOperation, node.Terminal());
      }

      return object;
   }
   // otherwise compile message call
   else {
      _writer.setMessage(*scope.tape, messageRef);

   //   bool directMode = test(_optFlag, optDirectConstant);

      recordStep(scope, node.Terminal(), dsProcedureStep);

      if (/*directMode && */object.kind == okConstant) {
         // if static message is sent to a constant class
         if (object.type == otClass) {
            _writer.callResolvedMethod(*scope.tape, object.extraparam, messageRef, 0);
         }
         // if static message is sent to a integer constant class
         else if (object.type == otInt) {
            _writer.callResolvedMethod(*scope.tape, scope.moduleScope->mapConstantReference(INT_CLASS),
               messageRef, 0);
         }
         // if static message is sent to a long integer constant class
         else if (object.type == otLong) {
            _writer.callResolvedMethod(*scope.tape, scope.moduleScope->mapConstantReference(LONG_CLASS),
               messageRef, 0);
         }
         // if static message is sent to a integer constant class
         else if (object.type == otReal) {
            _writer.callResolvedMethod(*scope.tape, scope.moduleScope->mapConstantReference(REAL_CLASS), messageRef, 0);
         }
         // if static message is sent to a integer constant class
         else if (object.type == otLiteral) {
            _writer.callResolvedMethod(*scope.tape, scope.moduleScope->mapConstantReference(WSTR_CLASS), messageRef, 0);
         }
         // if external role is provided
         else if (object.type == otRole) {
      //      if (directMode) {
               _writer.callResolvedMethod(*scope.tape, object.reference, messageRef);
      //      }
      //      else _writer.sendRoleMessage(*scope.tape, object.reference, paramCount);
         }
         else _writer.callResolvedMethod(*scope.tape, object.reference, messageRef, 0);
      }
      // if message sent to the class parent
      else if (object.kind == okSuper) {
         _writer.callResolvedSelfMessage(*scope.tape, object.reference, messageRef);
      }
      // if run-time external role is provided
      else if (object.type == otRole) {
         _writer.callRoleMessage(*scope.tape, paramCount);
      }
      else if (catchMode) {
         _writer.callRoleMessage(*scope.tape, scope.moduleScope->mapConstantReference(EXCEPTION_ROLE) , paramCount);
         _writer.nextCatch(*scope.tape);
      }
      else _writer.callDispatcher(*scope.tape, 0, paramCount);
   }

   if (test(mode, CTRL_TYPED_MODE) && getVerb(messageRef) == GET_MESSAGE_ID) {
      return ObjectInfo(okRegister, scope.moduleScope->mapSubjectType(signRef), paramCount);
   }
   else if (test(mode, CTRL_TYPEDOUT_MODE)) {
      // if it is out-assigning
      // the assigning should not be done once again
      return ObjectInfo(okIdle);
   }
   else return ObjectInfo(okRegister);   
}

ObjectInfo Compiler :: compileMessage(DNode node, CodeScope& scope, ObjectInfo object, int mode)
{
   size_t spaceToRelease = 0;
   ref_t messageRef = compileMessageParameters(node, scope, object, getTypedOutMode(mode), spaceToRelease);
   
   if (spaceToRelease > 0) {
      // if open argument list is used
      ObjectInfo retVal = compileMessage(node, scope, object, messageRef, mode);

      // it should be removed from the stack
      _writer.releaseObject(*scope.tape, spaceToRelease);

      return retVal;
   }
   else return compileMessage(node, scope, object, messageRef, mode);
}

ObjectInfo Compiler :: compileEvalMessage(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   size_t count = countSymbol(node, nsMessageParameter);

   _writer.declareArgumentList(*scope.tape, count + 1);
   _writer.loadObject(*scope.tape, object);

   object = boxObject(scope, object, mode);

   _writer.saveObject(*scope.tape, ObjectInfo(okCurrent));

   compilePresavedMessageParameters(node, scope, mode);

   // skip all except the last message parameter
   while (node.nextNode() == nsMessageParameter)
      node = node.nextNode();

   return compileMessage(node, scope, object, encodeMessage(0, EVAL_MESSAGE_ID, count), mode);
}

ObjectInfo Compiler :: compileOperations(DNode node, CodeScope& scope, ObjectInfo object, int mode)
{
   ObjectInfo currentObject = object;

   DNode member = node.nextNode();

   if (object.type == otControl) {
      currentObject = compileControlVirtualExpression(member, scope, currentObject, mode);
      if (currentObject.kind == okUnknown) {
         currentObject = object;
      }
      else return currentObject;
   }
   else if (object.kind == okExternal) {
      currentObject = compileExternalCall(member, scope, node.Terminal(), mode);
      if (test(mode, CTRL_TRY_MODE)) {
         mode &= ~CTRL_TRY_MODE;

         member = member.nextNode();
         compilePrimitiveCatch(member, scope);            
      }
      member = member.nextNode();
   }

   bool catchMode = false;
   if (test(mode, CTRL_TRY_MODE)) {
      _writer.declareTry(*scope.tape);

      mode &= ~CTRL_TRY_MODE;
   }

   while (member != nsNone) {
      if (member == nsExtension) {
         currentObject = compileExtension(member, scope, currentObject, mode);
      }
      else if (member==nsMessageOperation) {
         currentObject = compileMessage(member, scope, currentObject, mode);
      }
      else if (member==nsMessageParameter) {
         currentObject = compileEvalMessage(member, scope, currentObject, mode);
      }
      else if (member == nsSwitching) {
         compileSwitch(member, scope, currentObject);

         currentObject = ObjectInfo(okRegister);
      }
      else if (member == nsAssigning) {
         if (currentObject.type != otNone) {
            ObjectInfo info = compileExpression(member.firstChild(), scope, definePrimitiveExprPrefix(currentObject.type));

            _writer.loadObject(*scope.tape, info);

            compileStackAssignment(member, scope, currentObject, info);
         }
         else {
            ObjectInfo info = compileExpression(member.firstChild(), scope, 0);

            _writer.loadObject(*scope.tape, info);
            info = boxObject(scope, info, mode);

            compileAssignment(member, scope, currentObject);
         }

         currentObject = ObjectInfo(okRegister);
      }
      else if (member == nsAltMessageOperation) {
         if (!catchMode) {
            _writer.declareCatch(*scope.tape);
            catchMode = true;
         }
         currentObject = compileMessage(member, scope, ObjectInfo(okRegister), mode | CTRL_CATCH_MODE);
      }
      else if (member == nsL3Operation || member == nsL4Operation || member == nsL5Operation || member == nsL6Operation 
         || member == nsL7Operation || member == nsL0Operation) 
      {
         currentObject = compileOperator(member, scope, currentObject, mode);
      }
      member = member.nextNode();
   }

   if (catchMode) {
      _writer.endCatch(*scope.tape);
   }

   return currentObject;
}

ObjectInfo Compiler :: compileExtension(DNode& node, CodeScope& scope, ObjectInfo object, int mode)
{
   ModuleScope* moduleScope = scope.moduleScope;
   ObjectInfo   role;

   DNode roleNode = node.firstChild();
   // check if the extension can be used as a static role (it is constant)
   if (roleNode.firstChild() == nsNone) {
      int flags = 0;

      role = scope.mapObject(roleNode.Terminal());
      if (role.kind == okSymbol || role.kind == okConstant) {
         // if the symbol is used inside itself
         if (role.reference == scope.getClassRefId()) {
            flags = scope.getClassFlags();
         }
         // otherwise
         else {
            ClassInfo roleClass;
            moduleScope->loadClassInfo(roleClass, moduleScope->module->resolveReference(role.reference));

            flags = roleClass.header.flags;
         }
      }
      // if the symbol VMT can be used as an external role
      if (test(flags, elStateless)) {
         role = ObjectInfo(okConstant, otRole, role.reference);
      }
      else role = ObjectInfo(okSymbol, otRole);
   }
   else role = ObjectInfo(okSymbol, otRole);

   // override standard message compiling routine
   node = node.nextNode();

   size_t spaceToRelease = 0;
   ref_t messageRef = compileMessageParameters(node, scope, object, 0, spaceToRelease);

   // if it is a not a constant, compile a role
   if (role.kind != okConstant)
      _writer.loadObject(*scope.tape, compileObject(roleNode, scope, mode));

   // send message to the role
   ObjectInfo retVal = compileMessage(node, scope, role, messageRef, mode);

   if (spaceToRelease > 0)
      _writer.releaseObject(*scope.tape, spaceToRelease);

   return retVal;
}

void Compiler :: compileActionVMT(DNode node, InlineClassScope& scope, bool withParameters)
{
   _writer.declareClass(scope.tape, scope.reference);

   MethodScope methodScope(&scope);
   methodScope.message = encodeVerb(SEND_MESSAGE_ID);

   ref_t actionMessage = encodeVerb(EVAL_MESSAGE_ID);
   if (withParameters) {
      // define message parameter
      DNode args = node.firstChild();
      actionMessage = declareInlineArgumentList(args, methodScope);

      node = node.select(nsSubCode);
   }

   // if it is single expression
   DNode expr = node.firstChild();
   if (expr == nsExpression && expr.nextNode() == nsNone) {
      if (!withParameters)
         actionMessage = encodeVerb(EVAL_MESSAGE_ID);

      compileInlineAction(expr, methodScope, actionMessage);
   }
   else compileAction(node, methodScope, actionMessage);

   _writer.endClass(scope.tape);

   // stateless inline class
   if (scope.info.fields.Count()==0 && !test(scope.info.header.flags, elStructureRole)) {
      scope.info.header.flags |= elStateless;
   }
   else scope.info.header.flags &= ~elStateless;

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   scope.save(_writer);
}

void Compiler :: compileNestedVMT(DNode node, InlineClassScope& scope)
{
   _writer.declareClass(scope.tape, scope.reference);

   DNode member = node.firstChild();
//   if (member == nsActionExpression) {
//      // if it is an action symbol with a parameter
//      compileVMT(node, scope);
//   }
//   else if (node == nsOverride) {
//      compileVMT(member, scope);
//
//      // compile extender
//      scope.info.header.flags |= elVMTAnyHandler;
//
//      // for any handler message id is 0
//      MethodScope methodScope(&scope, TERMINAL_MESSAGE_ID);
//
//      compileRedirectMethod(member.firstChild(), methodScope, etOverride, false);
//   }
   /*else */compileVMT(member, scope);

   _writer.endClass(scope.tape);

   // stateless inline class
   if (scope.info.fields.Count()==0 && !test(scope.info.header.flags, elStructureRole)) {
      scope.info.header.flags |= elStateless;
   }
   else scope.info.header.flags &= ~elStateless;

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   scope.save(_writer);
}

ObjectInfo Compiler :: compileNestedExpression(DNode node, CodeScope& ownerScope, InlineClassScope& scope, int mode)
{
   if (test(scope.info.header.flags, elStateless)) {
      // if it is a stateless class

      return ObjectInfo(okConstant, scope.reference);
   }
   else {
      // dynamic binary symbol
      if (test(scope.info.header.flags, elStructureRole)) {
         _writer.newStructure(*ownerScope.tape, scope.info.size, scope.reference);

         if (scope.outers.Count() > 0)
            scope.raiseError(errInvalidInlineClass, node.Terminal());
      }
      // dynamic normal symbol
      else _writer.newObject(*ownerScope.tape, scope.info.fields.Count(), scope.reference, scope.moduleScope->nilReference);

      Map<const wchar16_t*, InlineClassScope::Outer>::Iterator outer_it = scope.outers.start();
      while(!outer_it.Eof()) {
         ObjectInfo info = (*outer_it).outerObject;

         //NOTE: info should be either fields or locals
         // if outerfield is used, the accumulator should be preserved
         if (info.kind == okOuterField) {
            _writer.pushObject(*ownerScope.tape, ObjectInfo(okRegister));
            _writer.pushObject(*ownerScope.tape, info);
            _writer.popObject(*ownerScope.tape, ObjectInfo(okRegister));
         }
         else _writer.pushObject(*ownerScope.tape, info);
         _writer.popObject(*ownerScope.tape, ObjectInfo(okRegisterField, (*outer_it).reference));

         outer_it++;
      }

      return ObjectInfo(okRegister);
   }
}

ObjectInfo Compiler :: compileNestedExpression(DNode node, CodeScope& ownerScope, int mode)
{
//   recordStep(ownerScope, node.Terminal(), dsStep);

   InlineClassScope scope(&ownerScope, mapNestedExpression(ownerScope, mode));

   // nested class is sealed
   scope.info.header.flags |= elSealed;

   // if it is an action code block
   if (node == nsSubCode) {
      compileParentDeclaration(scope.moduleScope->mapConstantReference(ACTION_CLASS), scope);

      compileActionVMT(node, scope, false);
   }
   // if it is an action code block
   else if (node == nsInlineExpression) {
      compileParentDeclaration(scope.moduleScope->mapConstantReference(ACTION_CLASS), scope);

      compileActionVMT(node, scope, true);
   }
   // if it is inherited symbol expression
   else if (node.Terminal() != nsNone) {
	   // inherit parent
      compileParentDeclaration(node, scope);

      compileNestedVMT(node.firstChild(), scope);
   }
   // if it is normal symbol expression
   else {
      compileParentDeclaration(DNode(), scope);

      compileNestedVMT(node, scope);
   }
   return compileNestedExpression(node, ownerScope, scope, mode & ~CTRL_ROOT);
}

ObjectInfo Compiler :: compileCollection(DNode objectNode, CodeScope& scope, int mode)
{
   return compileCollection(objectNode, scope, mode, scope.moduleScope->mapConstantReference(ARRAY_CLASS));
}

ObjectInfo Compiler :: compileCollection(DNode node, CodeScope& scope, int mode, ref_t vmtReference)
{
   int counter = 0;

   // all collection memebers should be created before the collection itself
   while (node != nsNone) {
      saveObject(scope, compileExpression(node, scope, mode), mode);

      node = node.nextNode();
      counter++;
   }

   // create the collection
   _writer.newObject(*scope.tape, counter, vmtReference, scope.moduleScope->nilReference);

   // assign the members
   while (counter > 0) {
      _writer.popObject(*scope.tape, ObjectInfo(okRegisterField, counter - 1));

      counter--;
   }

   return ObjectInfo(okRegister);
}

ObjectInfo Compiler :: compileGetProperty(DNode node, CodeScope& scope, int mode, ref_t vmtReference)
{
   // compile property subject
   saveObject(scope, compileMessageReference(node, scope, mode), mode);

   // compile property content
   saveObject(scope, compileExpression(node.nextNode(), scope, mode), mode);

   // create the collection
   _writer.newObject(*scope.tape, 2, vmtReference, scope.moduleScope->nilReference);

   // assign the members
   _writer.popObject(*scope.tape, ObjectInfo(okRegisterField, 1));
   _writer.popObject(*scope.tape, ObjectInfo(okRegisterField, 0));

   return ObjectInfo(okRegister);
}

ObjectInfo Compiler :: compileGetProperty(DNode objectNode, CodeScope& scope, int mode)
{
   return compileGetProperty(objectNode, scope, mode, scope.moduleScope->mapConstantReference(GETPROPERTY_CLASS));
}

ObjectInfo Compiler :: compileRetExpression(DNode node, CodeScope& scope, int mode)
{
   ObjectInfo info = compileExpression(node, scope, mode);

   _writer.loadObject(*scope.tape, info);

   boxObject(scope, ObjectInfo(okRegister, info.type), mode);

   compileEndStatement(node, scope);

   _writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

   return ObjectInfo(okRegister);
}

ObjectInfo Compiler :: compileExpression(DNode node, CodeScope& scope, int mode)
{
   DNode member = node.firstChild();

   ObjectInfo objectInfo;
   if (member==nsObject) {
      objectInfo = compileObject(member, scope, mode & ~CTRL_TYPEDOUT_MODE);
   }

   if (member != nsNone) {
      if (findSymbol(member, nsAltMessageOperation)) {
         objectInfo = compileOperations(member, scope, objectInfo, (mode | CTRL_TRY_MODE) & ~CTRL_ROOT);
      }
      else objectInfo = compileOperations(member, scope, objectInfo, mode & ~CTRL_ROOT);
   }

   return objectInfo;
}

ObjectInfo Compiler :: compileBranching(DNode thenNode, CodeScope& scope, int verb, int subCodeMode)
{
   // execute the block if the condition
   // is true / false
   if (verb == IF_MESSAGE_ID || verb == IFNOT_MESSAGE_ID) {
      _writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

      _writer.jumpIfEqual(*scope.tape, (verb == IF_MESSAGE_ID) ? scope.moduleScope->trueReference : scope.moduleScope->falseReference);
   }

   CodeScope subScope(&scope);
   DNode thenCode = thenNode.firstChild().firstChild();
   if (thenCode.firstChild().nextNode() != nsNone) {
      compileCode(thenCode, subScope, subCodeMode );
   }
   // if it is inline action
   else compileRetExpression(thenCode.firstChild(), scope, 0);

   return ObjectInfo(okRegister);
}

ObjectInfo Compiler :: compileControlVirtualExpression(DNode messageNode, CodeScope& scope, ObjectInfo info, int mode)
{
   int oldBreakLabel = scope.breakLabel;
   size_t size = 0;
   ref_t message = mapMessage(messageNode, scope, size);
   ref_t verb = getVerb(message);
   // if it is control while&do
   if (getSignature(message) == scope.moduleScope->whileSignRef && (verb == EVAL_MESSAGE_ID)) {
      DNode condNode = messageNode.select(nsMessageParameter);
      DNode loopNode = goToSymbol(condNode.nextNode(), nsMessageParameter);

      // check if the loop body can be virtually implemented
      // if it is not possible - leave the procedure 
      if (loopNode.firstChild().firstChild() != nsSubCode)
         return ObjectInfo();

      scope.breakLabel = _writer.declareLabel(*scope.tape);
      _writer.declareLoop(*scope.tape/*, true*/);

      // condition inline action should be compiled as a normal expression
      ObjectInfo cond;      
      condNode = condNode.firstChild();
      if (condNode.firstChild() == nsSubCode) {
         cond = compileExpression(condNode.firstChild().firstChild(), scope, mode);
      }
      else cond = compileObject(condNode, scope, mode);

      // get the current value
      _writer.loadObject(*scope.tape, cond);

      compileBranching(loopNode, scope, IF_MESSAGE_ID, 0);

      _writer.endLoop(*scope.tape);
      _writer.loadObject(*scope.tape, ObjectInfo(okConstant, scope.moduleScope->nilReference | mskConstantRef));
      // !!NOTE: break label should be resolved after assigning nil as a result of the loop
      _writer.endLabel(*scope.tape);

      // restore previous label
      scope.breakLabel = oldBreakLabel;

      return ObjectInfo(okRegister);
   }
   // if it is control do&until
   else if (getSignature(message) == scope.moduleScope->untilSignRef && (verb == DO_MESSAGE_ID)) {
      DNode loopNode = messageNode.select(nsMessageParameter);
      DNode condNode = goToSymbol(loopNode.nextNode(), nsMessageParameter);

      scope.breakLabel = _writer.declareLoop(*scope.tape/*, true*/);

      CodeScope subScope(&scope);

      DNode code = loopNode.firstChild().firstChild();
      // check if the loop body can be virtually implemented
      // if it is not possible - leave the procedure 
      if (code != nsSubCode)
         return ObjectInfo();

      compileCode(code, subScope);

      // condition inline action should be compiled as a normal expression
      //!! temporal: make sure the condition is simple or compile the whole code
      ObjectInfo cond = compileExpression(condNode.firstChild().firstChild().firstChild(), scope, mode);
      _writer.loadObject(*scope.tape, cond);

      _writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);
      _writer.jumpIfNotEqual(*scope.tape, scope.moduleScope->trueReference);
      _writer.endLoop(*scope.tape);

      // restore previous label
      scope.breakLabel = oldBreakLabel;

      return ObjectInfo(okRegister);
   }
   else return ObjectInfo(okUnknown);
}

void Compiler :: compileThrow(DNode node, CodeScope& scope, int mode)
{
   compileExpression(node.firstChild(), scope, mode);
   _writer.throwCurrent(*scope.tape);
}

void Compiler :: compileBreak(DNode node, CodeScope& scope, int mode)
{
   //scope codeScope.breakLabel

   ObjectInfo retVal = compileExpression(node.firstChild(), scope, mode);
   if (retVal.kind != okRegister) {
      _writer.popObject(*scope.tape, ObjectInfo(okRegister));
   }

   _writer.breakLoop(*scope.tape, scope.breakLabel);

   // if it is not a virtual mode, turn the break handler on
   if (scope.breakLabel < 0) {
      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);
      methodScope->withBreakHandler = true;
   }
}

void Compiler :: compileLoop(DNode node, CodeScope& scope, int mode)
{
   _writer.declareLoop(*scope.tape/*, true*/);

   compileExpression(node.firstChild(), scope, mode);

   _writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);

   _writer.jumpIfNotEqual(*scope.tape, scope.moduleScope->nilReference);
   _writer.endLoop(*scope.tape);
}

void Compiler :: compileEndStatement(DNode node, CodeScope& scope)
{
   if (scope.reserved > 0) {
      scope.reserved = scope.saved;
   }
}

void Compiler :: compileCode(DNode node, CodeScope& scope, int mode)
{
   bool needVirtualEnd = true;
   DNode statement = node.firstChild();

   // skip subject argument
   while (statement == nsSubjectArg || statement == nsMethodParameter)
      statement= statement.nextNode();

   while (statement != nsNone) {
      DNode hints = skipHints(statement);

      _writer.declareStatement(*scope.tape);

      switch(statement) {
         case nsExpression:
            compileExpression(statement, scope, 0);
            break;
         case nsThrow:
            compileThrow(statement, scope, 0);
            break;
         case nsBreak:
            compileBreak(statement, scope, 0);
            break;
         case nsLoop:
            compileLoop(statement, scope, CTRL_LOOP);
            break;
         case nsRetStatement:
         {
            compileRetExpression(statement.firstChild(), scope, 0);
            compileEndStatement(node, scope);

            _writer.gotoEnd(*scope.tape, test(mode, CTRL_PREV_EXIT_MODE) ? baPreviousLabel : baCurrentLabel);
            break;
         }           
         case nsVariable:
            compileVariable(statement, scope, hints);
            break;
         case nsCodeEnd:
            needVirtualEnd = false;
            recordStep(scope, statement.Terminal(), dsEOP);
            break;
      }
      compileEndStatement(node, scope);

      statement = statement.nextNode();
   }

   if (needVirtualEnd)
      _writer.declareBreakpoint(*scope.tape, 0, 0, 0, dsVirtualEnd);
}

void Compiler :: compileExternalArguments(DNode arg, CodeScope& scope, ExternalScope& externalScope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   bool literalParam = false;
   while (arg == nsSubjectArg) {
      ExternalScope::ParamInfo param;
      bool output = false;
      param.subject = moduleScope->mapSubject(arg.Terminal(), output);
      param.index = externalScope.operands.Count() + 1;
      param.output = output ? -1 : 0;

      arg = arg.nextNode();
      if (arg == nsMessageParameter || arg == nsTypedMessageParameter) {
         ObjectInfo info = compileObject(arg.firstChild(), scope, 0);
         saveObject(scope, info, 0);

         // if it is typed message, subject should be sent to the parameter
         if (arg == nsTypedMessageParameter) {
            _writer.setMessage(*scope.tape, encodeMessage(param.subject, GET_MESSAGE_ID, 0));
            _writer.callDispatcher(*scope.tape, 0);
            _writer.pushObject(*scope.tape, ObjectInfo(okRegister));

            info = ObjectInfo(okSymbol);
         }

         // HOTFIX: if it is a literal length output parameter
         // special code should be used to store it
         if (param.subject == moduleScope->lengthSubject && output) {
            if (info.type == otLiteral) {
               param.output = -2;
            }
         }

         // if it is nil
         if (info.type == otNone && (info.kind == okSymbol || info.kind == okConstant) && info.reference == moduleScope->nilReference) {
            param.offset = -1;

            // we do not store nil
            _writer.releaseObject(*scope.tape);

            if (!param.output)
               scope.raiseError(errInvalidOperation, arg.firstChild().Terminal());
         }
         else {
            externalScope.frameSize++;
            param.offset = externalScope.frameSize;
         }

         arg = arg.nextNode();
      }
      else scope.raiseError(errInvalidOperation, arg.Terminal());

      externalScope.operands.push(param);
   }
}

void Compiler :: reserveExternalOutputParameters(CodeScope& scope, ExternalScope& externalScope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   // prepare output parameters
   Stack<ExternalScope::ParamInfo>::Iterator out_it = externalScope.operands.start();
   while (!out_it.Eof()) {
      if ((*out_it).subject == moduleScope->wideStrSubject) {
         externalScope.frameSize++;

         if ((*out_it).output) {
            ExternalScope::OutputInfo output;
            output.subject = (*out_it).subject;
            output.target = (*out_it).offset;   
            output.offset = externalScope.frameSize;
            output.type = (*out_it).output;

            externalScope.output.push(output);
         }

         // for widestr parameter, index is used to refer to the original text
         (*out_it).index = (*out_it).offset;
         (*out_it).offset = externalScope.frameSize;

         _writer.declareVariable(*scope.tape);
      }
      else if ((*out_it).subject == moduleScope->dumpSubject) {
         externalScope.frameSize++;

         if ((*out_it).output) {
            ExternalScope::OutputInfo output;
            output.subject = (*out_it).subject;
            output.target = (*out_it).offset;   
            output.offset = externalScope.frameSize;

            externalScope.output.push(output);
         }

         // for dump parameter, index is used to refer to the original text
         (*out_it).index = (*out_it).offset;
         (*out_it).offset = externalScope.frameSize;

         _writer.declareVariable(*scope.tape);
      }
      else if ((*out_it).output) {
         ExternalScope::OutputInfo output;
         output.subject = (*out_it).subject;

         if (output.subject == moduleScope->intSubject || output.subject == moduleScope->lengthSubject) 
         {
            externalScope.frameSize++;

            output.target = (*out_it).offset;
            output.offset = externalScope.frameSize;
            output.type = (*out_it).output;
            (*out_it).offset = externalScope.frameSize;

            _writer.declareVariable(*scope.tape);
         }

         externalScope.output.push(output);
      }

      out_it++;
   }   
}

void Compiler :: reserveExternalLiteralParameters(CodeScope& scope, ExternalScope& externalScope)
{
   ModuleScope* moduleScope = scope.moduleScope;

   Stack<ExternalScope::ParamInfo>::Iterator out_it = externalScope.operands.start();
   while (!out_it.Eof()) {
      if ((*out_it).subject == moduleScope->wideStrSubject) {
         _writer.loadObject(*scope.tape, ObjectInfo(okBlockLocal, (*out_it).index));
         _writer.saveStr(*scope.tape, (*out_it).output);
         _writer.saveObject(*scope.tape, ObjectInfo(okBlockLocal, (*out_it).offset));
      }
      else if ((*out_it).subject == moduleScope->dumpSubject) {
         _writer.loadObject(*scope.tape, ObjectInfo(okBlockLocal, (*out_it).index));
         _writer.saveDump(*scope.tape, (*out_it).output);
         _writer.saveObject(*scope.tape, ObjectInfo(okBlockLocal, (*out_it).offset));
      }
      out_it++;
   }   
}

ObjectInfo Compiler :: compileExternalCall(DNode node, CodeScope& scope, const wchar16_t* dllName, int mode)
{
   ObjectInfo retVal(okRegister);

   ModuleScope* moduleScope = scope.moduleScope;

   ReferenceNs name(DLL_NAMESPACE);
   name.combine(dllName + strlen(EXTERNAL_MODULE) + 1);
   name.append(".");
   name.append(node.Terminal());

   ref_t reference = moduleScope->module->mapReference(name);

   // compile argument list
   ExternalScope externalScope;

   _writer.declareExternalBlock(*scope.tape);

   compileExternalArguments(node.firstChild(), scope, externalScope);

   // close the managed stack
   _writer.exclude(*scope.tape);

   // prepare output parameters / widestr references
   reserveExternalOutputParameters(scope, externalScope);

   // prepare widestr parameters;
   reserveExternalLiteralParameters(scope, externalScope);

   // prepare the function output if required
   if (test(mode, CTRL_TYPED_MODE)) {
      // if the output parameter is required
      switch (mode & ~CTRL_MASK) {
         case TypeCode::tInt:
         case 0:
         {
            allocatePrimitiveObject(scope, TypeCode::tInt, retVal);
            break;
         }
      }
   }

   // save function parameters
   int count = externalScope.operands.Count();
   while (externalScope.operands.Count() > 0) {
      ExternalScope::ParamInfo param = externalScope.operands.pop();

      if (param.subject == moduleScope->wideStrSubject || param.subject == moduleScope->dumpSubject) {
         _writer.pushObject(*scope.tape, ObjectInfo(okBlockLocal, param.offset));
      }
      else if (param.output) {
         _writer.pushObject(*scope.tape, ObjectInfo(okBlockLocalAddress, param.offset));
      }
      else if (param.subject == moduleScope->intSubject || param.subject == moduleScope->handleSubject) {
         _writer.pushObject(*scope.tape, ObjectInfo(okBlockOuterField, param.offset));
      }
      count--;
   }

   // call the function
   _writer.callExternal(*scope.tape, reference, externalScope.frameSize);

   // save the function result
   if (retVal.kind == okLocalAddress) {
      _writer.saveObject(*scope.tape, ObjectInfo(okLocalAddress, otInt, retVal.reference));
   }

   // save the output length parameter
   Stack<ExternalScope::OutputInfo>::Iterator it = externalScope.output.start();
   while (!it.Eof()) {
      if ((*it).subject == moduleScope->lengthSubject) {
         _writer.loadObject(*scope.tape, ObjectInfo(okBlockLocalAddress, (*it).offset));
         if ((*it).type == -2) {
            _writer.setStrLength(*scope.tape, ObjectInfo(okBlockLocal, (*it).target));
         }
         else _writer.setDumpLength(*scope.tape, ObjectInfo(okBlockLocal, (*it).target));
      }

      it++;
   }

   // save the output parameters
   ExternalScope::OutputInfo output;
   while (externalScope.output.Count() > 0) {
      output = externalScope.output.pop();

      if(output.target != -1) {
         if (output.subject == moduleScope->intSubject || output.subject == moduleScope->handleSubject) {
            _writer.copyInt(*scope.tape, ObjectInfo(okBlockLocal, output.offset), ObjectInfo(okBlockLocal, output.target));
         }
         else if (output.subject == moduleScope->wideStrSubject || output.subject == moduleScope->dumpSubject) {
            // NOTE: we have to make sure that the literal length is already set

            _writer.pushObject(*scope.tape, ObjectInfo(okBlockLocal, output.target));
            if (output.subject == moduleScope->wideStrSubject) {
               _writer.loadStr(*scope.tape, ObjectInfo(okBlockLocal, output.offset));
            }
            else _writer.loadDump(*scope.tape, ObjectInfo(okBlockLocal, output.offset));
         }
      }
   }

   // close the managed stack
   _writer.endExternalBlock(*scope.tape);

   return retVal;
}

int getPrimitiveSize(int mode)
{
   ObjectType operand = ModeToType(mode);

   switch(operand) {
      case otRef:
      case otInt:
      case otLength:
      case otIndex:
      case otShort:
         return 1;
      case otLong:
      case otReal:
         return 2;
      case otByteArray:
         // byte array size should be alligned to 4
         return (mode - TypeCode::tBytes + 3) >> 2;
      default:
         return 0;
   }
}

bool Compiler :: allocatePrimitiveObject(CodeScope& scope, int mode, ObjectInfo& exprOperand)
{
   int size = getPrimitiveSize(mode);
   if (size > 0) {
      exprOperand.kind = okLocalAddress;
      exprOperand.type = ModeToType(mode);
      // the offset should include frame header offset
      exprOperand.reference = -4 - scope.reserved;

      // allocate
      scope.reserved += size;

      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

      // if it is not enough place to allocate
      if (methodScope->reserved < scope.reserved) {
         ByteCodeIterator allocStatement = scope.tape->find(bcOpen);
         // reserve place for stack allocated object
         (*allocStatement).argument += size;

         // if stack was not allocated before
         // update method enter code
         if (methodScope->reserved == 0) {
            // to include new frame header
            (*allocStatement).argument += 2;

            _writer.insertStackAlloc(allocStatement, *scope.tape, size);
         }
         // otherwise update the size 
         else _writer.updateStackAlloc(allocStatement, *scope.tape, size);

         methodScope->reserved += size;
      }

      // indicate the frame usage
      // to prevent commenting frame operation out
      methodScope->masks = MTH_FRAME_USED;

      return true;
   }
   else return false;
}

FunctionCode Compiler :: definePrimitiveOperationCode(CodeScope& scope, int operator_id, ObjectInfo& result, ObjectInfo loperand, ObjectInfo roperand, int mode)
{
   TypeCode ltype = MapTypeCode(loperand.type);
   TypeCode rtype = MapTypeCode(roperand.type);

   if (IsVarOperator(operator_id)) {
      result.kind = okRegister;
      result.type = loperand.type;

      if (operator_id == WRITE_MESSAGE_ID && loperand.kind != okField) {
         operator_id = SAVE_MESSAGE_ID;
      }
      else if (operator_id == APPEND_MESSAGE_ID) {
         operator_id = ADD_MESSAGE_ID;
      }
      else if (operator_id == REDUCE_MESSAGE_ID) {
         operator_id = SUB_MESSAGE_ID;
      }
      else if (operator_id == INCREASE_MESSAGE_ID) {
         operator_id = MUL_MESSAGE_ID;
      }
      else if (operator_id == SEPARATE_MESSAGE_ID) {
         operator_id = DIV_MESSAGE_ID;
      }

      if (ltype == rtype) {
         return (FunctionCode)operator_id;
      }
      else if ((ltype == TypeCode::tLong && rtype == TypeCode::tInt) || (ltype == TypeCode::tReal && rtype == TypeCode::tInt)
          || (ltype == TypeCode::tReal && rtype == TypeCode::tLong)) 
      {
         return (FunctionCode)(operator_id + (rtype << 8));
      }
   }
   else if (IsBranchOperator(operator_id)) {
      result.kind = okRegister;
      result.type = loperand.type;

      if (ltype == rtype) {
         return (FunctionCode)operator_id;
      }
   }
   else if (IsExprOperator(operator_id)) {
      if (ltype != MapTypeCode(result.type)) {
         return (FunctionCode)0;
      }
      if (ltype == rtype) {
         return (FunctionCode)operator_id;
      }
   }
   else if (operator_id == REFER_MESSAGE_ID) {
      if (ltype == TypeCode::tParams && rtype == TypeCode::tInt) {
         return fnGetAt;
      }
   }
   return (FunctionCode)0;
}

ObjectInfo Compiler :: compilePrimitiveOperator(DNode& node, CodeScope& scope, int operator_id, ObjectInfo loperand, ObjectInfo roperand, int mode)
{
   ObjectInfo retVal;
   int paramCount = 1;
   bool compareMode = false;
   bool invertMode = false;
   bool copyMode = false;
   bool referMode = false;

   // for variable operator, acc is the source
   if (operator_id == WRITE_MESSAGE_ID) {
      if (loperand.kind != okField) {
         _writer.pushObject(*scope.tape, loperand);
      }
      else paramCount = 0;

      _writer.loadObject(*scope.tape, roperand);
   }
   // otherwise acc is target
   else {
      // if the left operand is a result of operation - bad luck, we have to reverse the order
      if (loperand.kind == okRegister || loperand.kind == okSymbol) {
         _writer.loadObject(*scope.tape, roperand);
         _writer.saveObject(*scope.tape, ObjectInfo(okCurrent, 1));
         _writer.popObject(*scope.tape, ObjectInfo(okRegister));
      }
      else {
         _writer.pushObject(*scope.tape, roperand);
         _writer.loadObject(*scope.tape, loperand);
      }
   }

   if (IsBranchOperator(operator_id)) {
      // if it is notequal operator, implement as reverted equal
      if (operator_id == NOTEQUAL_MESSAGE_ID) {
         operator_id = EQUAL_MESSAGE_ID;

         invertMode = true;
      }
      else if (operator_id == GREATER_MESSAGE_ID) {
         operator_id = NOTGREATER_MESSAGE_ID;

         invertMode = true;
      }
      else if (operator_id == NOTLESS_MESSAGE_ID) {
         operator_id = LESS_MESSAGE_ID;

         invertMode = true;
      }
      else compareMode = true;
   }
   else if (IsExprOperator(operator_id)) {
      copyMode = true;

      _writer.pushObject(*scope.tape, ObjectInfo(okRegister));

      if (allocatePrimitiveObject(scope, mode, retVal)) {
         _writer.loadObject(*scope.tape, retVal);
      }
      else scope.raiseError(errInvalidOperation, node.Terminal());   

      paramCount++;
   }
   else if (operator_id == REFER_MESSAGE_ID) {
      referMode = true;
   }

   FunctionCode functionCode = definePrimitiveOperationCode(scope, operator_id, retVal, loperand, roperand, mode);
   if (functionCode == FunctionCode::fnUnknown)
      scope.raiseError(errInvalidOperation, node.Terminal());   
   
   if (copyMode) {
      _writer.executeFunction(*scope.tape, retVal, ObjectInfo(okCurrent), functionCode);

      paramCount--;
   }
   else if (referMode && functionCode == fnGetAt) {
      _writer.getOpenParam(*scope.tape);
   }
   else _writer.executeFunction(*scope.tape, retVal, functionCode);

   // for compare operator
   // the result should be true / false
   if (compareMode || invertMode) {
      paramCount += 2;

      // for inverted compare operator
      // the result should be reverted
      if (invertMode) {
         _writer.pushObject(*scope.tape, ObjectInfo(okConstant, scope.moduleScope->falseReference));
         _writer.pushObject(*scope.tape, ObjectInfo(okConstant, scope.moduleScope->trueReference));
      }
      else {
         _writer.pushObject(*scope.tape, ObjectInfo(okConstant, scope.moduleScope->trueReference));
         _writer.pushObject(*scope.tape, ObjectInfo(okConstant, scope.moduleScope->falseReference));
      }

      _writer.selectObject(*scope.tape, ObjectInfo(okCurrent));
   }

   if (paramCount > 0)
      _writer.releaseObject(*scope.tape, paramCount);

   retVal.kind = okRegister;

   return retVal;
}

ObjectInfo Compiler :: compilePrimitiveCatch(DNode node, CodeScope& scope)
{
   _writer.declarePrimitiveCatch(*scope.tape);

   size_t size = 0;
   ref_t message = mapMessage(node, scope, size);
   if (message == encodeMessage(0, RAISE_MESSAGE_ID, 1)) {
      compileThrow(node.firstChild().firstChild(), scope, 0);
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());

   _writer.endPrimitiveCatch(*scope.tape);

   return ObjectInfo(okRegister);
}

ref_t Compiler :: declareInlineArgumentList(DNode arg, MethodScope& scope)
{
   IdentifierString signature;

   ref_t sign_id = 0;

   // if method has generic (unnamed) argument list
   while (arg == nsMethodParameter) {
      TerminalInfo paramName = arg.Terminal();
      int index = 1 + scope.parameters.Count();
      scope.parameters.add(paramName, Parameter(index, otNone));

      arg = arg.nextNode();      
   }
   bool first = true;
   while (arg == nsSubjectArg) {
      TerminalInfo subject = arg.Terminal();

      if (!first) {
         signature.append('&');
      }
      else first = false;

      ObjectType type = otNone;
      if (subject.symbol == tsReference) {
         type = scope.moduleScope->mapSubjectType(subject);
      }
      signature.append(subject);

      // declare method parameter
      arg = arg.nextNode();

      if (arg == nsMethodParameter) {
         // !! check duplicates

         int index = 1 + scope.parameters.Count();
         scope.parameters.add(arg.Terminal(), Parameter(index, type));

         arg = arg.nextNode();
      }
   }

   if (!emptystr(signature))
      sign_id = scope.moduleScope->module->mapSubject(signature, false);

   return encodeMessage(sign_id, EVAL_MESSAGE_ID, scope.parameters.Count());
}

void Compiler :: declareArgumentList(DNode node, MethodScope& scope)
{
   IdentifierString signature;

   TerminalInfo verb = node.Terminal();
   ref_t verb_id = _verbs.get(verb.value);
   ref_t sign_id = 0;

   // if it is a generic verb, make sure no parameters are provided
   if ((verb_id == SEND_MESSAGE_ID || verb_id == DISPATCH_MESSAGE_ID) && node.firstChild() == nsSubjectArg) {
      scope.raiseError(errInvalidOperation, verb);
   }

   DNode arg = node.firstChild();

   bool first = true;
   int paramCount = 0;

   if (verb_id == 0) {
      scope.withCustomVerb = true;

      signature.append(verb);

      // if followed by argument list - it is a EVAL verb
      if (arg == nsSubjectArg || arg == nsMethodParameter || arg == nsMethodOpenParameter) {
         verb_id = EVAL_MESSAGE_ID;
         first = false;
      }
      // otherwise it is GET message
      else verb_id = GET_MESSAGE_ID;
   }

   // if method has an open argument list
   if (arg == nsMethodOpenParameter) {
      scope.parameters.add(arg.Terminal(), Parameter(1, otParams));

      // TO INDICATE OPEN ARGUMENT LIST
      paramCount = OPEN_ARG_COUNT;
   }
   else {
      // if method has generic (unnamed) argument list
      while (arg == nsMethodParameter) {
         int index = 1 + scope.parameters.Count();

         if (scope.parameters.exist(arg.Terminal()))
            scope.raiseError(errDuplicatedLocal, arg.Terminal());

         scope.parameters.add(arg.Terminal(), Parameter(index, otNone));
         paramCount++;

         arg = arg.nextNode();      
      }

      // if method has named argument list
      while (arg == nsSubjectArg) {
         TerminalInfo subject = arg.Terminal();

         // out argument should be last one
         if (test(scope.masks, MTH_OPENARG_USED))
            scope.raiseError(errInvalidOperation, subject);

         if (!first) {
            signature.append('&');
         }
         else first = false;

         bool out = false;
         ObjectType type = otNone;
         if (subject.symbol == tsReference) {
            type = scope.moduleScope->mapSubjectType(subject, out);
         }
         signature.append(subject);

         arg = arg.nextNode();

         if (arg == nsMethodParameter) {
            if (scope.parameters.exist(arg.Terminal()))
               scope.raiseError(errDuplicatedLocal, arg.Terminal());

            int index = 1 + scope.parameters.Count();
            
            if (!out) {
               paramCount++;
               scope.parameters.add(arg.Terminal(), Parameter(index, type));
            }
            // out argument should be treated in special way
            else {
               scope.parameters.add(arg.Terminal(), Parameter(index, type));

               scope.masks |= MTH_OPENARG_USED;
            }

            arg = arg.nextNode();
         }
      }
   }

   // if signature is presented
   if (!emptystr(signature)) {
      sign_id = scope.moduleScope->module->mapSubject(signature, false);
   }

   // declare method parameter debug info
   //      //_writer.declareLocalInfo(*codeScope.tape, SELF_VAR, -5);

   scope.message = encodeMessage(sign_id, verb_id, paramCount);
}

void Compiler :: compileTransmitor(DNode node, CodeScope& scope)
{
   if (node == nsImport) {
      ReferenceNs reference(PACKAGE_MODULE, INLINE_MODULE);
      reference.combine(node.Terminal());

      importCode(node, *scope.moduleScope, scope.tape, reference);
   }
   else {
      DNode nextNode = node.nextNode();

      // !! currently only simple construction is supported
      if (node == nsObject && node.firstChild() == nsNone && nextNode == nsNone) {
         ObjectInfo extension = compileTerminal(node, scope, 0);
         ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

         _writer.extendObject(*scope.tape, extension);
      }
      else scope.raiseError(errInvalidOperation, node.Terminal());
   }
}

void Compiler :: compileDispatcher(DNode sign, CodeScope& scope)
{
   int parameters = countSymbol(sign, nsMessageParameter);

   // if direct dispatching is possible
   if (parameters == 1 && sign.nextNode() == nsMessageParameter) {
      ref_t sign_id = encodeMessage(scope.moduleScope->module->mapSubject(sign.Terminal(), false), 0, 1);

      DNode paramNode = sign.nextNode().firstChild();
      // if direct callback is possible
      if (paramNode.nextNode() == nsNone && paramNode.firstChild() == nsNone) {
         ObjectInfo param = compileTerminal(paramNode, scope, 0);

         if (param.kind == okSelf) {
         }
         else if (param.kind == okField) {
            _writer.swapObject(*scope.tape, okRegister, 2);
            _writer.loadObject(*scope.tape, ObjectInfo(okRegisterField, param.reference));
            _writer.swapObject(*scope.tape, okRegister, 2);
         }
         else scope.raiseError(errUnknownObject, sign.nextNode().firstChild().Terminal());
      }
      // otherwise self and message should be stored
      else {
         DNode paramNode = sign.nextNode().firstChild();

         _writer.swapObject(*scope.tape, okRegister, 2);
         _writer.newSelf(*scope.tape);
         _writer.pushObject(*scope.tape, ObjectInfo(okCurrentMessage));

         ObjectInfo param = compileObject(paramNode, scope, 0);
         if (param.kind != okRegister)
            _writer.popObject(*scope.tape, ObjectInfo(okRegister));

         _writer.popObject(*scope.tape, ObjectInfo(okCurrentMessage));

         _writer.releaseSelf(*scope.tape);
         _writer.swapObject(*scope.tape, okRegister, 2);
      }
      _writer.callBack(*scope.tape, sign_id);
   }
   // !! temporally raise an error
   else scope.raiseError(errUnknownObject, sign.nextNode().firstChild().Terminal());
}

void Compiler :: compileAction(DNode node, MethodScope& scope, ref_t actionMessage)
{
   // check if the method is inhreited and update vmt size accordingly
   scope.include();

   CodeScope codeScope(&scope);

   _writer.declareGenericAction(*codeScope.tape, scope.message, actionMessage);

   declareParameterDebugInfo(scope, codeScope.tape, false);

   compileCode(node, codeScope);

   if (scope.withBreakHandler) {
      _writer.exitGenericAction(*codeScope.tape, scope.parameters.Count() + 1, scope.reserved);

      compileBreakHandler(codeScope, 0);
      _writer.endIdleMethod(*codeScope.tape);
   }         
   else _writer.endGenericAction(*codeScope.tape, scope.parameters.Count() + 1, scope.reserved);

   // method optimization
   // if self / variables are not used try to comment frame openning / closing
   if (!test(scope.masks, MTH_FRAME_USED))
      _writer.commentFrame(codeScope.tape->end());
}

void Compiler :: compileInlineAction(DNode node, MethodScope& scope, ref_t actionMessage)
{
   // check if the method is inhreited and update vmt size accordingly
   scope.include();

   CodeScope codeScope(&scope);

   _writer.declareGenericAction(*codeScope.tape, scope.message, actionMessage);

   declareParameterDebugInfo(scope, codeScope.tape, false);

   compileRetExpression(node, codeScope, 0);

   if (scope.withBreakHandler) {
      _writer.exitGenericAction(*codeScope.tape, scope.parameters.Count() + 1, scope.reserved);
      compileBreakHandler(codeScope, 0);
      _writer.endIdleMethod(*codeScope.tape);
   }         
   else _writer.endGenericAction(*codeScope.tape, scope.parameters.Count() + 1, scope.reserved);
}

void Compiler :: compileResend(DNode node, CodeScope& scope)
{
   // if it is a resend to the constant object / field
   if (node.firstChild() == nsNone) {
      ObjectInfo target = compileTerminal(node, scope, 0);
      if (target.kind == okConstant || target.kind == okField) {
         _writer.resend(*scope.tape, target);
      }
      else scope.raiseError(errInvalidOperation, node.Terminal());  
   }
   // if it is resend to itself
   // Make sure only verb / custom verb is used
   else if (node.firstChild() == nsSubjectArg) {
      MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

      TerminalInfo verb = node.firstChild().Terminal();
      ref_t verb_id = _verbs.get(verb.value);
      int sign_id = getSignature(methodScope->message);
      int paramCount = getParamCount(methodScope->message);

      _writer.loadObject(*scope.tape, ObjectInfo(okCurrent, 1));

      if (verb_id == 0) {
         verb_id = paramCount == 0 ? GET_MESSAGE_ID : EVAL_MESSAGE_ID;

         // if it is custom verb
         // simulate the verb change by creating a new signature
         IdentifierString newSignature(verb.value);

         if (sign_id != 0) {
            const wchar16_t* sign = scope.moduleScope->module->resolveSubject(sign_id);

            // ignore the first argument if it is a custom verb
            if (methodScope->withCustomVerb) {
               int index = StringHelper::find(sign, '&');

               newSignature.append(sign + index);
            }
            else {
               newSignature.append('&');
               newSignature.append(sign);                  
            }
         }

         _writer.redirectVerb(*scope.tape, encodeMessage(scope.moduleScope->module->mapSubject(newSignature, false), 
            verb_id, paramCount));
      }
      else _writer.redirectVerb(*scope.tape, encodeMessage(sign_id, verb_id, paramCount));
   }
   else scope.raiseError(errInvalidOperation, node.Terminal());  
}

void Compiler :: compileMessageDispatch(DNode node, CodeScope& scope)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

  // int parameters = countSymbol(node, nsMessageParameter);

   _writer.declareMethod(*scope.tape, methodScope->message, true);

   compileMessage(node, scope, ObjectInfo(okSelf), 0);

   compileEndStatement(node, scope);

   _writer.endMethod(*scope.tape, getParamCount(methodScope->message) + 1, methodScope->reserved, true);

   // method optimization
   // if self / variables are not used try to comment frame openning / closing
   if (!test(methodScope->masks, MTH_FRAME_USED))
      _writer.commentFrame(scope.tape->end());
}

void Compiler :: compileBreakHandler(CodeScope& scope, int mode)
{
   MethodScope* methodScope = (MethodScope*)scope.getScope(Scope::slMethod);

   ref_t vmtRef = scope.moduleScope->mapConstantReference(BREAK_EXCEPTION_CLASS);

   scope.tape->setPredefinedLabel(-1);
   _writer.pushObject(*scope.tape, ObjectInfo(okRegister));
   _writer.newObject(*scope.tape, 0, vmtRef, scope.moduleScope->nilReference);
   _writer.popObject(*scope.tape, ObjectInfo(okRegisterField));            
   _writer.throwCurrent(*scope.tape);
}

void Compiler :: compileMethod(DNode node, MethodScope& scope/*, DNode hints*/)
{
   // check if the method is inhreited and update vmt size accordingly
   scope.include();

//   // compile constructor hints
//   int mode = scope.compileHints(hints);

   int paramCount = getParamCount(scope.message);

   CodeScope codeScope(&scope);

   if (getVerb(scope.message) == DISPATCH_MESSAGE_ID) {
      _writer.declareMethod(*codeScope.tape, scope.message, false);
      compileDispatcher(node.select(nsDispatch).firstChild(), codeScope);
      _writer.endMethod(*codeScope.tape, 2, scope.reserved, false);
   }
   else if (scope.message == encodeVerb(SEND_MESSAGE_ID)) {
      _writer.declareMethod(*codeScope.tape, scope.message, false);
      compileTransmitor(node.select(nsResend).firstChild(), codeScope);
      _writer.endMethod(*codeScope.tape, 2, scope.reserved, false);
   }
   else {
      DNode resendBody = node.select(nsResendExpression);
      DNode dispatchBody = node.select(nsDispatchExpression);
      DNode importBody = node.select(nsImport);

      // check if it is resend
      if (importBody == nsImport) {
         ReferenceNs reference(PACKAGE_MODULE, INLINE_MODULE);
         reference.combine(importBody.Terminal());

         _writer.declareIdleMethod(*codeScope.tape, scope.message);
         importCode(importBody, *scope.moduleScope, codeScope.tape, reference);
         _writer.endIdleMethod(*codeScope.tape);
      }
      else if (resendBody != nsNone) {
         _writer.declareMethod(*codeScope.tape, scope.message, false);
         compileResend(resendBody.firstChild(), codeScope);
         _writer.endMethod(*codeScope.tape, paramCount + 1, scope.reserved, false);
      }
      // check if it is dispatch
      else if (dispatchBody != nsNone) {
         compileMessageDispatch(dispatchBody.firstChild(), codeScope);
      }
      else {
         _writer.declareMethod(*codeScope.tape, scope.message);

         declareParameterDebugInfo(scope, codeScope.tape, true);

         DNode body = node.select(nsSubCode);
         // if method body is a return expression
         if (body==nsNone) {
            compileCode(node, codeScope);
         }
         // if method body is a set of statements
         else compileCode(body, codeScope);

         _writer.loadObject(*codeScope.tape, ObjectInfo(okSelf));

         int stackToFree = paramCount + 1;

      //   if (scope.testMode(MethodScope::modLock)) {
      //      _writer.endSyncMethod(*codeScope.tape, -1);
      //   }
         if (scope.withBreakHandler) {
            _writer.exitMethod(*codeScope.tape, stackToFree, scope.reserved);
            compileBreakHandler(codeScope, 0);
            _writer.endIdleMethod(*codeScope.tape);
         }         
         else _writer.endMethod(*codeScope.tape, stackToFree, scope.reserved);
      }
   }
//   // critical section entry if sync hint declared
//   if (scope.testMode(MethodScope::modLock)) {
//      ownerScope->info.header.flags |= elWithLocker;
//
//      _writer.tryLock(*codeScope.tape, 1);
//   }
//   // safe point (no need in extra one because lock already has one safe point)
//   else if (scope.testMode(MethodScope::modSafePoint))
//      _writer.declareSafePoint(*codeScope.tape);

   // method optimization
   // if self / variables are not used try to comment frame openning / closing
   if (!test(scope.masks, MTH_FRAME_USED))
      _writer.commentFrame(codeScope.tape->end());
}

void Compiler :: compileConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   // NOTE: the method is class class member though it is compiled within class scope
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.getIt(scope.message);
   if (it.Eof()) {
      classClassScope.info.methods.add(scope.message, true);
   }
   else (*it) = true;

   CodeScope codeScope(&scope);

//   // compile constructor hints
//   int mode = scope.compileHints(hints);

   // HOTFIX: constructor is declared in class class but should be executed if the class instance
   codeScope.tape = &classClassScope.tape;

   DNode body = node.select(nsSubCode);
   DNode dispatch = node.select(nsDispatchExpression);

   if (dispatch != nsNone) {
      compileMessageDispatch(dispatch.firstChild(), codeScope);
   }
   else {
      _writer.declareMethod(*codeScope.tape, scope.message, false);

      // call default constructor
      if (test(classScope->info.header.flags, elDynamicRole)) {
         // if binary class
         Parameter firstParameter = *scope.parameters.start();
         // the dynamic object constructor's first parameter should be a length
         if (firstParameter.type != otLength)
            scope.raiseError(errInvalidOperation, node.Terminal());

         // push the first parameter
         _writer.pushObject(*codeScope.tape, ObjectInfo(okCurrent, 2));
         // push class class
         _writer.pushObject(*codeScope.tape, ObjectInfo(okRegister));

         _writer.setMessage(*codeScope.tape, encodeMessage(0, NEWOBJECT_MESSAGE_ID, 1));
         _writer.callMethod(*codeScope.tape, 2, 1);
      }  
      else {
         // push class class
         _writer.pushObject(*codeScope.tape, ObjectInfo(okRegister));

         _writer.setMessage(*codeScope.tape, encodeVerb(NEWOBJECT_MESSAGE_ID));
         _writer.callMethod(*codeScope.tape, 2, 0);
      }
               
      DNode importBody = node.select(nsImport);

      // check if it is resend
      if (importBody == nsImport) {
         ReferenceNs reference(PACKAGE_MODULE, INLINE_MODULE);
         reference.combine(importBody.Terminal());

         importCode(importBody, *scope.moduleScope, codeScope.tape, reference);
         _writer.endIdleMethod(*codeScope.tape);
      }
      else {
         _writer.newFrame(*codeScope.tape);

         declareParameterDebugInfo(scope, codeScope.tape, true);

         compileCode(body, codeScope);

         _writer.loadObject(*codeScope.tape, ObjectInfo(okSelf));

         _writer.endMethod(*codeScope.tape, getParamCount(scope.message) + 1, scope.reserved);
      }

      // method optimization
      // if self / variables are not used try to comment frame openning / closing
      if (!test(scope.masks, MTH_FRAME_USED))
         _writer.commentFrame(codeScope.tape->end());
   }
}

void Compiler :: compileDefaultConstructor(DNode node, MethodScope& scope, ClassScope& classClassScope, DNode hints)
{
   ClassScope* classScope = (ClassScope*)scope.getScope(Scope::slClass);

   // check if the method is inhreited and update vmt size accordingly
   // NOTE: the method is class class member though it is compiled within class scope
   ClassInfo::MethodMap::Iterator it = classClassScope.info.methods.getIt(scope.message);
   if (it.Eof()) {
      classClassScope.info.methods.add(scope.message, true);
   }
   else (*it) = true;

   CodeScope codeScope(&scope);

   // compile constructor hints
   //int mode = scope.compileHints(hints);

   // HOTFIX: constructor is declared in class class but should be executed if the class instance
   codeScope.tape = &classClassScope.tape;

   _writer.declareMethod(*codeScope.tape, scope.message, false);

   // binary class
   if (test(classScope->info.header.flags, elStructureRole)) {
      if (test(classScope->info.header.flags, elDynamicRole)) {
         if ((classScope->info.header.flags & elDebugMask) == elDebugLiteral) {
            _writer.newWideLiteral(*codeScope.tape, classScope->reference, 2);
         }
         else _writer.newByteArray(*codeScope.tape, classScope->reference, 2);
      }
      else _writer.newStructure(*codeScope.tape, classScope->info.size, classScope->reference);
   }
   // array
   else if (test(classScope->info.header.flags, elDynamicRole)) {
      _writer.newDynamicObject(*codeScope.tape, classScope->reference, 2, scope.moduleScope->nilReference);
   }
   // normal class
   else _writer.newObject(*codeScope.tape, classScope->info.fields.Count(), classScope->reference, scope.moduleScope->nilReference);

   // dynamic default constructor always has one argument
   if (test(classScope->info.header.flags, elDynamicRole)) {
      _writer.endMethod(*codeScope.tape, 2, scope.reserved, false);
   }
   // static default constructor always has no arguments
   else _writer.endMethod(*codeScope.tape, 1, scope.reserved, false);
}

void Compiler :: compileVMT(DNode member, ClassScope& scope)
{
   while (member != nsNone) {
//      DNode hints = skipHints(member);

      switch(member) {
         case nsMethod:
         {
            MethodScope methodScope(&scope);

            // if it is a didpatcher
            if (member.firstChild() == nsDispatch) {
               methodScope.message = encodeVerb(DISPATCH_MESSAGE_ID);
            }
            // if it is a transmitor
            else if (member.firstChild() == nsResend) {
               methodScope.message = encodeVerb(SEND_MESSAGE_ID);
            }
            else declareArgumentList(member, methodScope);

            // check if there is no duplicate method
            if (scope.info.methods.exist(methodScope.message, true))
               scope.raiseError(errDuplicatedMethod, member.Terminal());

            compileMethod(member, methodScope/*, hints*/);
            break;
         }
      }
      member = member.nextNode();
   }
}

void Compiler :: compileFieldDeclarations(DNode& member, ClassScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if (member==nsField) {
         // a class with a dynamic length structure must have no fields
         if (test(scope.info.header.flags, elDynamicRole))
            scope.raiseError(errIllegalField, member.Terminal());

         // currently class - structure must have only one field
         if (test(scope.info.header.flags, elStructureRole))
            scope.raiseError(errIllegalField, member.Terminal());

         if (scope.info.fields.exist(member.Terminal()))
            scope.raiseError(errDuplicatedField, member.Terminal());

         size_t sizeValue = 0;
         scope.compileFieldSizeHint(hints, sizeValue);

         // if it is a data field
         if (sizeValue != 0) {
            if (!test(scope.info.header.flags, elStructureRole) && scope.info.fields.Count() > 0)
               scope.raiseError(errIllegalField, member.Terminal());

            // if it is a dynamic array
            if (sizeValue == (size_t)-4) {
               scope.info.header.flags |= elDynamicRole;

               sizeValue = 0; // !! to indicate dynamic object
            }
            // if it is a char array / literal
            else if (sizeValue == (size_t)-2) {
               scope.info.header.flags |= elDynamicRole;
               scope.info.header.flags |= elStructureRole;
               scope.info.size = sizeValue;

               sizeValue = 0; // !! to indicate dynamic object
            }
            // if it is a dynamic byte array
            else if (sizeValue == (size_t)-1) {
               scope.info.header.flags |= elDynamicRole;
               scope.info.header.flags |= elStructureRole;
               scope.info.size = sizeValue;

               sizeValue = 0; // !! to indicate dynamic object
            }
            // if it is a data field
            else {
               scope.info.header.flags |= elStructureRole;
               scope.info.size += sizeValue;
            }

            scope.compileFieldHints(hints, 0);

            scope.info.fields.add(member.Terminal(), sizeValue);
         }
         // if it is a normal field
         else {
            int offset = scope.info.fields.Count();
            scope.info.fields.add(member.Terminal(), offset);

            scope.compileFieldHints(hints, offset);
         }
      }
      else {
         // due to current syntax we need to reset hints back, otherwise they will be skipped
         if (hints != nsNone)
            member = hints;

         break;
      }
      member = member.nextNode();
   }
}

void Compiler :: compileSymbolCode(ClassScope& scope)
{
   // creates implicit symbol
   SymbolScope symbolScope(scope.moduleScope, scope.reference);

   _writer.declareSymbol(symbolScope.tape, symbolScope.reference);
   _writer.pushObject(symbolScope.tape, ObjectInfo(okConstant, otClass, scope.reference));
   _writer.popObject(symbolScope.tape, ObjectInfo(okRegister));
   _writer.endSymbol(symbolScope.tape);

   // create byte code sections
   _writer.flush(symbolScope.tape, scope.moduleScope->module, scope.moduleScope->debugModule);
}

void Compiler :: compileClassClassDeclaration(DNode node, ClassScope& classClassScope, ClassScope& classScope)
{
   _writer.declareClass(classClassScope.tape, classClassScope.reference);

   // inherit class class parent
   if (classScope.info.header.parentRef != 0) {
      ref_t superClass = classClassScope.moduleScope->mapConstantReference(SUPER_CLASS);
      // NOTE: if it is a super class direct child
      //       super class is used as a base for its class class
      //       otherwise class class should be inherited
      if (classScope.info.header.parentRef != superClass) {
         IdentifierString classClassParentName(classClassScope.moduleScope->module->resolveReference(classScope.info.header.parentRef));
         classClassParentName.append(CLASSCLASS_POSTFIX);

         classClassScope.info.header.parentRef = classClassScope.moduleScope->module->mapReference(classClassParentName);
      }
      else classClassScope.info.header.parentRef = superClass;
   }

   InheritResult res = inheritClass(classClassScope, classClassScope.info.header.parentRef);
   //if (res == irObsolete) {
   //   scope.raiseWarning(wrnObsoleteClass, node.Terminal());
   //}
   if (res == irInvalid) {
      classClassScope.raiseError(errInvalidParent, node.Terminal());
   }
   else if (res == irSealed) {
      classClassScope.raiseError(errSealedParent, node.Terminal());
   }
   else if (res == irUnsuccessfull)
      classClassScope.raiseError(node != nsNone ? errUnknownClass : errUnknownBaseClass, node.Terminal());

   // class class is always stateless
   classClassScope.info.header.flags |= elStateless;

   DNode member = node.firstChild();
   // compile constructors
   while (member != nsNone) {
      DNode hints = skipHints(member);

      if (member == nsConstructor) {
         MethodScope methodScope(&classScope);
         declareArgumentList(member, methodScope);

         // check if there is no duplicate method
         if (classClassScope.info.methods.exist(methodScope.message, true))
            classClassScope.raiseError(errDuplicatedMethod, member.Terminal());

         if (test(classScope.info.header.flags, elStateless))
            classClassScope.raiseError(errInvalidOperation, member.Terminal());

         compileConstructor(member, methodScope, classClassScope, hints);
      }
      member = member.nextNode();
   }

   if (!test(classScope.info.header.flags, elStateless)) {
      if (test(classScope.info.header.flags, elDynamicRole)) {
         // dynamic class has a special default constructor
         MethodScope methodScope(&classScope);
         methodScope.message = encodeMessage(0, NEWOBJECT_MESSAGE_ID, 1);

         compileDefaultConstructor(DNode(), methodScope, classClassScope, DNode());
      }
      else {
         MethodScope methodScope(&classScope);
         methodScope.message = encodeVerb(NEWOBJECT_MESSAGE_ID);

         compileDefaultConstructor(DNode(), methodScope, classClassScope, DNode());
      }
   }

   _writer.endClass(classClassScope.tape);

   // optimize
   optimizeTape(classClassScope.tape);

   // create byte code
   classClassScope.save(_writer);
}

void Compiler :: compileClassDeclaration(DNode node, ClassScope& scope, DNode hints)
{
   scope.compileHints(hints);

   _writer.declareClass(scope.tape, scope.reference);

   DNode member = node.firstChild();
   if (member==nsBaseClass) {
      compileParentDeclaration(member, scope);

      member = member.nextNode();
   }
   else compileParentDeclaration(DNode(), scope);

   compileFieldDeclarations(member, scope);

   // define class class name
   IdentifierString classClassName(scope.moduleScope->module->resolveReference(scope.reference));
   classClassName.append(CLASSCLASS_POSTFIX);

   // check if the class is stateless
   if (scope.info.fields.Count() == 0
      && !test(scope.info.header.flags, elStructureRole)
      && !test(scope.info.header.flags, elDynamicRole)
      /* && !test(scope.info.header.flags, elWithLocker)*/)
   {
      scope.info.header.flags |= elStateless;

      //scope.moduleScope->defineConstant(scope.reference);
   }
   else scope.info.header.flags &= ~elStateless;

   // if it is super class
   if (scope.info.header.parentRef == 0) {
      // super class is class class itself
      scope.info.classClassRef = scope.reference;
   }
   else scope.info.classClassRef = scope.moduleScope->module->mapReference(classClassName);

   compileVMT(member, scope);

   _writer.endClass(scope.tape);

   // compile explicit symbol
   compileSymbolCode(scope);

   // optimize
   optimizeTape(scope.tape);

   // create byte code
   scope.save(_writer);
}

void Compiler :: compileSymbolDeclaration(DNode node, SymbolScope& scope, /*DNode hints, */bool isStatic)
{
//   scope.compileHints(hints);

   DNode expression = node.firstChild();

   // compile symbol into byte codes
   if (isStatic) {
      _writer.declareStaticSymbol(scope.tape, scope.reference);
   }
   else _writer.declareSymbol(scope.tape, scope.reference);

   CodeScope codeScope(&scope);

   // compile symbol body
   _writer.loadObject(*codeScope.tape, compileExpression(expression, codeScope, CTRL_ROOT));

   _writer.declareBreakpoint(scope.tape, 0, 0, 0, dsVirtualEnd);

   if (isStatic) {
      _writer.endStaticSymbol(scope.tape, scope.reference);
   }
   else _writer.endSymbol(scope.tape);

   // optimize
   optimizeTape(scope.tape);

   // create byte code sections
   _writer.flush(scope.tape, scope.moduleScope->module, scope.moduleScope->debugModule);
}

void Compiler :: compileIncludeModule(DNode node, ModuleScope& scope, DNode hints)
{
   if (hints != nsNone)
      scope.raiseWarning(wrnUnknownHint, hints.Terminal());

   const wchar16_t* ns = node.Terminal();

   // check if the module exists
   _Module* module = scope.project->loadModule(ns, true);
   if (!module)
      scope.raiseWarning(wrnUnknownModule, node.Terminal());

   const wchar16_t* value = retrieve(scope.defaultNs.start(), ns, NULL);
   if (value == NULL) {
      scope.defaultNs.add(ns);
   }
}

void Compiler :: compileForward(DNode node, ModuleScope& scope, DNode hints)
{
   bool constant;
   scope.compileForwardHints(hints, constant);

   TerminalInfo shortcut = node.Terminal();

   if (!scope.defineForward(shortcut.value, node.firstChild().Terminal().value, constant))
      scope.raiseError(errDuplicatedDefinition, shortcut);
}

void Compiler :: compileDeclarations(DNode& member, ModuleScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      TerminalInfo name = member.Terminal();
      ref_t reference = scope.mapTerminal(name);
      // check for duplicate declaration
      if (scope.module->mapSection(reference | mskSymbolRef, true))
         scope.raiseError(errDuplicatedSymbol, name);

      switch (member) {
         case nsClass:
         {
            // compile class
            ClassScope classScope(&scope, reference);
            compileClassDeclaration(member, classScope, hints);

            // compile class class if it is not itself (i.e. it is not super class)
            if (classScope.info.classClassRef != classScope.reference) {
               ClassScope classClassScope(&scope, classScope.info.classClassRef);
               compileClassClassDeclaration(member, classClassScope, classScope);
            }
            break;
         }
         case nsSymbol:
         case nsStatic:
         {
            SymbolScope symbolScope(&scope, reference);
            compileSymbolDeclaration(member, symbolScope, /*hints, */(member == nsStatic));
            break;
         }
      }
      member = member.nextNode();
   }
}

void Compiler :: compileIncludeSection(DNode& member, ModuleScope& scope)
{
   while (member != nsNone) {
      DNode hints = skipHints(member);

      switch (member) {
         case nsInclude:
            if (member.firstChild() == nsForward) {
               compileForward(member, scope, hints);
            }
            else compileIncludeModule(member, scope, hints);
            break;
         default:
            // due to current syntax we need to reset hints back, otherwise they will be skipped
            if (hints != nsNone)
               member = hints;

            return;
      }
      member = member.nextNode();
   }
}

void Compiler :: compileModule(DNode node, ModuleScope& scope)
{
   DNode member = node.firstChild();

   compileIncludeSection(member, scope);

   compileDeclarations(member, scope);
}

bool Compiler :: validate(Project& project, _Module* module, int reference)
{
   int   mask = reference & mskAnyRef;
   ref_t extReference = 0;
   const wchar16_t* refName = module->resolveReference(reference & ~mskAnyRef);
   _Module* extModule = project.resolveModule(refName, extReference, true);

   return (extModule != NULL && extModule->mapSection(extReference | mask, true) != NULL);
}

void Compiler :: validateUnresolved(Project& project)
{
   for (List<Unresolved>::Iterator it = _unresolveds.start() ; !it.Eof() ; it++) {
      if (!validate(project, (*it).module, (*it).reference)) {
         const wchar16_t* refName = (*it).module->resolveReference((*it).reference & ~mskAnyRef);

         project.raiseWarning(wrnUnresovableLink, (*it).fileName, (*it).row, (*it).col, refName);
      }
   }
}

void Compiler :: compile(const _path_t* source, MemoryDump* buffer, ModuleScope& scope)
{
   _writer.declareModule(scope.debugModule, scope.sourcePath);

   // parse
   TextFileReader sourceFile(source, scope.project->getDefaultEncoding(), true);
   if (!sourceFile.isOpened())
      scope.project->raiseError(errInvalidFile, source);

   buffer->clear();
   MemoryWriter bufWriter(buffer);
   DerivationWriter writer(&bufWriter);
   _parser.parse(&sourceFile, &writer, scope.project->getTabSize());

   // compile
   MemoryReader bufReader(buffer);
   DerivationReader reader(&bufReader);

   compileModule(reader.readRoot(), scope);
}

void Compiler :: createModuleInfo(ModuleScope& scope, const wchar16_t* path, bool withDebugInfo, Map<const wchar16_t*, ModuleInfo>& modules)
{
   _Module* module = scope.project->createModule(path);

   const wchar16_t* name = module->Name();

   ModuleInfo info = modules.get(name);
   if (!info.codeModule) {
      info.codeModule = module;
      if (withDebugInfo)
         info.debugModule = scope.project->createDebugModule(name);

      modules.add(name, info);
   }

   scope.init(module, info.debugModule, path);
}

bool Compiler :: run(Project& project)
{
   bool withDebugInfo = project.BoolSetting(opDebugMode);
   Map<const wchar16_t*, ModuleInfo> modules(ModuleInfo(NULL, NULL));  

   MemoryDump  buffer;                // temporal derivation buffer
   for(SourceIterator it = project.getSourceIt() ; !it.Eof() ; it++) {
      try {
         // create or update module
         ModuleScope scope(&project, &_unresolveds);
         createModuleInfo(scope, it.key(), withDebugInfo, modules);

         project.printInfo("%s", scope.sourcePath);

         // compile source
         compile(*it, &buffer, scope); // !! better to save modules after all files are compiled
      }
      catch (LineTooLong& e)
      {
         project.raiseError(errLineTooLong, it.key(), e.row, 1);
      }
      catch (InvalidChar& e)
      {
         project.raiseError(errInvalidChar, it.key(), e.row, e.column, String<wchar16_t, 2>(&e.ch, 1));
      }
      catch (SyntaxError& e)
      {
         project.raiseError(e.error, it.key(), e.row, e.column, e.token);
      }
   }

   Map<const wchar16_t*, ModuleInfo>::Iterator it = modules.start();
   while (!it.Eof()) {
      ModuleInfo info = *it;

      project.saveModule(info.codeModule, ConstantIdentifier("nl"));

      if (info.debugModule)
         project.saveModule(info.debugModule, ConstantIdentifier("dnl"));

      it++;
   }

   // validate the unresolved forward refereces if unresolved reference warning is enabled
   validateUnresolved(project);

   return !project.HasWarnings();
}
