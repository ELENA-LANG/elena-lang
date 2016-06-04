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

typedef Stack<ScriptBookmark> ScriptStack;

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

void InlineScriptParser :: writeSubject(TapeWriter& writer, ident_t message)
{
   IdentifierString reference;
   reference.append('0');
   reference.append('#');
   reference.append(0x20);
   reference.append('&');
   reference.append(message);

   writer.writeCommand(PUSHG_TAPE_MESSAGE_ID, reference);
}

void InlineScriptParser :: writeObject(TapeWriter& writer, char state, ident_t token)
{
   if (StringHelper::compare(token, ".")) {
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
            //         default:
            //            throw EParseError(reader.info.column, reader.info.row);
      }
   }
}

int InlineScriptParser :: parseStack(_ScriptReader& reader, TapeWriter& writer, Stack<ScriptBookmark>& stack)
{
   IdentifierString message;
   bool empty = true;
   int paramCounter = -1;
   int counter = 0;
   int command = PUSHM_TAPE_MESSAGE_ID;
   while (stack.Count() > 0) {
      ScriptBookmark bm = stack.pop();

      ident_t token = reader.lookup(bm);
      if (bm.state == -2) {
         break;
      }
      switch (bm.state)
      {
         case -1:
         case -4:
            counter += parseStack(reader, writer, stack);
            paramCounter++;
            break;
         case -3:
            command = SEND_TAPE_MESSAGE_ID;
         case dfaIdentifier:
            if (command != NEW_TAPE_MESSAGE_ID) {
               if (!empty) {
                  message.insert("&",0);
                  message.insert(token, 0);
                  counter--; // HOTFIX : if it is the message subject, counter should not be changed
               }
               else {
                  empty = false;

                  message.append(token);
               }
            }
            else writeObject(writer, bm.state, token);

            counter++;
            break;
         case -6:
            if (empty) {
               message.copy(token);
               command = NEW_TAPE_MESSAGE_ID;
               empty = false;
            }      
            break;
         case -7:
         {
            ident_t msg = reader.lookup(bm);
            if (StringHelper::find(msg, '.')) {
               writer.writeCommand(PUSHE_TAPE_MESSAGE_ID, msg);
            }
            else writer.writeCommand(PUSHM_TAPE_MESSAGE_ID, msg);
            counter++;
            paramCounter++;
            break;
         }
         default:
            writeObject(writer, bm.state, token);
            paramCounter++;
            counter++;
            break;
      }
   }

   if (!empty) {
      if (command == NEW_TAPE_MESSAGE_ID) {
         writer.writeCommand(ARG_TAPE_MESSAGE_ID, message);
         writer.writeCommand(command, counter);
         counter = 1;
      }
      else writeMessage(writer, message, paramCounter, command);
   }

   return counter;
}

inline void appendBookmark(ScriptStack& stack, Stack<ScriptStack::Iterator>& brackets, ScriptBookmark bm)
{
   if (brackets.Count() != 0) {
      ScriptStack::Iterator it = brackets.peek();

      if ((*it).state != -1) {
         stack.insertBefore(it, bm);

         (*brackets.start()) = it;
      }
      else stack.insert(it, bm);
   }
   else stack.push(bm);
}

inline void insertBookmark(ScriptStack& stack, ScriptStack::Iterator it, ScriptBookmark bm)
{
   int level = 1;
   while (!it.Eof()) {
      if ((*it).state == -1) {
         level++;
      }
      else if ((*it).state == -2) {
         level--;
         if (level == 0) {
            stack.insert(it, bm);
         }
      }

      it++;
   }
}

inline void appendScope(ScriptStack& stack, Stack<ScriptStack::Iterator>& brackets, ScriptBookmark bm)
{
   if (brackets.Count() != 0) {
      ScriptStack::Iterator it = brackets.peek();

      if ((*it).state != -1) {
         ScriptStack::Iterator prev = it;

         stack.insertBefore(it, bm);

         (*brackets.start()) = it;
         brackets.push(prev);
         //brackets.push(it);
      }
      else {
         stack.insert(it, bm);
         it++;

         brackets.push(it);
      }
   }
   else {
      stack.push(bm);

      brackets.push(stack.start());
   }
}

void InlineScriptParser :: parseStatement(_ScriptReader& reader, ScriptBookmark& bm, TapeWriter& writer)
{   
   ScriptStack stack(ScriptBookmark(0, 0));
   Stack<ScriptStack::Iterator> brackets;

   do {
      if (reader.compare("(")) {
         appendScope(stack, brackets, ScriptBookmark(-1, -1));

         appendBookmark(stack, brackets, ScriptBookmark(-1, -2));
      }
      else if (reader.compare(")")) {
         brackets.pop();
      }
      else if (reader.compare(";")) {
         appendScope(stack, brackets, ScriptBookmark(-1, -1));

         ScriptStack::Iterator it = brackets.pop();

         insertBookmark(stack, it, ScriptBookmark(-1, -2));
      }
      else if (reader.compare("[")) {
         appendScope(stack, brackets, ScriptBookmark(-1, -2));
         appendBookmark(stack, brackets, ScriptBookmark(-1, -4));
      }
      else if (reader.compare("]")) {
         brackets.pop();
      }
      else if (reader.compare("^")) {
         bm = reader.read();
         bm.state = -3;

         appendBookmark(stack, brackets, bm);
      }
      else if (reader.compare("*")) {
         bm = reader.read();
         bm.state = -6;

         appendBookmark(stack, brackets, bm);
      }
      else if (reader.compare("%")) {
         bm = reader.read();
         bm.state = -7;

         appendBookmark(stack, brackets, bm);
      }
      else appendBookmark(stack, brackets, bm);

      bm = reader.read();

   } while (brackets.Count() > 0);

   parseStack(reader, writer, stack);
}

void InlineScriptParser :: parse(_ScriptReader& reader, TapeWriter& writer)
{
   ScriptBookmark bm = reader.read();
   while (!reader.Eof()) {      
      parseStatement(reader, bm, writer);
   }   

   writer.writeEndCommand();
}
