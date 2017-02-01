//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "inlineparser.h"
#include "bytecode.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

typedef Stack<ScriptBookmark> ScriptStack;

// --- VMTapeParser ---

VMTapeParser :: VMTapeParser()
{
   ByteCodeCompiler::loadVerbs(_verbs);
}

int VMTapeParser:: mapVerb(ident_t literal)
{
   if (_verbs.Count() == 0) {
      ByteCodeCompiler::loadVerbs(_verbs);
   }

   return _verbs.get(literal);
}

void VMTapeParser :: writeSubject(TapeWriter& writer, ident_t message)
{
   IdentifierString reference;
   reference.append('0');
   reference.append('#');
   reference.append(0x20);
   reference.append('&');
   reference.append(message);

   writer.writeCommand(PUSHG_TAPE_MESSAGE_ID, reference);
}
//bool InlineScriptParser :: insertObject(TapeWriter& writer, int bookmark, char state, ident_t token)
//{
//   if (token.compare(".")) {
//      writer.insertCommand(bookmark, POP_TAPE_MESSAGE_ID, 0);
//   }
//   else {
//      switch (state) {
//      case dfaInteger:
//         writer.insertCommand(bookmark, PUSHN_TAPE_MESSAGE_ID, token);
//         break;
//      case dfaReal:
//         writer.insertCommand(bookmark, PUSHR_TAPE_MESSAGE_ID, token);
//         break;
//      case dfaLong:
//         writer.insertCommand(bookmark, PUSHL_TAPE_MESSAGE_ID, token);
//         break;
//      case dfaQuote:
//         writer.insertCommand(bookmark, PUSHS_TAPE_MESSAGE_ID, token);
//         break;
//      case dfaFullIdentifier:
//         writer.insertCommand(bookmark, CALL_TAPE_MESSAGE_ID, token);
//         break;
//         //case dfaIdentifier:
//         //   writeSubject(writer, token);
//         //   break;
//      default:
//         return false;
//      }
//   }
//   return true;
//}

bool VMTapeParser :: writeObject(TapeWriter& writer, char state, ident_t token)
{
   if (token.compare(".")) {
      writer.writeCommand(POP_TAPE_MESSAGE_ID);
   }
   else {
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
         case dfaIdentifier:
            writeSubject(writer, token);
            break;
         default:
            return false;
      }
   }
   return true;
}

bool VMTapeParser :: parseMessage(ident_t message, IdentifierString& reference)
{
   int paramCounter = 0;
   size_t length = getlength(message);
   length = message.find('[');
   if (length != NOTFOUND_POS) {
      reference.copy(message + length + 1);
      if (reference[reference.Length() - 1] == ']') {
         reference.truncate(reference.Length() - 1);
         if (emptystr(reference)) {
            paramCounter = 12;
         }
         else paramCounter = reference.ident().toInt();

      }
      else return false;
   }
   else return false;

   int verb_id = 0;
   size_t subjPos = message.find('&');
   if (subjPos != NOTFOUND_POS) {
      reference.copy(message, subjPos);
      verb_id = mapVerb(reference);
      if (verb_id != 0) {
         message += subjPos + 1;
         length -= subjPos + 1;
      }         
   }
   else {
      reference.copy(message, length);

      verb_id = mapVerb(reference);
      if (verb_id != 0)
         message = NULL;
   }

   if (verb_id == 0)
      verb_id = (paramCounter == 0) ? GET_MESSAGE_ID : EVAL_MESSAGE_ID;

   reference.clear();
   reference.append('0' + (char)paramCounter);
   reference.append('#');
   reference.append(0x20 + (char)verb_id);
   if (!emptystr(message)) {
      reference.append('&');
      reference.append(message, length);
   }

   return true;
}

bool VMTapeParser:: writeMessage(TapeWriter& writer, ident_t message, int command)
{
   IdentifierString reference;

   if (parseMessage(message, reference)) {
      writer.writeCommand(command, reference);

      return true;
   }
   else return false;
}

