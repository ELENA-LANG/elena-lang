//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "inlineparser.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

#define TERMINAL_KEYWORD      "$terminal"
#define LITERAL_KEYWORD       "$literal"

// --- verbs ---

static MessageMap _verbs;

int mapVerb(const wchar16_t* literal)
{
   if (_verbs.Count() == 0) {
      loadVerbs(_verbs);
   }

   return _verbs.get(literal);
}

// --- TapeWriter ---

TapeWriter :: TapeWriter()
{
}

void TapeWriter :: writeCommand(size_t command, const wchar16_t* param)
{
   MemoryWriter writer(&_tape);
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
   writer.writeDWord(_tape.Length());
}

void TapeWriter :: writeCommand(size_t command, size_t param)
{
   MemoryWriter writer(&_tape);
   size_t recordPtr = writer.Position();

   // save tape record
   writer.writeDWord(command);
   writer.writeDWord(param);

   writer.writeDWord(recordPtr + 12);                // next pointer
}

void TapeWriter :: writeEndCommand()
{
   MemoryWriter writer(&_tape);

   writer.writeDWord(0);
}

void TapeWriter :: writeCallCommand(const wchar16_t* reference)
{
   writeCommand(CALL_TAPE_MESSAGE_ID, reference);
}

// --- InlineParser ---

void ScriptVMCompiler :: parseVMCommand(TextSourceReader& source, wchar16_t* token)
{
   LineInfo info;
   if(ConstantIdentifier::compare(token, "start")) {
      _writer.writeCommand(START_VM_MESSAGE_ID);
   }
   else if(ConstantIdentifier::compare(token, "config")) {
      info = source.read(token, IDENTIFIER_LEN);

      if (info.state == dfaIdentifier) {
         _writer.writeCommand(LOAD_VM_MESSAGE_ID, token);
      }
      else throw EParseError(info.column, info.row);
   }
//   else if(ConstantIdentifier::compare(token, "map")) {
//      info = source.read(token, IDENTIFIER_LEN);
//
//      IdentifierString forward;
//      if (info.state == dfaFullIdentifier) {
//         forward.append(token);
//
//         info = source.read(token, IDENTIFIER_LEN);
//         if(!ConstantIdentifier::compare(token, "="))
//            throw EParseError(info.column, info.row);
//
//         info = source.read(token, IDENTIFIER_LEN);
//         if (info.state == dfaFullIdentifier) {
//            forward.append('=');
//            forward.append(token);
//            _writer.writeCommand(MAP_VM_MESSAGE_ID, forward);
//         }
//         else throw EParseError(info.column, info.row);
//      }
//      else throw EParseError(info.column, info.row);
//   }
//   else if(ConstantIdentifier::compare(token, "use")) {
//      info = source.read(token, IDENTIFIER_LEN);
//
//      if (info.state == dfaQuote) {
//         QuoteTemplate<TempString> quote(info.line);
//
//         _writer.writeCommand(USE_VM_MESSAGE_ID, quote);
//      }
//      else {
//         Path package;
//         package.append(token);
//
//         info = source.read(token, IDENTIFIER_LEN);
//         if(!ConstantIdentifier::compare(token, "="))
//            throw EParseError(info.column, info.row);
//
//         info = source.read(token, IDENTIFIER_LEN);
//         if (info.state == dfaQuote) {
//            QuoteTemplate<TempString> quote(info.line);
//
//            package.append('=');
//            package.append(quote);
//            _writer.writeCommand(USE_VM_MESSAGE_ID, package);
//         }
//         else throw EParseError(info.column, info.row);
//      }
//   }
////   else if(ConstantIdentifier::compare(token, "var")) {
////      parseVariable(source, token);
////   }
   else throw EParseError(info.column, info.row);
}

////void InlineParser :: parseAction(TextSourceReader& source, wchar16_t* token/*, Terminal* terminal*/)
////{
////   LineInfo info = source.read(token, IDENTIFIER_LEN);
////
////   _writer.writeCommand(PUSHB_TAPE_MESSAGE_ID, token);
////}

void ScriptVMCompiler :: parseTerminal(const wchar16_t* token, char state, int row, int column)
{
   switch (state) {
//      case dfaInteger:
//         _writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, token);
//         break;
//      case dfaReal:
//         _writer.writeCommand(PUSHR_TAPE_MESSAGE_ID, token);
//         break;
//      case dfaLong:
//         _writer.writeCommand(PUSHL_TAPE_MESSAGE_ID, token);
//         break;
      case dfaQuote:
         _writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, token);
         break;
      case dfaFullIdentifier:
         _writer.writeCallCommand(token);
         break;
      default:
         throw EParseError(column, row);
   }
}

