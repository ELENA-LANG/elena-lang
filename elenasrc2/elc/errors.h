//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the ELENA Compiler error messages
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef jeterrorsH
#define jeterrorsH 1

namespace _ELENA_
{
  // --- Parser error messages ---
   #define errLineTooLong           "%s(%d:%d): error 001: Line too long\n"
   #define errInvalidChar           "%s(%d:%d): error 002: Invalid char %c\n"
   #define errInvalidSyntax         "%s(%d:%d): error 004: Invalid syntax near '%s'\n"
   #define errDotExpectedSyntax     "%s(%d:%d): error 005: '.' expected\n"
//   #define errCBrExpectedSyntax     "%s(%d:%d): error 006: ')' expected\n"
//   #define errOBrExpectedSyntax     "%s(%d:%d): error 007: '(' expected\n"
//   #define errOActionExpectedSyntax "%s(%d:%d): error 008: '(' or '[' expected\n"
   #define errCSBrExpectedSyntax    "%s(%d:%d): error 009: ']' expected\n"
//   #define errCBraceExpectedSyntax  "%s(%d:%d): error 010: '}' expected\n"
//   #define errVarNameExpectedSyntax "%s(%d:%d): error 011: public or private identifier expected\n"
//   #define errExtensionNotAllowed   "%s(%d:%d): error 012: role cannot have an extension\n"
   #define errMethodNameExpected    "%s(%d:%d): error 013: method identifier expected\n"
//   #define errEqualExpected         "%s(%d:%d): error 014: '=' expected\n"
//   #define errSymbolOnlyExpected    "%s(%d:%d): error 015: '#symbol' or '#class' expected\n"
//   #define errCommaExpectedSyntax   "%s(%d:%d): error 016: ',' expected\n"
   #define errObjectExpected        "%s(%d:%d): error 017: object expected\n"
   #define errMessageExpected       "%s(%d:%d): error 018: message expected\n"

  // --- Compiler error messages ---
   #define errDuplicatedSymbol	   "%s(%d:%d): error 102: Class '%s' already exists\n"
   #define errDuplicatedMethod      "%s(%d:%d): error 103: Method '%s' already exists in the class\n"
   #define errUnknownClass          "%s(%d:%d): error 104: Class '%s' doesn't exists\n"
   #define errDuplicatedLocal       "%s(%d:%d): error 105: Variable '%s' already exists\n"
   #define errUnknownObject         "%s(%d:%d): error 106: Unknown object '%s'\n"
   #define errInvalidOperation	   "%s(%d:%d): error 107: Invalid operation with '%s'\n"
   #define errDuplicatedField       "%s(%d:%d): error 109: Field '%s' already exists in the class\n"
   #define errIllegalField          "%s(%d:%d): error 111: Illegal field declaration '%s'\n"
   #define errTooManyParameters     "%s(%d:%d): error 113: Too many parameters for '%s' message\n"
////   #define errUnknownRole           "%s(%d:%d): error 117: Unknown role '%s'\n"
   #define errDuplicatedDefinition  "%s(%d:%d): error 119: Duplicate definition: '%s' already declared\n"
////   #define errInvalidProperty       "%s(%d:%d): error 121: Invalid or none-existing property '%s'\n"
////   #define errInvalidRedirectMessage "%s(%d:%d): error 127: It is not possible to use redirect message in this case\n"
   #define errInvalidIntNumber      "%s(%d:%d): error 130: Invalid integer value %s\n"
   #define errUnknownMessage        "%s(%d:%d): error 131: Unknown message %s\n"
   #define errUnknownSubject        "%s(%d:%d): error 132: Unknown subject %s\n"
   #define errInvalidSubject        "%s(%d:%d): error 132: Invalid subject %s\n"
////   #define errDuplicatedArgument    "%s(%d:%d): error 133: Argument '%s' already exists\n"
////   #define errDuplicatedSubject     "%s(%d:%d): error 134: Subject '%s' already exists\n"
////   #define errUnmappedArgument      "%s(%d:%d): error 135: Argument '%s' is not mapped\n"
   #define errInvalidLink           "%s(%d:%d): error 136: The link '%s' cannot be resolved\n"
   #define errInvalidInlineClass    "%s(%d:%d): error 137: Inline structure cannot have external dependencies\n"
////   #define errUnknownExtRole        "%s(%d:%d): error 138: Unknown external role '%s' mapping\n"
   #define errInvalidParent         "%s(%d:%d): error 139: Invalid parent class %s\n"
//   #define errInvalidSync           "%s(%d:%d): error 140: The method %s cannot have a sync hint\n"
   #define errSealedParent          "%s(%d:%d): error 141: parent class %s cannot be inherited\n"
   #define errClosedParent          "%s(%d:%d): error 141: new method cannot be declared\n"
   #define errInvalidSymbolExpr     "%s(%d:%d): error 142: %s cannot be used inside a symbol\n"
   #define errInvalidRoleDeclr      "%s(%d:%d): error 143: %s cannot be used with a role\n"
   #define errInvalidHintValue      "%s(%d:%d): error 144: Invalid hint value '%s'\n"
   #define errNotApplicable         "%s(%d:%d): error 145: Illegal declaration '%s'\n"
   #define errNotSupportedType      "%s(%d:%d): error 146: Class '%s' is not compatible with its type\n" 
   #define errInvalidHint           "%s(%d:%d): error 147: Invalid hint '%s'\n"
   #define errStrongTypeNotAllowed  "%s(%d:%d): error 148: strong type '%s' cannot be used as a custom verb\n"
   #define errIllegalConstructor    "%s(%d:%d): error 149: Dynamic class requires implicit default constructor\n"
   #define errClosedMethod          "%s(%d:%d): error 150: sealed method cannot be overridden\n"

