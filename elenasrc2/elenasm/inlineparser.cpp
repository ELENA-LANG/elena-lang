//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "inlineparser.h"
#include "bytecode.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

////#define TERMINAL_KEYWORD      "$terminal"
////#define LITERAL_KEYWORD       "$literal"

// --- InlineParser ---

InlineScriptParser :: InlineScriptParser()
{
   ByteCodeCompiler::loadVerbs(_verbs);
}

int InlineScriptParser :: mapVerb(ident_t literal)
{
   if (_verbs.Count() == 0) {
      ByteCodeCompiler::loadVerbs(_verbs);
   }

   return _verbs.get(literal);
}

////void InlineScriptParser :: writeTerminal(TapeWriter& writer, const wchar16_t* token, char state, int col, int row)
////{
////}
////
//////int InlineScriptParser :: parseList(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, char terminator, int level, int& counter, Mode mode)
//////{  
////   //int tapeLength = 0;
////   //do {
////   //   const wchar16_t* token = reader.token;
////   //   char state = reader.info.state;
////
////   //   if (token[0] == ',') {
////   //      reader.read();
////   //   }
////   //   else if (token[0] == terminator) {
////   //      return tapeLength;
////   //   }
////   //   // if it is a field
////   //   else if (token[0] == '.' && mode == mdTape) {
////   //      counter++;
////
////   //      tapeLength += writeVariable(writer, level, level, mdTape);
////   //      tapeLength += (parseOperations(writer, reader, locals, level + 1, mode));
////   //   }
////   //   else {
////   //      counter++;
////
////   //      tapeLength += parseExpression(writer, reader, locals, level++, mode);
////   //   }
////   //}
////   //while (true);
//////}
////
////int InlineScriptParser :: writeVariable(TapeWriter& writer, int index, int level, Mode mode)
////{
////   if (mode == mdTape) {
////      IdentifierString param;
////      param.appendInt(index - level);
////
////      // system'dynamic'tapeControl read:index &:...
////      writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, param);
////      writer.writeCommand(PUSH_TAPE_MESSAGE_ID, ConstantIdentifier(TAPECONTROL_CLASS));
////      writer.writeCommand(PUSHM_TAPE_MESSAGE_ID,  ConstantIdentifier("?#>&1"));
////
////      return 3;
////   }
////   else {
////      writer.writeCommand(PUSH_VAR_MESSAGE_ID, index);
////
////      return 1;
////   }
////}
////
////int InlineScriptParser :: parseAction(TapeWriter& writer, _ScriptReader& reader)
////{
////   Map<const wchar16_t*, int> locals;
////
////   int level = 0;
////   int paramCount = 1;    
////   locals.add(ConstantIdentifier("self"), paramCount);   // self variable
////   while (reader.token[0] == '&') {
////      paramCount++;
////      locals.add(reader.read(), paramCount);
////
////      reader.read();
////   }
////
////   // reverse the parameters order
////   Map<const wchar16_t*, int>::Iterator it = locals.start();
////   while (!it.Eof()) {
////      (*it) = paramCount - (*it) + 1;
////      it++;
////   }
////
////   if (reader.token[0] != '[')
////      throw EParseError(reader.info.column, reader.info.row);
////
////   reader.read();
////   int counter = 0;
////   do {
////      const wchar16_t* token = reader.token;
////      char state = reader.info.state;
////
////      // if it is a block end
////      if (token[0] == ']') {
////         break;
////      }
////      else if (token[0] == ';') {
////         reader.read();
////      }
////      else counter += parseStatement(writer, reader, locals, level++, mdTape);
////      
////   } while (true);
////
////   writer.writeCommand(ARG_TAPE_MESSAGE_ID, ConstantIdentifier(TAPE_CLASS));
////   writer.writeCommand(NEW_TAPE_MESSAGE_ID, counter);
////
////   return 1;
////}
////
////int InlineScriptParser :: parseStruct(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals)
////{
////   int counter = 0;
////
////   reader.read();
////   do {
////      const wchar16_t* token = reader.token;
////      char state = reader.info.state;
////
////      if (token[0] == ',') {
////         reader.read();
////      }
////      else if (token[0] == '}') {
////         break;
////      }
////      else if (state == dfaIdentifier) {
////         counter++;
////
////         IdentifierString subject;
////
////         // reserve place for param counter
////         subject.append('0');
////
////         // place dummy verb
////         subject.append('#');
////         subject.append(0x20);
////
////         subject.append('&');
////         subject.append(token);
////
////         writer.writeCommand(PUSHG_TAPE_MESSAGE_ID, subject);
////
////         token = reader.read();
////         if (token[0] != ':')
////            throw EParseError(reader.info.column, reader.info.row);
////
////         token = reader.read();
////
////         parseExpression(writer, reader, locals, 0, mdRoot);
////      }
////      else throw EParseError(reader.info.column, reader.info.row);
////   }
////   while (true);
////
////   writer.writeCommand(ARG_TAPE_MESSAGE_ID, ConstantIdentifier(STRUCT_CLASS));
////   writer.writeCommand(NEW_TAPE_MESSAGE_ID, counter << 1);
////
////   return 1;
////}
////
////int InlineScriptParser :: parseObject(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode)
////{
////   int counter = 1;
////
////   if (reader.token[0]=='{') {
////      counter = parseStruct(writer, reader, locals);
////   }
////   else if (ConstantIdentifier::compare(reader.token, "[") || ConstantIdentifier::compare(reader.token, "&")) {
////      parseAction(writer, reader);
////   }
////   else {
////      switch (reader.info.state) {
////         case dfaInteger:
////            writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, reader.token);
////            break;
////         case dfaReal:
////            writer.writeCommand(PUSHR_TAPE_MESSAGE_ID, reader.token);
////            break;
////         case dfaLong:
////            writer.writeCommand(PUSHL_TAPE_MESSAGE_ID, reader.token);
////            break;
////         case dfaQuote:
////            writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, reader.token);
////            break;
////         case dfaFullIdentifier:
////            writer.writeCallCommand(reader.token);
////            break;
////         case dfaIdentifier:
////         {
////            int index = locals.get(reader.token);
////            if (index != 0) {
////               counter = writeVariable(writer, index, level, mode);
////            }
////            else throw EParseError(reader.info.column, reader.info.row);
////
////            break;
////         }
////         default:
////            throw EParseError(reader.info.column, reader.info.row);
////      }
////   }
////
////   reader.read();
////
////   return counter;
////}

