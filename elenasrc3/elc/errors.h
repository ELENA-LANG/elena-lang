//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the ELENA Compiler error messages
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ERRORS_H
#define ERRORS_H

#include "elena.h"
#include "cliconst.h"

namespace elena_lang
{
   constexpr auto errMsgInvalidSyntax        = "%s(%d:%d): error 004: Invalid syntax near '%s'\n";

   constexpr auto errMsgDuplicatedSymbol     = "%s(%d:%d): error 102: Class '%s' already exists\n";
   constexpr auto errMsgDuplicatedMethod     = "%s(%d:%d): error 103: Method '%s' already exists in the class\n";
   constexpr auto errMsgDuplicatedLocal      = "%s(%d:%d): error 105: Variable '%s' already exists\n";
   constexpr auto errMsgUnknownObject        = "%s(%d:%d): error 106: Unknown object '%s'\n";
   constexpr auto errMsgInvalidOperation     = "%s(%d:%d): error 107: Invalid operation with '%s'\n";
   constexpr auto errMsgDuplicatedDictionary = "%s(%d:%d): error 108: Dictionary '%s' already exists\n";
   constexpr auto errMsgTooManyParameters    = "%s(%d:%d): error 113: Too many parameters for '%s' message\n";
   constexpr auto errMsgDuplicatedDefinition = "%s(%d:%d): error 119: Duplicate definition: '%s' already declared\n";
   constexpr auto errMsgInvalidIntNumber     = "%s(%d:%d): error 130: Invalid integer value %s\n";
   constexpr auto errMsgCannotEval           = "%s(%d:%d): error 140: Cannot evaluate the expression %s\n";
   constexpr auto errMsgSealedParent         = "%s(%d:%d): error 141: parent class %s cannot be inherited\n";
   constexpr auto errMsgInvalidHint          = "%s(%d:%d): error 147: Invalid attribute '%s'\n";
   constexpr auto errMsgIllegalConstructor   = "%s(%d:%d): error 149: Constructor cannot be declared\n";
   constexpr auto errMsgIllegalStaticMethod  = "%s(%d:%d): error 151: Static method cannot be declared\n";

   constexpr auto errMsgIllegalMethod        = "%s(%d:%d): error 152: Illegal method declaration\n";
   constexpr auto errMsgNoBodyMethod         = "%s(%d:%d): error 180: Only abstract method can have no body\n";
   constexpr auto errMsgUnknownTemplate      = "%s(%d:%d): error 181: Unknown template %s\n";

   constexpr auto errMsgUnknownModule        = "linker: error 201: Unknown module '%s'\n";
   constexpr auto errMsgUnresovableLink      = "linker: error 202: Link '%s' is not resolved\n";
   constexpr auto errMsgInvalidModule        = "linker: error 203: Invalid module file '%s'\n";
   constexpr auto errMsgCannotCreate         = "linker: error 204: Cannot create a file '%s'\n";
   constexpr auto errMsgInvalidFile          = "linker: error 205: Invalid file '%s'\n";
   constexpr auto errMsgInvalidModuleVersion = "linker: error 210: Obsolete module file '%s'\n";
   constexpr auto errMsgEmptyTarget          = "linker: error 212: Target is not specified\n";

   constexpr auto errMsgParserNotInitialized = "internal error 300: a parser cannot be initialized\n";
   constexpr auto errMsgProjectAlreadyLoaded = "internal error 301: a project cannot be loaded: %s\n";

   constexpr auto wrnMsgUnknownHint          = "%s(%d:%d): warning 404: Unknown attribute '%s'\n";
   constexpr auto wrnMsgInvalidHint          = "%s(%d:%d): warning 406: Attribute '%s' cannot be applied here\n";

   constexpr auto wrnMsgSyntaxFileNotFound   = "warning 500: cannot open syntax file '%s'\n";
   constexpr auto wrnMsgInvalidConfig        = "warning 501: invalid or unknown config file %s\n";

   constexpr auto errMsgCommandSetAbsent     = "internal error 600: command set is not defined";
   constexpr auto errMsgReadOnlyModule       = "internal error 601: read-only module"; 
   constexpr auto errMsgNotDefinedBaseClass  = "internal error 602: base class is not defined";
   constexpr auto errMsgReferenceOverflow    = "internal error 603: Reference overflow";
   constexpr auto errMsgUnknownBaseClass     = "internal error 604: Base class doesn't exist\n";
   constexpr auto errMsgNoDispatcher         = "internal error 605: Default dispatch method is not declared in the base class\n";

   constexpr auto errMsgFatalError           = "Fatal error\n";
   constexpr auto errMsgUnrecognizedError    = "Unknown error\n";
   constexpr auto errMsgFatalLinkerError     = "Fatal linker error\n";
   constexpr auto errMsgNotImplemented       = "Not implemented error\n";
   constexpr auto errMsgCorruptedVMT         = "VMT structure is corrupt";

} // _ELENA_

#endif // ERRORS_H
