//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "transformer.h"

using namespace _ELENA_;

void Transformer::Scope :: writeToken(ident_t token, ScriptLog&)
{
   switch (channel) {
      case 1:
         buffer1.insert(token, 0);
         break;
      case 2:
         buffer2.append(token);
         break;
   }
}

void Transformer::Scope :: copyForward()
{
   buffer2.append(buffer1.str());
   buffer1.clear();
}

void Transformer::Scope :: flush(Scope* parent, ScriptLog& log)
{
   buffer1.append('\0');
   buffer2.append('\0');
   
   if (parent != NULL) {
      parent->counter += counter;

      parent->buffer1.insert(buffer2.str(), 0);
      parent->buffer1.insert(buffer1.str(), 0);
   }
   else {
      log.write(buffer1.str());
      log.write(buffer2.str());      
   }
}

// --- Transformer ---

Transformer :: Transformer(_Parser* baseParser)
{
   _baseParser = baseParser;
}

void Transformer :: writeToken(ident_t token, Scope* scope, ScriptLog& log)
{
   if (scope && scope->channel != 0) {
      scope->writeToken(token, log); 
      if (!scope->appendMode)
         scope->writeToken(" ", log);
   }
   else {
      log.write(token);
      if (!scope->appendMode)
         log.write(" ");
   }
}

bool Transformer :: parseDirective(_ScriptReader& reader, Scopes& scopes, ScriptLog& log)
{
   if (reader.compare("(")) {
      scopes.push(new Scope());
   }
   else if (reader.compare(")")) {
      Scope* scope = scopes.pop();

      scope->flush(scopes.peek(), log);

      freeobj(scope);
   }
   else if (reader.compare("<")) {
      scopes.peek()->channel = 1;
      scopes.peek()->appendMode = false;
   }
   else if (reader.compare(">")) {
      scopes.peek()->channel = 2;
      scopes.peek()->appendMode = false;
   }
   else if (reader.compare("<<")) {
      scopes.peek()->channel = 1;
      scopes.peek()->appendMode = true;
   }
   else if (reader.compare(">>")) {
      scopes.peek()->channel = 2;
      scopes.peek()->appendMode = true;
   }
   else if (reader.compare("~")) {
      scopes.peek()->copyForward();
   }
   else if (reader.compare("+")) {
      scopes.peek()->level++;
   }
   else if (reader.compare("-")) {
      scopes.peek()->level--;
   }
   else if (reader.compare("+=")) {
      scopes.peek()->counter++;
   }
   else if (reader.compare("-=")) {
      scopes.peek()->counter--;
   }
   else if (reader.compare("=")) {
      scopes.peek()->writeLevel(log);
      scopes.peek()->level = 0;
   }
   else if (reader.compare("#")) {
      scopes.peek()->writeCounter(log);
      scopes.peek()->counter = 0;
   }
   else return false;

   return true;
}

void Transformer :: parse(_ScriptReader& reader, MemoryDump* output)
{
   ScriptLog log;
   Scopes    scopes;

   ScriptBookmark bm = reader.read();
   while (!reader.Eof()) {
      if (bm.state == _ELENA_TOOL_::dfaQuote) {
         writeToken(reader.lookup(bm), scopes.peek(), log);
      }
      else if (!parseDirective(reader, scopes, log))
         throw EParseError(bm.column, bm.row);

      bm = reader.read();
   }

   IdentifierTextReader logReader((const char*)log.getBody());
   ScriptReader scriptReader(&logReader);

   _baseParser->parse(scriptReader, output);
}