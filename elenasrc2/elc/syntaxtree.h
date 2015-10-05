//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This file contains ELENA Engine Syntax Tree classes
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef syntaxTreeH
#define syntaxTreeH 1

namespace _ELENA_
{

// --- SyntaxType ---

enum LexicalType
{
   lxNone = 0,
   lxExpression,
   lxObject,
   lxSymbol,
   lxConstantString,
   lxCall,
   lxMessage,
};

// --- SyntaxWriter ---

class SyntaxWriter
{
   StreamWriter* _writer;

public:
   void newNode(LexicalType type, ref_t argument);
   void newNode(LexicalType type)
   {
      newNode(type, 0);
   }

   void closeNode();

   SyntaxWriter(StreamWriter* writer)
   {
      _writer = writer;
   }
};

// --- SyntaxReader ---

class SyntaxReader
{
public:
   // --- Node ---
   class Node
   {
   public:
      LexicalType type;
      ref_t       argument;

      Node()
      {
         type = lxNone;
         argument = 0;
      }
   };

private:
   StreamReader* _reader;

public:
   SyntaxReader(StreamReader* reader)
   {
      _reader = reader;
   }
};

} // _ELENA_

#endif // syntaxTreeH
