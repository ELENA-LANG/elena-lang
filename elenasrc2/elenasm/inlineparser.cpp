//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "inlineparser.h"
#include "bytecode.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

//typedef Stack<ScriptBookmark> ScriptStack;

// --- VMTapeParser ---

VMTapeParser :: VMTapeParser()
{
}

////int VMTapeParser:: mapVerb(ident_t literal)
////{
////   //if (_verbs.Count() == 0) {
////   //   ByteCodeCompiler::loadVerbs(_verbs);
////   //}
////
////   //return _verbs.get(literal);
////
////   return 0; // !! temporal
////}
//
//void VMTapeParser :: writeSubject(TapeWriter& writer, ident_t message)
//{
//   IdentifierString reference;
//   reference.append('0');
//   reference.append('#');
//   reference.append(0x20);
//   reference.append('&');
//   reference.append(message);
//
//   writer.writeCommand(PUSHG_TAPE_MESSAGE_ID, reference);
//}
//
//bool VMTapeParser :: writeObject(TapeWriter& writer, int state, ident_t token)
//{
//   if (token.compare(".")) {
//      writer.writeCommand(POP_TAPE_MESSAGE_ID);
//   }
//   else {
//      switch (state) {
//         case dfaInteger:
//            writer.writeCommand(PUSHN_TAPE_MESSAGE_ID, token);
//            break;
//         case dfaReal:
//            writer.writeCommand(PUSHR_TAPE_MESSAGE_ID, token);
//            break;
//         case dfaLong:
//            writer.writeCommand(PUSHL_TAPE_MESSAGE_ID, token);
//            break;
//         case dfaQuote:
//            writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, token);
//            break;
//         case dfaFullIdentifier:
//            writer.writeCallCommand(token);
//            break;
//         case dfaIdentifier:
//            writeSubject(writer, token);
//            break;
//         default:
//            return false;
//      }
//   }
//   return true;
//}
//
//bool VMTapeParser :: writeArgument(TapeWriter& writer, int state, ident_t token)
//{
//   if (state == dfaFullIdentifier) {
//      writer.writeCommand(ARG_TAPE_MESSAGE_ID, token);
//
//      return true;
//   }
//   else return false;
//}
//
//bool VMTapeParser :: writeArray(TapeWriter& writer, int state, ident_t token)
//{
//   throw InternalError("Not yet implemented"); // !! temporal
//
//   //if (state == dfaInteger) {
//   //   int value = token.toInt();
//
//   //   writer.writeCommand(NEW_TAPE_MESSAGE_ID, value);
//
//   //   return true;
//   //}
//   //else return false;
//}
//
//bool VMTapeParser :: parseMessage(ident_t message, IdentifierString& reference)
//{
//   int paramCounter = 0;
//   size_t length = getlength(message);
//   length = message.find('[');
//   if (length != NOTFOUND_POS) {
//      reference.copy(message + length + 1);
//      if (reference[reference.Length() - 1] == ']') {
//         reference.truncate(reference.Length() - 1);
//         //if (emptystr(reference)) {
//         //   paramCounter = OPEN_ARG_COUNT;
//         //}
//         /*else */paramCounter = reference.ident().toInt();
//
//      }
//      else return false;
//   }
//   else return false;
//
//   reference.clear();
//   reference.append('0' + (char)paramCounter);
//   reference.append(message, length);
//
//   return true;
//}
//
//bool VMTapeParser:: writeMessage(TapeWriter& writer, ident_t message, int command)
//{
//   IdentifierString reference;
//
//   if (parseMessage(message, reference)) {
//      writer.writeCommand(command, reference);
//
//      return true;
//   }
//   else return false;
//}
//
//bool VMTapeParser :: writeExtension(TapeWriter& writer, ident_t message, int command)
//{
//   IdentifierString reference;
//
//   size_t dotPos = message.find('.');
//   if (parseMessage(message + dotPos + 1, reference)) {
//      reference.insert(message, 0, dotPos + 1);
//
//      writer.writeCommand(command, reference);
//
//      return true;
//   }
//   else return false;
//}

void VMTapeParser :: parseInlineStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
{
   if (reader.compare("#start")) {
      writer.writeCommand(START_VM_MESSAGE_ID);
   }
   else throw EParseError(bm.column, bm.row);
}

inline void readToken(_ScriptReader& reader, ident_t expectedToken)
{
   ScriptBookmark bm;

   bm = reader.read();

   if (!reader.compare(expectedToken))
      throw EParseError(bm.column, bm.row);
}

//void VMTapeParser :: writeArgumentList(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
//{
//   if (bm.state == dfaFullIdentifier) {
//
//   }
//
//}
//
//int VMTapeParser :: parseArgumentList(_ScriptReader& reader, TapeWriter& writer)
//{
//   ScriptBookmark bm;
//   Stack<ScriptBookmark> callStack;
//
//   bm = reader.read();
//   while (reader.compare(")")) {
//      callStack.push(bm);
//      if (bm.state == dfaFullIdentifier)
//         skipArgumentList(reader);
//
//      bm = reader.read();
//   }
//
//   int counter = callStack.Count();
//   while (callStack.Count() > 0) {
//      bm = callStack.pop();
//      if (bm.state == dfaFullIdentifier) {
//         writeArgumentList(reader, bm, writer);
//
//
//      }
//      else throw EParseError(bm.column, bm.row);
//   }
//
//   return counter;
//}

