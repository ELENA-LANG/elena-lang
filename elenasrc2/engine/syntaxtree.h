//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Syntax Tree classes
//
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef syntaxTreeH
#define syntaxTreeH 1

namespace _ELENA_
{

// --- SyntaxType ---

enum LexicalType
{
   lxSimpleMask               = 0x002000,
   lxCodeScopeMask            = 0x004000,
   lxObjectMask               = 0x008000,
   lxExprMask                 = 0x00C000,
   lxTerminalMask             = 0x010000,
   lxSubOpMask                = 0x100000,
   lxPrimitiveOpMask          = 0x080000,

   lxInvalid                  = -2,
   lxNone                     = 0x000000,

   // scopes
   lxRoot                     = 0x000001,
   lxIdle                     = 0x000002,
   lxNamespace                = 0x000003,
   lxTemplate                 = 0x00000F,
   lxToken                    = 0x000010,
   lxSymbol                   = 0x000011,
   lxExpression               = 0x00C012,
   lxScope                    = 0x000013,
   lxClass                    = 0x000014,
   lxClassMethod              = 0x000016,
   lxParameter                = 0x000017,
   lxNestedClass              = 0x000018,
   lxCode                     = 0x00001A,
   lxMessage                  = 0x00001B, // arg - message
   lxDispatchCode             = 0x000020,
   lxAssign                   = 0x000021,
   lxParent                   = 0x000023,
   lxConstructor              = 0x000024,
   lxStaticMethod             = 0x000025,
   lxSwitchOption             = 0x00003C,
   lxWrapping                 = 0x00002B,
   lxLastSwitchOption         = 0x00003D,
   lxAttributeDecl            = 0x00004E,
   lxClassField               = 0x00004F,
   lxImplicitMessage          = 0x000067,
   lxSizeDecl                 = 0x000068,
   lxDynamicSizeDecl          = 0x000069,
   lxPropertyParam            = 0x00006B,
   lxClosureExpr              = 0x00006E,
   lxFieldInit                = 0x000077,
   lxSubMessage               = 0x00007D,
   lxInlineClosure            = 0x00007F,
   lxAttrExpression           = 0x000080,
   lxTemplateOperator         = 0x000081,
   lxFieldAccum               = 0x000083,

   lxTypecast                 = 0x000100,
   lxClassProperty            = 0x000101,

   // parameters
   lxEOF                      = 0x018003, // indicating closing code bracket
   lxLiteral                  = 0x018004,
   lxIdentifier               = 0x018005,
   lxReference                = 0x018007,
   lxInteger                  = 0x018008,
   lxHexInteger               = 0x018009,
   lxReal                     = 0x01800A,
   lxCharacter                = 0x01800B,
   lxLong                     = 0x01800C,
   lxWide                     = 0x01800D,
   lxExplicitConst            = 0x01800E,
   lxExplicitAttr             = 0x01800F,
   lxGlobalReference          = 0x018011,

   lxImporting                = 0x008101,
   lxNested                   = 0x008102, // arg - count
   lxStruct                   = 0x008103, // arg - count
   lxConstantSymbol           = 0x00A104, // arg - reference
   lxField                    = 0x008105, // arg - offset
   lxStaticField              = 0x008106, // arg - reference   // - lxClassStaticField
   lxSymbolReference          = 0x008107,
   lxLocalAddress             = 0x00A108, // arg - offset
   lxFieldAddress             = 0x008109, // arg - offset
   lxLocal                    = 0x00A10A, // arg - offset
   lxBlockLocal               = 0x00A10B, // arg - offset
   lxConstantString           = 0x00A10C, // arg - reference
   lxConstantWideStr          = 0x00A10D, // arg - reference
   lxConstantChar             = 0x00A10E, // arg - reference
   lxConstantInt              = 0x01A10F, // arg - reference
   lxConstantLong             = 0x01A110, // arg - reference
   lxConstantReal             = 0x01A111, // arg - reference
   lxClassSymbol              = 0x00A112, // arg - reference
   lxMessageConstant          = 0x00A113, // arg - rererence
   lxExtMessageConstant       = 0x00A114, // arg -reference
   lxSubjectConstant          = 0x00A115, // arg - reference
   lxStaticConstField         = 0x008116, // arg - reference
   lxNil                      = 0x00A117,
   lxCurrent                  = 0x00A118, // arg -offset
   lxResult                   = 0x00A119, // arg -offset
   lxResultField              = 0x00A11A, // arg -offset
   lxCurrentMessage           = 0x00A11B,
   lxSelfLocal                = 0x00A11C,
   lxConstantList             = 0x00A11E,   // arg - reference
   lxBlockLocalAddr           = 0x00A11F,   // arg - offset
   lxClassRefField            = 0x008120,   // arg - self instance offset
   lxBaseField                = 0x00A122,

