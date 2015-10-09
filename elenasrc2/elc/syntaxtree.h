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
   lxRoot,
   lxExpression,
   lxObject,
   lxSymbol,
   lxParam,
   lxConstantString,
   lxConstantClass,
   lxNil,
   lxCall,
   lxMessage,

   lxBreakpoint,
   lxBPCol,
   lxBPRow,
   lxBPLength,
};

// --- SyntaxWriter ---

class SyntaxWriter
{
   MemoryWriter _writer;

public:
   void clear()
   {
      _writer.seek(0);
   }

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

      bool operator == (LexicalType type)
      {
         return this->type == type;
      }
      bool operator != (LexicalType type)
      {
         return this->type != type;
      }

      Node firstChild() const
      {
         return reader->readFirstNode(position);
      }

      Node nextNode() const
      {
         return reader->readNextNode(position);
      }

      Node()
      {
         type = lxNone;
         argument = 0;

         reader = NULL;
      }
   };

private:
   MemoryReader _reader;

   Node read();

public:
   Node readRoot();
   Node readFirstNode(size_t position);
   Node readNextNode(size_t position);

   SyntaxReader(_Memory* dump)
      : _reader(dump)
   {      
   }
};

} // _ELENA_

#endif // syntaxTreeH
