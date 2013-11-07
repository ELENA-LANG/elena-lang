//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "textsource.h"
#include "bytecode.h"

#include <windows.h>
#include <stdarg.h>

using namespace _ELENA_;
using namespace _ELENA_TOOL_;


typedef MemoryTrie<ByteCodePattern>     MemoryByteTrie;
typedef MemoryTrieNode<ByteCodePattern> MemoryByteTrieNode;

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

void printLine(const wchar_t* msg, ...)
{
   va_list argptr;

   va_start(argptr, msg);
   vwprintf(msg, argptr);
   va_end(argptr);
   fflush(stdout);
}

ByteCodePattern decodeCommand(TextSourceReader& source, wchar16_t* token, LineInfo& info)
{
   ByteCodePattern pattern;

   pattern.code = ByteCodeCompiler::code(info.line);
   if (pattern.code == bcNone) {
      throw UnknownToken(info);
   }

   info = source.read(token, IDENTIFIER_LEN);
   // pattern arguments
   if (StringHelper::compare(info.line, _T("+"))) {
      info = source.read(token, IDENTIFIER_LEN);

      pattern.argumentType = braAdd;
      pattern.argument = StringHelper::strToInt(token);

      info = source.read(token, IDENTIFIER_LEN);
   }
   else if (StringHelper::compare(info.line, _T("-"))) {
      info = source.read(token, IDENTIFIER_LEN);

      pattern.argumentType = braAdd;
      pattern.argument = -StringHelper::strToInt(token);

      info = source.read(token, IDENTIFIER_LEN);
   }
   else if (StringHelper::compare(info.line, _T("="))) {
      info = source.read(token, IDENTIFIER_LEN);

      pattern.argumentType = braValue;
      pattern.argument = StringHelper::strToInt(token);

      info = source.read(token, IDENTIFIER_LEN);
   }
   //else if (StringHelper::compare(info.line, _T(":="))) {
   //   info = source.read(token, IDENTIFIER_LEN, false);

   //   pattern.argumentType = braMatch;
   //   pattern.argument = StringHelper::strToInt(token);

   //   info = source.read(token, IDENTIFIER_LEN, false);
   //}
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

size_t findChild(MemoryByteTrie& trie, size_t position, ByteCodePattern pattern)
{
   MemoryByteTrieNode node(&trie, position);

   MemoryByteTrieNode::ChildEnumerator children = node.Children();
   while (!children.Eof()) {
      MemoryByteTrieNode child = children.Node();
      if (child.Value() == pattern)
         return child.Position();

      children++;
   }

   return 0;
}

size_t addOpcode(MemoryByteTrie& trie, size_t parentPosition, ByteCodePattern pattern)
{
   size_t position = findChild(trie, parentPosition, pattern);
   if (position == 0) {
      position = trie.addNode(parentPosition, pattern);
   }
   return position;
}

size_t readTransform(size_t position, TextSourceReader& source, wchar16_t* token, LineInfo& info, MemoryByteTrie& trie)
{
   if (!StringHelper::compare(token, _T(";"))) {
      ByteCodePattern pattern = decodeCommand(source, token, info);

      // should be saved in reverse order, to simplify transform algorithm
      position = readTransform(position, source, token, info, trie);

      return addOpcode(trie, position, pattern);
   }
   else return position;
}

void appendOpCodeString(TextSourceReader& source, wchar16_t* token, LineInfo& info, MemoryByteTrie& trie)
{
   // save opcode pattern
   ByteCodePattern pattern = decodeCommand(source, token, info);

   size_t position = addOpcode(trie, 0, pattern);

   while (!StringHelper::compare(token, _T("=>"))) {
      position = addOpcode(trie, position, decodeCommand(source, token, info));
   }

   // save end state
   position = addOpcode(trie, position, ByteCodePattern(bcMatch));

   // save replacement (should be saved in reverse order, to simplify transform algorithm)
   info = source.read(token, IDENTIFIER_LEN);
   readTransform(position, source, token, info, trie);
}

void scanTrie(MemoryByteTrieNode& current, MemoryByteTrieNode::ChildEnumerator nodes, MemoryByteTrieNode& failedNode)
{
   while (!nodes.Eof()) {
      MemoryByteTrieNode child = nodes.Node();
      if (child.Value().code != bcMatch && child.Position() != current.Position()) {
         if (current.Value() == child.Value()) {
            MemoryByteTrieNode::ChildEnumerator next = current.Children();
            while (!next.Eof()) {
               MemoryByteTrieNode node = next.Node();
               scanTrie(node, child.Children(), child);

               next++;
            }
         }
         else if (failedNode.Position() != 0) {
            MemoryByteTrieNode::ChildEnumerator it = failedNode.find(current.Value());
            if (it.Eof()) {
               failedNode.link(current);
            }
         }

         // skip terminator
         if (child.Value().code != bcMatch) // !! useless condition?
            scanTrie(current, child.Children(), failedNode);
      }

      nodes++;
   }
}

void generateSuffixLinks(MemoryByteTrie& trie)
{
   MemoryByteTrieNode emptyNode;
   MemoryByteTrieNode node(&trie);

   MemoryByteTrieNode::ChildEnumerator children = node.Children();
   while (!children.Eof()) {
      MemoryByteTrieNode node = children.Node();
      scanTrie(node, node.Children(), emptyNode);

      children++;
   }
}

int main(int argc, char* argv[])
{
   printLine(_T("ELENA command line optimization table generator %d.%d.0 (C)2012 by Alexei Rakov\n"), ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION);
   if (argc != 2) {
      printLine(_T("og <optimization_file>"));
      return 0;
   }

   Path path(argv[1]);

   TextFileReader   sourceFile(path, feAnsi, false);
   TextSourceReader source(4, &sourceFile);
   LineInfo         info(0, 0, 0);
   TCHAR            token[IDENTIFIER_LEN + 1];

   ByteCodePattern defValue;
   MemoryByteTrie  trie(defValue);
   try
   {
      // add root
      trie.addRootNode(ByteCodePattern(bcNone));

      // generate tree
      while (true) {
         info = source.read(token, IDENTIFIER_LEN);

         if (info.state == dfaEOF) break;

         appendOpCodeString(source, token, info, trie);
      }

      // add suffix links
      generateSuffixLinks(trie);

      // save the result
      Path outputFile(NULL, path);
      outputFile.changeExtension(_T("dat"));

      FileWriter file(outputFile, feRaw, false);
      trie.save(&file);

      printLine(_T("\nSuccessfully created\n"));
   }
   catch(UnknownToken& token)
   {
      printLine(_T("(%d): Invalid token %s\n"), token.line.row, token.line.line);
   }

   return 0;
}

