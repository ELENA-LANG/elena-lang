//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Parser Symbol constants
//
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef syntaxH
#define syntaxH 1

//#pragma warning(disable : 4458)

namespace _ELENA_
{

// --- ELENA Parser Symbol constants ---
enum Symbol
{
   mskAnySymbolMask             = 0x0F000,               // masks
   mskTraceble                  = 0x01000,
   mskTerminal                  = 0x02000,
   mskError                     = 0x04000,
   //mskScope                     = 0x08000,

   nsNone                       = 0x00000,               // defaults
   nsStart                      = 0x00001,
   nsEps                        = 0x00002,

   tsEof                        = 0x03003,               // terminals
//   tsLiteral                    = 0x03004,
//   tsIdentifier                 = 0x03005,
//   tsPrivate                    = 0x03006,
//   tsReference                  = 0x03007,
//   tsInteger                    = 0x03008,
//   tsHexInteger                 = 0x03009,
//   tsReal                       = 0x0300A,
//   tsCharacter                  = 0x0300B,
//   tsLong                       = 0x0300C,
//   tsWide                       = 0x0300D,
//   tsExplicitConst              = 0x0300E,
//   tsAttribute                  = 0x0300F,
//   tsMember                     = 0x03010,
//   tsGlobal                     = 0x03011,
//
////   nsClass                      = 0x0100E,               // non-terminals
////   nsTemplate                   = 0x0100F,
//   nsSubjectArg                 = 0x01010,
////   nsSymbol                     = 0x01011,
//   nsExpression                 = 0x01012,
////   nsField                      = 0x01013,
//////   nsHint                       = 0x01014,
//   nsTokenParam                 = 0x01015,
////   nsMethodOpenParameter        = 0x01016,
//   nsMethodParameter            = 0x01017,
//   nsNestedClass                = 0x01018,
//   nsObject                     = 0x01019,
//   nsSubCode                    = 0x0101A,
//   nsMessageOperation           = 0x0101B,
//   nsMessageParameter           = 0x0101C,
//   nsCodeEnd                    = 0x0101D,
////   nsOpenMessageParameter       = 0x0101E,
//   nsL2Operation                = 0x0101F,
//   nsDispatchExpression         = 0x01020,
//   nsAssigning                  = 0x01021,
//////   nsStatic                     = 0x01022,
//   nsBaseClass                  = 0x01023,
//   nsL1Operation                = 0x01025,
//   nsL5Operation                = 0x01026,
//   nsRetStatement               = 0x01027,
//   nsL3Operation                = 0x01028,
//////   nsElseOperation              = 0x0102A,
//   nsExtension                  = 0x0102B,
//   nsAltMessageOperation        = 0x0102C,
//////   nsInclude                    = 0x0102D,
//   nsCatchMessageOperation      = 0x0102F,
//   nsResendExpression           = 0x01031,
//////   nsInlineExpression           = 0x01032,
//   nsMessageReference           = 0x01033,
//////   nsDispatchHandler            = 0x01036,
//   nsRootExpression             = 0x01038,
//////   nsNewOperator                = 0x0103A,
//   nsSwitching                  = 0x0103B,
//   nsSwitchOption               = 0x0103C,
//   nsLastSwitchOption           = 0x0103D,
////   nsBiggerSwitchOption         = 0x0103E,
////   nsLessSwitchOption           = 0x0103F,
//   nsL4Operation                = 0x01041,
//   nsSizeValue                  = 0x01042,
////   nsL0Operation                = 0x01043,
////   nsSubject                    = 0x01047,
////   nsInlineClosure              = 0x01048,
//////   nsImplicitConstructor        = 0x0104B,
//   nsToken                      = 0x0104C,
//   nsScope                      = 0x0104D,
//   nsAttribute                  = 0x0104E,
////   nsRootMessage                = 0x0104F,
//   nsLazyExpression             = 0x01050,
//   nsAngleOperator              = 0x01051,
////   nsAngleObject                = 0x01052,
//   nsArrayOperation             = 0x01053,
////   nsRootAngleOperator          = 0x01054,
////   nsRootAngleObject            = 0x01055,
////   nsRootL6Operation            = 0x01056,
//   nsXInlineClosure             = 0x01057, 
////   nsSizeExpression             = 0x01058,
////   nsArrayIdentifier            = 0x01059,
//   nsNestedSubCode              = 0x0105A,
//   nsNestedRootExpression       = 0x0105B,
//   nsL8Operation                = 0x0105C,
//   nsIdleMessageParameter       = 0x0105D,
//   nsExprMessageParameter       = 0x0105E,
//   nsReferenceExpression        = 0x0105F,
//   nsL0Operator                 = 0x01060,
//   nsL1Operator                 = 0x01061,
//   nsL2Operator                 = 0x01062,
//   nsL3Operator                 = 0x01063,
//   nsL4Operator                 = 0x01064,
//   nsClosingOperator            = 0x01065,
//
//   nsDeclarationEndExpected         = 0x04000,               // error-terminals
//   nsStatementEndExpected           = 0x04001,               
//   nsErrClosingSBracketExpected     = 0x04002,               // closing square bracket expected
////   nsErrNestedMemberExpected        = 0x04003,               
////   nsErrObjectExpected              = 0x04004,
////   nsErrMessageExpected             = 0x04005,
//   nsDirectiveEndExpected           = 0x04006,
//   nsInlineExpressionEndExpected    = 0x04007,
};

// --- TerminalInfo structure ---
struct TerminalInfo
{
   Symbol   symbol;

   size_t   disp;          // number of symbols (tab considered as a single char)
   size_t   row;
   size_t   col;           // virtual column
   size_t   length;
   ident_t  value;

   operator ident_t() const { return value; }

   bool operator == (TerminalInfo& terminal) const
   {
      return (symbol == terminal.symbol && value.compare(terminal.value));
   }

   bool operator != (TerminalInfo& terminal) const
   {
      return (symbol != terminal.symbol || !value.compare(terminal.value));
   }

   bool operator == (const Symbol& symbol) const
   {
      return (this->symbol == symbol);
   }

   bool operator != (const Symbol& symbol) const
   {
      return (this->symbol != symbol);
   }

   int Row() const { return row; }

   int Col() const { return col; }

   TerminalInfo()
   {
      this->symbol = nsNone;
      this->value = NULL;
   }
};

// --- _DerivationWriter ---

class _DerivationWriter
{
public:
   virtual void writeSymbol(Symbol symbol) = 0;
   virtual void writeTerminal(TerminalInfo& terminal) = 0;
};

////inline bool ifAny(Symbol target, Symbol value1, Symbol value2)
////{
////   return target == value1 || target == value2;
////}

} // _ELENA_

#endif // syntaxH
