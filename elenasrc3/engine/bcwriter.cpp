//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA byte code compiler class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "bcwriter.h"

#include "langcommon.h"

using namespace elena_lang;

typedef ByteCodeWriter::TapeScope TapeScope;

inline BuildKey operator | (const BuildKey& l, const BuildKey& r)
{
   return (BuildKey)((uint32_t)l | (uint32_t)r);
}

inline BuildKey operator & (const BuildKey& l, const BuildKey& r)
{
   return (BuildKey)((uint32_t)l & (uint32_t)r);
}

inline BuildKey operator ~ (BuildKey arg1)
{
   return (BuildKey)(~static_cast<unsigned int>(arg1));
}

inline bool testMask(BuildKey key, BuildKey mask)
{
   return (key & mask) == mask;
}

void openFrame(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   if (tapeScope.classMode) {
      for (int i = 0; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XFlushSI, i);
      }
   }

   int reservedManaged = tapeScope.reserved;
   int reservedUnmanaged = tapeScope.reservedN;

   tape.newLabel();
   tape.write(ByteCode::OpenIN, reservedManaged, reservedUnmanaged);
}

void closeFrame(CommandTape& tape, BuildNode& node, TapeScope& scope)
{
   int reservedUnmanaged = scope.reservedN;

   tape.setLabel();
   tape.write(ByteCode::CloseN, reservedUnmanaged);
}

void nilReference(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, 0);
}

void symbolCall(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::CallR, node.arg.reference | mskSymbolRef);
}

void classReference(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskVMTRef);
}

void sendOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   int vmtIndex = node.findChild(BuildKey::Index).arg.value;

   pos_t argCount = getArgCount(node.arg.reference);
   if ((int)argCount < tapeScope.scope->minimalArgList) {
      for (int i = argCount; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XStoreSIR, i, 0);
      }
   }

   tape.write(ByteCode::MovM, node.arg.reference);
   tape.write(ByteCode::CallVI, vmtIndex);
}

void directCallOp(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   ref_t targetRef = node.findChild(BuildKey::Type).arg.reference;

   pos_t argCount = getArgCount(node.arg.reference);
   if ((int)argCount < tapeScope.scope->minimalArgList) {
      for (int i = 0; i < tapeScope.scope->minimalArgList; i++) {
         tape.write(ByteCode::XStoreSIR, i, 0);
      }
   }

   tape.write(ByteCode::MovM, node.arg.reference);
   tape.write(ByteCode::CallMR, node.arg.reference, targetRef | mskVMTRef);
}

void exit(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::Quit);
}

void savingInStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::StoreSI, node.arg.value);
}

void assigningLocal(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::StoreFI, node.arg.value);
}

void copyingLocal(CommandTape& tape, BuildNode& node, TapeScope&)
{
   int n = node.findChild(BuildKey::Size).arg.value;

   tape.write(ByteCode::StoreSI, 0);
   tape.write(ByteCode::CopyDPN, node.arg.value, n);
}

void getLocal(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::PeekFI, node.arg.value);
}

void getLocalAddredd(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SetDP, node.arg.value);
}

void creatingClass(CommandTape& tape, BuildNode& node, TapeScope&)
{
   ref_t typeRef = node.findChild(BuildKey::Type).arg.reference;

   tape.write(ByteCode::NewIR, node.arg.value, typeRef | mskVMTRef);
}

void creatingStruct(CommandTape& tape, BuildNode& node, TapeScope&)
{
   ref_t typeRef = node.findChild(BuildKey::Type).arg.reference;

   tape.write(ByteCode::NewNR, node.arg.value, typeRef | mskVMTRef);
}

