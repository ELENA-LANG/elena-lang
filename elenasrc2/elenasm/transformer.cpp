//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "transformer.h"

using namespace _ELENA_;

// --- TextParser ---

void TextParser :: parse(_ScriptReader& reader, MemoryDump* output)
{
   MemoryWriter writer(output);

   ScriptBookmark bm = reader.read();
   int length = 0;
   while (!reader.Eof()) {
      ident_t s = reader.lookup(bm);

      int s_len = getlength(s);
      if (bm.state == _ELENA_TOOL_::dfaQuote) {
         writer.writeChar('\"');
         writer.writeLiteral(s, s_len);
         writer.writeChar('\"');
      }
      else writer.writeLiteral(s, s_len);

      length += s_len;
      if (length >= _width) {
         writer.writeLiteral("\r\n", 2);
         length = 0;
      }
      else writer.writeLiteral(" ", 1);

      bm = reader.read();
   }
   writer.writeByte(0);
}

// --- Transformer::Scope ---

void Transformer::Scope :: writeToken(ident_t token, ScriptLog&)
{
   switch (channel) {
      case 1:
         buffer1.insert(token, 0);
         break;
      case 2:
         buffer2.append(token);
         break;
   }
}

void Transformer::Scope :: copyForward()
{
   buffer2.append(buffer1.str());
   buffer1.clear();
}

void Transformer::Scope :: flush(Scope* parent, ScriptLog& log)
{
   buffer1.append('\0');
   buffer2.append('\0');
   
   if (parent != NULL) {
      parent->counter += counter;

      parent->buffer1.insert(buffer2.str(), 0);
      parent->buffer1.insert(buffer1.str(), 0);
   }
   else {
      log.write(buffer1.str());
      log.write(buffer2.str());      
   }
}

// --- Transformer ---

Transformer :: Transformer(_Parser* baseParser)
{
   _baseParser = baseParser;
}

void Transformer :: writeToken(ident_t token, Scope* scope, ScriptLog& log)
{
   if (scope && scope->channel != 0) {
      scope->writeToken(token, log); 
      if (!scope->appendMode)
         scope->writeToken(" ", log);
   }
   else {
      log.write(token);
      if (!(scope->appendMode))
         log.write(" ");
   }
}

bool Transformer :: parseDirective(_ScriptReader& reader, Scopes& scopes, ScriptLog& log)
{
   if (reader.compare("(")) {
      scopes.push(new Scope());
   }
   else if (reader.compare(")")) {
      Scope* scope = scopes.pop();

      scope->flush(scopes.peek(), log);

      freeobj(scope);
   }
   else if (reader.compare("<")) {
      scopes.peek()->channel = 1;
      scopes.peek()->appendMode = false;
   }
   else if (reader.compare(">")) {
      scopes.peek()->channel = 2;
      scopes.peek()->appendMode = false;
   }
   else if (reader.compare("<<")) {
      scopes.peek()->channel = 1;
      scopes.peek()->appendMode = true;
   }
   else if (reader.compare(">>")) {
      scopes.peek()->channel = 2;
      scopes.peek()->appendMode = true;
   }
   else if (reader.compare("~")) {
      scopes.peek()->copyForward();
   }
   else if (reader.compare("+")) {
      scopes.peek()->level++;
   }
   else if (reader.compare("-")) {
      scopes.peek()->level--;
   }
   else if (reader.compare("+=")) {
      scopes.peek()->counter++;
   }
   else if (reader.compare("-=")) {
      scopes.peek()->counter--;
   }
   else if (reader.compare("=")) {
      scopes.peek()->writeLevel(log);
      scopes.peek()->level = 0;
   }
   else if (reader.compare("#")) {
      scopes.peek()->writeCounter(log);
      scopes.peek()->counter = 0;
   }
   else return false;

   return true;
}

void Transformer :: parse(_ScriptReader& reader, MemoryDump* output)
{
   ScriptLog log;
   Scopes    scopes;

   ScriptBookmark bm = reader.read();
   while (!reader.Eof()) {
      if (bm.state == _ELENA_TOOL_::dfaQuote) {
         writeToken(reader.lookup(bm), scopes.peek(), log);
      }
      else if (!parseDirective(reader, scopes, log))
         throw EParseError(bm.column, bm.row);

      bm = reader.read();
   }

   IdentifierTextReader logReader((const char*)log.getBody());
   ScriptReader scriptReader(&logReader);

   _baseParser->parse(scriptReader, output);
}

// --- Builder ---

constexpr auto invokeV1 = 32;
constexpr auto loadStringCommand = 33;
constexpr auto loadTerminator = 34;

Builder :: Builder()
{
}

void Builder :: saveToken(MemoryWriter& writer, _ScriptReader& reader, ScriptBookmark bm)
{
   ident_t token = reader.lookup(bm);
   pos_t len = getlength(token);
   writer.writeDWord(len);
   writer.writeLiteral(token, len);
}

void Builder :: saveClass(MemoryWriter& writer, _ScriptReader& reader, Stack<ScriptBookmark>& stack, int allocated,
   int& maxAllocated, int& maxStackSize)
{
   pos_t start = writer.Position();
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
      else if (bm.state == _ELENA_TOOL_::dfaQuote) {
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

void Builder :: flush(MemoryWriter& writer, _ScriptReader& reader, Stack<ScriptBookmark>& stack)
{
   pos_t sizePos = writer.Position();
   writer.writeDWord(0);
   pos_t allocPos = writer.Position();
   writer.writeDWord(0);

   ScriptBookmark bm = stack.pop();
   int allocated = 0;
   int stackSize = 0;
   if (bm.state == 0) {
      saveClass(writer, reader, stack, 0, allocated, stackSize);
   }

   pos_t len = writer.Position() - sizePos - 8;

   writer.seek(sizePos);
   writer.writeDWord(len);
   writer.seek(allocPos);
   writer.writeDWord(allocated + stackSize);
   writer.seekEOF();
}

void Builder :: parse(_ScriptReader& reader, MemoryDump* output)
{
   MemoryWriter writer(output);

   ScriptBookmark defVal;
   Stack<ScriptBookmark> stack(defVal);
   Stack<int> bookmarks(0);

   ScriptBookmark bm = reader.read();
   int level = 0;
   while (!reader.Eof()) {
      if (reader.compare("(")) {
         bookmarks.push(stack.Count());
         level++;
      }
      else if (reader.compare(";")) {
         auto expr_it = stack.get(stack.Count() - bookmarks.peek());
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

   writer.writeDWord(0);
}
