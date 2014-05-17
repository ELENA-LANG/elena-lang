//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "inlineparser.h"
#include "bytecode.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

//#define TERMINAL_KEYWORD      "$terminal"
//#define LITERAL_KEYWORD       "$literal"

// --- TapeWriter ---

void TapeWriter :: writeCommand(size_t command, const wchar16_t* param)
{
   MemoryWriter writer(_tape);
   size_t recordPtr = writer.Position();

   // save tape record
   writer.writeDWord(command);
   if (!emptystr(param)) {
      writer.writeDWord(recordPtr + 12);        // literal pointer place holder
   }
   else writer.writeDWord(0);

   writer.writeDWord(0);                        // next pointer place holder

   if (!emptystr(param)) {
      // save reference
      writer.writeWideLiteral(param, getlength(param) + 1);
   }

   // update next field
   writer.seek(recordPtr + 8);
   writer.writeDWord(_tape->Length());
}

void TapeWriter :: writeCommand(size_t command, size_t param)
{
   MemoryWriter writer(_tape);
   size_t recordPtr = writer.Position();

   // save tape record
   writer.writeDWord(command);
   writer.writeDWord(param);

   writer.writeDWord(recordPtr + 12);                // next pointer
}

void TapeWriter :: writeEndCommand()
{
   MemoryWriter writer(_tape);

   writer.writeDWord(0);
}

void TapeWriter :: writeCallCommand(const wchar16_t* reference)
{
   writeCommand(CALL_TAPE_MESSAGE_ID, reference);
}

// --- InlineParser ---

InlineScriptParser :: InlineScriptParser()
{
   ByteCodeCompiler::loadVerbs(_verbs);
}

int InlineScriptParser :: mapVerb(const wchar16_t* literal)
{
   if (_verbs.Count() == 0) {
      ByteCodeCompiler::loadVerbs(_verbs);
   }

   return _verbs.get(literal);
}

//void InlineScriptParser :: writeTerminal(TapeWriter& writer, const wchar16_t* token, char state, int col, int row)
//{
//}
//
int InlineScriptParser :: parseList(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, char terminator, int level, int& counter, Mode mode)
{  
   int tapeLength = 0;
   do {
      const wchar16_t* token = reader.token;
      char state = reader.info.state;

      if (token[0] == ',') {
         reader.read();
      }
      else if (token[0] == terminator) {
         return tapeLength;
      }
      // if it is a field
      else if (token[0] == '.' && mode == mdTape) {
         counter++;

         tapeLength += writeVariable(writer, level, level, mdTape);
         tapeLength += (parseOperations(writer, reader, locals, level + 1, mode));
      }
      else {
         counter++;

         tapeLength += parseExpression(writer, reader, locals, level++, mode);
      }
   }
   while (true);
}

int InlineScriptParser :: writeVariable(TapeWriter& writer, int index, int level, Mode mode)
{
   if (mode == mdTape) {
      IdentifierString param;
      param.appendInt(index - level);

      // system'dynamic'tapeControl read:index &:...
      writer.writeCommand(PUSHM_TAPE_MESSAGE_ID,  ConstantIdentifier("?#>&1"));
      writer.writeCommand(PUSH_TAPE_MESSAGE_ID, ConstantIdentifier(TAPECONTROL_CLASS));
      writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, param);

      return 3;
   }
   else {
      writer.writeCommand(PUSH_VAR_MESSAGE_ID, index);

      return 1;
   }
}

int InlineScriptParser :: parseExpression(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode)
{
   int counter = 1;
   if (mode != mdRootOp) {
      if (reader.token[0]=='{') {
         parseStruct(writer, reader, locals);
      }
      else {
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
            case dfaIdentifier:
            {
               int index = locals.get(reader.token);
               if (index != 0) {
                  counter = writeVariable(writer, index, level, mode);
               }
               else throw EParseError(reader.info.column, reader.info.row);

               break;
            }
            default:
               throw EParseError(reader.info.column, reader.info.row);
         }
      }
      reader.read();
   }
   else mode = mdRoot;

   return counter + parseOperations(writer, reader, locals, level + 1, mode);
}

