//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "textsource.h"
#include "bytecode.h"
#include "syntaxtree.h"

#include <stdarg.h>

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

#define BUILD_VERSION   15

typedef Trie<ByteCodePattern>            ByteTrie;
typedef MemoryTrie<ByteCodePattern>      MemoryByteTrie;
typedef MemoryTrieNode<ByteCodePattern>  MemoryByteTrieNode;

struct UnknownToken
{
   LineInfo line;

   UnknownToken(LineInfo line)
   {
      this->line = line;
   }
};

void printLine(const char* msg, ...)
{
   va_list argptr;

   va_start(argptr, msg);
   vprintf(msg, argptr);
   va_end(argptr);
   fflush(stdout);
}

#ifdef _WIN32
void printLine(const wchar_t* msg, ...)
{
   va_list argptr;

   va_start(argptr, msg);
   vwprintf(msg, argptr);
   va_end(argptr);
   fflush(stdout);
}
#endif

ByteCodePattern decodeCommand(TextSourceReader& source, char* token, LineInfo& info)
{
   ByteCodePattern pattern;

   pattern.code = ByteCodeCompiler::code(info.line);
   if (pattern.code == bcNone) {
      throw UnknownToken(info);
   }

   info = source.read(token, IDENTIFIER_LEN);
   // pattern arguments
   if (info.line.compare("+")) {
      info = source.read(token, IDENTIFIER_LEN);

      pattern.argumentType = braAdd;
      pattern.argument = ident_t(token).toInt();

      info = source.read(token, IDENTIFIER_LEN);
   }
   else if (info.line.compare("-")) {
      info = source.read(token, IDENTIFIER_LEN);

      pattern.argumentType = braAdd;
      pattern.argument = -ident_t(token).toInt();

      info = source.read(token, IDENTIFIER_LEN);
   }
   else if (info.line.compare("=")) {
      info = source.read(token, IDENTIFIER_LEN);

      if (pattern.code > MAX_DOUBLE_ECODE && ByteCodeCompiler::IsJump(pattern.code)) {
         pattern.argumentType = braAditionalValue;
      }
      else pattern.argumentType = braValue;
      pattern.argument = ident_t(token).toInt();

      info = source.read(token, IDENTIFIER_LEN);
   }
   else if (info.line.compare(":=")) {
      info = source.read(token, IDENTIFIER_LEN);

      pattern.argumentType = braMatch;
      pattern.argument = ident_t(token).toInt();

      info = source.read(token, IDENTIFIER_LEN);
   }
   // TransformTape will set match the argument if the argument equals to the previous one
   else if (info.line.compare("~")) {
      if (pattern.code > MAX_DOUBLE_ECODE && ByteCodeCompiler::IsJump(pattern.code)) {
         pattern.argumentType = braAdditionalSame;
      }
      else pattern.argumentType = braSame;
      pattern.argument = 1;            

      info = source.read(token, IDENTIFIER_LEN);
   }
   // TransformTape will set match the argument if the argument does not equal to the previous one
   else if (info.line.compare("!")) {
      if (pattern.code > MAX_DOUBLE_ECODE && ByteCodeCompiler::IsJump(pattern.code)) {
         pattern.argumentType = braAdditionalSame;
      }
      else pattern.argumentType = braSame;
      pattern.argument = 0;

      info = source.read(token, IDENTIFIER_LEN);
   }
   //// !! is it used?
   //else if (StringHelper::compare(info.line, _T("<<"))) {
   //   pattern.argumentType = braCopy;

   //   info = source.read(token, IDENTIFIER_LEN, false);
   //   if (StringHelper::compare(info.line, _T("$arg1"))) {
   //      pattern.argument = 1;
   //   }
   //   else if (StringHelper::compare(info.line, _T("$arg2"))) {
   //      pattern.argument = 2;
   //   }
   //   else throw UnknownToken(info);

   //   info = source.read(token, IDENTIFIER_LEN, false);
   //}
   else pattern.argumentType = braNone;

   return pattern;
}

size_t readTransform(size_t position, TextSourceReader& source, char* token, LineInfo& info, ByteTrie& trie)
{
   if (!ident_t(token).compare(";")) {
      ByteCodePattern pattern = decodeCommand(source, token, info);

      // should be saved in reverse order, to simplify transform algorithm
      position = readTransform(position, source, token, info, trie);

      return trie.add(position, pattern);
   }
   else return position;
}

void appendOpCodeString(TextSourceReader& source, char* token, LineInfo& info, ByteTrie& trie)
{
   // save opcode pattern
   ByteCodePattern pattern = decodeCommand(source, token, info);

   size_t position = trie.add(0, pattern);

   while (!ident_t(token).compare("=>")) {
      position = trie.add(position, decodeCommand(source, token, info));
   }

   // save end state
   position = trie.add(position, ByteCodePattern(bcMatch));

   // save replacement (should be saved in reverse order, to simplify transform algorithm)
   info = source.read(token, IDENTIFIER_LEN);
   readTransform(position, source, token, info, trie);
}