void openStatement(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   DebugLineInfo symbolInfo = { DebugSymbol::Statement };
   tapeScope.scope->debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void closeStatement(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   DebugLineInfo symbolInfo = { DebugSymbol::End };
   tapeScope.scope->debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void addingBreakpoint(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   BuildNode row = node.findChild(BuildKey::Row);
   BuildNode col = node.findChild(BuildKey::Column);

   DebugLineInfo symbolInfo = { DebugSymbol::Breakpoint, col.arg.value, row.arg.value };
   tapeScope.scope->debug->write(&symbolInfo, sizeof(DebugLineInfo));

   tape.write(ByteCode::Breakpoint);
}

void intLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskIntLiteralRef);
}

void stringLiteral(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   tape.write(ByteCode::SetR, node.arg.reference | mskLiteralRef);
}

void goingToEOP(CommandTape& tape, BuildNode& node, TapeScope& tapeScope)
{
   //gotoEnd(tape, baFirstLabel);
   tape.write(ByteCode::Jump, PseudoArg::FirstLabel);
}

void allocatingStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::AllocI, node.arg.value);
}

void freeingStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::FreeI, node.arg.value);
}

void savingNInStack(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::Load);
   tape.write(ByteCode::SaveSI, node.arg.value);
}

void extCallOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::CallExtR, node.arg.reference | mskExternalRef, node.findChild(BuildKey::Count).arg.value);
}

void savingIndex(CommandTape& tape, BuildNode& node, TapeScope&)
{
   tape.write(ByteCode::SaveDP, node.arg.value);
}

void dispatchOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   mssg_t message = node.findChild(BuildKey::Message).arg.reference;

   tape.write(ByteCode::DispatchMR, message, node.arg.reference | mskConstArray);
}

void intOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.findChild(BuildKey::Index).arg.value;

   switch (node.arg.value) {
      case ADD_OPERATOR_ID:
         tape.write(ByteCode::CopyDPN, targetOffset, 4);
         tape.write(ByteCode::XMovSISI, 0, 1);
         tape.write(ByteCode::IAddDPN, targetOffset, 4);
         break;
      case SUB_OPERATOR_ID:
         tape.write(ByteCode::CopyDPN, targetOffset, 4);
         tape.write(ByteCode::XMovSISI, 0, 1);
         tape.write(ByteCode::ISubDPN, targetOffset, 4);
         break;
      default:
         throw InternalError(errFatalError);
   }
}

void binarySOp(CommandTape& tape, BuildNode& node, TapeScope&)
{
   // NOTE : sp[0] - loperand, sp[1] - roperand
   int targetOffset = node.findChild(BuildKey::Index).arg.value;
   int size = node.findChild(BuildKey::Size).arg.value;

   switch (node.arg.value) {
      case LEN_OPERATOR_ID:
         tape.write(ByteCode::PeekSI, 0);
         tape.write(ByteCode::NLen, size);
         tape.write(ByteCode::SaveDP, targetOffset, 4);
         break;
   default:
      throw InternalError(errFatalError);
   }
}

ByteCodeWriter::Saver commands[] =
{
   nullptr,
   openFrame,
   closeFrame,
   nilReference,
   symbolCall,
   classReference,
   sendOp,
   exit,
   savingInStack,
   assigningLocal,
   getLocal,
   creatingClass,
   openStatement,
   closeStatement,
   addingBreakpoint,
   addingBreakpoint,
   creatingStruct,
   intLiteral,
   stringLiteral,
   goingToEOP,
   getLocalAddredd,
   copyingLocal,
   allocatingStack,
   freeingStack,
   savingNInStack,
   extCallOp,
   savingIndex,
   directCallOp,
   dispatchOp,
   intOp,
   binarySOp,
};

// --- ByteCodeWriter ---

ByteCodeWriter :: ByteCodeWriter(LibraryLoaderBase* loader)
{
   _commands = commands;
   _loader = loader;
}

