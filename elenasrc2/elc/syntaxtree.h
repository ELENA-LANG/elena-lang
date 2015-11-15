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
   lxObjectMask      = 0x0100,
   lxExpressionMask  = 0x0200,
   lxPrimitiveOpMask = 0x1000,
   lxSimpleMask      = 0x4000,   // idicates if the implementation does not affect base / other registers

   lxEnding          = -1,

   lxNone            = 0x000,

   lxRoot            = 0x001,

   //lxCodeBlock       = 0x001,

   lxExpression      = 0x0301,
   lxBoxing          = 0x0302,
   lxCondBoxing      = 0x0303,
   lxArgBoxing       = 0x0304,
   lxTypecasting     = 0x0305,
   lxCalling         = 0x0306,
   lxDirectCalling   = 0x0307,
   lxSDirctCalling   = 0x0308,
   lxResending       = 0x0309,
   lxTrying          = 0x030A,
   lxLocking         = 0x030B,
   lxBranching       = 0x030C,
   lxSwitching       = 0x030D,
   lxLooping         = 0x030E,
   lxReturning       = 0x030F,
   lxThrowing        = 0x0310,
   lxStdExternalCall = 0x0311,
   lxExternalCall    = 0x0312,
   lxIntExtArgument  = 0x0313,
   lxExtArgument     = 0x0314,
   lxInternalCall    = 0x0315,
   lxMember          = 0x0316,
   lxAssigning       = 0x0317,
   lxArgUnboxing     = 0x0318,
   lxIf              = 0x0319,
   lxElse            = 0x031A,
   lxOption          = 0x031B,
   lxBody            = 0x031C,

   lxBoolOp          = 0x0320,
   lxNilOp           = 0x0321,
   lxIntOp           = 0x1322,
   lxLongOp          = 0x1323,
   lxRealOp          = 0x1324,
   lxIntArrOp        = 0x1325,
   lxArrOp           = 0x1326,

   lxNested          = 0x0101,
   lxStruct          = 0x0102,
   lxSymbol          = 0x0103,
   lxConstantSymbol  = 0x0104,
   lxField           = 0x0105,
   lxFieldAddress    = 0x0106,
   lxLocalAddress    = 0x4107,
   lxBlockLocalAddr  = 0x4108,
   lxLocal           = 0x4109,
   lxBlockLocal      = 0x410A,
   lxConstantString  = 0x410B,
   lxConstantChar    = 0x410C,
   lxConstantInt     = 0x410D,
   lxConstantLong    = 0x410E,
   lxConstantReal    = 0x410F,
   lxConstantClass   = 0x4110,
   lxMessageConstant = 0x4111,
   lxSignatureConstant = 0x4112,
   lxVerbConstant    = 0x4113,
   lxNil             = 0x4114,
   lxCurrent         = 0x4115,
   lxResult          = 0x4116,
   lxResultField     = 0x4117,
   lxCurrentMessage  = 0x4118,

   lxVariable        = 0x407,
   lxIntVariable     = 0x408,
   lxLongVariable    = 0x409,
   lxReal64Variable  = 0x40A,
   lxBytesVariable   = 0x40B,
   lxShortsVariable  = 0x40C,
   lxIntsVariable    = 0x40D,
   lxBinaryVariable  = 0x40E,
   lxReleasing       = 0x40F,

   lxTarget          = 0x801,
   lxType            = 0x802,
   lxStacksafe       = 0x803,
   lxTempLocal       = 0x804,
   lxOverridden      = 0x805,
   lxIfValue         = 0x806,
   lxElseValue       = 0x807,
   lxMessage         = 0x808,

   lxBreakpoint      = 0x2001,
   lxCol             = 0x2002,
   lxRow             = 0x2003,
   lxLength          = 0x2004,
   lxTerminal        = 0x2005,
   lxLevel           = 0x2006,
};

// --- SyntaxWriter ---

class SyntaxWriter
{
   MemoryWriter  _writer;
   Stack<size_t> _bookmarks;

public:
   int newBookmark()
   {
      _bookmarks.push(_writer.Position());

      return _bookmarks.Count();
   }

   void removeBookmark()
   {
      _bookmarks.pop();
   }

   void clear()
   {
      _writer.seek(0);
      _bookmarks.clear();
   }

   void insert(int position, LexicalType type, ref_t argument);
   void insert(int position, LexicalType type)
   {
      insert(type, 0);
   }
   void insert(LexicalType type, ref_t argument)
   {
      insert(0, type, argument);
   }
   void insert(LexicalType type)
   {
      insert(0, type, 0);
   }
   void insertChild(int position, LexicalType type, ref_t argument)
   {
      insert(position, type, argument);
      insert(position + 8, lxEnding, 0);
   }
   void insertChild(LexicalType type, ref_t argument)
   {      
      insert(lxEnding, 0);
      insert(type, argument);
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

// --- SyntaxTree ---

class SyntaxTree
{
public:
   // --- Node ---
   class Node
   {
      friend class SyntaxTree;

      SyntaxTree*   tree;
      size_t        position;

      Node(SyntaxTree* tree, size_t position, LexicalType type, ref_t argument);

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

      void operator = (LexicalType type)
      {
         this->type = type;

         int prevPos = tree->_reader.Position();

         tree->_reader.seek(position - 8);
         *(int*)(tree->_reader.Address()) = (int)type;
         tree->_reader.seek(prevPos);
      }

      Node firstChild() const
      {
         return tree->readFirstNode(position);
      }

      Node lastChild() const
      {
         Node current = firstChild();
         if (current != lxNone) {
            while (current.nextNode() != lxNone) {
               current = current.nextNode();
            }
         }
         return current;
      }

      Node nextNode() const
      {
         return tree->readNextNode(position);
      }

      Node prevNode() const
      {
         return tree->readPreviousNode(position);
      }

      Node()
      {
         type = lxNone;
         argument = 0;

         tree = NULL;
      }
   };

private:
   MemoryReader _reader;

   Node read();

public:
   static Node findChild(Node node, LexicalType type)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type) {
         current = current.nextNode();
      }

      return current;
   }

   static Node findChild(Node node, LexicalType type1, LexicalType type2)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type1) {
         if (current == type2)
            return current;

         current = current.nextNode();
      }

      return current;
   }

   static Node findChild(Node node, LexicalType type1, LexicalType type2, LexicalType type3)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type1) {
         if (current == type2 || current == type3)
            return current;

         current = current.nextNode();
      }

      return current;
   }

   static bool existChild(Node node, LexicalType type)
   {
      Node child = findChild(node, type);

      return child == type;
   }

   Node readRoot();
   Node readFirstNode(size_t position);
   Node readNextNode(size_t position);
   Node readPreviousNode(size_t position);

   SyntaxTree(_Memory* dump)
      : _reader(dump)
   {      
   }
};

} // _ELENA_

#endif // syntaxTreeH