SNodePattern decodeSourceNode(TextSourceReader& source, char* token, LineInfo& info, Map<ident_t, int>& tokens)
{
   SNodePattern pattern;
   pattern.type = (LexicalType)tokens.get(token);
   if (pattern.type == lxNone) {
      throw UnknownToken(info);
   }

   info = source.read(token, IDENTIFIER_LEN);
   if (ident_t(token).compare("|")) {
      info = source.read(token, IDENTIFIER_LEN);

      pattern.followType = (LexicalType)tokens.get(token);
      if (pattern.followType == lxNone) {
         throw UnknownToken(info);
      }

      info = source.read(token, IDENTIFIER_LEN);
   }

   if (ident_t(token).compare("=")) {
      info = source.read(token, IDENTIFIER_LEN);
      pattern.patternId = ident_t(token).toInt();
      if (!pattern.patternId)
         throw UnknownToken(info);

      info = source.read(token, IDENTIFIER_LEN);
      if (!ident_t(token).compare(";"))
         throw UnknownToken(info);
   }
   else if (ident_t(token).compare(",")) {
      info = source.read(token, IDENTIFIER_LEN);
   }
   else throw UnknownToken(info);

   return pattern;
}

void appendSourceString(TextSourceReader& source, char* token, LineInfo& info, SyntaxTrie& trie, Map<ident_t, int>& tokens)
{
   // save source pattern
   SNodePattern pattern = decodeSourceNode(source, token, info, tokens);

   size_t position = trie.add(0, pattern);

   while (!ident_t(token).compare(";")) {
      position = trie.add(position, decodeSourceNode(source, token, info, tokens));
   }

   //// save end state
   //position = trie.add(position, ByteCodePattern(bcMatch));

   //// save replacement (should be saved in reverse order, to simplify transform algorithm)
   //info = source.read(token, IDENTIFIER_LEN);
   //readTransform(position, source, token, info, trie);
}

bool isMatchNode(ByteCodePattern pattern)
{
   return pattern.code == bcMatch;
}

int parseOpcodeRule(Path& path)
{
   TextFileReader   sourceFile(path.c_str(), feUTF8, false);
   TextSourceReader source(4, &sourceFile);
   LineInfo         info(0, 0, 0);
   char             token[IDENTIFIER_LEN + 1];

   ByteCodePattern  defValue;
   ByteTrie         trie(defValue);
   try
   {
      // add root
      trie.addRoot(ByteCodePattern(bcNone));

      // generate tree
      while (true) {
         info = source.read(token, IDENTIFIER_LEN);

         if (info.state == dfaEOF) break;

         appendOpCodeString(source, token, info, trie);
      }

      // add suffix links
      trie.prepare(isMatchNode);

      // save the result
      Path outputFile(path);
      outputFile.changeExtension("dat");

      FileWriter file(outputFile.c_str(), feRaw, false);
      trie.save(&file);

      printLine("\nSuccessfully created\n");

      return 0;
   }
   catch (UnknownToken& token)
   {
      printLine("(%d): Invalid token %s\n", token.line.row, token.line.line);

      return -1;
   }
}

int parseSourceRules(Path& path)
{
   Map<ident_t, int> tokens;
   loadSyntaxTokens(tokens, true);

   TextFileReader   sourceFile(path.c_str(), feUTF8, false);
   TextSourceReader source(4, &sourceFile);
   LineInfo         info(0, 0, 0);
   char             token[IDENTIFIER_LEN + 1];

   SNodePattern     defValue;
   SyntaxTrie       trie(defValue);
   try
   {
      // add root
      trie.addRoot(SNodePattern(lxRoot));

      // generate tree
      while (true) {
         info = source.read(token, IDENTIFIER_LEN);

         if (info.state == dfaEOF) break;

         appendSourceString(source, token, info, trie, tokens);
      }

      //// add suffix links
      //trie.prepare(isMatchNode);

      // save the result
      Path outputFile(path);
      outputFile.changeExtension("dat");

      FileWriter file(outputFile.c_str(), feRaw, false);
      trie.save(&file);

      printLine("\nSuccessfully created\n");
      return 0;
   }
   catch (UnknownToken& token)
   {
      printLine("(%d): Invalid token %s\n", token.line.row, token.line.line);

      return -1;
   }
}

int main(int argc, char* argv[])
{
   int retVal = 0;
   printLine("ELENA command line optimization table generator %d.%d.%d (C)2012-21 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, BUILD_VERSION);
   if (argc == 2) {
      Path path(argv[1]);
      retVal = parseOpcodeRule(path);
   }
   else if (argv[1][0] == 's' && argv[1][1] == 0) {
      Path path(argv[2]);
      retVal = parseSourceRules(path);
   }
   else {
      printLine("og [s] <optimization_file>");
      return 0;
   }

   return retVal;
}

