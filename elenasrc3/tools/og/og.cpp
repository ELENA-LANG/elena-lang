//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ogconst.h"
#include "elena.h"

using namespace elena_lang;

#ifdef _MSC_VER

constexpr auto DEFAULT_ENCODING = FileEncoding::Ansi;

#elif __GNUG__

constexpr auto DEFAULT_ENCODING = FileEncoding::UTF8;

#endif

int main(int argc, char* argv[])
{
   printf(OG_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, SG_REVISION_NUMBER);
   if (argc < 2 || argc > 3) {
      printf(OG_HELP);
      return  -1;
   }
   try
   {
      //FileEncoding encoding = DEFAULT_ENCODING;
      //if (argc == 3) {
      //   ustr_t arg = argv[2];
      //   if (arg.compare("-cp", 3)) {
      //      encoding = (FileEncoding)StrConvertor::toUInt(arg.str() + 3, 10);
      //   }
      //   else {
      //      printf(SG_HELP);
      //      return  -1;
      //   }
      //}

      //PathString path(argv[1]);
      //TextFileReader source(path.str(), encoding, false);
      //if (!source.isOpen()) {
      //   printf(SG_FILENOTEXIST);
      //   return  -1;
      //}

      //ParserTable  table;
      //table.registerNonterminal(pkStart, "START");
      //table.registerNonterminal(pkEps, "eps");

      //parse_key_t rule[MAX_RULE_LEN];
      //size_t rule_len = 0;

      //Stack<size_t> bracketIndexes(-1);

      //ScriptReader reader(4, &source);
      //ScriptToken  token;
      //while (reader.read(token)) {
      //   if (token.state == dfaQuote) {
      //      rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, true);
      //   }
      //   else if (token.compare("__define")) {
      //      reader.read(token);

      //      ScriptToken key;
      //      reader.read(key);

      //      registerSymbol(table, token.token.str(), key.token.toInt(), false);

      //      reader.read(token);
      //      if (!token.compare(";"))
      //         throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
      //   }
      //   else if (token.compare("__defineterminal")) {
      //      reader.read(token);

      //      ScriptToken key;
      //      reader.read(key);

      //      registerSymbol(table, token.token.str(), key.token.toInt(), true);

      //      reader.read(token);
      //      if (!token.compare(";"))
      //         throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
      //   }
      //   else if (token.compare("::=")) {
      //      if (rule_len != 1)
      //         throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
      //   }
      //   else if (token.compare("^")) {
      //      reader.read(token);

      //      rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, false) | pkInjectable;
      //   }
      //   else if (token.compare("+")) {
      //      rule[rule_len - 1] = registerPlusRule(table, rule[rule_len - 1]);
      //   }
      //   else if (token.compare("*")) {
      //      rule[rule_len - 1] = registerStarRule(table, rule[rule_len - 1]);
      //   }
      //   else if (token.compare(";")) {
      //      table.registerRule(rule, rule_len);
      //      rule_len = 0;
      //   }
      //   else if (token.compare("?")) {
      //      rule[rule_len - 1] = registerEpsRule(table, rule[rule_len - 1]);
      //   }
      //   else if (token.compare("|")) {
      //      if (bracketIndexes.count() > 0) {
      //         registerBrackets(table, rule, rule_len, bracketIndexes.peek());
      //      }
      //      else if (rule_len != 1) {
      //         table.registerRule(rule, rule_len);
      //         rule_len = 1;
      //      }
      //      else throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
      //   }
      //   else if  (token.compare("{")) {
      //      rule[rule_len++] = 0;

      //      bracketIndexes.push(rule_len - 1);
      //   }
      //   else if (token.compare("}")) {
      //      registerBrackets(table, rule, rule_len, bracketIndexes.pop());
      //   }
      //   else rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, false);
      //}

      //printf("generating...\n");

      //auto ambigous = table.generate();
      //if (ambigous.value1) {
      //   ustr_t terminal = table.resolveKey(ambigous.value2);
      //   ustr_t nonterminal = table.resolveKey(ambigous.value1);

      //   printf(SG_AMBIGUOUS, terminal.str(), nonterminal.str());
      //   return -1;
      //}

      //printf("saving...\n");

      //PathString ext("dat");
      //path.changeExtension(*ext);

      //FileWriter file(*path, FileEncoding::Raw, false);
      //table.save(&file);
   }
   //catch (SyntaxError e)
   //{
   //   printf(e.message, e.lineInfo.row, e.lineInfo.column);

   //   return -1;
   //}
   catch (...)
   {
      printf(OG_FATAL);

      return -1;
   }

   return 0;
}
