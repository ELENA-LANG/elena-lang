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
   lxObjectMask      = 0x00100,
   lxExpressionMask  = 0x00200,
   lxPrimitiveOpMask = 0x01000,
   lxSimpleMask      = 0x04000,   // idicates if the implementation does not affect base / other registers
   lxMessageMask     = 0x10000,
   lxReferenceMask   = 0x20000,
   lxSubjectMask     = 0x40000,

   lxEnding          = -1,

   lxNone            = 0x00000,
   lxInvalid         = 0x00001,
   lxRoot            = 0x00002,

   lxExpression      = 0x00301,
   lxBoxing          = 0x00302,   // boxing of the argument, arg - size
   lxCondBoxing      = 0x00303,   // conditional boxing, arg - size
   lxUnboxing        = 0x00304,   // boxing and unboxing of the argument, arg - size
   lxArgBoxing       = 0x00305,   // argument list boxing, arg - size
   lxTypecasting     = 0x10306,   // typecasting, arg - message
   lxCalling         = 0x10307,   // sending a message, arg - message
   lxDirectCalling   = 0x10308,   // calling a method, arg - message
   lxSDirctCalling   = 0x10309,   // calling a virtual method, arg - message
   lxResending       = 0x1030A,   // resending a message, optional arg - message
   lxDispatching     = 0x1030B,   // dispatching a message, optional arg - message
   lxTrying          = 0x0030C,   // try-catch expression
   lxAlt             = 0x0030D,   // alt-catch expression
   lxLocking         = 0x0030E,   // lock expression
   lxBranching       = 0x0030F,   // branch expression
   lxSwitching       = 0x00310,
   lxLooping         = 0x00311,
   lxReturning       = 0x00312,
   lxThrowing        = 0x00313,
   lxStdExternalCall = 0x20314,  // calling an external function, arg - reference
   lxExternalCall    = 0x20315,  // calling an external function, arg - reference
   lxCoreAPICall     = 0x20316,  // calling an external function, arg - reference
   lxIntExtArgument  = 0x00317,
   lxExtArgument     = 0x00318,
   lxExtInteranlRef  = 0x00319,
   lxInternalCall    = 0x2031A,  // calling an internal function, arg - reference
   lxMember          = 0x0031B,  // a collection member, arg - offset
   lxAssigning       = 0x0031C,  // an assigning expression, arg - size
   lxArgUnboxing     = 0x0031D,
   lxIf              = 0x2031E,  // optional arg - reference
   lxElse            = 0x2031F,  // optional arg - reference
   lxOption          = 0x00320,
   //lxBody            = 0x00321,
   lxLocalUnboxing   = 0x00322, // arg - size
   lxNewFrame        = 0x00323, // if argument -1 - than with presaved message
   lxCreatingClass   = 0x00324, // arg - count
   lxCreatingStruct  = 0x00325, // arg - size
   lxExternFrame     = 0x00326,
   //lxTemplateCalling = 0x10327,

   //lxBoolOp          = 0x00328, // arg - operation id
   //lxNilOp           = 0x00329, // arg - operation id
   //lxIntOp           = 0x0132A, // arg - operation id
   //lxLongOp          = 0x0132B, // arg - operation id
   //lxRealOp          = 0x0132C, // arg - operation id
   //lxIntArrOp        = 0x0132D, // arg - operation id
   //lxArrOp           = 0x0132E, // arg - operation id

   //lxTemplAssigning  = 0x0032F, // arg - size

   lxNested          = 0x00101, // arg - count
   lxStruct          = 0x00102, // arg - count
   lxSymbol          = 0x20103, // arg - reference
   lxConstantSymbol  = 0x24104, // arg - reference
   lxField           = 0x00105, // arg - offset
   lxFieldAddress    = 0x00106, // arg - offset
   lxLocalAddress    = 0x04107, // arg - offset
   lxBlockLocalAddr  = 0x04108, // arg - offset
   lxLocal           = 0x04109, // arg - offset
   lxBoxableLocal    = 0x0410A, // arg - offset
   lxBlockLocal      = 0x0410B, // arg - offset
   lxConstantString  = 0x2410C, // arg - reference
   lxConstantWideStr = 0x2410D, // arg - reference
   lxConstantChar    = 0x2410E, // arg - reference
   lxConstantInt     = 0x2410F, // arg - reference
   lxConstantLong    = 0x24110, // arg - reference
   lxConstantReal    = 0x24111, // arg - reference
   lxConstantClass   = 0x24112, // arg - reference
   lxMessageConstant = 0x24113, // arg - rererence
   lxExtMessageConstant = 0x24114, // arg -reference
   lxSignatureConstant  = 0x24115, // arg - reference
   lxVerbConstant    = 0x24116, // arg - reference
   lxNil             = 0x04117,
   lxCurrent         = 0x04118, // arg -offset
   lxResult          = 0x04119, // arg -offset
   lxResultField     = 0x0411A, // arg -offset
   lxCurrentMessage  = 0x0411B,

   lxVariable        = 0x00407, // debug info only if lxFrameAttr is included
   lxIntVariable     = 0x00408,
   lxLongVariable    = 0x00409,
   lxReal64Variable  = 0x0040A,
   lxBytesVariable   = 0x0040B,
   lxShortsVariable  = 0x0040C,
   lxIntsVariable    = 0x0040D,
   lxBinaryVariable  = 0x0040E,
   lxParamsVariable  = 0x0040F,
   lxMessageVariable = 0x00410, // debug info only
   lxSelfVariable    = 0x00411, // debug info only
   lxReleasing       = 0x00412,
   lxImporting       = 0x00413,
   lxTemplateTarget  = 0x00414, // template target pseudo variable

   lxTarget          = 0x20801, // arg - reference
   lxCallTarget      = 0x20802, // arg - reference
   lxType            = 0x40803, // arg - subject
   //lxSubject         = 0x40804, // arg - subject
   //lxStacksafe       = 0x00805,
   lxTempLocal       = 0x00806,
   lxOverridden      = 0x00807,
   //lxIfValue         = 0x20808, // arg - reference
   //lxElseValue       = 0x20809, // arg - reference
   lxMessage         = 0x1080A, // arg - message
   //lxEmbeddable      = 0x0080B,
   //lxSize            = 0x0080C,
   lxReserved        = 0x0080D,
   lxParamCount      = 0x0080E,

   lxBreakpoint      = 0x02001,
   lxCol             = 0x02002,
   lxRow             = 0x02003,
   lxLength          = 0x02004,
   lxTerminal        = 0x02005,
   lxLevel           = 0x02006,
   lxClassName       = 0x02007, // arg - reference
   lxValue           = 0x02008,
   lxFrameAttr       = 0x02009,
   lxSourcePath      = 0x0200A,

   lxClassFlag       = 0x04001,      // class fields
   lxClassStructure  = 0x04002,      
   //lxClassArray      = 0x04003,
   //lxClassField      = 0x04005,
   lxClassMethod     = 0x14006,
   lxClassMethodAttr = 0x04007,
   //lxClassMethodOpt  = 0x04008,
   lxWarningMask     = 0x04009,

   lxTemplate        = 0x2400A,
   //lxFieldTemplate   = 0x2400A,
   //lxMethodTemplate  = 0x2400B,
};