   #define errUnknownBaseClass	   "internal error 500: Base class doesn't exists\n"
//   #define errInternalError         "internal error 599"
//   #define errInvalidDynamicError   "internal error 600"
   #define errCommandSetAbsent     "internal error 600: command set is not defined"
   #define errFatalLinker          "internal error 601: linker fatal error"

  // --- Linker error messages ---
   #define errUnknownModule         "linker: error 201: Unknown module '%s'\n"
   #define errUnresovableLink       "linker: error 202: Link '%s' is not resolved\n"
   #define errInvalidModule	      "linker: error 203: Invalid module file '%s'\n"
   #define errCannotCreate	         "linker: error 204: Cannot create a file '%s'\n"
   #define errInvalidFile           "linker: error 205: Invalid file '%s'\n"
   #define errDuplicatedModule      "linker: error 208: Module '%s' already exists in the project\n"
   #define errInvalidModuleVersion  "linker: error 210: Obsolete module file '%s'\n"
   #define errConstantExpectedLink  "linker: error 211: Symbol '%s' cannot be constant\n"
   #define errEmptyTarget           "linker: error 212: Target is not specified\n"

//  // --- Compiler internal error messages ---
//   #define errReferenceOverflow     "error 301: The section reference overflow\n"

  // --- Compiler warnings ---
   #define wrnUnresovableLink       "%s(%d:%d): warning 401: Link %s is unresolvable\n"
   #define wrnUnknownHint           "%s(%d:%d): warning 404: Unknown hint '%s'\n"
//   #define wrnUnknownHintValue      "%s(%d:%d): warning 405: Unknown class hint value '%s'\n"
   #define wrnInvalidHint           "%s(%d:%d): warning 406: Hint '%s' cannot be applied here\n"
   #define wrnUnknownMessage        "%s(%d:%d): warning 407: Message '%s' does not belong to the object\n"
//   #define wrnObsoleteMessage       "%s(%d:%d): warning 408: Message '%s' is obsolete"
//   #define wrnObsoleteConstruction  "%s(%d:%d): warning 409: Construction near '%s' is obsolete"
//   #define wrnProhibitedSubjectName "%s(%d:%d): warning 410: Subject name '%s' coincides with a message verb"
//   #define wrnUnknownSignature      "%s(%d:%d): warning 411: Unknown signature '%s'"
//   #define wrnObsolete              "%s(%d:%d): warning 412: Obsolete language construction near '%s'"
   #define wrnUnknownModule         "%s(%d:%d): warning 413: Unknown module '%s'\n"
   #define wrnOuterAssignment       "%s(%d:%d): warning 414: Outer assignment; the change will not be seen outside the nested code\n"
   #define wrnTypeMismatch          "%s(%d:%d): warning 415: Type mismatch, direct typecast is invoked\n"
//   #define wrnUnknownMessage        "%s(%d:%d): warning 416: method '%s' is not supported by the object\n"
   #define wrnBoxingCheck           "%s(%d:%d): warning 417: The object may be boxed\n" 
   #define wrnDuplicateExtension    "%s(%d:%d): warning 418: '%s': duplicate extension\n" 
   #define wrnUnboxinging           "%s(%d:%d): warning 419: '%s' will be unboxed\n" 

} // _ELENA_

#endif // jeterrors