int InlineScriptParser :: parseOperations(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode)
{
   int counter = 0;

   do {
      bool roleMode = false;
      const wchar16_t* token = reader.token;
      if (token[0]=='~') {
         roleMode = true;

         token = reader.read();

         if (reader.info.state == dfaFullIdentifier) {
            writer.writeCommand(PUSH_TAPE_MESSAGE_ID, token);
            counter++;
            level++;

            token = reader.read();
            if (token[0]!='.')
               throw EParseError(reader.info.column, reader.info.row);
         }
         else throw EParseError(reader.info.column, reader.info.row);
      }
      else if (token[0]!='.')
         return counter;

      IdentifierString message("0"); 
      int paramCount = 0; // NOTE: paramCount might be not equal to stackCount (the actual stack size) in the case if variables are used for virtual methods
      int stackCount = 0;

      token = reader.read();

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
      if (verbId == 0 && token[0] != '&' && token[0] != '(') {
         // replace EVAL with GET
         message[2] = 0x20 + GET_MESSAGE_ID;
      }
      else {
         while (token[0]!='(') {
            if (token[0] == '&') {
               message.append(token);
            }
            else throw EParseError(reader.info.column, reader.info.row);

            token = reader.read();
            message.append(token);

            token = reader.read();
         }

         reader.read();

         stackCount = parseList(writer, reader, locals, ')', level, paramCount, mode);
         counter += stackCount;

         message[0] = message[0] + paramCount;

         reader.read();
      }

      if (roleMode) {
         writer.writeCommand(REVERSE_TAPE_MESSAGE_ID, stackCount + 2);
         writer.writeCommand(REVERSE_TAPE_MESSAGE_ID, 2);
         writer.writeCommand(SENDR_TAPE_MESSAGE_ID, message);
      }
      else {
         writer.writeCommand(REVERSE_TAPE_MESSAGE_ID, stackCount + 1);
         writer.writeCommand(mode == mdTape ? PUSHM_TAPE_MESSAGE_ID : SEND_TAPE_MESSAGE_ID, message);
         counter++;
      }      
   }
   while(true);
}

void InlineScriptParser :: parseAction(TapeWriter& writer, _ScriptReader& reader)
{
   Map<const wchar16_t*, int> locals;

   int level = 0;
   int paramCount = 1;    // self variable
   while (reader.token[0] == '&') {
      paramCount++;
      locals.add(reader.read(), paramCount);

      reader.read();
   }

   if (reader.token[0] != '[')
      throw EParseError(reader.info.column, reader.info.row);

   reader.read();
   int counter = 0;
   do {
      const wchar16_t* token = reader.token;
      char state = reader.info.state;

      // if it is a block end
      if (token[0] == ']') {
         break;
      }
      else if (token[0] == ';') {
         reader.read();
      }
      // if it is a field
      else if (token[0] == '.') {
         writeVariable(writer, 0, level, mdTape);
         counter++;

         counter += parseOperations(writer, reader, locals, level + 1, mdTape);
      }
      /*else if (state == dfaIdentifier) {
         int index = locals.get(token);
         if (index == 0) {
            locals.add(token, 1 + locals.Count());
         }
         token = reader.read();
         state = reader.info.state;

         if (token[0] == '=' && index == 0) {
            reader.read();

            parseExpression(writer, reader, locals);
         }
         else parseExpression(writer, reader, locals);
      }
      else*/ counter += parseStatement(writer, reader, locals, level++, mdTape);

   } while (true);

   writer.writeCommand(NEWA_TAPE_MESSAGE_ID, counter);
}

void InlineScriptParser :: parseStruct(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals)
{
   int counter = 0;

   reader.read();
   do {
      const wchar16_t* token = reader.token;
      char state = reader.info.state;

      if (token[0] == ',') {
         reader.read();
      }
      else if (token[0] == '}') {
         break;
      }
      else if (state == dfaIdentifier) {
         counter++;

         IdentifierString subject;

         // reserve place for param counter
         subject.append('0');

         // place dummy verb
         subject.append('#');
         subject.append(0x20);

         subject.append('&');
         subject.append(token);

         writer.writeCommand(PUSHG_TAPE_MESSAGE_ID, subject);

         token = reader.read();
         if (token[0] != ':')
            throw EParseError(reader.info.column, reader.info.row);

         token = reader.read();
         if (ConstantIdentifier::compare(token, "[") || ConstantIdentifier::compare(token, "&")) {
            parseAction(writer, reader);
            
            reader.read();
         }
         else parseExpression(writer, reader, locals, 0, mdRoot);
      }
      else throw EParseError(reader.info.column, reader.info.row);
   }
   while (true);

   writer.writeCommand(NEWS_TAPE_MESSAGE_ID, counter << 1);
}

int InlineScriptParser :: parseStatement(TapeWriter& writer, _ScriptReader& reader, Map<const wchar16_t*, int>& locals, int level, Mode mode)
{
   bool tapeMode = (mode == mdTape);
   int counter = parseExpression(writer, reader, locals, level, mode);

   if (reader.token[0] == ';') {
      if (tapeMode)
         writer.writeCommand(POP_TAPE_MESSAGE_ID, 1);
   }
   else if (tapeMode && reader.token[0] == ']') {
      // do nothing
   }
   else if (!tapeMode && reader.info.state == dfaEOF) {
      // do nothing
   }
   else throw EParseError(reader.info.column, reader.info.row);

   return counter;
}

