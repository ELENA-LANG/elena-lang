//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2018, by Alexei Rakov
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

bool VMTapeParser :: writeObject(TapeWriter& writer, int state, ident_t token)
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

bool VMTapeParser :: writeArgument(TapeWriter& writer, int state, ident_t token)
{
   if (state == dfaFullIdentifier) {
      writer.writeCommand(ARG_TAPE_MESSAGE_ID, token);

      return true;
   }
   else return false;
}

bool VMTapeParser :: writeArray(TapeWriter& writer, int state, ident_t token)
{
   if (state == dfaInteger) {
      int value = token.toInt();

      writer.writeCommand(NEW_TAPE_MESSAGE_ID, value);

      return true;
   }
   else return false;
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
            paramCounter = OPEN_ARG_COUNT;
         }
         else paramCounter = reference.ident().toInt();

      }
      else return false;
   }
   else return false;

   reference.clear();
   reference.append('0' + (char)paramCounter);
   reference.append(message, length);

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

bool VMTapeParser :: writeExtension(TapeWriter& writer, ident_t message, int command)
{
   IdentifierString reference;

   size_t dotPos = message.find('.');
   if (parseMessage(message + dotPos + 1, reference)) {
      reference.insert(message, 0, dotPos + 1);

      writer.writeCommand(command, reference);

      return true;
   }
   else return false;
}

void VMTapeParser:: parseStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
{
   if (reader.compare("*")) {
      bm = reader.read();

      writeArgument(writer, bm.state, reader.lookup(bm));
   }
   else if (reader.compare("=")) {
      bm = reader.read();

      writeArray(writer, bm.state, reader.lookup(bm));
   }
   else if (reader.compare("^")) {
      bm = reader.read();

      writeMessage(writer, reader.lookup(bm), SEND_TAPE_MESSAGE_ID);
   }
   else if (reader.compare("%")) {
      bm = reader.read();
      ident_t message = reader.lookup(bm);
      if (message.find('.') != NOTFOUND_POS) {
         writeExtension(writer, reader.lookup(bm), PUSHE_TAPE_MESSAGE_ID);
      }
      else writeMessage(writer, reader.lookup(bm), PUSHM_TAPE_MESSAGE_ID);
   }
   else writeObject(writer, bm.state, reader.lookup(bm));
}

bool VMTapeParser :: parse(TapeWriter& writer, _ScriptReader& reader)
{
   bool idle = true;
   ScriptBookmark bm = reader.read();
   while (!reader.Eof()) {
      idle = false;

      parseStatement(reader, bm, writer);

      bm = reader.read();
   }

   return !idle;
}

void VMTapeParser :: parsePostfix(TapeWriter& writer)
{
   IdentifierTextReader logReader(_postfix);
   ScriptReader scriptReader(&logReader);

   parse(writer, scriptReader);
}

void VMTapeParser :: parse(_ScriptReader& reader, MemoryDump* output)
{
   TapeWriter writer(output);

   if (parse(writer, reader)) {
      if (_postfix.Length() != 0) {
         parsePostfix(writer);
      }
   }
   writer.writeEndCommand();
}
