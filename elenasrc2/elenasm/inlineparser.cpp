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

void InlineParser :: parseVMCommand(TextSourceReader& source, wchar16_t* token)
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
   else if(ConstantIdentifier::compare(token, "map")) {
      info = source.read(token, IDENTIFIER_LEN);

      IdentifierString forward;
      if (info.state == dfaFullIdentifier) {
         forward.append(token);

         info = source.read(token, IDENTIFIER_LEN);
         if(!ConstantIdentifier::compare(token, "="))
            throw EParseError(info.column, info.row);

         info = source.read(token, IDENTIFIER_LEN);
         if (info.state == dfaFullIdentifier) {
            forward.append('=');
            forward.append(token);
            _writer.writeCommand(MAP_VM_MESSAGE_ID, forward);
         }
         else throw EParseError(info.column, info.row);
      }
      else throw EParseError(info.column, info.row);
   }
   else if(ConstantIdentifier::compare(token, "use")) {
      info = source.read(token, IDENTIFIER_LEN);

      if (info.state == dfaQuote) {
         QuoteTemplate<TempString> quote(info.line);

         _writer.writeCommand(USE_VM_MESSAGE_ID, quote);
      }
      else {
         Path package;
         package.append(token);

         info = source.read(token, IDENTIFIER_LEN);
         if(!ConstantIdentifier::compare(token, "="))
            throw EParseError(info.column, info.row);

         info = source.read(token, IDENTIFIER_LEN);
         if (info.state == dfaQuote) {
            QuoteTemplate<TempString> quote(info.line);

            package.append('=');
            package.append(quote);
            _writer.writeCommand(USE_VM_MESSAGE_ID, package);
         }
         else throw EParseError(info.column, info.row);
      }
   }
//   else if(ConstantIdentifier::compare(token, "var")) {
//      parseVariable(source, token);
//   }
   else throw EParseError(info.column, info.row);
}

//void InlineParser :: parseAction(TextSourceReader& source, wchar16_t* token/*, Terminal* terminal*/)
//{
//   LineInfo info = source.read(token, IDENTIFIER_LEN);
//
//   _writer.writeCommand(PUSHB_TAPE_MESSAGE_ID, token);
//}

void InlineParser :: parseTerminal(const wchar16_t* token, char state, int row, int column)
{
   switch (state) {
      case dfaInteger:
         _writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, token);
         break;
      case dfaReal:
         _writer.writeCommand(PUSHR_TAPE_MESSAGE_ID, token);
         break;
      case dfaLong:
         _writer.writeCommand(PUSHL_TAPE_MESSAGE_ID, token);
         break;
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

void InlineParser :: parseMessage(TextSourceReader& source, wchar16_t* token, Terminal* terminal, int command)
{
   IdentifierString message("0"); 

   LineInfo info;
   info = source.read(token, IDENTIFIER_LEN);
   
   int verbId = mapVerb(token);
   if (verbId == 0)
      throw EParseError(info.column, info.row);

   message.append('#');
   message.append(0x20 + verbId);

   info = source.read(token, IDENTIFIER_LEN);
   while (token[0]!='(') {
      if (token[0] == '&') {
         message.append(token);
      }
      else throw EParseError(terminal->col, terminal->row);

      info = source.read(token, IDENTIFIER_LEN);
      message.append(token);

      info = source.read(token, IDENTIFIER_LEN);
   }

   info = source.read(token, IDENTIFIER_LEN);
   if (info.state != dfaInteger)
      throw EParseError(info.column, info.row);

   size_t paramCount = StringHelper::strToInt(token);

   info = source.read(token, IDENTIFIER_LEN);
   if (token[0] != ')')
      throw EParseError(terminal->col, terminal->row);

   message[0] = message[0] + paramCount;

   _writer.writeCommand(command, message);

   //   if (StringHelper::compare(token, TERMINAL_KEYWORD)) {
   //      if (terminal->state != dfaIdentifier)
   //         throw EParseError(terminal->col, terminal->row);

   //      messageId = mapVerb(terminal->value);
   //   }
   //   else messageId = mapVerb(token);
   //}
   //else throw EParseError(info.column, info.row);
}

void InlineParser :: compile(TextReader* script, Terminal* terminal)
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
//      // if it is a quote
//      else if(info.state == dfaQuote) {
//         QuoteTemplate<TempString> quote(info.line);
//
//         parseTerminal(quote, dfaQuote, info.row, info.column);
//      }
      //else if(ConstantIdentifier::compare(token, "@")) {
      //   parseAction(source, token/*, terminal*/);
      //}
//      else if(ConstantIdentifier::compare(token, "~")) {
//         parseRole(source, token, terminal);
//      }
      else if(ConstantIdentifier::compare(token, "^")) {
         parseMessage(source, token, terminal, SEND_TAPE_MESSAGE_ID);
      }
//      else if(ConstantIdentifier::compare(token, "%")) {
//         parseMessage(source, token, terminal, PUSHM_TAPE_MESSAGE_ID);
//      }
//      // start the object construction
//      else if(ConstantIdentifier::compare(token, "{")) {
//         _writer.writeCommand(START_TAPE_MESSAGE_ID);
//      }
//      else if(ConstantIdentifier::compare(token, "}")) {
//         parseNewObject(source, token, terminal);
//      }
      // if it is a terminal symbol
      else if (ConstantIdentifier::compare(token, TERMINAL_KEYWORD)) {
         parseTerminal(terminal->value, terminal->state, terminal->row, terminal->col);
      }
      else parseTerminal(token, info.state, info.row, info.column);
//   //   else if (StringHelper::compare(token, LITERAL_KEYWORD)) {
//   //      _writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, terminal->value);
//   //   }
   }
   while (true);
}

void* InlineParser :: generate()
{
   _writer.writeEndCommand();

   return _writer.extract();
}