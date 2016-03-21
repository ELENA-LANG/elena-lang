//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "inlineparser.h"
#include "bytecode.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

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

void InlineScriptParser :: writeMessage(TapeWriter& writer, ident_t message, int paramCounter, int command)
{
   IdentifierString reference;
   int verb_id = 0;

   int subjPos = StringHelper::find(message, '&');

   if (subjPos != -1) {
      reference.copy(message, subjPos);
      message += subjPos + 1;

      verb_id = mapVerb(reference);
   }
   else {
      verb_id = mapVerb(message);
      if (verb_id != 0)
         message = NULL;
   }

   if (verb_id == 0)
      verb_id = (paramCounter == 0) ? GET_MESSAGE_ID : EVAL_MESSAGE_ID;

   reference.clear();
   reference.append('0' + paramCounter);
   reference.append('#');
   reference.append(0x20 + verb_id);
   if (!emptystr(message)) {
      reference.append('&');
      reference.append(message);
   }

   writer.writeCommand(command, reference);
}

void InlineScriptParser :: writeObject(TapeWriter& writer, char state, ident_t token)
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
         //      case '^':
//      case '&':
//         // HOTFIX : set EVAL or GET depending on the parameter counter
//         if (token[2] == 0x20) {
//            if (token[0] == '0') {
//               ((ident_c*)token)[2] = GET_MESSAGE_ID + 0x20;
//            }
//            else ((ident_c*)token)[2] = EVAL_MESSAGE_ID + 0x20;
//         }
//
//         writer.writeCommand(state == '^' ? SEND_TAPE_MESSAGE_ID : PUSHM_TAPE_MESSAGE_ID, token);
//         break;
//      case '%':
//         writer.writeCommand(PUSHG_TAPE_MESSAGE_ID, token);
//         break;
//      case '<':
//      {
//         int var_level = StringHelper::strToInt(token);
//
//         writer.writeCommand(PUSH_VAR_MESSAGE_ID, var_level);
//
//         break;
//      }
//      case '>':
//      {
//         int var_level = StringHelper::strToInt(token);
//
//         writer.writeCommand(ASSIGN_VAR_MESSAGE_ID, var_level);
//
//         break;
//      }
//      //         default:
////            throw EParseError(reader.info.column, reader.info.row);
   }
}

//inline void save(MemoryDump& cache, char state, ident_t value)
//{
//   MemoryWriter argWriter(&cache);
//
//   argWriter.writeChar(state);
//   argWriter.writeLiteral(value);
//}
//
//inline void save(MemoryDump& cache, char state, int length, ident_t value)
//{
//   MemoryWriter argWriter(&cache);
//
//   argWriter.writeChar(state);
//   argWriter.writeDWord(length);
//   argWriter.writeLiteral(value);
//}
//
//void InlineScriptParser::writeLine(MemoryDump& line, TapeWriter& writer)
//{
//   MemoryReader reader(&line);
//   while (!reader.Eof()) {
//      char state = reader.getByte();
//
//      if (state == '*') {
//         int length = reader.getDWord();
//         writer.writeCommand(ARG_TAPE_MESSAGE_ID, reader.getLiteral(DEFAULT_STR));
//         writer.writeCommand(NEW_TAPE_MESSAGE_ID, length);
//      }
//      else writeObject(writer, state, reader.getLiteral(DEFAULT_STR));
//   }
//}
//
//void InlineScriptParser :: copyDump(MemoryDump& dump, MemoryDump& line, Stack<int>& arguments)
//{
//   MemoryReader reader(&dump);
//   while (arguments.Count() > 0) {
//      int position = arguments.pop();
//
//      reader.seek(position);
//      char state = reader.getByte();
//
//      if (state == '*') {
//         int length = reader.getDWord();
//
//         save(line, state, length, reader.getLiteral(DEFAULT_STR));
//      }
//      else if (state == '{') {
//         int level = 1;
//         while (level > 0) {
//            state = reader.getByte();
//            if (state == '{') {
//               level++;
//            }
//            else if (state == '}') {
//               level--;
//            }
//            else if (state == '*') {
//               int length = reader.getDWord();
//
//               save(line, state, length, reader.getLiteral(DEFAULT_STR));
//            }
//            else save(line, state, reader.getLiteral(DEFAULT_STR));
//         }
//      }
//      else save(line, state, reader.getLiteral(DEFAULT_STR));
//   }
//}