   lxCondBoxing               = 0x00C001,   // conditional boxing, arg - size
   lxBoxing                   = 0x00C002,   // boxing of the argument, arg - size
   lxLocalUnboxing            = 0x00C003,   // arg - size
   lxUnboxing                 = 0x00C004,   // boxing and unboxing of the argument, arg - size
   lxArgBoxing                = 0x00C005,   // argument list boxing, arg - size
   lxArgUnboxing              = 0x00C006,
   lxCalling                  = 0x10C007,   // sending a message, arg - message
   lxDirectCalling            = 0x10C008,   // calling a method, arg - message
   lxSDirctCalling            = 0x10C009,   // calling a virtual method, arg - message
   lxResending                = 0x00C00A,   // resending a message, optional arg - message / -1 (if follow-up operation is available)
   lxImplicitCall             = 0x00C00B,
   lxTrying                   = 0x00C00C,   // try-catch expression
   lxAlt                      = 0x00C00D,   // alt-catch expression
   lxImplicitJump             = 0x00C00E,
   lxBranching                = 0x00C00F,   // branch expression      
   lxSwitching                = 0x00C010,
   lxLooping                  = 0x00C011,
   lxStdExternalCall          = 0x00C014,   // calling an external function, arg - reference
   lxExternalCall             = 0x00C015,   // calling an external function, arg - reference
   lxCoreAPICall              = 0x00C016,   // calling an external function, arg - reference
   lxMethodParameter          = 0x00C017,
   lxAltExpression            = 0x00C018,
   lxIfNot                    = 0x00C019,   // optional arg - reference
   lxInternalCall             = 0x00C01A,   // calling an internal function, arg - reference
   lxIfN                      = 0x00C01B,   // arg - value
   lxIfNotN                   = 0x00C01C,   // arg - value
   lxLessN                    = 0x00C01D,   // arg - value
   lxNotLessN                 = 0x00C01E,   // arg - value
   lxIf                       = 0x00C01F,   // optional arg - reference
   lxElse                     = 0x00C020,   // optional arg - reference
   lxOption                   = 0x00C021,
   lxFieldExpression          = 0x00C022,
   lxExternFrame              = 0x004023,
   lxNewFrame                 = 0x004024,   // if argument -1 - than with presaved message
   lxCreatingClass            = 0x00C025,   // arg - count
   lxCreatingStruct           = 0x00C026,   // arg - size
   lxReturning                = 0x00C027,
   lxNewArrOp                 = 0x00C028,
   lxArrOp                    = 0x08C029,   // arg - operation id
   lxBinArrOp                 = 0x08C02A,   // arg - operation id
   lxArgArrOp                 = 0x08C02B,   // arg - operation id
   lxNilOp                    = 0x08C02C,   // arg - operation id
   lxBoolOp                   = 0x00C02D,   // arg - operation id

   lxGreaterN                 = 0x00C02E,   // arg - value
   lxNotGreaterN              = 0x00C02F,   // arg - value

   lxIntArrOp                 = 0x08C030,   // arg - operation id
   lxResendExpression         = 0x00C031, 
   lxByteArrOp                = 0x08C032, // arg - operation id
   lxShortArrOp               = 0x08C033, // arg - operation id
   lxDispatching              = 0x00C036,   // dispatching a message, optional arg - message
   lxAssigning                = 0x10C037,   // an assigning expression, arg - size
   lxIntOp                    = 0x18C038,   // arg - operation id
   lxLongOp                   = 0x18C039,   // arg - operation id
   lxRealOp                   = 0x18C03A,   // arg - operation id
   lxMultiDispatching         = 0x00C03B,
   lxSealedMultiDispatching   = 0x00C03C,
   lxCodeExpression           = 0x00C03D,
   lxCollection               = 0x00C03E,
   lxOverridden               = 0x004047,
   lxFinally                  = 0x004048,

