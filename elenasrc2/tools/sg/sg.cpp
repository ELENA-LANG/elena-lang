//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "sg.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

#define BUILD_NUMBER 4

// !! code duplication (syntax.h)
const int mskAnySymbolMask             = 0x07000;               // masks
const int mskTraceble                  = 0x01000;

int last_id = 0;

int _registerSymbol(ParserTable& table, ident_t symbol, int new_id)
{
   int id = (int)table.defineSymbol(symbol);
   if (id == 0) {
      id = new_id;

      if ((symbol[0]<'A')||(symbol[0]>'Z'))
         id |= ParserTable::mskTerminal;

      table.registerSymbol(id, symbol);

      if (last_id < (id & ~mskAnySymbolMask)) last_id = id & ~mskAnySymbolMask;

      if (last_id >= mskTraceble) {
         printLine("WARNING: symbol id is overflown %d", last_id);
      }
   }
   return id;
}

int registerSymbol(ParserTable& table, ident_t symbol, int new_id)
{
   if (StringHelper::compare(symbol, "||")) {
      return _registerSymbol(table, "|", new_id);
   }
   else if (StringHelper::compare(symbol, "&|")) {
      return _registerSymbol(table, "||", new_id);
   }
   else if (StringHelper::compare(symbol, "-->")) {
      return _registerSymbol(table, "->", new_id);
   }
   else return _registerSymbol(table, symbol, new_id);
}

int main(int argc, char* argv[])
{
   printf("ELENA command line syntax generator %d.%d.%d (C)2005-2015 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, BUILD_NUMBER);
   if (argc < 2 || argc > 3) {
      printLine("sg <syntax_file> [-cp<codepage>]");
      return 0;
   }
   try {
      int encoding = DEFAULT_ENCODING;

      if (argc==3) {
         if (StringHelper::compare(argv[2], "-cp", 3)) {
            encoding = StringHelper::strToInt(argv[2] + 3);
         }
         else {
            printLine("sg <syntax_file> [-cp<codepage>]");
            return 0;
         }
      }

      Path path;
      Path::loadPath(path, argv[1], strlen(argv[1]));
      TextFileReader   sourceFile(path, encoding, true);
      if (!sourceFile.isOpened()) {
         printLine("file not found %s", path);
      }

      TextSourceReader source(4, &sourceFile);
      ParserTable      table;
      LineInfo         info(0, 0, 0);
      ident_c          token[IDENTIFIER_LEN + 1];
      int              rule[20];
      int              rule_len = 0;
      bool             arrayCheck = false;

      table.registerSymbol(ParserTable::nsEps, "eps");

      while (true) {
         info = source.read(token, IDENTIFIER_LEN);

         if (info.state == dfaEOF) break;

         if (StringHelper::compare(token, "__define")) {
            source.read(token, IDENTIFIER_LEN);

            ident_c number[10];
            source.read(number, 10);

            registerSymbol(table, token, StringHelper::strToInt(number));
         }
         else if (StringHelper::compare(token, "->") && !arrayCheck) {
            if (rule_len > 2) {
               table.registerRule(rule[0], rule + 1, rule_len - 2);

               rule[0] = rule[rule_len - 1];
               rule_len = 1;
            }
            arrayCheck = true;
         }
         else if (StringHelper::compare(token, "|") && rule_len != 1) {
            arrayCheck = false;
            table.registerRule(rule[0], rule + 1, rule_len - 1);

            rule_len = 1;
         }
         else {
            arrayCheck = false;
            rule[rule_len++] = registerSymbol(table, token, last_id + 1);
            if (StringHelper::compare(token, "|"))
               source.read(token, IDENTIFIER_LEN);
         }
      }
      table.registerRule(rule[0], rule + 1, rule_len - 1);

      printLine("generating...\n");

      int ambigous = table.generate();
      if (ambigous) {
         printLine("error:ambigous rule %s\n", table.retrieveSymbol(ambigous));
         return -1;
      }

      printLine("saving...\n");

      path.changeExtension("dat");

      FileWriter file(path, feRaw, false);
      table.save(&file);
   }
   catch(_ELENA_::InvalidChar& e) {
      printLine("(%d:%d): Invalid char %c\n", e.row, e.column, e.ch);
   }
   return 0;
}