void InlineScriptParser :: parseScript(MemoryDump& tape, _ScriptReader& reader)
{
   TapeWriter                 writer(&tape);
   Map<const wchar16_t*, int> locals;

   reader.read();
   do {
      const wchar16_t* token = reader.token;
      char state = reader.info.state;

      // if it is a variable
      if (state == dfaEOF) {
         break;
      }
      else if (token[0] == ';') {
         reader.read();
      }
      else if (state == dfaIdentifier) {
         int count = locals.Count();
         int index = locals.get(token);
         if (index == 0) {
            locals.add(token, 1 + count);
         }
         token = reader.read();
         state = reader.info.state;

         if (token[0] == '=' && index == 0) {
            reader.read();

            parseExpression(writer, reader, locals, count - 1, mdRoot);
         }
         else {
            writeVariable(writer, index, count, mdRoot);

            parseStatement(writer, reader, locals, 1 + count, mdRootOp);
         }
      }
      else parseStatement(writer, reader, locals, locals.Count(), mdRoot);

   } while (true);

   writer.writeEndCommand();
}

void InlineScriptParser :: parseDirectives(MemoryDump& tape, _ScriptReader& reader)
{
   TapeWriter writer(&tape);

   do {
      const wchar16_t* token = reader.read();

      if (token[0]==';') {
         break;
      }
      else if(ConstantIdentifier::compare(token, "#start")) {
         writer.writeCommand(START_VM_MESSAGE_ID);
      }
      else if(ConstantIdentifier::compare(token, "#config")) {
         token = reader.read();
         if (reader.info.state == dfaIdentifier) {
            writer.writeCommand(LOAD_VM_MESSAGE_ID, token);
         }
         else throw EParseError(reader.info.column, reader.info.row);
      }
   }
   while(true);
}

//void ScriptVMCompiler :: parseVMCommand(TextSourceReader& source, wchar16_t* token)
//{
//   LineInfo info;
//   if(ConstantIdentifier::compare(token, "start")) {
//      _writer.writeCommand(START_VM_MESSAGE_ID);
//   }
//   else if(ConstantIdentifier::compare(token, "config")) {
//      info = source.read(token, LINE_LEN);
//
//      if (info.state == dfaIdentifier) {
//         _writer.writeCommand(LOAD_VM_MESSAGE_ID, token);
//      }
//      else throw EParseError(info.column, info.row);
//   }
////   else if(ConstantIdentifier::compare(token, "map")) {
////      info = source.read(token, LINE_LEN);
////
////      IdentifierString forward;
////      if (info.state == dfaFullIdentifier) {
////         forward.append(token);
////
////         info = source.read(token, LINE_LEN);
////         if(!ConstantIdentifier::compare(token, "="))
////            throw EParseError(info.column, info.row);
////
////         info = source.read(token, LINE_LEN);
////         if (info.state == dfaFullIdentifier) {
////            forward.append('=');
////            forward.append(token);
////            _writer.writeCommand(MAP_VM_MESSAGE_ID, forward);
////         }
////         else throw EParseError(info.column, info.row);
////      }
////      else throw EParseError(info.column, info.row);
////   }
////   else if(ConstantIdentifier::compare(token, "use")) {
////      info = source.read(token, LINE_LEN);
////
////      if (info.state == dfaQuote) {
////         QuoteTemplate<TempString> quote(info.line);
////
////         _writer.writeCommand(USE_VM_MESSAGE_ID, quote);
////      }
////      else {
////         Path package;
////         package.append(token);
////
////         info = source.read(token, LINE_LEN);
////         if(!ConstantIdentifier::compare(token, "="))
////            throw EParseError(info.column, info.row);
////
////         info = source.read(token, LINE_LEN);
////         if (info.state == dfaQuote) {
////            QuoteTemplate<TempString> quote(info.line);
////
////            package.append('=');
////            package.append(quote);
////            _writer.writeCommand(USE_VM_MESSAGE_ID, package);
////         }
////         else throw EParseError(info.column, info.row);
////      }
////   }
//////   else if(ConstantIdentifier::compare(token, "var")) {
//////      parseVariable(source, token);
//////   }
//   else throw EParseError(info.column, info.row);
//}
//
//void ScriptVMCompiler :: parseNewObject(TextSourceReader& source, wchar16_t* token)
//{
//   LineInfo info;
//
//   info = source.read(token, LINE_LEN);
//
//   // if it is a reference
//   if(info.state == dfaFullIdentifier) {
//      _writer.writeCommand(NEW_TAPE_MESSAGE_ID, token);
//   }
//   else throw EParseError(info.column, info.row);
//}
//
////void InlineParser :: parseVariable(TextSourceReader& source, wchar16_t* token)
////{
////   _writer.writeCommand(PUSH_VAR_MESSAGE_ID, StringHelper::strToInt(token));
////}
//
//void* ScriptVMCompiler :: generate()
//{
//   _writer.writeEndCommand();
//
//   return _writer.extract();
//}