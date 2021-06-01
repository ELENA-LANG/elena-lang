//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef transformerH
#define transformerH 1

#include "scriptengine.h"

namespace _ELENA_
{

// --- TextParser ---

class TextParser : public _Parser
{
   int _width;

public:
   virtual bool setPostfix(ident_t)
   {
      return false;
   }

   virtual bool parseGrammarRule(_ScriptReader&)
   {
      return false;
   }

   virtual bool parseGrammarMode(_ScriptReader&)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, MemoryDump* output);

   TextParser()
   {
      _width = 0x50;
   }
};

// --- Transformer ---

class Transformer : public _Parser
{
protected:
   struct Scope
   {
      int                  channel;
      int                  level;
      int                  counter;
      bool                 appendMode;
      DynamicString<char/*, 2048*/>  buffer1;
      DynamicString<char/*, 2048*/>  buffer2;

      void writeToken(ident_t token, ScriptLog& log);
      void writeLevel(ScriptLog& log)
      {
         IdentifierString number;
         number.copyInt(level);

         writeToken(number.c_str(), log);
      }
      void writeCounter(ScriptLog& log)
      {
         IdentifierString number;
         number.copyInt(counter);

         writeToken(number.c_str(), log);
      }

      void flush(Scope* parent, ScriptLog& log);

      void copyForward();

      Scope()
         //: buffer1(2048), buffer2(2048)
      {
         level = 0;
         channel = 0;
         counter = 0;
         appendMode = false;
      }

      virtual ~Scope()
      {

      }
   };

   typedef Stack<Scope*> Scopes;

   _Parser* _baseParser;

   void writeToken(ident_t token, Scope* scope, ScriptLog& log);

   bool parseDirective(_ScriptReader& reader, Scopes& scopes, ScriptLog& log);

public:
   virtual bool setPostfix(ident_t)
   {
      return false;
   }

   virtual bool parseGrammarRule(_ScriptReader&)
   {
      return false;
   }

   virtual bool parseGrammarMode(_ScriptReader&)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, MemoryDump* output);

   Transformer(_Parser* baseParser);
};

class Builder : public _Parser
{
protected:
   void saveToken(MemoryWriter& writer, _ScriptReader& reader, ScriptBookmark bm);

   void saveClass(MemoryWriter& writer, _ScriptReader& reader, Stack<ScriptBookmark>& stack, int allocated, 
      int& maxAllocated, int& maxStackSize);
   void flush(MemoryWriter& writer, _ScriptReader& reader, Stack<ScriptBookmark>& stack);

public:
   virtual bool setPostfix(ident_t)
   {
      return false;
   }

   virtual bool parseGrammarRule(_ScriptReader&)
   {
      return false;
   }

   virtual bool parseGrammarMode(_ScriptReader&)
   {
      return false;
   }

   virtual void parse(_ScriptReader& reader, MemoryDump* output);

   Builder();
};

} // _ELENA_

#endif // transformerH
