//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Syntax Tree classes
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef syntaxTreeH
#define syntaxTreeH 1

//#pragma warning(disable : 4458)

namespace _ELENA_
{

// --- SyntaxType ---

enum LexicalType
{
   lxSimpleMask      = 0x02000,
   lxCodeScopeMask   = 0x04000,
   lxObjectMask      = 0x08000,
   lxExprMask        = 0x0C000,
   lxTerminalMask    = 0x10000,
   lxTerminalObjMask = 0x18000,
//   lxReferenceMask   = 0x40000,
   lxPrimitiveOpMask = 0x80000,

   lxEnding          = -1,
   lxInvalid         = -2,
   lxNone            = 0x00000,

   // scopes
   lxRoot            = 0x00001,
   lxIdle            = 0x00002,
   lxObject          = 0x00003,
   lxAttributeDecl   = 0x00004,
   lxClass           = 0x0000E,
   lxTemplate        = 0x0000F,
   lxSymbol          = 0x00011,
   lxClassField      = 0x00013,
   lxFieldTemplate   = 0x00014,
   lxAttributeValue  = 0x00015,
   lxClassMethod     = 0x00016,
   lxNestedClass     = 0x00018,
   lxCode            = 0x0001A,
   lxDispatchCode    = 0x00020,
   lxStatic          = 0x00022,
   lxConstructor     = 0x00024,
   lxExtension       = 0x0002B,
   lxAltOperation    = 0x0002C,
   lxCatchOperation  = 0x0002F,
   lxLoop            = 0x00030,
//   lxInlineExpression= 0x00032,
   lxMessageReference= 0x00033,
   lxExtern          = 0x00039,
   lxSwitchOption    = 0x0003C,
   lxLastSwitchOption = 0x0003D,
   lxBiggerSwitchOption = 0x0003E,
   lxLessSwitchOption = 0x0003F,
   lxLazyExpression  = 0x00040,

   lxDefaultGeneric  = 0x00046,
   lxSubject         = 0x00047,     // !! obsolete??
//   lxImplicitConstructor = 0x0004B,
   lxScope           = 0x0004D,
   lxMessageParameter= 0x0C04E,

   // parameters
   lxEOF             = 0x18003, // indicating closing code bracket
   lxLiteral         = 0x18004,
   lxIdentifier      = 0x18005,
   lxPrivate         = 0x18006,
   lxReference       = 0x18007,
   lxInteger         = 0x18008,
   lxHexInteger      = 0x18009,
   lxReal            = 0x1800A,
   lxCharacter       = 0x1800B,
   lxLong            = 0x1800C,
   lxWide            = 0x1800D,
   lxExplicitConst   = 0x1800E,
   lxExplicitAttr    = 0x1800F,

   lxImporting       = 0x08101,
   lxNested          = 0x08102, // arg - count
   lxStruct          = 0x08103, // arg - count
   lxConstantSymbol  = 0x0A104, // arg - reference
   lxField           = 0x08105, // arg - offset
   lxStaticField     = 0x08106, // arg - reference
   lxSymbolReference = 0x08107,
   lxLocalAddress    = 0x0A108, // arg - offset
   lxFieldAddress    = 0x08109, // arg - offset
   lxLocal           = 0x0A10A, // arg - offset
   lxBlockLocal      = 0x0A10B, // arg - offset
   lxConstantString  = 0x0A10C, // arg - reference
   lxConstantWideStr = 0x0A10D, // arg - reference
   lxConstantChar    = 0x0A10E, // arg - reference
   lxConstantInt     = 0x1A10F, // arg - reference
   lxConstantLong    = 0x1A110, // arg - reference
   lxConstantReal    = 0x1A111, // arg - reference
   lxConstantClass   = 0x0A112, // arg - reference
   lxMessageConstant = 0x0A113, // arg - rererence
   lxExtMessageConstant = 0x0A114, // arg -reference
   lxSignatureConstant  = 0x0A115, // arg - reference
   lxVerbConstant    = 0x0A116, // arg - reference
   lxNil             = 0x0A117,
   lxCurrent         = 0x0A118, // arg -offset
   lxResult          = 0x0A119, // arg -offset
   lxResultField     = 0x0A11A, // arg -offset
   lxCurrentMessage  = 0x0A11B,
   lxThisLocal       = 0x0A11C,
   lxConstantList    = 0x0A11E, // arg - reference
   lxBlockLocalAddr  = 0x0A11F, // arg - offset

