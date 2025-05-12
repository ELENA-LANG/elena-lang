//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Syntax Tree class declaration
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include "elena.h"
#include "tree.h"

namespace elena_lang
{
   enum class SyntaxKey
   {
      // masks
      ScopeMask               = 0x000800,
      MemberMask              = 0x000400,
      DeclarationMask         = 0x001000,
      TerminalMask            = 0x002000,
      ErrorMask               = 0x004000,

      None                    = 0x000000,
      Root                    = 0x000001,

      eof                     = 0x002003,
      identifier              = 0x003004,
      integer                 = 0x003005,
      string                  = 0x003006,
      hexinteger              = 0x003007,
      reference               = 0x003008,
      eop                     = 0x003009,
      character               = 0x00300A,
      globalreference         = 0x00300B,
      wide                    = 0x00300C,
      constant                = 0x00300D,
      longinteger             = 0x00300E,
      real                    = 0x00300F,
      interpolate             = 0x003010,

      // NOTE : low word should be unique for every key
      Declaration             = 0x001400,

      Class                   = 0x001001,
      Symbol                  = 0x001002,
      Parent                  = 0x001003,
      Method                  = 0x001004,
      StaticMethod            = 0x001005,
      Template                = 0x001006,
      InlineTemplate          = 0x001007,
      TemplateCode            = 0x001008,
      Constructor             = 0x001009,
      Parameter               = 0x00100A,
      EOP                     = 0x00100B,
      Field                   = 0x00100C,
      Import                  = 0x00100D,
      PredefinedMethod        = 0x00100E,
      InlinePropertyTemplate  = 0x00100F,
      CodeBlock               = 0x001C10,
      WithoutBody             = 0x001C11,
      Importing               = 0x001C12,
      InlineTemplateExpr      = 0x001013,
      MetaDictionary          = 0x001020,
      MetaExpression          = 0x001021,
      IncludeStatement        = 0x001022,
      ImportStatement         = 0x001023,
      CondStatement           = 0x001024,
      SharedMetaDictionary    = 0x001025,
      EndCondStatement        = 0x001026,
      ElseCondStatement       = 0x001027,
      Object                  = 0x001031,
      TemplateType            = 0x001032,
      ArrayType               = 0x001033,
      ReturnExpression        = 0x001C34,
      NestedExpression        = 0x001835,
      GetExpression           = 0x001C36,
      InitExpression          = 0x001C37,
      ExtensionTemplate       = 0x001038,
      AccumExpression         = 0x001C39,
      NullableType            = 0x00103A,
      AsyncOperation          = 0x00183B,
      IndexerOperation        = 0x001841,
      AssignOperation         = 0x001842,
      AddAssignOperation      = 0x001843,
      AddOperation            = 0x001844,
      SubOperation            = 0x001845,
      LenOperation            = 0x001846,
      IfOperation             = 0x001847,
      LessOperation           = 0x001848,
      NameOperation           = 0x001849,
      EqualOperation          = 0x00184A,
      NotOperation            = 0x00184B,
      NotEqualOperation       = 0x00184C,
      LoopOperation           = 0x00184D,
      IfNotOperation          = 0x00184E,
      IfElseOperation         = 0x00184F,
      MulOperation            = 0x001850,
      DivOperation            = 0x001851,
      NotLessOperation        = 0x001852,
      GreaterOperation        = 0x001853,
      NotGreaterOperation     = 0x001854,
      ExternOperation         = 0x001855,
      NegateOperation         = 0x001856,
      ValueOperation          = 0x001857,
      BAndOperation           = 0x001858,
      BOrOperation            = 0x001859,
      BXorOperation           = 0x00185A,
      BNotOperation           = 0x00185B,
      ShlOperation            = 0x00185C,
      ShrOperation            = 0x00185D,
      SubAssignOperation      = 0x00185E,
      MulAssignOperation      = 0x00185F,
      DivAssignOperation      = 0x001860,
      AndOperation            = 0x001861,
      OrOperation             = 0x001862,
      XorOperation            = 0x001863,
      BreakOperation          = 0x001864,
      LazyOperation           = 0x001865,
      TupleAssignOperation    = 0x001866,
      ContinueOperation       = 0x001867,
      YieldOperation          = 0x001868,
      ExprValOperation        = 0x001869,

