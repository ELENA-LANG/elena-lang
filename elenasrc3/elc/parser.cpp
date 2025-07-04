//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Parser class implementation.
//
//                                             (C)2021-2024, by Aleksey Rakov
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
      case dfaGlobal:
         return _terminalKeys.globalreference;
      case dfaQuote:
      case dfaQuoteCode:
      case dfaAltQuote:
         return _terminalKeys.string;
      case dfaStartInterpol:
         return _terminalKeys.interpolate;
      case dfaWideQuote:
         return _terminalKeys.wide;
      case dfaEOF:
         return _terminalKeys.eof;
      case dfaInteger:
         return _terminalKeys.integer;
      case dfaGenericReal:
      case dfaReal:
      case dfaRealPostfix:
         return _terminalKeys.real;
      case dfaCharacter:
         return _terminalKeys.character;
      case dfaCustomNumber:
      {
         switch (info.symbol[info.symbol.length() - 1]) {
            case 'h':
               return _terminalKeys.hexinteger;
            case 'l':
               return _terminalKeys.longinteger;
            default:
               return _terminalKeys.customnumber;
         }
         break;
      }
      default:
         return _table.resolveSymbol(info.symbol);
   }   
}

inline ustr_t getToken(TerminalInfo& terminalInfo)
{
   return terminalInfo.info.state == dfaEOF ? "<end of the file>" : terminalInfo.info.symbol;
}

inline int getErrorCode(parse_key_t symbol)
{
   switch ((SyntaxKey)symbol) {
      //case nsDeclarationEndExpected:
      //case nsStatementEndExpected:
      //case nsDirectiveEndExpected:
      //case nsInlineExpressionEndExpected:
      //   return errDotExpectedSyntax;
      case SyntaxKey::ErrClosingBlockExpected:
         return errCBrExpectedSyntax;
         //case nsErrNestedMemberExpected:
         //   return errMethodNameExpected;
         //case nsErrObjectExpected:
         //   return errObjectExpected;
         //case nsErrMessageExpected:
         //   return errMessageExpected;
      default:
         return errInvalidSyntax;
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
            if (!testany(current, pkInjectable)) {
               stack.push(0);
               writer->newNode(current);
            }
            else {
               if (current == pkTransformMark) {
                  current = stack.pop();
                  if (current == pkTransformMark) {
                     current = stack.pop();
                     if (current == pkTransformMark) {
                        current = stack.pop();
                        // ^= operation - R nodes must be merged
                        writer->mergeRChildren(current & ~pkInjectable);
                     }
                     // ^^ operation - L nodes must be merged
                     else writer->mergeLChildren(current & ~pkInjectable);
                  }
                  // = operation - the last node must be enclosed
                  else writer->encloseLastChild(current & ~pkInjectable);
               }
               else writer->injectNode(current & ~pkInjectable);                  
               current = stack.pop();
               continue;
            }
         }
         if (!_table.read(current, terminalInfo.key, stack))
            return false;

         if (test(current, pkError))
            throw ParserError(
               _presenter->getMessage(getErrorCode(current)),
               terminalInfo.info.lineInfo, getToken(terminalInfo));
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

   List<char*, freestr> dynamicStrings(nullptr);
   SourceReader reader(4, source);

   stack.push(pkStart);
   do {
      terminalInfo.info = reader.read(_buffer, IDENTIFIER_LEN, dynamicStrings);
      terminalInfo.key = resolveTerminal(terminalInfo.info);

      if (!derive(terminalInfo, stack, writer))
         throw ParserError(_presenter->getMessage(errInvalidSyntax), terminalInfo.info.lineInfo,
            getToken(terminalInfo));

      if (test(terminalInfo.key, pkTraceble))
         writer->appendTerminal(terminalInfo.key, terminalInfo.info.symbol, terminalInfo.info.lineInfo);

   } while (terminalInfo.key != eof);

   dynamicStrings.clear();
}
