//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Parser Symbol constants
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef syntaxH
#define syntaxH 1

namespace _ELENA_
{

// --- ELENA Parser Symbol constants ---
enum Symbol
{
   mskAnySymbolMask             = 0x07000,               // masks
   mskTraceble                  = 0x01000,
   mskTerminal                  = 0x02000,
   mskError                     = 0x04000,

   nsNone                       = 0x00000,               // defaults
   nsStart                      = 0x00001,
   nsEps                        = 0x00002,

   tsEof                        = 0x03003,               // terminals
   tsLiteral                    = 0x03004,
   tsIdentifier                 = 0x03005,
   tsPrivate                    = 0x03006,
   tsReference                  = 0x03007,
   tsInteger                    = 0x03008,
   tsHexInteger                 = 0x03009,
   tsReal                       = 0x0300A,
   tsCharacter                  = 0x0300B,
   tsLong                       = 0x0300C,

   nsClass                      = 0x0100E,               // non-terminals
   nsSubjectArg                 = 0x01010,
   nsSymbol                     = 0x01011,
   nsExpression                 = 0x01012,
   nsField                      = 0x01013,
   nsHint                       = 0x01014,
   nsHintValue                  = 0x01015,
   nsMethod                     = 0x01016,
   nsMethodParameter            = 0x01017,
   nsNestedClass                = 0x01018,
   nsObject                     = 0x01019,
   nsSubCode                    = 0x0101A,
   nsMessageOperation           = 0x0101B,
   nsMessageParameter           = 0x0101C,
   nsCodeEnd                    = 0x0101D,
   nsVariable                   = 0x0101E,
   nsL4Operation                = 0x0101F,
   nsDispatchExpression         = 0x01020,
   nsAssigning                  = 0x01021,
   nsStatic                     = 0x01022,
   nsBaseClass                  = 0x01023,
   nsConstructor                = 0x01024,
   nsL3Operation                = 0x01025,
   nsL7Operation                = 0x01026,
   nsRetStatement               = 0x01027,
   nsL5Operation                = 0x01028,
   nsTry                        = 0x01029,
   nsElseOperation              = 0x0102A,
   nsExtension                  = 0x0102B,
   nsAltMessageOperation        = 0x0102C,
   nsInclude                    = 0x0102D,
   nsForward                    = 0x0102E,
   nsCatchMessageOperation      = 0x0102F,
   nsLoop                       = 0x01030,
   nsResendExpression           = 0x01031,
   nsInlineExpression           = 0x01032,
   nsMessageReference           = 0x01033,
   nsThrow                      = 0x01034,
   nsImport                     = 0x01035,
   nsDispatchHandler            = 0x01036,
   nsLock                       = 0x01037,
   nsSwitching                  = 0x0103B,
   nsSwitchOption               = 0x0103C,
   nsLastSwitchOption           = 0x0103D,
   nsBiggerSwitchOption         = 0x0103E,
   nsLessSwitchOption           = 0x0103F,
   nsL6Operation                = 0x01041,
   nsSizeValue                  = 0x01042,
   nsL0Operation                = 0x01043,
   nsDefaultGeneric             = 0x01046,
   nsType                       = 0x01047,
   //nsSymbolReference            = 0x01048,

   nsDeclarationEndExpected         = 0x04000,               // error-terminals
   nsStatementEndExpected           = 0x04001,               
   nsErrClosingSBracketExpected     = 0x04002,               // closing square bracket expected
   nsErrNestedMemberExpected        = 0x04003,               
   nsErrObjectExpected              = 0x04004,
   nsErrMessageExpected             = 0x04005,
   nsDirectiveEndExpected           = 0x04006,
};

//inline bool ifAny(Symbol target, Symbol value1, Symbol value2)
//{
//   return target == value1 || target == value2;
//}

} // _ELENA_

#endif // syntaxH
