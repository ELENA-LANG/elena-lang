//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Syntax Tree classes
//
//                                              (C)2005-2016, by Alexei Rakov
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
   //lxPrimitiveOpMask = 0x1000,
   lxSimpleMask      = 0x4000,   // idicates if the implementation does not affect base / other registers

   lxEnding          = -1,

   lxNone            = 0x0000,
   lxInvalid         = 0x0001,
   lxRoot            = 0x0002,

   lxBaseClass       = 0x0010,

   lxExpression      = 0x0301,
   lxBoxing          = 0x0302,   // boxing of the argument
   lxCondBoxing      = 0x0303,   // conditional boxing
   lxUnboxing        = 0x0304,   // boxing and unboxing of the argument
   lxArgBoxing       = 0x0305,
   lxTypecasting     = 0x0306,
   //lxCalling         = 0x0307,
   lxDirectCalling   = 0x0308,
   lxSDirctCalling   = 0x0309,
   lxResending       = 0x030A,
   lxTrying          = 0x030B,
   lxAlt             = 0x030C,
   lxLocking         = 0x030D,
   //lxBranching       = 0x030E,
   //lxSwitching       = 0x030F,
   lxLooping         = 0x0310,
   lxReturning       = 0x0311,
   lxThrowing        = 0x0312,
   //lxStdExternalCall = 0x0313,
   //lxExternalCall    = 0x0314,
   //lxCoreAPICall     = 0x0315,
   //lxIntExtArgument  = 0x0316,
   //lxExtArgument     = 0x0317,
   //lxExtInteranlRef  = 0x0318,
   //lxInternalCall    = 0x0319,
   lxMember          = 0x031A,
   lxAssigning       = 0x031B,
   lxArgUnboxing     = 0x031C,
   //lxIf              = 0x031D,
   lxElse            = 0x031E,
   //lxOption          = 0x031F,
   lxBody            = 0x0320,
   lxLocalUnboxing   = 0x0321,
   lxNewFrame        = 0x322,       // if argument -1 - than with presaved message

   //lxBoolOp          = 0x0322,
   //lxNilOp           = 0x0323,
   //lxIntOp           = 0x1324,
   //lxLongOp          = 0x1325,
   //lxRealOp          = 0x1326,
   //lxIntArrOp        = 0x1327,
   //lxArrOp           = 0x1328,

   lxNested          = 0x0101,
   //lxStruct          = 0x0102,
   lxSymbol          = 0x0103,
   lxConstantSymbol  = 0x4104,
   lxField           = 0x0105,
   lxFieldAddress    = 0x0106,
   lxLocalAddress    = 0x4107,
   lxBlockLocalAddr  = 0x4108,
   lxLocal           = 0x4109,
   lxBlockLocal      = 0x410A,
   lxConstantString  = 0x410B,
   lxConstantWideStr = 0x410C,
   lxConstantChar    = 0x410D,
   lxConstantInt     = 0x410E,
   lxConstantLong    = 0x410F,
   lxConstantReal    = 0x4110,
   lxConstantClass   = 0x4111,
   lxMessageConstant = 0x4112,
   lxExtMessageConstant = 0x4113,
   lxSignatureConstant  = 0x4114,
   lxVerbConstant    = 0x4115,
   lxNil             = 0x4116,
   lxCurrent         = 0x4117,
   lxResult          = 0x4118,
   lxResultField     = 0x4119,
   lxCurrentMessage  = 0x411A,

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
   //lxStacksafe       = 0x803,
   lxTempLocal       = 0x804,
   lxOverridden      = 0x805,
   //lxIfValue         = 0x806,
   //lxElseValue       = 0x807,
   lxMessage         = 0x808,
   //lxEmbeddable      = 0x809,
   lxSize            = 0x80A,
   lxReserved        = 0x80B,
   lxParamCount      = 0x80C,

   lxBreakpoint      = 0x2001,
   lxCol             = 0x2002,
   lxRow             = 0x2003,
   lxLength          = 0x2004,
   lxTerminal        = 0x2005,
   lxLevel           = 0x2006,
   lxClassName       = 0x2007,
   //lxValue           = 0x2008,

   lxClassFlag       = 0x4001,      // class fields
   lxClassStructure  = 0x4002,      
   lxClassArray      = 0x4003,
   lxClassExtension  = 0x4004,
   lxClassField      = 0x4005,
   lxClassMethod     = 0x4006,
   lxClassMethodAttr = 0x4007,
   lxClassMethodOpt  = 0x4008,
};