   lxCondBoxing      = 0x0C001,   // conditional boxing, arg - size
   lxBoxing          = 0x0C002,   // boxing of the argument, arg - size
   lxLocalUnboxing   = 0x0C003,   // arg - size
   lxUnboxing        = 0x0C004,   // boxing and unboxing of the argument, arg - size
   lxArgBoxing       = 0x0C005,   // argument list boxing, arg - size
   lxArgUnboxing     = 0x0C006,
   lxCalling         = 0x0C007,   // sending a message, arg - message
   lxDirectCalling   = 0x0C008,   // calling a method, arg - message
   lxSDirctCalling   = 0x0C009,   // calling a virtual method, arg - message
   lxResending       = 0x0C00A,   // resending a message, optional arg - message / -1 (if follow-up operation is available)
   lxTrying          = 0x0C00C,   // try-catch expression
   lxAlt             = 0x0C00D,   // alt-catch expression
   lxBranching       = 0x0C00F,   // branch expression
   lxSwitching       = 0x0C010,
   lxLooping         = 0x0C011,
   lxExpression      = 0x0C012,
   lxThrowing        = 0x0C013,
   lxStdExternalCall = 0x0C014,   // calling an external function, arg - reference
   lxExternalCall    = 0x0C015,   // calling an external function, arg - reference
   lxCoreAPICall     = 0x0C016,   // calling an external function, arg - reference
   lxMethodParameter = 0x0C017,
   lxAltExpression   = 0x0C018,
   lxIfNot           = 0x0C019,   // optional arg - reference
   lxInternalCall    = 0x0C01A,   // calling an internal function, arg - reference
   lxIf              = 0x0C01F,   // optional arg - reference
   lxElse            = 0x0C020,   // optional arg - reference
   lxOption          = 0x0C021,
   lxFieldExpression = 0x0C022,
   lxExternFrame     = 0x04023,
   lxNewFrame        = 0x04024,   // if argument -1 - than with presaved message
   lxCreatingClass   = 0x0C025,   // arg - count
   lxCreatingStruct  = 0x0C026,   // arg - size
   lxReturning       = 0x0C027,
   lxNewOp           = 0x0C028,
   lxArrOp           = 0x8C029, // arg - operation id
   lxBinArrOp        = 0x8C02A, // arg - operation id
   lxArgArrOp        = 0x8C02B, // arg - operation id
   lxNilOp           = 0x8C02C, // arg - operation id
   lxIntArrOp        = 0x8C030,   // arg - operation id
   lxResendExpression= 0x0C031,
   lxByteArrOp       = 0x8C032, // arg - operation id
   lxShortArrOp      = 0x8C033, // arg - operation id
//   lxReleasing       = 0x0C034,
   lxDispatching     = 0x0C036,   // dispatching a message, optional arg - message
   lxAssigning       = 0x0C037,   // an assigning expression, arg - size
   lxIntOp           = 0x8C038,   // arg - operation id
   lxLongOp          = 0x8C039,   // arg - operation id
   lxRealOp          = 0x8C03A,   // arg - operation id
   lxOverridden      = 0x04047,