      Postfix                 = 0x001069,

      TemplatePostfix         = 0x00106A,
      ReferOperation          = 0x00186B,
      IncOperation            = 0x00186C,
      DecOperation            = 0x00186D,
      SizeOperation           = 0x00186E,
      EnumPostfix             = 0x00106F,
      TemplateArg             = 0x001070,
      Dimension               = 0x001471,
      NestedBlock             = 0x001080,
      ClosureBlock            = 0x001081,
      Expression              = 0x001890,
      L5Expression            = 0x001891,
      SubExpression           = 0x001892,
      L8Expression            = 0x001893,
      RootExpression          = 0x001894,
      L6Expression            = 0x001895,
      TExpression             = 0x001896,
      L4Expression            = 0x001897,
      NTExpression            = 0x001898,
      L7Expression            = 0x001899,
      L3SingleExpression      = 0x00189A,
      NestedRootExpression    = 0x00189B,
      TemplateOperation       = 0x00189C,
      L5SubExpression         = 0x00189D,
      InterpolExpression      = 0x00189E,
      L3Expression            = 0x00189F,

      TemplateExpression      = 0x0018A0,
      KeyValueExpression      = 0x0018A1,
      ClosureOperation        = 0x0018A2,
      Interpolation           = 0x0018A3,
      LTExpression            = 0x0018A4,
      FieldInitializer        = 0x0018B0,
      Message                 = 0x0010C0,
      MessageOperation        = 0x0018C1,
      ResendOperation         = 0x001CC2,
      RedirectOperation       = 0x001CC3,
      PropertyOperation       = 0x0018C4,
      RedirectDispatch        = 0x001CC5,
      ResendDispatch          = 0x001CC6,
      CatchOperation          = 0x001CC7,
      CatchDispatch           = 0x001CC8,
      RedirectTryDispatch     = 0x001CC9,
      DirectResend            = 0x001CCA,
      Redirect                = 0x001CCB,
      AltOperation            = 0x001CCC,
      IsNilOperation          = 0x001CCD,
      ComplexName             = 0x0010CE,
      InlinePostfix           = 0x0010CF,
      SwitchOperation         = 0x001CD0,
      SwitchOption            = 0x0018D1,
      SwitchLastOption        = 0x0018D2,
      SwitchCode              = 0x0018D3,
      CollectionExpression    = 0x0018D4,
      TupleCollection         = 0x0018D5,
      Operator                = 0x0010D6,
      SubVariable             = 0x0010D7,
      SubDeclaration          = 0x0010D8,
      FinallyBlock            = 0x001CD9,
      FinalOperation          = 0x001CDA,
      ParameterBlock          = 0x0010F0,
      StaticInitializerMethod = 0x0010F1,
      PrimitiveCollection     = 0x0018F2,
      BranchOperation         = 0x0018F3,
      TupleBlock              = 0x0010F4,
      TupleType               = 0x0010F5,

      ErrClosingBlockExpected = 0x04002,               // closing curly bracket expected

      Name                    = 0x000101,
      Namespace               = 0x000103,
      SourcePath              = 0x000104,
      Attribute               = 0x000105,
      Hints                   = 0x000106,
      Type                    = 0x000107,
      TemplateArgParameter    = 0x000108,
      NameParameter           = 0x000109,
      TemplateParameter       = 0x00010A,
      Autogenerated           = 0x00010B,
      Multimethod             = 0x00010C,
      OutputInfo              = 0x00010D,
      Target                  = 0x00010E,
      ByRefRetMethod          = 0x00010F,
      NameArgParameter        = 0x000110,
      FillingAttr             = 0x000111,
      ProxyDispatcher         = 0x000112,
      EnumNameArgParameter    = 0x000113,
      EnumArgParameter        = 0x000114,
      NillableInfo            = 0x000115,
      HasStaticConstructor    = 0x000116,
      AsyncInvoker            = 0x000117,
      SourceRef               = 0x000118,
      Shortcut                = 0x000119,