   lxOperator                 = 0x10025,
   lxIntVariable              = 0x10028,
   lxLongVariable             = 0x10029,
   lxReal64Variable           = 0x1002A,
   lxForward                  = 0x1002E,
   lxVariable                 = 0x10037,
   lxBinaryVariable           = 0x10038,
   lxMember                   = 0x10039,  // a collection member, arg - offset
   lxOuterMember              = 0x1003A,  // a collection member, arg - offset
   lxIntsVariable             = 0x1003B,
   lxBytesVariable            = 0x1003C,
   lxShortsVariable           = 0x1003D,
   lxParamsVariable           = 0x1003E,

   // attributes
   lxAttribute                = 0x20000,
   lxSourcePath               = 0x20001,
   lxCol                      = 0x20003,
   lxRow                      = 0x20004,
   lxLength                   = 0x02005,
   lxBreakpoint               = 0x20006,
   lxNameAttr                 = 0x20029,
   lxImport                   = 0x20007,
   lxReserved                 = 0x20008,
   lxAllocated                = 0x20009,
   lxParamCount               = 0x2000A,
   lxClassFlag                = 0x2000B, // class fields
   lxTarget                   = 0x2000C, // arg - reference
   lxMessageVariable          = 0x2000D, // debug info only
   lxSelfVariable             = 0x2000E, // debug info only
   lxLevel                    = 0x20011,
   lxTypeAttribute            = 0x20012,
   lxCallTarget               = 0x20013, // arg - reference
   lxClassName                = 0x20014, // arg - identifier
   lxIntValue                 = 0x20015, // arg - integer value
   lxTempLocal                = 0x20016,
   lxIfValue                  = 0x20017, // arg - reference
   lxElseValue                = 0x20018, // arg - reference
   lxSize                     = 0x20019,
   lxTemplateParam            = 0x2001A,
   lxEmbeddable               = 0x2001B,
   lxIntExtArgument           = 0x2001C,
   lxExtArgument              = 0x2001D,
   lxExtInteranlRef           = 0x2001E,
   lxBinarySelf               = 0x20023, // debug info only
   lxOvreriddenMessage        = 0x20024, // arg - message ; used for extension / implicit constructor call
   lxInclude                  = 0x20027,
   lxTemplateField            = 0x20028,
   lxStacksafeAttr            = 0x2002B,
   lxEmbeddableAttr           = 0x2002D,
   lxBoxableAttr              = 0x2002E,
   lxExtArgumentRef           = 0x20031,
   lxInternalRef              = 0x20032,
   lxEmbeddableMssg           = 0x20034,
   lxBoxingRequired           = 0x20035,
   lxAutogenerated            = 0x20038,
   lxStaticAttr               = 0x2003B,
   lxAutoMultimethod          = 0x2003F,
   lxElement                  = 0x20043,
   lxTypecasting              = 0x20044,
   lxIntConversion            = 0x20045,
   lxTemplateNameParam        = 0x20046,
   lxTemplateMsgParam         = 0x20047,
   lxTemplateIdentParam       = 0x20048,
   lxByRefTarget              = 0x20049,
   lxArrayType                = 0x2004A,
   lxDimensionAttr            = 0x2004B,
   lxCheckLocal               = 0x2004C,

   lxTempAttr                 = 0x2010D,
   lxSubOpMode                = 0x2010E
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
      friend class SyntaxTree;

      SyntaxTree*  _syntaxTree;
      Stack<pos_t> _bookmarks;
      pos_t        _current;
      int          _pendingBookmarks;

      void insertPendingBookmarks(pos_t position)
      {
         while (_pendingBookmarks) {
            _bookmarks.push(position);
            _pendingBookmarks--;
         }
      }

      void inject(pos_t position, LexicalType type, ref_t argument, pos_t strArgRef);
      void insert(LexicalType type, ref_t argument, pos_t strArgRef, bool newMode);