   lxBaseParent      = 0x10023,
   lxOperator        = 0x10025,
   lxIntVariable     = 0x10028,
   lxLongVariable    = 0x10029,
   lxReal64Variable  = 0x1002A,
   lxForward         = 0x1002E,
   lxVariable        = 0x10037,
   lxBinaryVariable  = 0x10038,
   lxMember          = 0x10039,  // a collection member, arg - offset
   lxOuterMember     = 0x1003A,  // a collection member, arg - offset
   lxIntsVariable    = 0x1003B,
   lxBytesVariable   = 0x1003C,
   lxShortsVariable  = 0x1003D,
   lxParamsVariable  = 0x1003E,

   // attributes
   lxAttribute       = 0x20000,
   lxSourcePath      = 0x20001,
   lxTerminal        = 0x20002,
   lxCol             = 0x20003,
   lxRow             = 0x20004,
   lxLength          = 0x02005,
   lxBreakpoint      = 0x20006,
   lxImport          = 0x20007,
   lxReserved        = 0x20008,
   lxAllocated       = 0x20009,
   lxParamCount      = 0x2000A,
   lxClassFlag       = 0x2000B, // class fields
   lxTarget          = 0x2000C, // arg - reference
   lxMessageVariable = 0x2000D, // debug info only
   lxSelfVariable    = 0x2000E, // debug info only
   lxMessage         = 0x2000F, // arg - message
   lxAssign          = 0x20010,
   lxLevel           = 0x20011,
   lxType            = 0x20012, // arg - subject
   lxCallTarget      = 0x20013, // arg - reference
   lxClassName       = 0x20014, // arg - identifier
   lxIntValue        = 0x20015, // arg - integer value
   lxTempLocal       = 0x20016,
   lxIfValue         = 0x20017, // arg - reference
   lxElseValue       = 0x20018, // arg - reference
   lxSize            = 0x20019,
   lxTemplateParam   = 0x2001A,
   lxEmbeddable      = 0x2001B,
   lxIntExtArgument  = 0x2001C,
   lxExtArgument     = 0x2001D,
   lxExtInteranlRef  = 0x2001E,
//   lxConstAttr       = 0x2001F,
//   lxWarningMask     = 0x20020,
   lxOperatorAttr    = 0x20021,
//   lxNotFoundAttr    = 0x20022,
   lxBinarySelf      = 0x20023, // debug info only
   lxOvreriddenMessage = 0x20024, // arg - message ; used for extension / implicit constructor call
//   lxClassRef        = 0x20025,
//   lxPreloadedAttr   = 0x20026,
   lxInclude         = 0x20027,
   lxTemplateField   = 0x20028,
   lxNameAttr        = 0x20029,
   lxTypeAttr        = 0x2002A,
   lxStacksafeAttr   = 0x2002B,
   lxTemplateType    = 0x2002C,
   lxEmbeddableAttr  = 0x2002D,
   lxBoxableAttr     = 0x2002E,
   lxClassRefAttr    = 0x2002F,
   lxClosureMessage  = 0x20030,
   lxExtArgumentType = 0x20031,
   lxInternalRef     = 0x20032,
   lxTemplateVar     = 0x20033,

   lxTempAttr        = 0x2010D,
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
      MemoryWriter  _bodyWriter;
      MemoryWriter  _stringWriter;
      Stack<size_t> _bookmarks;

   public:
      bool hasBookmarks() const
      {
         return _bookmarks.Count() != 0;
      }

      int setBookmark(size_t position)
      {
         _bookmarks.push(position);
         return _bookmarks.Count();
      }

      int newBookmark()
      {
         _bookmarks.push(_bodyWriter.Position());

         return _bookmarks.Count();
      }

      void removeBookmark()
      {
         _bookmarks.pop();
      }

      void clear()
      {
         _bodyWriter.seek(0);
         _stringWriter.seek(0);
         _bookmarks.clear();
      }

//      void clear(int bookmark)
//      {
//         size_t position = (bookmark == 0) ? _bookmarks.peek() : *_bookmarks.get(_bookmarks.Count() - bookmark);
//
//         _writer.seek(position);
//         _bookmarks.clear();
//      }

