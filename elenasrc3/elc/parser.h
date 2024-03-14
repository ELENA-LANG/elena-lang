//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Parser class declaration.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PARSER_H
#define PARSER_H

#include "source.h"
#include "parsertable.h"
#include "clicommon.h"

namespace elena_lang
{
   struct ParserError : SyntaxError
   {
      ustr_t token;
      path_t path;

      ParserError(ustr_t message, LineInfo info, ustr_t token);
   };

   // --- TerminalInfo ---
   struct TerminalInfo
   {
      SourceInfo  info;
      parse_key_t key;

      TerminalInfo()
      {
         key = 0;
      }
   };

   // --- TerminalMap ---
   struct TerminalMap
   {
      parse_key_t eof;
      parse_key_t identifier;
      parse_key_t reference;
      parse_key_t globalreference;
      parse_key_t string, character;
      parse_key_t wide;
      parse_key_t integer, hexinteger;
      parse_key_t longinteger, real;
      parse_key_t customnumber;

      TerminalMap()
      {
         this->eof = this->identifier = 0;
         this->reference = 0;
         this->globalreference = 0;
         this->string = this->character = 0;
         this->wide = 0;
         this->integer = this->hexinteger = 0;
         this->longinteger = this->real = 0;
         this->customnumber = 0;
      }
      TerminalMap(parse_key_t eof,
         parse_key_t identifier,
         parse_key_t reference,
         parse_key_t globalreference,
         parse_key_t string,
         parse_key_t character,
         parse_key_t wide,
         parse_key_t integer,
         parse_key_t hexinteger,
         parse_key_t longinteger,
         parse_key_t real,
         parse_key_t customnumber)
      {
         this->eof = eof;
         this->identifier = identifier;
         this->reference = reference;
         this->globalreference = globalreference;
         this->string = string;
         this->character = character;
         this->wide = wide;
         this->integer = integer;
         this->hexinteger = hexinteger;
         this->longinteger = longinteger;
         this->real = real;
         this->customnumber = customnumber;
      }
   };

   // --- Parser ---
   class Parser
   {
      char              _buffer[IDENTIFIER_LEN + 1];

      ParserTable       _table;
      TerminalMap       _terminalKeys;

      PresenterBase*    _presenter;

      parse_key_t resolveTerminal(SourceInfo& info);

      bool derive(TerminalInfo& terminalInfo, ParserTable::ParserStack& stack, SyntaxWriterBase* writer);

   public:
      void parse(UStrReader* reader, SyntaxWriterBase* writer);

      Parser(StreamReader* syntax, TerminalMap& terminals, PresenterBase* presenter);
   };

}

#endif