void ByteCodeWriter :: openSymbolDebugInfo(Scope& scope, ustr_t symbolName)
{
   if (scope.debug->position() == 0)
      scope.debug->writeDWord(0);

   // map symbol debug info, starting the symbol with # to distinsuish from class
   IdentifierString bookmark("'#", symbolName + 1);
   scope.moduleScope->debugModule->mapPredefinedReference(*bookmark, scope.debug->position());

   pos_t namePosition = scope.debugStrings->position();

   if (isWeakReference(symbolName)) {
      IdentifierString fullName(scope.moduleScope->debugModule->name(), symbolName);

      scope.debugStrings->writeString(*fullName);
   }
   else scope.debugStrings->writeString(symbolName);

   DebugLineInfo symbolInfo = { DebugSymbol::Symbol };
   symbolInfo.addresses.source.nameRef = namePosition;

   scope.debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: openClassDebugInfo(Scope& scope, ustr_t className, ref_t flags)
{
   if (scope.debug->position() == 0)
      scope.debug->writeDWord(0);

   scope.moduleScope->debugModule->mapPredefinedReference(className, scope.debug->position());

   pos_t namePosition = scope.debugStrings->position();

   if (isWeakReference(className)) {
      IdentifierString fullName(scope.moduleScope->debugModule->name(), className);

      scope.debugStrings->writeString(*fullName);
   }
   else scope.debugStrings->writeString(className);

   DebugLineInfo symbolInfo = { DebugSymbol::Class };
   symbolInfo.addresses.classSource.nameRef = namePosition;
   symbolInfo.addresses.classSource.flags = flags;

   scope.debug->write(&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: openMethodDebugInfo(Scope& scope)
{
   DebugLineInfo symbolInfo = { DebugSymbol::Procedure };
   //symbolInfo.addresses.source.nameRef = sourceRef;

   scope.debug->write((void*)&symbolInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: endDebugInfo(Scope& scope)
{
   DebugLineInfo endInfo = { DebugSymbol::End };

   scope.debug->write(&endInfo, sizeof(DebugLineInfo));
}

void ByteCodeWriter :: importTree(CommandTape& tape, BuildNode node, Scope& scope)
{
   ustr_t referenceName = scope.moduleScope->resolveFullName(node.arg.reference);
   SectionInfo importInfo;
   if (isWeakReference(referenceName)) {
      importInfo = _loader->getSection(ReferenceInfo(scope.moduleScope->module, referenceName), mskProcedureRef, false);
   }
   else importInfo = _loader->getSection(ReferenceInfo(referenceName), mskProcedureRef, false);

   tape.import(importInfo.module, importInfo.section, true, scope.moduleScope);
}

void ByteCodeWriter :: saveTape(CommandTape& tape, BuildNode node, TapeScope& tapeScope)
{
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::Import:
            importTree(tape, current, *tapeScope.scope);
            break;
         case BuildKey::NestedClass:
            saveClass(current, tapeScope.scope->moduleScope, tapeScope.scope->minimalArgList);
            break;
         default:
            _commands[(int)current.key](tape, current, tapeScope);
            break;
      }
   
      current = current.nextNode();
   }
}

void ByteCodeWriter :: saveSymbol(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList)
{
   auto section = moduleScope->mapSection(node.arg.reference | mskSymbolRef, false);
   MemoryWriter writer(section);

   Scope scope = { nullptr, &writer, moduleScope, nullptr, nullptr, minimalArgList };

   if (moduleScope->debugModule) {
      // initialize debug info writers
      MemoryWriter debugWriter(moduleScope->debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(moduleScope->debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debug = &debugWriter;
      scope.debugStrings = &debugStringWriter;

      openSymbolDebugInfo(scope, moduleScope->module->resolveReference(node.arg.reference & ~mskAnyRef));
      saveProcedure(node, scope, false);
      endDebugInfo(scope);
   }
   else saveProcedure(node, scope, false);
}

void ByteCodeWriter :: optimizeTape(CommandTape& tape)
{
   //// optimize unused and idle jumps
   //while (CommandTape::optimizeJumps(tape));
}

void ByteCodeWriter :: saveProcedure(BuildNode node, Scope& scope, bool classMode)
{
   TapeScope tapeScope = {
      &scope,
      node.findChild(BuildKey::Reserved).arg.value,
      node.findChild(BuildKey::ReservedN).arg.value,
      classMode
   };

   CommandTape tape;
   saveTape(tape, node.findChild(BuildKey::Tape), tapeScope);

   // optimize
   optimizeTape(tape);

   MemoryWriter* code = scope.code;
   pos_t sizePlaceholder = code->position();
   code->writePos(0);

   // create byte code sections
   tape.saveTo(code);

   pos_t endPosition = code->position();
   pos_t size = endPosition - sizePlaceholder - sizeof(pos_t);

   code->seek(sizePlaceholder);
   code->writePos(size);
   code->seek(endPosition);
}

void ByteCodeWriter :: saveVMT(BuildNode node, Scope& scope)
{
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      if (current == BuildKey::Method) {
         MethodEntry entry = { current.arg.reference, scope.code->position() };
         scope.vmt->write(&entry, sizeof(MethodEntry));

         if (scope.moduleScope->debugModule) {
            openMethodDebugInfo(scope);
            saveProcedure(current, scope, true);
            endDebugInfo(scope);
         }
         else saveProcedure(current, scope, true);
      }
      else if (current == BuildKey::AbstractMethod) {
         MethodEntry entry = { current.arg.reference, INVALID_POS };
         scope.vmt->write(&entry, sizeof(MethodEntry));
      }

      current = current.nextNode();
   }
}

void ByteCodeWriter :: saveClass(BuildNode node, SectionScopeBase* moduleScope, int minimalArgList)
{
   // initialize bytecode writer
   MemoryWriter codeWriter(moduleScope->mapSection(node.arg.reference | mskClassRef, false));

   auto vmtSection = moduleScope->mapSection(node.arg.reference | mskVMTRef, false);
   MemoryWriter vmtWriter(vmtSection);

   vmtWriter.writeDWord(0);                     // save size place holder
   pos_t classPosition = vmtWriter.position();

   // copy class meta data header + vmt size
   MemoryReader metaReader(moduleScope->mapSection(node.arg.reference | mskMetaClassInfoRef, true));
   ClassInfo info;
   info.load(&metaReader);

   // reset VMT length
   info.header.count = 0;
   for (auto m_it = info.methods.start(); !m_it.eof(); ++m_it) {
      //NOTE : ingnore statically linked methods
      if (!test(m_it.key(), STATIC_MESSAGE))
         info.header.count++;
   }

   vmtWriter.write(&info.header, sizeof(ClassHeader));  // header

   Scope scope = { &vmtWriter, &codeWriter, moduleScope, nullptr, nullptr, minimalArgList };
   if (moduleScope->debugModule) {
      // initialize debug info writers
      MemoryWriter debugWriter(moduleScope->debugModule->mapSection(DEBUG_LINEINFO_ID, false));
      MemoryWriter debugStringWriter(moduleScope->debugModule->mapSection(DEBUG_STRINGS_ID, false));

      scope.debug = &debugWriter;
      scope.debugStrings = &debugStringWriter;

      openClassDebugInfo(scope, moduleScope->module->resolveReference(node.arg.reference & ~mskAnyRef), info.header.flags);
      saveVMT(node, scope);
      endDebugInfo(scope);
   }
   else saveVMT(node, scope);

   pos_t size = vmtWriter.position() - classPosition;
   vmtSection->write(classPosition - 4, &size, sizeof(size));
}

void ByteCodeWriter :: save(BuildTree& tree, SectionScopeBase* moduleScope, int minimalArgList)
{
   BuildNode node = tree.readRoot();
   BuildNode current = node.firstChild();
   while (current != BuildKey::None) {
      switch (current.key) {
         case BuildKey::Symbol:
            saveSymbol(current, moduleScope, minimalArgList);
            break;
         case BuildKey::Class:
            saveClass(current, moduleScope, minimalArgList);
            break;
         default:
            // to make compiler happy
            break;
      }

      current = current.nextNode();
   }
}
