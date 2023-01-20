//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains Syntax Tree class declaration
//
//                                             (C)2021-2023, by Aleksey Rakov
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
      MetaDictionary          = 0x001020,
      MetaExpression          = 0x001021,
      IncludeStatement        = 0x001022,
      ReloadStatement         = 0x001023,
      Object                  = 0x001031,
      TemplateType            = 0x001032,
      ArrayType               = 0x001033,
      ReturnExpression        = 0x001C34,
      NestedExpression        = 0x001835,
      GetExpression           = 0x001C36,
      InitExpression          = 0x001C37,
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
      Postfix                 = 0x001068,
      TemplatePostfix         = 0x001069,
      //MethodPostfix           = 0x00106A,
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
      TemplateExpression      = 0x0018A0,
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
      OutputType              = 0x00010D,
      Target                  = 0x00010E,
      ByRefRetMethod          = 0x00010F,
      NameArgParameter        = 0x000110,

      Column                  = 0x000201,
      Row                     = 0x000202,

      Idle                    = 0x000F01,
   };

   typedef Tree<SyntaxKey, SyntaxKey::None>::Writer SyntaxTreeWriter;
   typedef Tree<SyntaxKey, SyntaxKey::None>::Node   SyntaxNode;

   // --- SyntaxTree ---
   class SyntaxTree : public Tree<SyntaxKey, SyntaxKey::None>
   {
   public:
      void save(MemoryBase* section);
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

      static void copyNewNode(SyntaxTreeWriter& writer, SyntaxNode node)
      {
         if (node.arg.strArgPosition != INVALID_POS) {
            writer.newNode(node.key, node.identifier());
         }
         else writer.newNode(node.key, node.arg.reference);
      }

      static void copyNode(SyntaxTreeWriter& writer, SyntaxNode node, bool includingNode = false);
      static void saveNode(SyntaxNode node, MemoryBase* section, bool includingNode = false);
   };

   // --- BuildTreeWriter ---

}

#endif