   public:
      bool hasBookmarks() const
      {
         return _bookmarks.Count() != 0;
      }

      int newBookmark()
      {
         _pendingBookmarks++;

         return _bookmarks.Count() + _pendingBookmarks;
      }

      void trim();

      void removeBookmark()
      {
         if (_pendingBookmarks > 0) {
            _pendingBookmarks--;
         }
         else _bookmarks.pop();         
      }

      void clear()
      {
         _syntaxTree->clear();
         _bookmarks.clear();
         _current = INVALID_REF;
         _pendingBookmarks = 0;
      }

      void inject(LexicalType type, ident_t argument)
      {
         inject(_bookmarks.peek(), type, 0, _syntaxTree->saveStrArgument(argument));
      }
      void inject(LexicalType type, ref_t argument)
      {
         inject(_bookmarks.peek(), type, argument, INVALID_REF);
      }
      void inject(LexicalType type)
      {
         inject(_bookmarks.peek(), type, 0, INVALID_REF);
      }
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
      void insertNode(LexicalType type)
      {
         insert(type, 0, INVALID_REF, false);
      }
      void insertNode(LexicalType type, int argument)
      {
         insert(type, argument, INVALID_REF, false);
      }
      void newPrependedNode(LexicalType type, int argument)
      {
         insert(type, argument, INVALID_REF, true);
      }

      void closeNode();

      bool seekUp(LexicalType type);
      void findRoot()
      {
         if (_current == INVALID_REF && !_syntaxTree->isEmpty()) {
            _current = 0;
         }
      }

      Writer(SyntaxTree& tree)
      {
         _current = INVALID_REF;
         _syntaxTree = &tree;
         _pendingBookmarks = 0;
      }
      Writer(Writer& writer)
      {
         _current = writer._current;
         _syntaxTree = writer._syntaxTree;
         _pendingBookmarks = 0;
      }
   };

   struct NodePattern;

   // --- Node ---
   class Node
   {
      friend class SyntaxTree;

      SyntaxTree* tree;
      pos_t       position;

      Node(SyntaxTree* tree, pos_t position, LexicalType type, ref_t argument, int strArgument);

      //Node appendStrNode(LexicalType nodeType, int strOffset)
      //{
      //   int end_position = tree->seekNodeEnd(position);

      //   return tree->insertStrNode(end_position, nodeType, strOffset);
      //}

   public:
      LexicalType   type;
      ref_t         argument;
      pos_t         strArgument;   // if strArgument is not -1 - it contains the position of the argument string

//      SyntaxTree* Tree()
//      {
//         return tree;
//      }