      Column                  = 0x000201,
      Row                     = 0x000202,

      ExternalTree            = 0x000301,
      ExternalFunction        = 0x000302,

      Idle                    = 0x000F01,
   };

   typedef Tree<SyntaxKey, SyntaxKey::None>::Writer      SyntaxTreeWriter;
   typedef Tree<SyntaxKey, SyntaxKey::None>::Node        SyntaxNode;

   typedef Map<ustr_t, SyntaxKey, allocUStr, freeUStr>   TokenMap;

   // --- SyntaxTree ---
   class SyntaxTree : public Tree<SyntaxKey, SyntaxKey::None>
   {
   public:
      static void loadTokens(TokenMap& map);

      bool save(MemoryBase* section);
      void load(MemoryBase* section);

      static bool testSuperKey(SyntaxKey key, SyntaxKey superKey)
      {
         return ((parse_key_t)key & 0xFFFFFFF0) == (parse_key_t)superKey;
      }

      static bool test(SyntaxKey key, SyntaxKey mask)
      {
         return elena_lang::test((parse_key_t)key, (parse_key_t)mask);
      }

      static parse_key_t toParseKey(SyntaxKey key)
      {
         return (parse_key_t)key;
      }

      static SyntaxKey fromParseKey(parse_key_t key)
      {
         return (SyntaxKey)key;
      }

      static bool compareNodes(SyntaxNode n1, SyntaxNode n2)
      {
         if (n1.key != n2.key)
            return false;

         if (n1.arg.strArgPosition != INVALID_POS) {
            if (n2.arg.strArgPosition == INVALID_POS || !n1.identifier().compare(n2.identifier()))
               return false;
         }
         else if (n2.arg.strArgPosition != INVALID_POS) {
            return false;
         }

         return n1.arg.reference == n2.arg.reference;
      }

      static void copyNewNode(SyntaxTreeWriter& writer, SyntaxNode node)
      {
         if (node.arg.strArgPosition != INVALID_POS) {
            if (writer.getOwner()->isTreeNode(node)) {
               IdentifierString tmp(node.identifier());
               writer.newNode(node.key, *tmp);
            }
            else writer.newNode(node.key, node.identifier());
         }
         else writer.newNode(node.key, node.arg.reference);
      }

      static void injectNode(SyntaxTreeWriter& writer, SyntaxNode node)
      {
         if (node.arg.strArgPosition != INVALID_POS) {
            writer.inject(node.key, node.identifier());
         }
         else writer.inject(node.key, node.arg.reference);

         copyNode(writer, node);

         writer.closeNode();
      }

      static void copyNode(SyntaxTreeWriter& writer, SyntaxNode node, bool includingNode = false);
      static void copyNodeSafe(SyntaxTreeWriter& writer, SyntaxNode node, bool includingNode = false);
      static void saveNode(SyntaxNode node, MemoryBase* section, bool includingNode = false);

      static bool compare(SyntaxNode n1, SyntaxNode n2, bool onlyChildren)
      {
         if (onlyChildren || compareNodes(n1, n2)) {
            SyntaxNode c1 = n1.firstChild();
            SyntaxNode c2 = n2.firstChild();
            while (c1.key != SyntaxKey::None) {
               if (!compare(c1, c2, false))
                  return false;

               c1 = c1.nextNode();
               c2 = c2.nextNode();
            }

            return c2.key == SyntaxKey::None;
         }

         return false;
      }
   };
}
#endif