//void VMTapeParser:: parseBuildScriptStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
//{
//   if (bm.state == dfaFullIdentifier) {
//      readToken(reader, "(");
//
//      int counter = parseArgumentList(reader, writer);
//
//   }
//   else throw EParseError(bm.column, bm.row);
//
////   if (reader.compare("*")) {
////      bm = reader.read();
////
////      writeArgument(writer, bm.state, reader.lookup(bm));
////   }
////   else if (reader.compare("=")) {
////      bm = reader.read();
////
////      writeArray(writer, bm.state, reader.lookup(bm));
////   }
////   else if (reader.compare("^")) {
////      bm = reader.read();
////
////      //writeMessage(writer, reader.lookup(bm), SEND_TAPE_MESSAGE_ID);
////
////      throw InternalError("Not yet implemened"); // !! temporal
////   }
////   else if (reader.compare("%")) {
////      bm = reader.read();
////      ident_t message = reader.lookup(bm);
////      if (message.find('.') != NOTFOUND_POS) {
////         writeExtension(writer, reader.lookup(bm), PUSHE_TAPE_MESSAGE_ID);
////      }
////      else writeMessage(writer, reader.lookup(bm), PUSHM_TAPE_MESSAGE_ID);
////   }
////   else writeObject(writer, bm.state, reader.lookup(bm));
//}

void VMTapeParser :: parseInlineScript(TapeWriter& writer, _ScriptReader& reader)
{
   ScriptBookmark bm = reader.read();
   while (!reader.Eof()) {
      parseInlineStatement(reader, bm, writer);

      bm = reader.read();
   }
}

int VMTapeParser :: writeBuildScriptArgumentList(_ScriptReader& reader, ScriptBookmark terminator, 
   Stack<ScriptBookmark>& callStack, TapeWriter& writer)
{
   int counter = 0;
   ScriptBookmark bm = callStack.pop();
   while (!bm.compare(terminator)) {
      counter++;

      writeBuildScriptStatement(reader, bm, callStack, writer);

      bm = callStack.pop();
   }

   return 0;
}

void VMTapeParser :: writeBuildScriptStatement(_ScriptReader& reader, ScriptBookmark bm, Stack
   <ScriptBookmark>& callStack, TapeWriter& writer)
{
   if (bm.state == dfaFullIdentifier) {
      int counter = writeBuildScriptArgumentList(reader, bm, callStack, writer);

      writer.writeCommand(ARG_TAPE_MESSAGE_ID, reader.lookup(bm));
      writer.writeCommand(NEW_TAPE_MESSAGE_ID, counter);
   }
   else if (bm.state == dfaQuote) {
      writer.writeCommand(PUSHS_TAPE_MESSAGE_ID, reader.lookup(bm));
   }
}

void VMTapeParser :: parseBuildScriptArgumentList(_ScriptReader& reader, Stack<ScriptBookmark>& callStack)
{
   readToken(reader, "(");

   ScriptBookmark bm = reader.read();
   while (reader.compare(")")) {
      parseBuildScriptStatement(reader, callStack);
   
      bm = reader.read();
   }
}

void VMTapeParser :: parseBuildScriptArgumentList(_ScriptReader& reader, Stack<ScriptBookmark>& callStack)
{
   ScriptBookmark bm = reader.read();
   if (bm.state == dfaFullIdentifier) {
      callStack.push(bm);

      parseBuildScriptArgumentList(reader, callStack);

      callStack.push(bm);
   }
   else if (bm.state == dfaQuote) {
      callStack.push(bm);
   }
   else throw EParseError(bm.column, bm.row);
}

bool VMTapeParser :: parseBuildScript(TapeWriter& writer, _ScriptReader& reader)
{
   ScriptBookmark bm = reader.read();
   Stack<ScriptBookmark> callStack;
   while (!reader.Eof()) {
      parseBuildScriptArgumentList(reader, callStack);

      bm = reader.read();
   }

   bool idle = callStack.Count() > 0;
   while (callStack.Count() > 0) {
      ScriptBookmark bm = callStack.pop();

      writeBuildScriptStatement(reader, bm, callStack, writer);
   }

   return !idle;
}

void VMTapeParser :: parseInline(ident_t script, TapeWriter& writer)
{
   IdentifierTextReader logReader(script);
   ScriptReader scriptReader(&logReader);

   parseInlineScript(writer, scriptReader);
}

void VMTapeParser :: parse(_ScriptReader& reader, MemoryDump* output)
{
   TapeWriter writer(output);

   if (parseBuildScript(writer, reader)) {
//      if (_postfix.Length() != 0) {
//         parsePostfix(writer);
//      }
   }
   writer.writeEndCommand();
}
