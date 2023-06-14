//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "vmparser.h"

#include "langcommon.h"

using namespace elena_lang;

// --- VMTapeParser ---

VMTapeParser :: VMTapeParser()
{
   
}

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command)
{
   rdataWriter.writeDWord(command);
}

bool VMTapeParser :: parseDirective(ScriptEngineReaderBase& reader, MemoryDump* tape)
{
   TapeWriter writer(tape);

   do {
      if (reader.compare(";") || reader.compare("]]")) {
         break;
      }
      else if (reader.compare("#start")) {
         writer.write(VM_TERMINAL_CMD);
         writer.write(VM_INIT_CMD);
      }
      else return false;

      reader.read();
   } while (true);

   return true;
}

int VMTapeParser :: writeBuildScriptArgumentList(ScriptEngineReaderBase& reader, ScriptBookmark terminator,
   ScriptStack& callStack, TapeWriter& writer)
{
   int counter = 0;
   ScriptBookmark bm = callStack.pop();
   while (!bm.compare(terminator)) {
      writeBuildScriptStatement(reader, bm, callStack, writer);
      writer.write(VM_SET_ARG_CMD, counter);

      counter++;
      bm = callStack.pop();
   }

   return counter;
}

void VMTapeParser :: writeBuildScriptStatement(ScriptEngineReaderBase& reader, ScriptBookmark bm, ScriptStack& callStack,
   TapeWriter& writer)
{
   if (bm.state == dfaReference) {
      int counter = writeBuildScriptArgumentList(reader, bm, callStack, writer);

      writer.write(VM_ARG_TAPE_CMD, counter + 1);
      writer.write(VM_NEW_CMD, reader.lookup(bm));
   }
   else if (bm.state == dfaQuote) {
      writer.write(VM_STRING_CMD, reader.lookup(bm));
   }
   else if (reader.lookup(bm).compare(";")) {
      bm = callStack.pop();

      writeBuildScriptStatement(reader, bm, callStack, writer);
   }
}

inline void readToken(ScriptEngineReaderBase& reader, ustr_t expectedToken)
{
   ScriptBookmark bm = reader.read();

   if (!reader.compare(expectedToken))
      throw SyntaxError("Parse error", bm.lineInfo);
}

int VMTapeParser :: parseBuildScriptArgumentList(ScriptEngineReaderBase& reader, ScriptStack& callStack)
{
   int maxArgCount = 2;
   int currentArgCount = 0;

   pos_t exprBookmark = callStack.count();
   readToken(reader, "(");

   ScriptBookmark bm = reader.read();
   while (!reader.compare(")")) {
      maxArgCount = _max(maxArgCount, parseBuildScriptStatement(reader, bm, callStack, exprBookmark));
      currentArgCount++;

      bm = reader.read();
   }

   return _max(maxArgCount, currentArgCount);
}

int VMTapeParser :: parseBuildScriptStatement(ScriptEngineReaderBase& reader, ScriptBookmark& bm, 
   ScriptStack& callStack, pos_t exprBookmark)
{
   int retVal = 0;

   if (bm.state == dfaReference) {
      callStack.push(bm);

      retVal = parseBuildScriptArgumentList(reader, callStack);

      callStack.push(bm);
   }
   else if (bm.state == dfaQuote) {
      callStack.push(bm);

      retVal = 1;
   }
   else if (reader.lookup(bm).compare(";")) {
      auto expr_it = callStack.get(callStack.count() - exprBookmark);
      ScriptBookmark expr_bm = *expr_it;
      callStack.insert(expr_it, expr_bm);
      callStack.push(expr_bm);
      callStack.push(bm);
   }
   else throw SyntaxError("Invalid operation", bm.lineInfo);

   return retVal;
}

bool VMTapeParser :: parseBuildScript(ScriptEngineReaderBase& reader, TapeWriter& writer)
{
   bool idle = true;

   ScriptBookmark bm = reader.read();
   Stack<ScriptBookmark> callStack({});
   int maxArgCount = 2;
   while (!reader.eof()) {
      idle = false;

      maxArgCount = _max(maxArgCount, parseBuildScriptStatement(reader, bm, callStack, 0));

      bm = reader.read();
   }

   writer.write(VM_ALLOC_CMD, maxArgCount);

   while (callStack.count() > 0) {
      bm = callStack.pop();

      writeBuildScriptStatement(reader, bm, callStack, writer);
   }

   return !idle;
}

void VMTapeParser :: parse(ScriptEngineReaderBase& reader, MemoryDump* output)
{
   TapeWriter writer(output);

   if (parseBuildScript(reader, writer)) {
      if (_postfix.length() != 0) {
         parseInline(_postfix.str(), writer);
      }
   }

   writer.write(VM_ENDOFTAPE_CMD);
}

void VMTapeParser :: parseInlineStatement(ScriptEngineReaderBase& reader, ScriptBookmark& bm, TapeWriter& writer)
{
   //if (reader.compare("#start")) {
   //   writer.addVMTapeEntry(START_VM_MESSAGE_ID);
   //}
   //else if (reader.compare("^")) {
   //   bm = reader.read();

   //   writeMessage(writer, reader.lookup(bm), SEND_TAPE_MESSAGE_ID);
   //}
   /*else*/ throw SyntaxError("Invalid operation", bm.lineInfo);
}

void VMTapeParser :: parseInlineScript(ScriptEngineReaderBase& reader, TapeWriter& writer)
{
   ScriptBookmark bm = reader.read();
   while (!reader.eof()) {
      parseInlineStatement(reader, bm, writer);

      bm = reader.read();
   }
}

void VMTapeParser :: parseInline(ustr_t script, TapeWriter& writer)
{
   IdentifierTextReader logReader(script);
   ScriptEngineReader scriptReader(&logReader);

   parseInlineScript(scriptReader, writer);
}

bool VMTapeParser :: parseGrammarRule(ScriptEngineReaderBase& reader)
{
   return false;
}