      void set(int bookmark, LexicalType type);

      void insert(int bookmark, LexicalType type, ref_t argument);
      void insert(int bookmark, LexicalType type, ident_t argument);
//      void insert(int bookmark, LexicalType type)
//      {
//         insert(type, 0);
//      }
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
      void insertChildren(int bookmark, LexicalType type, ref_t argument, LexicalType subType, ref_t subArgument)
      {
         insert(bookmark, lxEnding, 0);
         insert(bookmark, lxEnding, 0);
         insert(bookmark, subType, subArgument);
         insert(bookmark, type, argument);
      }
      void insertChild(int bookmark, LexicalType type, ref_t argument)
      {
         insert(bookmark, lxEnding, 0);
         insert(bookmark, type, argument);
      }
      void insertChild(int bookmark, LexicalType type, ident_t argument)
      {
         insert(bookmark, lxEnding, 0);
         insert(bookmark, type, argument);
      }
//      void insertChild(LexicalType type, ref_t argument)
//      {
//         insert(lxEnding, 0);
//         insert(type, argument);
//      }

      void newNode(LexicalType type, ref_t argument);
      void newNode(LexicalType type, int argument)
      {
         newNode(type, (ref_t)argument);
      }
      void newNode(LexicalType type, ident_t argument);
      void newNode(LexicalType type)
      {
         newNode(type, 0u);
      }
      void appendNode(LexicalType type, ref_t argument)
      {
         newNode(type, argument);
         closeNode();
      }
      void appendNode(LexicalType type, int argument)
      {
         newNode(type, argument);
         closeNode();
      }
      void appendNode(LexicalType type, ident_t argument)
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

      Writer(SyntaxTree& tree)
         : _bodyWriter(&tree._body), _stringWriter(&tree._strings)
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

      Node(SyntaxTree* tree, size_t position, LexicalType type, ref_t argument, int strArgument);

      Node appendStrNode(LexicalType nodeType, int strOffset)
      {
         int end_position = tree->seekNodeEnd(position);

         return tree->insertStrNode(end_position, nodeType, strOffset);
      }

   public:
      LexicalType   type;
      ref_t         argument;
      int           strArgument;   // if strArgument is not -1 - it contains the position of the argument string

//      SyntaxTree* Tree()
//      {
//         return tree;
//      }

      ident_t identifier()
      {
         if (strArgument >= 0) {
            return (const char*)(tree->_strings.get(strArgument));
         }
         else return NULL;
      }

      operator LexicalType() const { return type; }

      bool operator == (LexicalType operand)
      {
         return this->type == operand;
      }
      bool operator != (LexicalType operand)
      {
         return this->type != operand;
      }

      void operator = (LexicalType operand)
      {
         this->type = operand;

         MemoryReader reader(&tree->_body, position - 12);

         *(int*)(reader.Address()) = (int)operand;
      }

      void set(LexicalType type, ref_t argument)
      {
         (*this) = type;
         setArgument(argument);
      }

      void setArgument(ref_t argument)
      {
         this->argument = argument;

         MemoryReader reader(&tree->_body, position - 8);
         *(int*)(reader.Address()) = (int)argument;
      }

      void setArgument(int argument)
      {
         setArgument((ref_t)argument);
      }

      Node firstChild() const
      {
         if (tree != NULL) {
            return tree->readFirstNode(position);
         }
         else return Node();
      }

      Node firstChild(LexicalType mask) const
      {
         Node current = firstChild();

         while (current != lxNone && !test(current.type, mask))
            current = current.nextNode();

         return current;
      }

      Node findNext(LexicalType mask) const
      {
         Node current = *this;

         while (current != lxNone && !test(current.type, mask))
            current = current.nextNode();

         return current;
      }

      Node findSubNodeMask(LexicalType mask)
      {
         Node child = firstChild(mask);
         if (child == lxExpression) {
            return child.findSubNodeMask(mask);
         }
         else return child;
      }

