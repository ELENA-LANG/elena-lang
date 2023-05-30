//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "transformer.h"

using namespace elena_lang;

constexpr auto invokeV1 = 32;
constexpr auto loadStringCommand = 33;
constexpr auto loadTerminator = 34;

// --- ScriptEngineTextParser ---

bool ScriptEngineTextParser :: parseGrammarRule(ScriptEngineReaderBase& reader)
{
   return false;
}

void ScriptEngineTextParser :: parse(ScriptEngineReaderBase& reader, MemoryDump* output)
{
   MemoryWriter writer(output);

   ScriptBookmark bm = reader.read();
   int length = 0;
   while (!reader.eof()) {
      ustr_t s = reader.lookup(bm);

      pos_t s_len = getlength_pos(s);
      if (bm.state == dfaQuote) {
         writer.writeChar('\"');
         writer.writeString(s, s_len);
         writer.writeChar('\"');
      }
      else writer.writeString(s, s_len);

      length += s_len;
      if (length >= _width) {
         writer.writeString("\r\n", 2);
         length = 0;
      }
      else writer.writeString(" ", 1);

      bm = reader.read();
   }
   writer.writeByte(0);
}

// --- ScriptEngineBuilder ---

void ScriptEngineBuilder :: saveToken(MemoryWriter& writer, ScriptEngineReaderBase& reader, ScriptBookmark bm)
{
   ustr_t token = reader.lookup(bm);
   pos_t len = getlength_pos(token);
   writer.writeDWord(len);
   writer.writeString(token, len);
}

void ScriptEngineBuilder :: saveClass(MemoryWriter& writer, ScriptEngineReaderBase& reader, Stack<ScriptBookmark>& stack,
   int allocated, int& maxAllocated, int& maxStackSize)
{
   pos_t start = writer.position();
   int counter = 0;

   allocated++;

   ScriptBookmark bm = stack.pop();
   while (true) {
      if (bm.state == 0 || bm.state == -1) {
         saveClass(writer, reader, stack, allocated, maxAllocated, maxStackSize);
      }
      else if (bm.state == -1) {
         if (counter > ARG_COUNT) {
            writer.insertDWord(start, 0);
            writer.insertByte(start, (char)loadTerminator);

            writer.writeByte(invokeV1);
            saveToken(writer, reader, bm);

            counter++;
         }
         else {
            writer.writeByte(counter);
            saveToken(writer, reader, bm);
         }

         if (counter > maxStackSize)
            maxStackSize = counter;

         counter = 1;
      }
      else if (bm.state == dfaQuote) {
         writer.writeByte(loadStringCommand);
         saveToken(writer, reader, bm);

         allocated++;
      }
      else break;

      counter++;
      bm = stack.pop();
   }

   if (counter > maxStackSize)
      maxStackSize = counter;

   if (counter > ARG_COUNT) {
      writer.insertDWord(start, 0);
      writer.insertByte(start, (char)loadTerminator);

      writer.writeByte(invokeV1);
      saveToken(writer, reader, bm);
   }
   else {
      writer.writeByte(counter);
      saveToken(writer, reader, bm);
   }

   if (allocated > maxAllocated)
      maxAllocated = allocated;
}

void ScriptEngineBuilder :: flush(MemoryWriter& writer, ScriptEngineReaderBase& reader, Stack<ScriptBookmark>& stack)
{
   pos_t allocPos = writer.position();
   writer.writeDWord(0);

   ScriptBookmark bm = stack.pop();
   int allocated = 0;
   int stackSize = 0;
   if (bm.state == 0) {
      saveClass(writer, reader, stack, 0, allocated, stackSize);
   }

   writer.seek(allocPos);
   writer.writeDWord(allocated + stackSize);
   writer.seekEOF();
}

void ScriptEngineBuilder :: parse(ScriptEngineReaderBase& reader, MemoryDump* output)
{
   MemoryWriter writer(output);

   ScriptBookmark defVal = {};
   Stack<ScriptBookmark> stack(defVal);
   Stack<int> bookmarks(0);

   ScriptBookmark bm = reader.read();
   int level = 0;
   while (!reader.eof()) {
      if (reader.compare("(")) {
         bookmarks.push(stack.count());
         level++;
      }
      else if (reader.compare(";")) {
         auto expr_it = stack.get(stack.count() - bookmarks.peek());
         ScriptBookmark expr_bm = *expr_it;

         stack.insert(expr_it, expr_bm);
         stack.push(defVal);
      }
      else if (reader.compare(")")) {
         bookmarks.pop();
         stack.push(defVal);
         level--;
         if (!level) {
            flush(writer, reader, stack);
         }
      }
      else stack.push(bm);

      bm = reader.read();
   }
   if (output->length() == 0)
      writer.writeDWord(0);
}

bool ScriptEngineBuilder :: parseGrammarRule(ScriptEngineReaderBase& reader)
{
   return false;
}
