//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Parser Symbol constants
//
//                                              (C)2005-2019, by Alexei Rakov
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
   tsLiteral                    = 0x03004,
   tsIdentifier                 = 0x03005,
   tsReference                  = 0x03007,
   tsInteger                    = 0x03008,
   tsHexInteger                 = 0x03009,
   tsReal                       = 0x0300A,
   tsCharacter                  = 0x0300B,
   tsLong                       = 0x0300C,
   tsWide                       = 0x0300D,
   tsExplicitConst              = 0x0300E,
   tsAttribute                  = 0x0300F,
//   tsMember                     = 0x03010,
   tsGlobal                     = 0x03011,

   nsToken                      = 0x01010,               // non-terminals
   nsRootExpression             = 0x01011,
   nsExpression                 = 0x01012,
   nsScope                      = 0x01013,
   nsMethodParameter            = 0x01017,
   nsNestedClass                = 0x01018,
   nsSubCode                    = 0x0101A,
   nsMessageOperation           = 0x0101B,
   nsCodeEnd                    = 0x0101D,
   nsDispatchExpression         = 0x01020,
   nsAssignOperator             = 0x01021,
   nsBase                       = 0x01023,
   nsResendExpression           = 0x01031,
   nsSwitching                  = 0x0103B,
   nsSwitchOption               = 0x0103C,
   nsLastSwitchOption           = 0x0103D,
   nsAttribute                  = 0x0104E,
   nsL1Operator                 = 0x01061,
   nsL2Operator                 = 0x01062,
   nsL3Operator                 = 0x01063,
   nsL5Operator                 = 0x01064,
   nsRetExpression              = 0x01066,
   nsImplicitMessage            = 0x01067,
   nsSize                       = 0x01068,
   nsDynamicSize                = 0x01069,
   nsOperandExpression          = 0x0106A,
   nsEmptyParameter             = 0x0106B,
   nsArrayOperator              = 0x0106C,
   nsSubExpression              = 0x0106D,
   nsClosure                    = 0x0106E,
   nsSingleExpression           = 0x0106F,
   nsSubSingleExpression        = 0x01070,
   nsL6Operator                 = 0x01071,
   nsL6Operand                  = 0x01072,
   nsL3Operand                  = 0x01073,
   nsL4Operator                 = 0x01074,
   nsCollection                 = 0x01075,
   nsL2Operand                  = 0x01076,
   nsFieldInit                  = 0x01077,
   nsL4Operand                  = 0x01078,
   nsAssignmentOperand          = 0x01079,
   nsL0Operator                 = 0x0107A,
   nsNestedStatements           = 0x0107B,
   nsNestedStatement            = 0x0107C,
   nsSubMessage                 = 0x0107D,
   nsL6bOperator                = 0x0107E,
   nsL1Operand                  = 0x01082,

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
