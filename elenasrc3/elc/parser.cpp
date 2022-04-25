//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Parser class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "parser.h"
#include "errors.h"

using namespace elena_lang;

typedef ParserTable::ParserStack ParserStack;

// --- ParserError ---

ParserError :: ParserError(ustr_t message, LineInfo info, ustr_t token)
   : SyntaxError(message, info)
{
   this->token = token;
}

// --- Parser ---

Parser :: Parser(StreamReader* syntax, TerminalMap& terminals, PresenterBase* presenter)
{
   _table.load(syntax);
   _terminalKeys = terminals;
   _presenter = presenter;
}

parse_key_t Parser :: resolveTerminal(SourceInfo& info)
{
   switch (info.state) {
      case dfaIdentifier:
         return _terminalKeys.identifier;
      case dfaReference:
         return _terminalKeys.reference;
      case dfaQuote:
      case dfaQuoteCode:
         return _terminalKeys.string;
      case dfaEOF:
         return _terminalKeys.eof;
      case dfaInteger:
         return _terminalKeys.integer;
      case dfaHexInteger:
         return _terminalKeys.hexinteger;
      default:
         return _table.resolveSymbol(info.symbol);
   }   
}

bool Parser :: derive(TerminalInfo& terminalInfo, ParserStack& stack, SyntaxWriterBase* writer)
{
   parse_key_t current = stack.pop();
   while (!test(current, pkTerminal)) {
      bool traceble = test(current, pkTraceble);
      if (current == 0) {
         writer->closeNode();
      }
      else {
         if (traceble) {
            if (!test(current, pkInjectable)) {
               stack.push(0);
               writer->newNode(current);
            }
            else {
               writer->injectNode(current & ~pkInjectable);
               current = stack.pop();
               continue;
            }
         }
         if (!_table.read(current, terminalInfo.key, stack))
            return false;

         //if (test(current, mskError))
         //   throw SyntaxError(terminal.Col(), terminal.Row(), terminal.value, getError(current));

      }
      current = stack.pop();
   }
   return (terminalInfo.key == current);
}

void Parser :: parse(UStrReader* source, SyntaxWriterBase* writer)
{
   parse_key_t eof = _terminalKeys.eof;

   TerminalInfo terminalInfo;
   ParserStack  stack(0);

   SourceReader reader(4, source);

   stack.push(pkStart);
   do {
      terminalInfo.info = reader.read(_buffer, IDENTIFIER_LEN);
      terminalInfo.key = resolveTerminal(terminalInfo.info);

      if (!derive(terminalInfo, stack, writer))
         throw ParserError(_presenter->getMessage(errInvalidSyntax), terminalInfo.info.lineInfo, terminalInfo.info.symbol);

      if (test(terminalInfo.key, pkTraceble))
         writer->appendTerminal(terminalInfo.key, _buffer, terminalInfo.info.lineInfo);

   } while (terminalInfo.key != eof);
}