void InlineScriptParser :: readMessage(_ScriptReader& reader, IdentifierString& message)
{   
   message[0] = '0';
   message[1] = 0;

   ident_t token = reader.read();

   int verbId = mapVerb(token);
   if (verbId != 0) {
      message.append('#');
      message.append(0x20 + verbId);
   }
   else {
      message.append('#');
      message.append(EVAL_MESSAGE_ID + 0x20);
      message.append('&');
      message.append(token);
   }

   token = reader.read();
   // check if it is a field
   while (token[0]=='&') {
      message.append(token);

      token = reader.read();
      message.append(token);

      token = reader.read();
   }
}

////int InlineScriptParser :: parseReverseList(TapeWriter& writer, _ScriptReader& reader/*, Map<const wchar16_t*, int>& locals, int level, Mode mode*/)
////{
////   int paramCount = 0;
////   reader.read();
////
////   while (reader.token[0]!=')') {
////      paramCount += parseExpression(writer, reader);
////
////      reader.read();
////   }
////
//////   do {
//////            reader.read();
//////            if (reader.token[0] == ')' && paramCount == 0) {
//////               // replace EVAL with GET if required
//////               if (message[2] == 0x20 + EVAL_MESSAGE_ID) {
//////                  message[2] = 0x20 + GET_MESSAGE_ID;
//////               }
//////            }
//////            else {
//////               paramCount++;
//////               counter += parseExpression(paramWriter, reader, locals, currentLevel++, mode);
//////
//////               writer.insert(bookmark, &paramTape);
//////               paramTape.clear();
//////            }
//////   }
//////   while (reader.token[0]==',');
////
////   return paramCount;
////}
////
////int InlineScriptParser :: parseExpression(TapeWriter& writer, _ScriptReader& reader/*, Map<const wchar16_t*, int>& locals, int level, Mode mode*/)
////{
//////   size_t bookmark = writer.Position();
////   int counter = 1;
////
////   if (reader.token[0]=='(') {
////      counter = parseReverseList(writer, reader);
////
////      reader.read();
////      if (reader.token[0] == '.') {
////         counter = parseMessage(writer, reader, counter);
////      }
////   }
////   else {
////      switch (reader.info.state) {
//////         case dfaInteger:
//////            writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, reader.token);
//////            break;
//////         case dfaReal:
//////            writer.writeCommand(PUSHR_TAPE_MESSAGE_ID, reader.token);
//////            break;
//////         case dfaLong:
//////            writer.writeCommand(PUSHL_TAPE_MESSAGE_ID, reader.token);
//////            break;
////         case dfaQuote:
////            writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, reader.token);
////            break;
////         case dfaFullIdentifier:
////            writer.writeCallCommand(reader.token);
////            break;
//////         case dfaIdentifier:
//////         {
//////            int index = locals.get(reader.token);
//////            if (index != 0) {
//////               counter = writeVariable(writer, index, level, mode);
//////            }
//////            else throw EParseError(reader.info.column, reader.info.row);
//////
//////            break;
//////         }
//////         default:
//////            throw EParseError(reader.info.column, reader.info.row);
////      }
////   }
////
////
//////   int currentLevel = level;
//////
//////   if (mode == mdRoot)
//////      mode = mdRootOp;
//////
//////   if (reader.token[0]=='~') {
//////      int roleCounter = 1;
//////      while (reader.token[0]=='~') {
//////         reader.read();
//////
//////         parseObject(writer, reader, locals, currentLevel, mode);
//////
//////         currentLevel++;
//////         roleCounter++;
//////      }
//////
//////      writer.writeCommand(ARG_TAPE_MESSAGE_ID, ConstantIdentifier(WRAP_CLASS));
//////      writer.writeCommand(NEW_TAPE_MESSAGE_ID, counter << 1);
//////
//////      currentLevel = level;
//////   }
//////
//////   while (reader.token[0]=='.') {
//////      IdentifierString message; 
//////      readMessage(reader, message);
//////
//////      // if it is a method
//////      if (reader.token[0] == '(') {
//////         MemoryDump paramTape;
//////         TapeWriter paramWriter(&paramTape);
//////
//////         int paramCount = 0;
//////         do {
//////            reader.read();
//////            if (reader.token[0] == ')' && paramCount == 0) {
//////               // replace EVAL with GET if required
//////               if (message[2] == 0x20 + EVAL_MESSAGE_ID) {
//////                  message[2] = 0x20 + GET_MESSAGE_ID;
//////               }
//////            }
//////            else {
//////               paramCount++;
//////               counter += parseExpression(paramWriter, reader, locals, currentLevel++, mode);
//////
//////               writer.insert(bookmark, &paramTape);
//////               paramTape.clear();
//////            }
//////         }
//////         while (reader.token[0]==',');
//////
//////         message[0] = message[0] + paramCount;
//////
//////         if (reader.token[0] != ')')
//////            throw EParseError(reader.info.column, reader.info.row);
//////
//////         reader.read();
//////      }
//////      writer.writeCommand(mode == mdTape ? PUSHM_TAPE_MESSAGE_ID : SEND_TAPE_MESSAGE_ID, message);
//////      counter++;
//////      currentLevel = level;
//////   }
////   return counter;
////}
//
////int InlineScriptParser :: parseStatement(/*TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode*/)
////{
////   bool tapeMode = (mode == mdTape);
////
////   int counter = 0;
////   if (reader.info.state == dfaIdentifier)
////   {
////      int index = locals.get(reader.token);
////      bool newVariable = false;
////      if (index == 0) {
////         index = 1 + locals.Count();
////         newVariable = true;
////
////         locals.add(reader.token, index);
////      }
////
////      size_t bookmark = reader.Position();
////      const wchar16_t* token = reader.read();
////      // check if it is an assignment operation
////      if(token[0]=='=') {
////         reader.read();
////         counter = parseExpression(writer, reader, locals, index, mode);
////
////         if (newVariable) {
////            return counter;
////         }
////         else if (tapeMode) {
////            // !! temporally not implemented
////         } 
////         else writer.writeCommand(ASSIGN_VAR_MESSAGE_ID, index);
////      }
////      else {
////         // no operation with undeclared variable is possible
////         if (newVariable)
////            throw EParseError(reader.info.column, reader.info.row);
////
////         // otherwise rollback and continue the normal routine
////         reader.seek(bookmark);
////         token = retrieveKey(locals.start(), index, DEFAULT_STR);
////         reader.info.state = dfaIdentifier;
////
////         StringHelper::copy(reader.token, token, getlength(token) + 1);
////
////         counter = parseExpression(writer, reader, locals, level, mode);
////      }
////   }
////   else counter = parseExpression(writer, reader, locals, level, mode);
////
////   if (reader.token[0] == ';') {
////      if (!tapeMode)
////         writer.writeCommand(POP_TAPE_MESSAGE_ID, 1);
////   }
////   else if (tapeMode && reader.token[0] == ']') {
////      // do nothing
////   }
////   else if (!tapeMode && reader.info.state == dfaEOF) {
////      // do nothing
////   }
////   else throw EParseError(reader.info.column, reader.info.row);
//
////   return counter;
////}