//bool InlineScriptParser :: insertMessage(TapeWriter& writer, int bookmark, ident_t message, int paramCounter, int command)
//{
//   IdentifierString reference;
//
//   if (parseMessage(message, reference, paramCounter)) {
//      writer.insertCommand(bookmark, command, reference);
//
//      return true;
//   }
//   else return false;
//}
//
//bool InlineScriptParser :: writeExtension(TapeWriter& writer, ident_t message, int paramCounter, int command)
//{
//   IdentifierString reference;
//
//   int dotPos = message.find('.');
//   if (parseMessage(message + dotPos + 1, reference, paramCounter)) {
//      reference.insert(message, 0, dotPos + 1);
//
//      writer.writeCommand(command, reference);
//
//      return true;
//   }
//   else return false;
//}
//
//bool InlineScriptParser :: insertExtension(TapeWriter& writer, int bookmark, ident_t message, int paramCounter, int command)
//{
//   IdentifierString reference;
//
//   size_t dotPos = message.find('.');
//   if (parseMessage(message + dotPos + 1, reference, paramCounter)) {
//      reference.insert(message, 0, dotPos + 1);
//
//      writer.insertCommand(bookmark, command, reference);
//
//      return true;
//   }
//   else return false;
//}
//
//int InlineScriptParser :: parseExpression(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer, int bookmark)
//{
//   bm = reader.read();
//
//   IdentifierString message;
//
//   int saved = writer.Bookmark();
//   int paramCounter = 0;
//   int counter = 0;
//   while (!reader.compare(")")) {
//      int offset = writer.Bookmark() - saved;
//
//      if (reader.compare("+")) {
//         bm = reader.read();
//
//         if (!emptystr(message)) {
//            message.append("&");
//         }
//         else paramCounter = 0;
//
//         message.append(reader.lookup(bm));
//      }
//      else if (reader.compare("^")) {
//         bm = reader.read();
//         if (reader.compare("=")) {
//            insertMessage(writer, bookmark + offset, message, paramCounter, SEND_TAPE_MESSAGE_ID);
//
//            message.clear();
//         }
//         else insertMessage(writer, bookmark + offset, reader.lookup(bm), -1, SEND_TAPE_MESSAGE_ID);
//
//         counter = 1;
//      }
//      else if (reader.compare("%")) {
//         bm = reader.read();
//         if (reader.compare("=")) {
//            insertMessage(writer, bookmark + offset,  message, paramCounter, PUSHM_TAPE_MESSAGE_ID);
//
//            message.clear();
//         }
//         else {
//            ident_t s = reader.lookup(bm);
//            if (s.find('.') != NOTFOUND_POS) {
//               insertExtension(writer, bookmark + offset, reader.lookup(bm), -1, PUSHE_TAPE_MESSAGE_ID);
//            }
//            else insertMessage(writer, bookmark + offset, reader.lookup(bm), -1, PUSHM_TAPE_MESSAGE_ID);
//         }
//
//         counter++;
//      }
//      else if (reader.compare("(")) {
//         counter += parseExpression(reader, bm, writer, bookmark);
//         paramCounter++;
//      }
//      else {
//         insertObject(writer, bookmark, bm.state, reader.lookup(bm));
//         paramCounter++;
//         counter++;
//      }
//
//      bm = reader.read();
//   }
//
//   return counter;
//}
//
//void InlineScriptParser :: parseArray(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
//{
//   bm = reader.read();
//
//   int counter = 0;
//   while (!reader.compare("]")) {
//      if (reader.compare("*")) {
//         bm = reader.read();
//
//         writer.writeCommand(ARG_TAPE_MESSAGE_ID, reader.lookup(bm));
//         writer.writeCommand(NEW_TAPE_MESSAGE_ID, counter);
//
//         counter = 1;
//      }
//      else counter += parseStatement(reader, bm, writer);
//
//      bm = reader.read();
//   }
//}

void VMTapeParser:: parseStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
{
//   if (reader.compare("(")) {
//      return parseExpression(reader, bm, writer, writer.Bookmark());
//   }
//   else if (reader.compare("[")) {
//      parseArray(reader, bm, writer);
//   }
   /*else */if (reader.compare("^")) {
      bm = reader.read();

      writeMessage(writer, reader.lookup(bm), SEND_TAPE_MESSAGE_ID);
   }
//   else if (reader.compare("%")) {
//      bm = reader.read();
//      if (reader.compare("=")) {
//         writeMessage(writer, reader.lookup(bm), -1, PUSHM_TAPE_MESSAGE_ID);
//      }
//      else {
//         ident_t message = reader.lookup(bm);
//         if (message.find('.') != -1) {
//            writeExtension(writer, reader.lookup(bm), -1, PUSHE_TAPE_MESSAGE_ID);
//         }
//         else writeMessage(writer, reader.lookup(bm), -1, PUSHM_TAPE_MESSAGE_ID);
//      }
//   }
   else writeObject(writer, bm.state, reader.lookup(bm));

//   return 1;
}

void VMTapeParser :: parse(_ScriptReader& reader, MemoryDump* output)
{
   TapeWriter writer(output);

   ScriptBookmark bm = reader.read();
   while (!reader.Eof()) {      
      parseStatement(reader, bm, writer);

      bm = reader.read();
   }   

   writer.writeEndCommand();
}
