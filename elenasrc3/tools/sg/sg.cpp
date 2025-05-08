//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "sgconst.h"
#include "elena.h"
#include "scriptreader.h"
#include "parsertable.h"

using namespace elena_lang;

constexpr size_t MAX_RULE_LEN = 50;

#ifdef _MSC_VER

constexpr auto DEFAULT_ENCODING = FileEncoding::Ansi;

#elif __GNUG__

constexpr auto DEFAULT_ENCODING = FileEncoding::UTF8;

#endif

typedef Map<ustr_t, int, allocUStr, freeUStr> Coordinates;

parse_key_t lastKey = 0;

parse_key_t registerSymbol(ParserTable& table, ustr_t symbol, parse_key_t newKey, bool terminalMode)
{
   parse_key_t key = table.resolveSymbol(symbol);
   if (!key) {
      while (!emptystr(table.resolveKey(newKey))) {
         newKey++;
      }

      if ((newKey & ~pkAnySymbolMask) > 0xFFF)
         throw ParserOverflowError();

      bool terminal = terminalMode || (symbol[0] < 'A') || (symbol[0] > 'Z');
      table.registerSymbol(newKey, symbol, terminal);

      if (lastKey < (newKey & ~pkAnySymbolMask))
         lastKey = newKey & ~pkAnySymbolMask;

      key = newKey;
   }

   return key;
}

parse_key_t registerStarRule(ParserTable& table, parse_key_t key)
{
   IdentifierString ruleName("AUTO_", table.resolveKey(key));
   ruleName.append("*");

   parse_key_t star_key = table.resolveSymbol(*ruleName);
   if (!star_key) {
      star_key = registerSymbol(table, *ruleName, lastKey + 1, false);

      parse_key_t rule[3];
      rule[0] = star_key;

      rule[1] = key;
      rule[2] = star_key;
      table.registerRule(rule, 3);

      rule[1] = pkEps;
      table.registerRule(rule, 2);
   }

   return star_key;
}

parse_key_t registerPlusRule(ParserTable& table, parse_key_t key)
{
   IdentifierString ruleName("AUTO_", table.resolveKey(key));
   ruleName.append("+");

   parse_key_t plus_key = table.resolveSymbol(*ruleName);
   if (!plus_key) {
      plus_key = registerSymbol(table, *ruleName, lastKey + 1, false);

      parse_key_t rule[3];
      rule[0] = plus_key;
      rule[1] = key;
      rule[2] = registerStarRule(table, key);

      table.registerRule(rule, 3);
   }

   return plus_key;
}

parse_key_t registerEpsRule(ParserTable& table, parse_key_t key)
{
   IdentifierString ruleName("AUTO_", table.resolveKey(key));
   ruleName.append("?");

   parse_key_t eps_key = table.resolveSymbol(*ruleName);
   if (!eps_key) {
      eps_key = registerSymbol(table, *ruleName, lastKey + 1, false);

      parse_key_t rule[2];
      rule[0] = eps_key;
      rule[1] = key;
      table.registerRule(rule, 2);

      rule[1] = pkEps;
      table.registerRule(rule, 2);
   }

   return eps_key;
}

void registerBrackets(ParserTable& table, parse_key_t* rule, size_t& rule_len, size_t start, Coordinates& coordinates, int row)
{
   parse_key_t bracket_key = rule[start];
   if (!bracket_key) {
      IdentifierString ruleName("AUTO$");
      do
      {
         ruleName.truncate(5);

         lastKey++;
         ruleName.appendInt(lastKey);
      } while (!emptystr(table.resolveKey(lastKey)));

      bracket_key = registerSymbol(table, *ruleName, lastKey + 1, false);

      coordinates.add(*ruleName, row);

      rule[start] = bracket_key;
   }

   parse_key_t bracket_rule[MAX_RULE_LEN];
   bracket_rule[0] = bracket_key;
   size_t bracket_rule_len = 1;

   // copy the bracket rule
   for (size_t i = start + 1; i < rule_len; i++) {
      bracket_rule[bracket_rule_len++] = rule[i];
   }

   table.registerRule(bracket_rule, bracket_rule_len);

   rule_len = start + 1;
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

      Coordinates coordiantes(-1);

      ParserTable  table;
      table.registerNonterminal(pkStart, "START");
      table.registerNonterminal(pkEps, "eps");

      parse_key_t rule[MAX_RULE_LEN];
      size_t rule_len = 0;

      Stack<size_t> bracketIndexes(-1);

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
            if (token.compare("^")) {
               // NOTE : ^^ means merge lchildren
               rule[rule_len++] = pkTransformMark;
               rule[rule_len++] = pkTransformMark;

               reader.read(token);
            }
            else if (token.compare("=")) {
               // NOTE : ^^ means merge rchildren
               rule[rule_len++] = pkTransformMark;
               rule[rule_len++] = pkTransformMark;
               rule[rule_len++] = pkTransformMark;

               reader.read(token);
            }
            rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, false) | pkInjectable;
         }
         else if (token.compare("=")) {
            reader.read(token);
            rule[rule_len++] = pkTransformMark;
            rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, false) | pkInjectable;
         }
         else if (token.compare("+")) {
            if (rule_len > 0) {
               rule[rule_len - 1] = registerPlusRule(table, rule[rule_len - 1]);
            }
            else throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else if (token.compare("*")) {
            if (rule_len > 0) {
               rule[rule_len - 1] = registerStarRule(table, rule[rule_len - 1]);
            }
            else throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else if (token.compare(";")) {
            table.registerRule(rule, rule_len);
            rule_len = 0;
         }
         else if (token.compare("?")) {
            if (rule_len > 0) {
               rule[rule_len - 1] = registerEpsRule(table, rule[rule_len - 1]);
            }
            else throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else if (token.compare("|")) {
            if (bracketIndexes.count() > 0) {
               registerBrackets(table, rule, rule_len, bracketIndexes.peek(), coordiantes, token.lineInfo.row);
            }
            else if (rule_len != 1) {
               table.registerRule(rule, rule_len);
               rule_len = 1;
            }
            else throw SyntaxError(SG_INVALID_RULE, token.lineInfo);
         }
         else if  (token.compare("{")) {
            rule[rule_len++] = 0;

            bracketIndexes.push(rule_len - 1);
         }
         else if (token.compare("}")) {
            registerBrackets(table, rule, rule_len, bracketIndexes.pop(), coordiantes, token.lineInfo.row);
         }
         else rule[rule_len++] = registerSymbol(table, *token.token, lastKey + 1, false);
      }

      printf("generating...\n");

      auto ambigous = table.generate();
      if (ambigous.value1) {
         ustr_t nonterminal = table.resolveKey(ambigous.value2);
         ustr_t terminal = table.resolveKey(ambigous.value1);

         printf(SG_AMBIGUOUS, nonterminal.str(), terminal.str(), coordiantes.get(nonterminal.str()));
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
