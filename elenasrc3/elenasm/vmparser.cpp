//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023-2024, by Aleksey Rakov
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
         writer.write(VM_INIT_CMD);
      }
      else if (reader.compare("#terminal")) {
         writer.write(VM_TERMINAL_CMD);
      }
      else if (reader.compare("#config")) {
         ScriptBookmark bm = reader.read();
         if (bm.state == dfaIdentifier) {
            writer.write(VM_CONFIG_CMD, reader.lookup(bm));
         }
         else throw SyntaxError("Invalid directive", bm.lineInfo);
      }
      else if (reader.compare("#set")) {
         ScriptBookmark bm = reader.read();
         if (reader.compare("preloaded")) {
            bm = reader.read();
            if (bm.state == dfaQuote) {
               writer.write(VM_PRELOADED_CMD, reader.lookup(bm));
            }
            else throw SyntaxError("Invalid directive", bm.lineInfo);
         }
         else throw SyntaxError("Invalid directive", bm.lineInfo);
      }
      else if (reader.compare("#map")) {
         ScriptBookmark bm = reader.read();

         if (bm.state == dfaQuote) {
            writer.write(VM_FORWARD_CMD, reader.lookup(bm));
         }
         else {
            IdentifierString forward;
            if (bm.state == dfaReference) {
               forward.append(reader.lookup(bm));

               bm = reader.read();
               if (!reader.compare("="))
                  throw SyntaxError("Invalid #map command", bm.lineInfo);

               bm = reader.read();
               if (bm.state == dfaReference) {
                  forward.append('=');
                  forward.append(reader.lookup(bm));
                  writer.write(VM_FORWARD_CMD, *forward);
               }
               throw SyntaxError("Invalid #map command", bm.lineInfo);
            }
            else throw SyntaxError("Invalid #map command", bm.lineInfo);
         }
      }
      else if (reader.compare("#use")) {
         ScriptBookmark bm = reader.read();

         if (bm.state == dfaQuote) {
            writer.write(VM_PACKAGE_CMD, reader.lookup(bm));
         }
         else {
            IdentifierString package;
            package.append(reader.lookup(bm));

            bm = reader.read();
            if (!reader.compare("="))
               throw SyntaxError("Invalid #use command", bm.lineInfo);

            bm = reader.read();
            if (bm.state == dfaQuote) {
               package.append('=');
               package.append(reader.lookup(bm));
               writer.write(VM_PACKAGE_CMD, *package);
            }
            else throw SyntaxError("Invalid #use command", bm.lineInfo);
         }
      }
      else if (reader.compare("#postfix")) {
         ScriptBookmark bm = reader.read();

         _postfix.append(reader.lookup(bm));
      }
      else return false;

      reader.read();
   } while (true);

   return true;
}

int VMTapeParser :: writeBuildScriptArgumentList(ScriptEngineReaderBase& reader, ScriptBookmark terminator,
   ScriptStack& callStack, TapeWriter& writer)
{
   CachedList<pos_t, 5> argPositions;

   pos_t position = writer.writeAndGetArgPosition(VM_ALLOC_CMD, 0);

   int counter = 0;
   ScriptBookmark bm = callStack.pop();
   while (!bm.compare(terminator)) {
      writeBuildScriptStatement(reader, bm, callStack, writer);
      argPositions.add(writer.writeAndGetArgPosition(VM_SET_ARG_CMD, counter));

      counter++;
      bm = callStack.pop();
   }

   writer.fixArg(position, counter);

   // NOTE : we need to reverse the order of arguments
   if (counter > 1) {
      for (int i = 0; i < counter; i++) {
         writer.fixArg(argPositions[i], counter - i - 1);
      }
   }

   return counter;
}

void VMTapeParser :: writeBuildScriptStatement(ScriptEngineReaderBase& reader, ScriptBookmark bm, ScriptStack& callStack,
   TapeWriter& writer)
{
   if (bm.state == dfaReference) {
      int counter = writeBuildScriptArgumentList(reader, bm, callStack, writer);

      writer.write(VM_ARG_TAPE_CMD, counter);
      writer.write(VM_NEW_CMD, reader.lookup(bm));

      writer.write(VM_FREE_CMD, counter);
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

void VMTapeParser :: parseBuildScriptArgumentList(ScriptEngineReaderBase& reader, ScriptStack& callStack)
{
   pos_t exprBookmark = callStack.count();
   readToken(reader, "(");

   ScriptBookmark bm = reader.read();
   while (!reader.compare(")")) {
      parseBuildScriptStatement(reader, bm, callStack, exprBookmark);

      bm = reader.read();
   }
}

void VMTapeParser :: parseBuildScriptStatement(ScriptEngineReaderBase& reader, ScriptBookmark& bm, 
   ScriptStack& callStack, pos_t exprBookmark)
{
   if (bm.state == dfaReference) {
      callStack.push(bm);

      parseBuildScriptArgumentList(reader, callStack);

      callStack.push(bm);
   }
   else if (bm.state == dfaQuote) {
      callStack.push(bm);
   }
   else if (reader.lookup(bm).compare(";")) {
      auto expr_it = callStack.get(callStack.count() - exprBookmark);
      ScriptBookmark expr_bm = *expr_it;
      callStack.insert(expr_it, expr_bm);
      callStack.push(expr_bm);
      callStack.push(bm);
   }
   else throw SyntaxError("Invalid operation", bm.lineInfo);
}

bool VMTapeParser :: parseBuildScript(ScriptEngineReaderBase& reader, TapeWriter& writer)
{
   bool idle = true;

   ScriptBookmark bm = reader.read();
   Stack<ScriptBookmark> callStack({});
   while (!reader.eof()) {
      idle = false;

      parseBuildScriptStatement(reader, bm, callStack, 0);

      bm = reader.read();
   }

   if (!idle) {
      while (callStack.count() > 0) {
         bm = callStack.pop();

         writeBuildScriptStatement(reader, bm, callStack, writer);
      }

      return true;
   }

   return false;
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
   if (reader.compare("^")) {
      bm = reader.read();

      writer.write(VM_SET_ARG_CMD, 0);
      writer.write(VM_SEND_MESSAGE_CMD, reader.lookup(bm));
   }
   else throw SyntaxError("Invalid operation", bm.lineInfo);
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