// --- SyntaxWriter ---

class SyntaxWriter
{
   MemoryWriter  _writer;
   Stack<size_t> _bookmarks;

public:
   int setBookmark(size_t position)
   {
      _bookmarks.push(position);
      return _bookmarks.Count();
   }

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

   void insert(int bookmark, LexicalType type, ref_t argument);
   void insert(int bookmark, LexicalType type)
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
   void insertChild(int bookmark, LexicalType type, ref_t argument)
   {
      insert(bookmark, lxEnding, 0);
      insert(bookmark, type, argument);
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
   struct NodePattern;

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

      SyntaxTree* Tree()
      {
         return tree;
      }

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

      void setArgument(ref_t argument)
      {
         this->argument = argument;

         int prevPos = tree->_reader.Position();

         tree->_reader.seek(position - 4);
         *(int*)(tree->_reader.Address()) = (int)argument;

         tree->_reader.seek(prevPos);
      }

      Node firstChild() const
      {
         if (tree != NULL) {
            return tree->readFirstNode(position);
         }
         else return Node();
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

      void insertNode(LexicalType type, int argument = 0)
      {
         tree->insertNode(position, type, argument);
      }

      void appendNode(LexicalType type, int argument = 0)
      {
         Node lastNode = lastChild();

         tree->insertNode(lastNode.position + 8, type, argument);
      }

      Node findPattern(NodePattern pattern)
      {
         return tree->findPattern(*this, 1, pattern);
      }

      Node()
      {
         type = lxNone;
         argument = 0;

         tree = NULL;
      }
   };

   struct NodePattern
   {
      LexicalType type;
      LexicalType alt_type1;

      bool match(Node node)
      {
         return node.type == type || node.type == alt_type1;
      }

      NodePattern()
      {
         type = lxNone;
         alt_type1 = lxInvalid;
      }
      NodePattern(LexicalType type)
      {
         this->type = type;
         this->alt_type1 = lxInvalid;
      }
      NodePattern(LexicalType type1, LexicalType type2)
      {
         this->type = type1;
         this->alt_type1 = type2;
      }
   };

private:
   _Memory*     _dump;
   MemoryReader _reader;

   Node read();

public:
   static int countChild(Node node, LexicalType type)
   {
      int counter = 0;
      Node current = node.firstChild();

      while (current != lxNone) {
         if (current == type)
            counter++;

         current = current.nextNode();
      }

      return counter;
   }

   static Node findChild(Node node, LexicalType type)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type) {
         current = current.nextNode();
      }

      return current;
   }

   static Node findMatchedChild(Node node, int mask)
   {
      Node current = node.firstChild();

      while (current != lxNone && !test(current.type, mask)) {
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

   static bool existChild(Node node, LexicalType type1, LexicalType type2)
   {
      Node child = findChild(node, type1, type2);

      return child != lxNone;
   }

   Node readRoot();
   Node readFirstNode(size_t position);
   Node readNextNode(size_t position);
   Node readPreviousNode(size_t position);

   Node insertNode(size_t position, LexicalType type, int argument);

   bool matchPattern(Node node, int mask, int counter, ...);
   Node findPattern(Node node, int counter, ...);

   SyntaxTree(_Memory* dump)
      : _reader(dump)
   {
      _dump = dump;
   }
};

} // _ELENA_

#endif // syntaxTreeH