      ident_t identifier()
      {
         if (strArgument != INVALID_REF) {
            return (const char*)(tree->_strings.get(strArgument));
         }
         else return nullptr;
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

      bool operator == (Node operand)
      {
         return this->position == operand.position && this->tree == operand.tree;
      }
      bool operator != (Node operand)
      {
         return this->position != operand.position || this->tree != operand.tree;
      }

      void operator = (LexicalType operand)
      {
         this->type = operand;

         MemoryReader reader(&tree->_body, position - 12);

         tree->save(*this);
      }

      void set(LexicalType type, ref_t argument)
      {
         this->type = type;
         this->argument = argument;

         tree->save(*this);
      }
      void set(LexicalType type, ident_t argument)
      {
         this->type = type;
         this->argument = 0;
         this->strArgument = tree->saveStrArgument(argument);

         tree->save(*this);
      }

      void setArgument(ref_t argument)
      {
         this->argument = argument;

         tree->save(*this);
      }
      void setStrArgument(ident_t argument)
      {
         this->strArgument = tree->saveStrArgument(argument);

         tree->save(*this);
      }

      void setArgument(int argument)
      {
         setArgument((ref_t)argument);
      }

      Node firstChild() const
      {
         if (tree != NULL) {
            return tree->read(tree->getChild(position));
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
         Node current = firstChild(mask);
         if (current == lxExpression) {
            return current.findSubNodeMask(mask);
         }
         else return current;
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
         Node current = firstChild();
         while (current != lxNone && current.type != type1) {
            if (current == lxExpression) {
               Node subNode = current.findSubNode(type1, type2);
               if (subNode != lxNone)
                  return subNode;
            }
            else if (current == type2)
               break;

            current = current.nextNode();
         }

         return current;
      }
      Node findSubNode(LexicalType type1, LexicalType type2, LexicalType type3)
      {
         Node current = firstChild();
         while (current != lxNone && current.type != type1) {
            if (current == lxExpression) {
               Node subNode = current.findSubNode(type1, type2, type3);
               if (subNode != lxNone)
                  return subNode;
            }
            else if (current == type2)
               break;
            else if (current == type3)
               break;

            current = current.nextNode();
         }

         return current;
      }

      Node findSubNode(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4)
      {
         Node child = firstChild();
         while (child != lxNone && child.type != type1) {
            if (child == lxExpression) {
               Node subNode = child.findSubNode(type1, type2, type3, type4);
               if (subNode != lxNone)
                  return subNode;
            }
            else if (child == type2)
               break;
            else if (child == type3)
               break;
            else if (child == type4)
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
            return tree->read(tree->getNext(position));
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

      Node nextSubNodeMask(LexicalType mask)
      {
         Node child = nextNode(mask);
         if (child == lxExpression) {
            return child.findSubNodeMask(mask);
         }
         else return child;
      }

      Node prevNode() const
      {
         return tree->read(tree->getPrevious(position));
      }

      Node prevNode(LexicalType mask) const
      {
         Node current = prevNode();

         while (current != lxNone && !test(current.type, mask))
            current = current.prevNode();

         return current;
      }

      Node lastNode() const
      {
         Node last = *this;
         Node current = nextNode();
         while (current != lxNone) {
            last = current;
            current = current.nextNode();
         }

         return last;
      }

      Node parentNode() const
      {
         return tree->read(tree->getParent(position));
      }

      Node insertNode(LexicalType type, int argument = 0)
      {
         return tree->read(tree->insertChild(position, type, argument, INVALID_REF));
      }

      Node insertNode(LexicalType type, ident_t argument)
      {
         return tree->read(tree->insertChild(position, type, 0, tree->saveStrArgument(argument)));
      }

      Node appendNode(LexicalType type, int argument = 0)
      {
         return tree->read(tree->appendChild(position, type, argument, INVALID_REF));
      }
      Node appendNode(LexicalType type, ident_t argument)
      {
         return tree->read(tree->appendChild(position, type, 0, tree->saveStrArgument(argument)));
      }

      Node injectNode(LexicalType type, int argument = 0)
      {
         pos_t child = tree->getChild(position);
         if (child != INVALID_REF) {
            return tree->read(tree->injectChild(child, type, argument, INVALID_REF));
         }
         else return appendNode(type, argument);
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
      bool existChild(LexicalType type1, LexicalType type2, LexicalType type3)
      {
         return findChild(type1, type2, type3) != lxNone;
      }
      bool existSubChild(LexicalType type1, LexicalType type2)
      {
         return findSubNode(type1, type2) != lxNone;
      }
      bool existSubChild(LexicalType type1)
      {
         return findSubNode(type1) != lxNone;
      }

      bool compare(LexicalType type1, LexicalType type2)
      {
         return (this->type == type1) || (this->type == type2);
      }
      bool compare(LexicalType type1, LexicalType type2, LexicalType type3)
      {
         return (this->type == type1) || (this->type == type2) || (this->type == type3);
      }
      bool compare(LexicalType type1, LexicalType type2, LexicalType type3, LexicalType type4)
      {
         return (this->type == type1) || (this->type == type2) || (this->type == type3) || (this->type == type4);
      }

      Node()
      {
         type = lxNone;
         position = argument = 0;
         strArgument = INVALID_REF;

         tree = NULL;
      }
   };

   struct NodePattern
   {
      LexicalType type;
      int         argument;
      int         patternId;

      bool match(Node node)
      {
         return node.type == type;
      }

      bool operator ==(NodePattern node) const
      {
         return (this->type == node.type);
      }

      bool operator !=(NodePattern node) const
      {
         return (this->type != node.type);
      }

      NodePattern()
      {
         type = lxNone;
         this->argument = 0;
         this->patternId = 0;
      }
      NodePattern(LexicalType type)
      {
         this->type = type;
         this->argument = 0;
         this->patternId = 0;
      }
      NodePattern(LexicalType type, int argument)
      {
         this->type = type;
         this->argument = argument;
         this->patternId = 0;
      }
      NodePattern(LexicalType type, int argument, int patternId)
      {
         this->type = type;
         this->argument = argument;
         this->patternId = patternId;
      }
   };

private:
   pos_t saveStrArgument(ident_t strArgument);

   pos_t newRoot(LexicalType type, int argument, pos_t strArgumentRef);
   pos_t appendChild(pos_t position, LexicalType type, int argument, pos_t strArgumentRef);  // append a child to the end
   pos_t insertChild(pos_t position, LexicalType type, int argument, pos_t strArgumentRef);  // insert a child at the start 
   pos_t insertSibling(pos_t position, LexicalType type, int argument, pos_t strArgumentRef);  // insert a child node between the current node and the node children
   pos_t injectChild(pos_t position, LexicalType type, int argument, pos_t strArgumentRef);  // insert a child node between the current node and the node children
   pos_t injectSibling(pos_t position, LexicalType type, int argument, pos_t strArgumentRef);  // insert a child node between the current node and the node children
   void clearSibling(pos_t position);
   void clearChildren(pos_t position);

   pos_t getParent(pos_t position);
   pos_t getChild(pos_t position);
   pos_t getNext(pos_t position);
   pos_t getPrevious(pos_t position);

   LexicalType getType(pos_t position);

   Node read(pos_t position);
   void save(Node& node);
   void refresh(Node& node);

public:
   static void moveNodes(Writer& writer, SyntaxTree& buffer);
   static void copyNode(Writer& writer, LexicalType type, Node owner);
   static void copyNode(Writer& writer, Node node);
   static void copyNode(Node source, Node destination);
   static void copyNodeSafe(Node source, Node destination, bool inclusingNode = false);
   static void saveNode(Node node, _Memory* dump, bool includingNode = false);
   static void loadNode(Node node, _Memory* dump);

   static int countNodeMask(Node current, LexicalType mask)
   {
      int counter = 0;
      while (current != lxNone) {
         if (test(current.type, mask))
            counter++;

         current = current.nextNode();
      }

      return counter;
   }

   static int countNode(Node current, LexicalType type)
   {
      int counter = 0;
      while (current != lxNone) {
         if (current == type)
            counter++;

         current = current.nextNode();
      }

      return counter;
   }

   static int countNode(Node current, LexicalType type1, LexicalType type2)
   {
      int counter = 0;
      while (current != lxNone) {
         if (current == type1 || current == type2)
            counter++;

         current = current.nextNode();
      }

      return counter;
   }

   static int countNode(Node current, LexicalType type1, LexicalType type2, LexicalType type3)
   {
      int counter = 0;
      while (current != lxNone) {
         if (current.compare(type1, type2, type3))
            counter++;

         current = current.nextNode();
      }

      return counter;
   }

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

   static int countChildMask(Node node, LexicalType mask)
   {
      int counter = 0;
      Node current = node.firstChild();

      while (current != lxNone) {
         if (test(current.type, mask))
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
   //static bool matchPattern(Node node, int mask, int counter, ...);

   static Node findTerminalInfo(Node node);

   //static bool apply(Node node, Trie<NodePattern>& trie);

   void createRoot(LexicalType type, int argument)
   {
      newRoot(type, argument, INVALID_REF);
   }
   Node readRoot();

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

   bool isEmpty()
   {
      return _body.Length() == 0;
   }

   SyntaxTree()
   {
   }
   SyntaxTree(pos_t size)
      : _body(size), _strings(size)
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
   return expr.findSubNode(lxMessage, lxAssign/*, lxOperator*/) == lxNone;
}

typedef SyntaxTree::Writer       SyntaxWriter;
typedef SyntaxTree::Node         SNode;
typedef SyntaxTree::NodePattern  SNodePattern;

typedef Trie<SNodePattern>           SyntaxTrie;
typedef MemoryTrieNode<SNodePattern> SyntaxTrieNode;

} // _ELENA_

#endif // syntaxTreeH