// --- SyntaxTree ---

class SyntaxTree
{
   MemoryDump _body;
   MemoryDump _strings;

public:
   // --- SyntaxWriter ---

   class Writer
   {
      MemoryWriter  _writer;
      MemoryWriter  _stringWriter;
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
      void insertChild(int start_bookmark, int end_bookmark, LexicalType type, ref_t argument)
      {
         insert(end_bookmark, lxEnding, 0);
         insert(start_bookmark, type, argument);
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
      void newNode(LexicalType type, ident_t argument)
      {
         newNode(type, _stringWriter.Position());
         _stringWriter.writeLiteral(argument);
      }
      void newNode(LexicalType type)
      {
         newNode(type, 0u);
      }
      void appendNode(LexicalType type, ref_t argument)
      {
         newNode(type, argument);
         closeNode();
      }
      void appendNode(LexicalType type, ident_t argument)
      {
         appendNode(type, _stringWriter.Position());
         _stringWriter.writeLiteral(argument);
      }
      void appendNode(LexicalType type)
      {
         newNode(type);
         closeNode();
      }

      void closeNode();

      Writer(SyntaxTree& tree)
         : _writer(&tree._body), _stringWriter(&tree._strings)
      {
      }
   };

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

      ident_t identifier()
      {
         if (type != lxNone) {
            return (ident_t)tree->_strings.get(argument);
         }
         else NULL;
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

         MemoryReader reader(&tree->_body, position - 8);

         *(int*)(reader.Address()) = (int)type;
      }

