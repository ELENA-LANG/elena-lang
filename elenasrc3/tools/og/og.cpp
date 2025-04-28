//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                             (C)2021-2025, by Aleksey Rakov
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
typedef MemoryTrieBuilder<BuildPattern>         BuildTrieBuilder;

bool isMatchNode(ByteCodePattern pattern)
{
   return pattern.code == ByteCode::Match;
}

bool isMatchNode(BuildPattern pattern)
{
   return pattern.key == BuildKey::Match;
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
      else if (token.compare("$3")) {
         // NOTE: it is a special case, there both $1 and $2 argument are set
         pattern.argValue = 3;
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

BuildPattern decodeBuildPattern(BuildKeyMap& dictionary, ScriptReader& reader, ScriptToken& token)
{
   BuildPattern pattern = { dictionary.get(token.token.str()) };

   reader.read(token);

   if (pattern.key == BuildKey::None)
      throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

   if (token.compare("=")) {
      reader.read(token);

      pattern.argValue = token.token.toInt();
      pattern.argType = BuildPatternType::MatchArg;

      reader.read(token);
   }
   else {
      if (token.compare("$1")) {
         pattern.argValue = 1;
         pattern.argType = BuildPatternType::Set;

         reader.read(token);
      }
      else if (token.compare("$2")) {
         pattern.argValue = 2;
         pattern.argType = BuildPatternType::Set;

         reader.read(token);
      }
      else if (token.compare("#1")) {
         pattern.argValue = 1;
         pattern.argType = BuildPatternType::Match;
         reader.read(token);
      }
      else if (token.compare("#2")) {
         pattern.argValue = 2;
         pattern.argType = BuildPatternType::Match;
         reader.read(token);
      }
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

pos_t readBuildTransformPart(pos_t position, ScriptReader& reader, ScriptToken& token, BuildTrieBuilder& trie, BuildKeyMap& dictionary)
{
   if (!token.compare(";")) {
      BuildPattern pattern = decodeBuildPattern(dictionary, reader, token);

      if (token.compare(",")) {
         reader.read(token);
      }
      else if (!token.compare(";"))
         throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

      // should be saved in reverse order, to simplify transform algorithm
      position = readBuildTransformPart(position, reader, token, trie, dictionary);

      return trie.add(position, pattern);
   }
   return position;
}

void parseOpcodeRule(ScriptReader& reader, ScriptToken& token, ByteCodeTrieBuilder& trie)
{
   // save opcode pattern
   ByteCodePattern pattern = decodePattern(reader, token);

   pos_t position = trie.add(0, pattern);

   ByteCodePatternType matchArg = ByteCodePatternType::None;
   while (!token.compare("=>")) {
      if (token.compare(",")) {
         reader.read(token);
      }
      else throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

      position = trie.add(position, decodePattern(reader, token));

      if (token.compare("$")) {
         matchArg = ByteCodePatternType::IfAccFree;
         reader.read(token);
         if(!token.compare("=>"))
            throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);
      }
   }

   // save end state
   position = trie.add(position, { ByteCode::Match, matchArg });

   // save replacement (should be saved in reverse order, to simplify transform algorithm)
   reader.read(token);
   readTransformPart(position, reader, token, trie);
}

int parseByteCodeRuleSet(FileEncoding encoding, path_t path)
{
   ByteCodeTrieBuilder trie({ ByteCode::None });

   TextFileReader source(path.str(), encoding, false);
   if(!source.isOpen()) {
      printf(OG_FILENOTEXIST);

      throw AbortError();
   }

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

void parseBuildCodeRule(ScriptReader& reader, ScriptToken& token, BuildTrieBuilder& trie, BuildKeyMap& dictionary)
{
   // save opcode pattern
   BuildPattern pattern = decodeBuildPattern(dictionary, reader, token);

   pos_t position = trie.add(0, pattern);

   while (!token.compare("=>")) {
      if (token.compare(",")) {
         reader.read(token);
      }
      else throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);

      position = trie.add(position, decodeBuildPattern(dictionary, reader, token));
   }

   reader.read(token);
   if (token.state == dfaInteger) {
      position = trie.add(position, { BuildKey::Match, BuildPatternType::None, token.token.toInt() });

      reader.read(token);
      if(!token.compare(";"))
         throw SyntaxError(OG_INVALID_OPCODE, token.lineInfo);
   }
   else {
      // save end state
      position = trie.add(position, { BuildKey::Match });

      // save replacement (should be saved in reverse order, to simplify transform algorithm)
      readBuildTransformPart(position, reader, token, trie, dictionary);
   }
}

int parseBuildKeyRules(FileEncoding encoding, path_t path)
{
   BuildKeyMap dictionary({ BuildKey::None });
   BuildTree::loadBuildKeyMap(dictionary);

   BuildTrieBuilder trie({ });

   TextFileReader source(path.str(), encoding, false);
   if (!source.isOpen()) {
      printf(OG_FILENOTEXIST);

      throw AbortError();
   }

   ScriptReader reader(4, &source);
   ScriptToken  token;

   // add root
   trie.addRoot({ BuildKey::Root });

   // generate tree
   while (reader.read(token)) {
      parseBuildCodeRule(reader, token, trie, dictionary);
   }

   // add suffix links
   //trie.prepare(isMatchNode);

   // save the result
   PathString outputFile(path);
   outputFile.changeExtension("dat");

   FileWriter file(*outputFile, FileEncoding::Raw, false);
   if (!file.isOpen()) {
      IdentifierString pathStr(*outputFile);

      printf("\nCannot create a file %s\n", pathStr.str());

      return -1;
   }

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
         retVal = parseByteCodeRuleSet(DEFAULT_ENCODING, *path);
      }
      else if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 's') {
         PathString path(argv[2]);
         retVal = parseBuildKeyRules(DEFAULT_ENCODING, *path);
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
