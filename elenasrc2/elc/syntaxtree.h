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
   MemoryWriter _writer;

public:
   void newNode(LexicalType type, ref_t argument);
   void newNode(LexicalType type)
   {
      newNode(type, 0);
   }
   void appendNode(LexicalType type, ref_t argument)
   {
      newNode(type, argument);
      closeNode();
   }
   void appendNode(LexicalType type)
   {
      newNode(type);
      closeNode();
   }

   void closeNode();

   SyntaxWriter(_Memory* dump)
      : _writer(dump)
   {
   }
};

// --- SyntaxReader ---

class SyntaxReader
{
public:
   // --- Node ---
   class Node
   {
      friend class SyntaxReader;

      SyntaxReader* reader;
      size_t        position;

      Node(SyntaxReader* reader, size_t position, LexicalType type, ref_t argument);

   public:
      LexicalType   type;
      ref_t         argument;

      Node()
      {
         type = lxNone;
         argument = 0;

         reader = NULL;
      }
   };

private:
   MemoryReader _reader;

public:
   Node Root();

   SyntaxReader(_Memory* dump)
      : _reader(dump)
   {      
   }
};

} // _ELENA_

#endif // syntaxTreeH