void InlineScriptParser :: writeObject(TapeWriter& writer, char state, ident_t token, Map<ident_t, int>& locals)
{
   switch (state) {
      case dfaInteger:
         writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, token);
         break;
      case dfaReal:
         writer.writeCommand(PUSHR_TAPE_MESSAGE_ID, token);
         break;
      case dfaLong:
         writer.writeCommand(PUSHL_TAPE_MESSAGE_ID, token);
         break;
      case dfaQuote:
         writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, token);
         break;
      case dfaFullIdentifier:
         writer.writeCallCommand(token);
         break;
      case '^':
         // HOTFIX : replace EVAL to GET if no parameters are provided
         if (token[0] == '0' && token[2] == EVAL_MESSAGE_ID + 0x20) {
            ((ident_c*)token)[2] = GET_MESSAGE_ID + 0x20;
         }

         writer.writeCommand(SEND_TAPE_MESSAGE_ID, token);
         break;
      case '<':
      {
         int var_level = 0;         
         if (token[0] >= '0' && token[0] <= '9') {
            var_level = StringHelper::strToInt(token);
         }
         else var_level = locals.get(token);

         writer.writeCommand(PUSH_VAR_MESSAGE_ID, var_level);

         break;
      }
      case '>':
      {
         int var_level = 0;
         if (token[0] >= '0' && token[0] <= '9') {
            var_level = StringHelper::strToInt(token);
         }
         else var_level = locals.get(token);

         writer.writeCommand(ASSIGN_VAR_MESSAGE_ID, var_level);

         break;
      }
      //         default:
//            throw EParseError(reader.info.column, reader.info.row);
   }
}

void InlineScriptParser :: writeDump(TapeWriter& writer, MemoryDump& dump, Stack<int>& arguments, int level, Map<ident_t, int>& locals)
{
   MemoryReader reader(&dump);
   while (arguments.Count() > 0) {
      int position = arguments.pop();

      reader.seek(position);
      char state = reader.getByte();
      
      // if new variable
      if (state == ':') {
         locals.add(reader.getLiteral(DEFAULT_STR), level);
      }
      else writeObject(writer, state, reader.getLiteral(DEFAULT_STR), locals);
   }
}

inline void saveToCache(MemoryDump& cache, char state, ident_t value)
{
   MemoryWriter argWriter(&cache);

   argWriter.writeChar(state);
   argWriter.writeLiteral(value);
}

void InlineScriptParser :: parseTape(_ScriptReader& reader, TapeWriter& writer, ident_t terminator)
{
   Map<ident_t, int> locals(-1);

   Stack<int> arguments;
   Stack<Scope> scopes(Scope(0, 0));
   MemoryDump cache;
   
   int level = 0;
   while (true) {
      char state = reader.info.state;
      if (state == dfaQuote) {
         arguments.push(cache.Length());

         saveToCache(cache, dfaQuote, reader.token);

         reader.read();
      }
      else if (state == dfaEOF && emptystr(terminator)) {
         break;
      }
      else if (state == dfaFullIdentifier) {
         arguments.push(cache.Length());

         saveToCache(cache, dfaFullIdentifier, reader.token);

         reader.read();
      }
      else if (state == dfaInteger) {
         arguments.push(cache.Length());

         saveToCache(cache, dfaInteger, reader.token);

         reader.read();
      }
      else if (state == dfaLong) {
         arguments.push(cache.Length());

         saveToCache(cache, dfaLong, reader.token);

         reader.read();
      }
      else if (state == dfaReal) {
         arguments.push(cache.Length());

         saveToCache(cache, dfaReal, reader.token);

         reader.read();
      }
      else if (StringHelper::compare(reader.token, terminator)) {
         break;
      }
      else if (StringHelper::compare(reader.token, "[")) {
         scopes.push(Scope(level, arguments.Count() + 1));

         reader.read();
      }
      else if (StringHelper::compare(reader.token, "]")) {
         scopes.pop();

         writeDump(writer, cache, arguments, level, locals);
         arguments.clear();
         cache.trim(0);

         reader.read();
      }
      else if (StringHelper::compare(reader.token, "(")) {
         scopes.push(Scope(level, arguments.Count() + 1));

         reader.read();
      }
      else if (StringHelper::compare(reader.token, ")")) {
         Scope scope = scopes.pop();

         int pos = *arguments.get(arguments.Count() - scopes.peek().arg_level);

         MemoryReader cacheReader(&cache, pos);
         char op = cacheReader.getByte();
         if (op == '^') {
            // update message counter
            cache.writeByte(pos + 1, '0' + (level - scope.level));
         }

         level = scope.level;

         reader.read();
      }
      else if (StringHelper::compare(reader.token, "+")) {
         level++;

         reader.read();
      }
      else if (StringHelper::compare(reader.token, "-")) {
         level--;

         reader.read();
      }
      else if (StringHelper::compare(reader.token, ".")) {
         writer.writeCommand(POP_TAPE_MESSAGE_ID, 1);

         reader.read();
      }
      else if (StringHelper::compare(reader.token, ":")) {
         arguments.push(cache.Length());

         reader.read();
         saveToCache(cache, ':', reader.token);

         reader.read();
      }
      else if (StringHelper::compare(reader.token, "<")) {
         arguments.push(cache.Length());

         reader.read();
         saveToCache(cache, '<', reader.token);

         reader.read();
      }
      else if (StringHelper::compare(reader.token, ">")) {
         arguments.push(cache.Length());

         reader.read();
         saveToCache(cache, '>', reader.token);

         reader.read();
      }
      else if (StringHelper::compare(reader.token, "^")) {
         IdentifierString message;
         readMessage(reader, message);

         arguments.insert(arguments.get(arguments.Count() - scopes.peek().arg_level), cache.Length());
         saveToCache(cache, '^', message);
      }
   }
}

void InlineScriptParser :: parse(_ScriptReader& reader, TapeWriter& writer)
{
   reader.read();
   parseTape(reader, writer, DEFAULT_STR);

   writer.writeEndCommand();
}
