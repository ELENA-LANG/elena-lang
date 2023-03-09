//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2023 by Aleksey Rakov
//---------------------------------------------------------------------------

#include "scriptparser.h"

using namespace elena_lang;

// --- ScriptParser ---

ScriptEngineReader :: ScriptEngineReader(UStrReader* textReader)
   : sourceReader(4, textReader), token({}), _eof(false), coordinates(nullptr)
{
}

ScriptEngineReader :: ScriptEngineReader(UStrReader* textReader, CoordMap* coordinates)
   : ScriptEngineReader(textReader)
{
   this->coordinates = coordinates;
}

ScriptBookmark ScriptEngineReader :: read()
{
   ScriptBookmark bm = {};

   if (sourceReader.read(token)) {
      bm.offset = buffer.length();
      bm.state = token.state;

      LineInfo savedLineInfo = { INVALID_POS };
      if (coordinates)
         savedLineInfo = coordinates->get(token.lineInfo.position);

      if (savedLineInfo.position != INVALID_POS) {
         bm.lineInfo = savedLineInfo;
      }
      else bm.lineInfo = token.lineInfo;

      if (token.state == dfaEOF) {
         _eof = true;
      }
   }
   else {
      _eof = true;
      bm.state = dfaEOF;
   }

   MemoryWriter writer(&buffer);
   writer.writeString(*token.token);

   return bm;
}

bool ScriptEngineReader :: compare(ustr_t value)
{
   return token.compare(value);
}

ustr_t ScriptEngineReader :: lookup(ScriptBookmark& bm)
{
   if (bm.offset == INVALID_POS)
      return nullptr;

   MemoryReader reader(&buffer, bm.offset);

   return reader.getString(DEFAULT_STR);
}

// --- ScriptEngineLog ---