      Node findSubNode(LexicalType type)
      {
         Node current = firstChild();
         while (current != lxNone && current.type != type) {
            if (current == lxExpression) {
               Node subNode = current.findSubNode(type);
               if (subNode != lxNone)
                  return subNode;
            }
            current = current.nextNode();
         }

         return current;
      }
      Node findSubNode(LexicalType type1, LexicalType type2)
      {
         Node child = firstChild();
         while (child != lxNone && child.type != type1) {
            if (child == lxExpression) {
               Node subNode = child.findSubNode(type1, type2);
               if (subNode != lxNone)
                  return subNode;
            }
            else if (child == type2)
               break;

            child = child.nextNode();
         }

         return child;
      }
      Node findSubNode(LexicalType type1, LexicalType type2, LexicalType type3)
      {
         Node child = firstChild();
         while (child != lxNone && child.type != type1) {
            if (child == lxExpression) {
               Node subNode = child.findSubNode(type1, type2, type3);
               if (subNode != lxNone)
                  return subNode;
            }
            else if (child == type2)
               break;
            else if (child == type3)
               break;

            child = child.nextNode();
         }

         return child;
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
         if (tree != NULL) {
            return tree->readNextNode(position);
         }
         else return Node();
      }

      Node nextNode(LexicalType mask) const
      {
         Node current = nextNode();

         while (current != lxNone && !test(current.type, mask))
            current = current.nextNode();

         return current;
      }

      Node prevNode() const
      {
         return tree->readPreviousNode(position);
      }

      Node parentNode() const
      {
         return tree->readParentNode(position);
      }

      Node insertNode(LexicalType type, int argument = 0)
      {
         return tree->insertNode(position, type, argument);
      }

      Node insertNode(LexicalType type, ident_t argument)
      {
         return tree->insertNode(position, type, argument);
      }

      Node appendNode(LexicalType type, int argument = 0)
      {
         int end_position = tree->seekNodeEnd(position);

         return tree->insertNode(end_position, type, argument);
      }
      Node appendNode(LexicalType type, ident_t argument)
      {
         int end_position = tree->seekNodeEnd(position);

         return tree->insertNode(end_position, type, argument);
      }

      Node injectNode(LexicalType type, int argument = 0)
      {
         int start_position = position;
         int end_position = tree->seekNodeEnd(position);
         
         return tree->insertNode(start_position, end_position, type, argument);
      }

      void refresh()
      {
         tree->refresh(*this);
      }

      Node findPattern(NodePattern pattern)
      {
         return tree->findPattern(*this, 1, pattern);
      }