void InlineScriptParser :: parseStack(_ScriptReader& reader, TapeWriter& writer, Stack<ScriptBookmark>& stack)
{
   IdentifierString message;
   bool empty = true;
   int paramCounter = -1;  // NOTE: to exclude target object
   int command = PUSHM_TAPE_MESSAGE_ID;
   while (stack.Count() > 0) {
      ScriptBookmark bm = stack.pop();

      ident_t token = reader.lookup(bm);
      if (bm.state == -1) {
         break;
      }
      else if (bm.state == -2) {
         parseStack(reader, writer, stack);
         paramCounter++;
      }
      else if (bm.state == -3) {         
         command = SEND_TAPE_MESSAGE_ID;
      }
      else if (bm.state == dfaIdentifier) {
         if (!empty) {
            message.append('&');
         }
         else empty = false;

         message.append(token);
      }
      else {
         writeObject(writer, bm.state, token);
         paramCounter++;
      }
   }

   if (!empty) {
      writeMessage(writer, message, paramCounter, command);
   }
}

int InlineScriptParser :: parseExpression(_ScriptReader& reader, TapeWriter& writer)
{   
   int     counter = 0;

   Stack<ScriptBookmark> stack(ScriptBookmark(0, 0));

   int level = 1;
   while (level > 0) {
      ScriptBookmark bm = reader.read();

      if (reader.compare("(")) {
         stack.push(ScriptBookmark(-1, -1));
         level++;
      }
      else if (reader.compare(")")) {
         stack.push(ScriptBookmark(-1, -2));
         level--;
      }
      else if (reader.compare("^")) {
         stack.push(ScriptBookmark(-1, -3));
      }
      else if (bm.state == dfaFullIdentifier) {
         stack.push(bm);
      //   counter++;
      }
      else if (bm.state == dfaIdentifier) {
         stack.push(bm);
         //   counter++;
      }
      else if (bm.state == dfaQuote) {
         stack.push(bm);
         //counter++;
      }
      else if (bm.state == dfaInteger) {
         stack.push(bm);
         //counter++;
      }
      else if (bm.state == dfaLong) {
         stack.push(bm);
         //counter++;
      }
      else if (bm.state == dfaReal) {
         stack.push(bm);
         //counter++;
      }
      //      char state = reader.info.state;
//      if (state == dfaEOF) {
//         break;
//      }
//      if (state == dfaQuote) {
//         if (reverseMode) {
//            arguments.push(cache.Length());
//
//            save(cache, dfaQuote, reader.token);
//         }
//         else  save(line, dfaQuote, reader.token);
//
//         counter++;
//         (*scopes.start()).level++;
//         reader.read();
//      }
//      else if (state == dfaFullIdentifier) {
//         if (reverseMode) {
//            arguments.push(cache.Length());
//
//            save(cache, dfaFullIdentifier, reader.token);
//         }
//         else save(line, dfaFullIdentifier, reader.token);
//
//         counter++;
//         (*scopes.start()).level++;
//         reader.read();
//      }
//      else if (state == dfaInteger) {
//         if (reverseMode) {
//            arguments.push(cache.Length());
//
//            save(cache, dfaInteger, reader.token);
//         }
//         else save(line, dfaInteger, reader.token);
//
//         counter++;
//         (*scopes.start()).level++;
//         reader.read();
//      }
//      else if (state == dfaLong) {
//         if (reverseMode) {
//            arguments.push(cache.Length());
//
//            save(cache, dfaLong, reader.token);
//         }
//         else save(line, dfaLong, reader.token);
//
//         counter++;
//         (*scopes.start()).level++;
//         reader.read();
//      }
//      else if (state == dfaReal) {
//         if (reverseMode) {
//            arguments.push(cache.Length());
//
//            save(cache, dfaReal, reader.token);
//         }
//         else save(line, dfaReal, reader.token);
//
//         counter++;
//         (*scopes.start()).level++;
//         reader.read();
//      }
      // if it is a new expression
//      else if (reader.compare("[")) {
//         // set reverse mode on, add new scope
//         reverseMode = true;
//         scopes.push(Scope(0, arguments.Count() + 1));
//
//         reader.read();
//      }
//      else if (reader.compare("]")) {
//          scopes.pop();
//
//         copyDump(cache, line, arguments);
//         cache.trim(0);
//
//         reader.read();
//         reverseMode = false;
//      }
//      else if (reader.compare("(")) {
//         scopes.push(Scope(0, arguments.Count() + 1));
//
//         reader.read();
//      }
//      else if (reader.compare(")")) {
//         Scope scope = scopes.pop();
//
//         int pos = *arguments.get(arguments.Count() - scopes.peek().arg_level);
//
//         MemoryReader cacheReader(&cache, pos);
//         char op = cacheReader.getByte();
//         if (op == '^' || op == '&') {
//            // update message counter
//            char prm_count = cacheReader.getByte();
//
//            cache.writeByte(pos + 1, prm_count + scope.level);
//
//            if (op == '^')
//               counter -= scope.level;
//         }
//
//         reader.read();
//      }
//      else if (StringHelper::compare(reader.token, "^")) {
//         IdentifierString message;
//         readMessage(reader, message);
//
//         arguments.insert(arguments.get(arguments.Count() - scopes.peek().arg_level), cache.Length());
//         save(cache, '^', message);
//      }
//      else if (StringHelper::compare(reader.token, "&")) {
//         IdentifierString message;
//         readMessage(reader, message);
//
//         arguments.insert(arguments.get(arguments.Count() - scopes.peek().arg_level), cache.Length());
//         save(cache, '&', message);
//         counter++;
//      }
//      else if (StringHelper::compare(reader.token, "%")) {
//         IdentifierString message;
//         readMessage(reader, message, true);
//
//         if (reverseMode) {
//            arguments.push(cache.Length());
//            save(cache, '%', message);
//         }
//         else save(line, '%', message);
//
//         counter++;
//      }
//      else if (StringHelper::compare(reader.token, "*")) {
//         arguments.push(cache.Length());
//
//         reader.read();
//         save(cache, '*', 0, reader.token);
//
//         reader.read();
//         counter++;
//      }
//      else if (StringHelper::compare(reader.token, "<")) {
//         arguments.push(cache.Length());
//
//         reader.read();
//         save(cache, '<', reader.token);
//
//         reader.read();
//         (*scopes.start()).level++;
//         counter++;
//      }
//      else if (StringHelper::compare(reader.token, ">")) {
//         arguments.push(cache.Length());
//
//         reader.read();
//         save(cache, '>', reader.token);
//
//         reader.read();
//      }
//      else if (StringHelper::compare(reader.token, ";") || StringHelper::compare(reader.token, ",")) {
//         break;
//      }
//      else if (StringHelper::compare(reader.token, "{")) {
//         scopes.push(Scope(0, arguments.Count()));
//
//         arguments.push(cache.Length());
//
//         reader.read();
//
//         cache.writeByte(cache.Length(), '{');
//         int statement_length = parseStatement(reader, cache);
//         cache.writeByte(cache.Length(), '}');
//
//         Scope scope = scopes.pop();
//         int pos = *arguments.get(arguments.Count() - scope.arg_level);
//
//         MemoryReader cacheReader(&cache, pos);
//         char op = cacheReader.getByte();
//         if (op == '*') {
//            // update length counter
//            cache.writeDWord(pos + 1, statement_length);
//         }
//
//         (*scopes.start()).level++;
//         reader.read();
//      }
//      else if (StringHelper::compare(reader.token, "}")) {
//         break;
//      }
   }

   stack.pop(); // remove last bookmark

   parseStack(reader, writer, stack);

   return counter;
}

int InlineScriptParser :: parseStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
{
   if (reader.compare("(")) {
      parseExpression(reader, writer);
   }
   else if (bm.state == dfaFullIdentifier) {
      writer.writeCallCommand(reader.lookup(bm));
   }

   return 0;
}

void InlineScriptParser :: parse(_ScriptReader& reader, TapeWriter& writer)
{
   while (true) {
      ScriptBookmark bm = reader.read();
      if (bm.state == dfaEOF) {
         break;
      }
      else parseStatement(reader, bm, writer);

//      if (reader.token[0] == ';' && reader.info.state != dfaQuote) {
//         writer.writeCommand(POP_TAPE_MESSAGE_ID, 1);
//      }
   }   

   writer.writeEndCommand();
}
