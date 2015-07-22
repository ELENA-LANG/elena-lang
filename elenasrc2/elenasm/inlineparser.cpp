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

////int InlineScriptParser :: parseMessage(TapeWriter& writer, _ScriptReader& reader, int counter)
////{
////}
////
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

void InlineScriptParser :: writeObject(TapeWriter& writer, _ScriptReader& reader)
{
   switch (reader.info.state) {
      case dfaInteger:
         writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, reader.token);
         break;
      case dfaReal:
         writer.writeCommand(PUSHR_TAPE_MESSAGE_ID, reader.token);
         break;
      case dfaLong:
         writer.writeCommand(PUSHL_TAPE_MESSAGE_ID, reader.token);
         break;
      case dfaQuote:
         writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, reader.token);
         break;
      case dfaFullIdentifier:
         writer.writeCallCommand(reader.token);
         break;
//         case dfaIdentifier:
//         {
//            int index = locals.get(reader.token);
//            if (index != 0) {
//               counter = writeVariable(writer, index, level, mode);
//            }
//            else throw EParseError(reader.info.column, reader.info.row);
//
//            break;
//         }
//         default:
//            throw EParseError(reader.info.column, reader.info.row);
   }

   reader.read();
}

bool InlineScriptParser :: parseToken(_ScriptReader& reader, TapeWriter& writer, int& level, Map<ident_t, int>& locals)
{
   ident_t token = reader.token;

   if (StringHelper::compare(token, "#send")) {
      parseSend(reader, writer, level, locals);
   }
   else if (StringHelper::compare(token, "#set")) {
      reader.read();

      if (reader.info.state == dfaIdentifier) {
         locals.add(reader.token, level);
      }
      reader.read();
   }
   else if (StringHelper::compare(token, "^")) {
      reader.read();

      int var_level = 0;
      if (reader.info.state == dfaIdentifier) {
         var_level = locals.get(reader.token);
      }
      else if (reader.info.state == dfaInteger) {
         var_level = StringHelper::strToInt(reader.token);
      }
      writer.writeCommand(PUSH_VAR_MESSAGE_ID, var_level);
      level++;

      reader.read();
   }
   else {
      writeObject(writer, reader);
      level++;
   }

   return true;
}

void InlineScriptParser :: parseSend(_ScriptReader& reader, TapeWriter& writer, int& level, Map<ident_t, int>& locals)
{
   IdentifierString message;

   int start_level = level;
   reader.read();
   while (true) {
      ident_t token = reader.token;

      if (token[0] == ')')
         break;
      else if (token[0] == '(') {
         start_level = level;

         reader.read();
      }
      else if (StringHelper::compare(token, "%")) {
         readMessage(reader, message);
      }
      else parseToken(reader, writer, level, locals);
   }

   int counter = level - start_level;
   if (counter > 0)
      writer.writeCommand(REVERSE_TAPE_MESSAGE_ID, counter + 1);

   message[0] = message[0] + counter;
   // HOTFIX : replace EVAL with GET if no parameters are provided
   if (counter == 0 && message[2] == (EVAL_MESSAGE_ID + 0x20)) {
      message[2] = GET_MESSAGE_ID + 0x20;
   }

   level -= counter;

   writer.writeCommand(SEND_TAPE_MESSAGE_ID, message);

   reader.read();
}

void InlineScriptParser :: parse(_ScriptReader& reader, TapeWriter& writer)
{
   Map<ident_t, int> locals(-1);
   int  level = 1;

   reader.read();
   do {
      char state = reader.info.state;

      if (state == dfaEOF) {
         break;
      }
      else parseToken(reader, writer, level, locals);
   } 
   while (true);

   writer.writeEndCommand();
}