      Node findChild(LexicalType type)
      {
         Node current = firstChild();

         while (current != lxNone && current != type) {
            current = current.nextNode();
         }

         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2)
      {
         Node current = firstChild();
      
         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
      
            current = current.nextNode();
         }
      
         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2, LexicalType type3)
      {
         Node current = firstChild();

         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
            else if (current == type3)
               return current;

            current = current.nextNode();
         }

         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4)
      {
         Node current = firstChild();

         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
            else if (current == type3)
               return current;
            else if (current == type4)
               return current;

            current = current.nextNode();
         }

         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5)
      {
         Node current = firstChild();

         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
            else if (current == type3)
               return current;
            else if (current == type4)
               return current;
            else if (current == type5)
               return current;

            current = current.nextNode();
         }

         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6)
      {
         Node current = firstChild();

         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
            else if (current == type3)
               return current;
            else if (current == type4)
               return current;
            else if (current == type5)
               return current;
            else if (current == type6)
               return current;

            current = current.nextNode();
         }

         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6, LexicalType type7)
      {
         Node current = firstChild();

         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
            else if (current == type3)
               return current;
            else if (current == type4)
               return current;
            else if (current == type5)
               return current;
            else if (current == type6)
               return current;
            else if (current == type7)
               return current;

            current = current.nextNode();
         }

         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6, LexicalType type7, LexicalType type8)
      {
         Node current = firstChild();

         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
            else if (current == type3)
               return current;
            else if (current == type4)
               return current;
            else if (current == type5)
               return current;
            else if (current == type6)
               return current;
            else if (current == type7)
               return current;
            else if (current == type8)
               return current;

            current = current.nextNode();
         }

         return current;
      }
      Node findChild(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6, LexicalType type7, LexicalType type8, LexicalType type9)
      {
         Node current = firstChild();

         while (current != lxNone && current != type1) {
            if (current == type2)
               return current;
            else if (current == type3)
               return current;
            else if (current == type4)
               return current;
            else if (current == type5)
               return current;
            else if (current == type6)
               return current;
            else if (current == type7)
               return current;
            else if (current == type8)
               return current;
            else if (current == type9)
               return current;

            current = current.nextNode();
         }

         return current;
      }

      bool existChild(LexicalType type)
      {
         return findChild(type) == type;
      }
      bool existChild(LexicalType type1, LexicalType type2)
      {
         return findChild(type1, type2) != lxNone;
      }

      Node()
      {
         type = lxNone;
         argument = 0;
         strArgument = -1;

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
   void refresh(Node& node);

public:
   static void moveNodes(Writer& writer, SyntaxTree& buffer);
   static void moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type);
   static void moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2);
   static void moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3);
   static void moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4);
   static void moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5);
   static void moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6);
   static void moveNodes(Writer& writer, SyntaxTree& buffer, LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4, LexicalType type5, LexicalType type6, LexicalType type7);
   static void copyNode(Writer& writer, LexicalType type, Node owner);
   static void copyNode(Writer& writer, Node node);
   static void copyNode(Node source, Node destination);
   static void copyNodeSafe(Node source, Node destination, bool inclusingNode = false);
   static void saveNode(Node node, _Memory* dump, bool inclusingNode = false);
   static void loadNode(Node node, _Memory* dump);

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

   static int countChild(Node node, LexicalType type1, LexicalType type2)
   {
      int counter = 0;
      Node current = node.firstChild();

      while (current != lxNone) {
         if (current == type1 || current == type2)
            counter++;

         current = current.nextNode();
      }

      return counter;
   }

   static Node findPattern(Node node, int counter, ...);
   static bool matchPattern(Node node, int mask, int counter, ...);

   Node readRoot();
   Node readFirstNode(size_t position);
   Node readNextNode(size_t position);
   Node readPreviousNode(size_t position);
   Node readParentNode(size_t position);

   size_t seekNodeEnd(size_t position);

   Node insertNode(size_t position, LexicalType type, int argument);
   Node insertStrNode(size_t position, LexicalType type, int strArgument);
   Node insertNode(size_t position, LexicalType type, ident_t argument);
   Node insertNode(size_t start_position, size_t end_position, LexicalType type, int argument);

   bool save(_Memory* section)
   {
      MemoryWriter writer(section);

      writer.writeDWord(_body.Length());
      writer.write(_body.get(0), _body.Length());

      writer.writeDWord(_strings.Length());
      writer.write(_strings.get(0), _strings.Length());

      return _body.Length() > 0;
   }

   void load(_Memory* section)
   {
      _body.clear();
      _strings.clear();

      MemoryReader reader(section);
      int bodyLength = reader.getDWord();
      _body.load(&reader, bodyLength);

      int stringLength = reader.getDWord();
      _strings.load(&reader, stringLength);
   }

   void clear()
   {
      _body.clear();
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

inline bool isSingleStatement(SyntaxTree::Node expr)
{
   return expr.findSubNode(lxMessage, lxAssign, lxOperator) == lxNone;
}

typedef SyntaxTree::Writer       SyntaxWriter;
typedef SyntaxTree::Node         SNode;
typedef SyntaxTree::NodePattern  SNodePattern;

} // _ELENA_

#endif // syntaxTreeH