void ScriptVMCompiler :: parseMessage(TextSourceReader& source, wchar16_t* token, int command)
{
   IdentifierString message("0"); 

   LineInfo info = source.read(token, IDENTIFIER_LEN);

   int verbId = mapVerb(token);

   if (verbId == 0)
      throw EParseError(info.column, info.row);

//   if (command == DSEND_TAPE_MESSAGE_ID) {
//      _writer.writeCommand(command, verbId);
//   }
//   else {
      message.append('#');
      message.append(0x20 + verbId);

      info = source.read(token, IDENTIFIER_LEN);
      while (token[0]!='(') {
         if (token[0] == '&') {
            message.append(token);
         }
         else throw EParseError(info.column, info.row);

         info = source.read(token, IDENTIFIER_LEN);
         message.append(token);

         info = source.read(token, IDENTIFIER_LEN);
      }

      info = source.read(token, IDENTIFIER_LEN);

      size_t paramCount = 0;
      if (token[0]!=')' || info.state == dfaQuote) {
         while (true) {
            paramCount++;

            // read argument
            if(info.state == dfaQuote) {
               QuoteTemplate<TempString> quote(info.line);

               parseTerminal(quote, dfaQuote, info.row, info.column);
            }
            else parseTerminal(token, info.state, info.row, info.column);

            info = source.read(token, IDENTIFIER_LEN);
            while(token[0] == '.') {
               parseMessage(source, token, SEND_TAPE_MESSAGE_ID);
            }

            if(token[0] == ',') {
               info = source.read(token, IDENTIFIER_LEN);
            }
            else break;
         }
      }
//      if (info.state != dfaInteger)
//         throw EParseError(info.column, info.row);
//      paramCount = StringHelper::strToInt(token);

      if (token[0] != ')')
         throw EParseError(info.column, info.row);

      message[0] = message[0] + paramCount;

      _writer.writeCommand(command, message);
//   }
}

//void InlineParser :: parseDynamicArray(TextSourceReader& source, wchar16_t* token, Terminal* terminal)
//{
//   LineInfo info;
//
//   info = source.read(token, IDENTIFIER_LEN);
//
//   // if it is dynamic message operation
//   if(ConstantIdentifier::compare(token, "^")) {
//      parseMessage(source, token, terminal, DSEND_TAPE_MESSAGE_ID);
//   }
//   else throw EParseError(info.column, info.row);
//}
//
//void InlineParser :: parseVariable(TextSourceReader& source, wchar16_t* token)
//{
//   _writer.writeCommand(PUSH_VAR_MESSAGE_ID, StringHelper::strToInt(token));
//}

void ScriptVMCompiler :: compile(TextReader* script)
{
   TextSourceReader source(4, script);

   LineInfo info;
   wchar16_t  token[IDENTIFIER_LEN];

   do {
      info = source.read(token, IDENTIFIER_LEN);

      // if it is a vm command
      if (info.state == dfaEOF) {
         break;
      }
      else if(info.state == dfaKeyword) {
         parseVMCommand(source, token + 1);
      }
      // if it is a quote
      else if(info.state == dfaQuote) {
         QuoteTemplate<TempString> quote(info.line);

         parseTerminal(quote, dfaQuote, info.row, info.column);
      }
//      // if it is a quote
//      //else if(ConstantIdentifier::compare(token, "@")) {
//      //   parseAction(source, token/*, terminal*/);
//      //}
////      else if(ConstantIdentifier::compare(token, "~")) {
////         parseRole(source, token, terminal);
////      }
      else if(ConstantIdentifier::compare(token, ".")) {
         parseMessage(source, token, SEND_TAPE_MESSAGE_ID);
      }
//      else if (ConstantIdentifier::compare(token, "^")) {
//         _writer.writeCommand(POP_TAPE_MESSAGE_ID, 1);
//      }
//      else if (ConstantIdentifier::compare(token, "(")) {
//         _writer.writeCommand(START_TAPE_MESSAGE_ID);
//      }
//      else if (ConstantIdentifier::compare(token, ")")) {
//         parseDynamicArray(source, token, terminal);
//      }
////      else if(ConstantIdentifier::compare(token, "%")) {
////         parseMessage(source, token, terminal, PUSHM_TAPE_MESSAGE_ID);
////      }
////      // start the object construction
////      else if(ConstantIdentifier::compare(token, "{")) {
////         _writer.writeCommand(START_TAPE_MESSAGE_ID);
////      }
////      else if(ConstantIdentifier::compare(token, "}")) {
////         parseNewObject(source, token, terminal);
////      }
//      // if it is a terminal symbol
//      else if (ConstantIdentifier::compare(token, TERMINAL_KEYWORD)) {
//         parseTerminal(terminal->value, terminal->state, terminal->row, terminal->col);
//      }
//      // if it is a literal terminal symbol
//      else if (ConstantIdentifier::compare(token, LITERAL_KEYWORD)) {
//         _writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, terminal->value);
//      }
//      // if it is a variable
//      // should be the last one
//      else if (token[0] == '$' && isNumeric(token + 1, getlength(token + 1))) {
//         parseVariable(source, token + 1);
//      }
      else parseTerminal(token, info.state, info.row, info.column);
   }
   while (true);
}

void* ScriptVMCompiler :: generate()
{
   _writer.writeEndCommand();

   return _writer.extract();
}