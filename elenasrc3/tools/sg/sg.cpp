//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "sgconst.h"
#include "elena.h"
#include "scriptreader.h"
#include "parsertable.h"
#include "tree.h"

using namespace elena_lang;

constexpr size_t MAX_RULE_LEN = 20;

#ifdef _MSC_VER

constexpr auto DEFAULT_ENCODING = FileEncoding::Ansi;

#elif __GNUG__

constexpr auto DEFAULT_ENCODING = FileEncoding::UTF8;

#endif

typedef Tree<parse_key_t, 0> SyntaxTempTree;

parse_key_t lastKey = 0;

parse_key_t registerSymbol(ParserTable& table, ustr_t symbol, parse_key_t newKey, bool terminalMode)
{
   parse_key_t key = table.resolveSymbol(symbol);
   if (!key) {
      while (!emptystr(table.resolveKey(newKey))) {
         newKey++;
      }

      bool terminal = terminalMode || (symbol[0] < 'A') || (symbol[0] > 'Z');
      table.registerSymbol(newKey, symbol, terminal);

      if (lastKey < (newKey & ~pkAnySymbolMask))
         lastKey = newKey & ~pkAnySymbolMask;

      key = newKey;
   }

   return key;
}

int main(int argc, char* argv[])
{
   printf(SG_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, SG_REVISION_NUMBER);
   if (argc < 2 || argc > 3) {
      printf(SG_HELP);
      return  -1;
   }
   try
   {
      FileEncoding encoding = DEFAULT_ENCODING;
      if (argc == 3) {
         ustr_t arg = argv[2];
         if (arg.compare("-cp", 3)) {
            encoding = (FileEncoding)StrConvertor::toUInt(arg.str() + 3, 10);
         }
         else {
            printf(SG_HELP);
            return  -1;
         }
      }

      PathString path(argv[1]);
      TextFileReader source(path.str(), encoding, false);
      if (!source.isOpen()) {
         printf(SG_FILENOTEXIST);
         return  -1;
      }

      ParserTable  table;
      table.registerNonterminal(pkStart, "START");
      table.registerNonterminal(pkEps, "eps");

      parse_key_t rule[MAX_RULE_LEN];
      size_t rule_len = 0;

      ScriptReader reader(4, &source);
      ScriptToken  token;
      while (reader.read(token)) {
         if (token.state == dfaQuote) {
            rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, true);
         }
         else if (token.compare("__define")) {
            reader.read(token);

            ScriptToken key;
            reader.read(key);

            registerSymbol(table, token.token.str(), key.token.toInt(), false);

            reader.read(token);
            if (!token.compare(";"))
               throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else if (token.compare("__defineterminal")) {
            reader.read(token);

            ScriptToken key;
            reader.read(key);

            registerSymbol(table, token.token.str(), key.token.toInt(), true);

            reader.read(token);
            if (!token.compare(";"))
               throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else if (token.compare("::=")) {
            if (rule_len != 1)
               throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else if (token.compare("^")) {
            reader.read(token);

            rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, false) | pkInjectable;
         }
         else if (token.compare(";")) {
            table.registerRule(rule, rule_len);
            rule_len = 0;
         }
         else if (token.compare("|")) {
            if (rule_len != 1) {
               table.registerRule(rule, rule_len);
               rule_len = 1;
            }
            else throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, false);
      }

      printf("generating...\n");

      auto ambigous = table.generate();
      if (ambigous.value1) {
         ustr_t terminal = table.resolveKey(ambigous.value2);
         ustr_t nonterminal = table.resolveKey(ambigous.value1);

         printf(SG_AMBIGUOUS, terminal.str(), nonterminal.str());
         return -1;
      }

      printf("saving...\n");

      PathString ext("dat");
      path.changeExtension(*ext);

      FileWriter file(*path, FileEncoding::Raw, false);
      table.save(&file);
   }
   catch (SyntaxError e)
   {
      printf(e.message, e.lineInfo.row, e.lineInfo.column);

      return -1;
   }
   catch (...)
   {
      printf(SG_FATAL);

      return -1;
   }

   return 0;
}
