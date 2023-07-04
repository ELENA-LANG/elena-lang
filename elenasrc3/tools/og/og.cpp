//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "buildtree.h"
#include "elena.h"
#include "bytecode.h"
#include "ogconst.h"
#include "scriptreader.h"

using namespace elena_lang;

#ifdef _MSC_VER

constexpr auto DEFAULT_ENCODING = FileEncoding::UTF8;

#elif __GNUG__

constexpr auto DEFAULT_ENCODING = FileEncoding::UTF8;

#endif

typedef MemoryTrieBuilder<ByteCodePattern>         ByteCodeTrieBuilder;

bool isMatchNode(ByteCodePattern pattern)
{
   return pattern.code == ByteCode::Match;
}

ByteCodePattern decodePattern(ScriptReader& reader, ScriptToken& token)
{
   ByteCodePattern pattern = { ByteCode::None };

   IdentifierString command(token.token.str());

   size_t timePos = command.length();

   reader.read(token);

   command.append(' ');
   command.append(token.token.str());

   pattern.code = ByteCodeUtil::code(*command);
   if (pattern.code == ByteCode::None) {
      command.truncate(timePos);

      pattern.code = ByteCodeUtil::code(*command);
   }
   else reader.read(token);

   if (pattern.code == ByteCode::None) {
      throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);
   }

   if (token.compare(":")) {
      reader.read(token);

      pattern.argType = ByteCodePatternType::Match;
      if (token.compare("$1")) {
         pattern.argValue = 1;
         pattern.argType = ByteCodePatternType::Set;

         reader.read(token);
      }
      else if (token.compare("$2")) {
         pattern.argValue = 2;
         pattern.argType = ByteCodePatternType::Set;

         reader.read(token);
      }
      else if (token.compare("#1")) {
         pattern.argValue = 1;
         pattern.argType = ByteCodePatternType::Match;
         reader.read(token);
      }
      else if (token.compare("#2")) {
         pattern.argValue = 2;
         pattern.argType = ByteCodePatternType::Match;
         reader.read(token);
      }
      else throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);
   }
   else if (token.compare("=")) {
      reader.read(token);

      pattern.argValue = token.token.toInt();
      pattern.argType = ByteCodePatternType::MatchArg;

      reader.read(token);
   }

   return pattern;
}

pos_t readTransformPart(pos_t position, ScriptReader& reader, ScriptToken& token, ByteCodeTrieBuilder& trie)
{
   if (!token.compare(";")) {
      ByteCodePattern pattern = decodePattern(reader, token);

      if (token.compare(",")) {
         reader.read(token);
      }
      else if (!token.compare(";"))
         throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

      // should be saved in reverse order, to simplify transform algorithm
      position = readTransformPart(position, reader, token, trie);

      return trie.add(position, pattern);
   }
   return position;
}

void parseOpcodeRule(ScriptReader& reader, ScriptToken& token, ByteCodeTrieBuilder& trie)
{
   // save opcode pattern
   ByteCodePattern pattern = decodePattern(reader, token);

   pos_t position = trie.add(0, pattern);

   while (!token.compare("=>")) {
      if (token.compare(",")) {
         reader.read(token);
      }
      else throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

      position = trie.add(position, decodePattern(reader, token));
   }

   // save end state
   position = trie.add(position, { ByteCode::Match });

   // save replacement (should be saved in reverse order, to simplify transform algorithm)
   reader.read(token);
   readTransformPart(position, reader, token, trie);
}

int parseRuleSet(FileEncoding encoding, path_t path)
{
   ByteCodeTrieBuilder trie({ ByteCode::None });

   TextFileReader source(path.str(), encoding, false);
   ScriptReader reader(4, &source);
   ScriptToken  token;

   // add root
   trie.addRoot({ ByteCode::Nop });

   // generate tree
   while (reader.read(token)) {
      parseOpcodeRule(reader, token, trie);
   }

   // add suffix links
   trie.prepare(isMatchNode);

   // save the result
   PathString outputFile(path);
   outputFile.changeExtension("dat");

   FileWriter file(*outputFile, FileEncoding::Raw, false);
   trie.save(&file);

   printf("\nSuccessfully created\n");

   return 0;
}

BuildKeyPattern decodeBuildPattern(BuildKeyMap& dictionary, ScriptReader& reader, ScriptToken& token)
{
   BuildKeyPattern pattern = { dictionary.get(token.token.str())};
   if (pattern.type == BuildKey::None)
      throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

   reader.read(token);

   if (token.compare("=")) {
      reader.read(token);

      int patternId = token.token.toInt();
      if (!patternId)
         throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

      pattern.pattternId = patternId;

      reader.read(token);
   }

   return pattern;
}

void parseBuildCodeRule(BuildCodeTrie& trie, BuildKeyMap& dictionary, ScriptReader& reader, ScriptToken& token)
{
   BuildKeyPattern pattern = decodeBuildPattern(dictionary, reader, token);

   pos_t position = trie.add(0, pattern);

   while (!token.compare(";")) {
      position = trie.add(position, decodeBuildPattern(dictionary, reader, token));
   }
}

int parseSourceRules(FileEncoding encoding, path_t path)
{
   BuildKeyMap dictionary({ BuildKey::None });
   BuildTree::loadBuildKeyMap(dictionary);

   BuildCodeTrie       trie({ BuildKey::None });

   TextFileReader source(path.str(), encoding, false);
   ScriptReader reader(4, &source);
   ScriptToken  token;

   trie.addRoot({ BuildKey::Root });

   // generate tree
   while (reader.read(token)) {
      parseBuildCodeRule(trie, dictionary, reader, token);
   }

   // save the result
   PathString outputFile(path);
   outputFile.changeExtension("dat");

   FileWriter file(*outputFile, FileEncoding::Raw, false);
   trie.save(&file);

   printf("\nSuccessfully created\n");

   return 0;
}

int main(int argc, char* argv[])
{
   int retVal = 0;

   printf(OG_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, SG_REVISION_NUMBER);
   try
   {
      if (argc == 2) {
         PathString path(argv[1]);
         retVal = parseRuleSet(DEFAULT_ENCODING, *path);
      }
      else if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 's') {
         PathString path(argv[2]);
         retVal = parseSourceRules(DEFAULT_ENCODING, *path);
      }
      else {
         printf(OG_HELP);
         return  -1;
      }
   }
   catch (SyntaxError e)
   {
      printf(e.message, e.lineInfo.row, e.lineInfo.column);

      return -1;
   }
   catch (...)
   {
      printf(OG_FATAL);

      return -1;
   }

   return retVal;
}