      void setArgument(ref_t argument)
      {
         this->argument = argument;

         MemoryReader reader(&tree->_body, position - 4);
         *(int*)(reader.Address()) = (int)argument;
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

      Node parentNode() const
      {
         return tree->readParentNode(position);
      }

      void insertNode(LexicalType type, int argument = 0)
      {
         tree->insertNode(position, type, argument);
      }

      void appendNode(LexicalType type, int argument = 0)
      {
         int end_position = tree->seekNodeEnd(position);

         tree->insertNode(end_position, type, argument);
      }

      void injectNode(LexicalType type, int argument = 0)
      {
         int start_position = position;
         int end_position = tree->seekNodeEnd(position);
         
         tree->insertNode(start_position, end_position, type, argument);
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
   Node read(StreamReader& reader);

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

   static Node findChild(Node node, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type1) {
         if (current == type2 || current == type3 || current == type4)
            return current;

         current = current.nextNode();
      }

      return current;
   }

   static Node findChild(Node node, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5)
   {
      Node current = node.firstChild();

      while (current != lxNone && current != type1) {
         if (current == type2 || current == type3 || current == type4 || current == type5)
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

   _Memory* Strings() { return &_strings; }

   Node readRoot();
   Node readFirstNode(size_t position);
   Node readNextNode(size_t position);
   Node readPreviousNode(size_t position);
   Node readParentNode(size_t position);

   size_t seekNodeEnd(size_t position);

   Node insertNode(size_t position, LexicalType type, int argument);
   Node insertNode(size_t start_position, size_t end_position, LexicalType type, int argument);

   bool matchPattern(Node node, int mask, int counter, ...);
   Node findPattern(Node node, int counter, ...);

   ref_t writeString(ident_t string)
   {
      MemoryWriter writer(&_strings);
      ref_t position = writer.Position();

      writer.writeLiteral(string);

      return position;
   }

   void save(_Memory* section)
   {
      MemoryWriter writer(section);

      writer.writeDWord(_body.Length());
      writer.write(_body.get(0), _body.Length());

      writer.writeDWord(_strings.Length());
      writer.write(_strings.get(0), _strings.Length());
   }

   SyntaxTree()
   {
   }
   SyntaxTree(_Memory* dump)
   {
      MemoryReader reader(dump);

      _body.load(&reader, reader.getDWord());
      _strings.load(&reader, reader.getDWord());
   }
};

SyntaxTree::Node findSubNode(SyntaxTree::Node node, LexicalType type);
SyntaxTree::Node findSubNodeMask(SyntaxTree::Node node, int mask);

typedef SyntaxTree::Writer       SyntaxWriter;
typedef SyntaxTree::Node         SNode;
typedef SyntaxTree::NodePattern  SNodePattern;


} // _ELENA_

#endif // syntaxTreeH
