//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the ELENA Compiler error messages
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ERRORS_H
#define ERRORS_H

#include "elena.h"
#include "cliconst.h"

namespace elena_lang
{
   constexpr auto errMsgInvalidSyntax           = "\n%s(%d:%d): error 004: Invalid syntax near '%s'\n";

   constexpr auto errMsgDuplicatedSymbol        = "\n%s(%d:%d): error 102: Class '%s' already exists\n";
   constexpr auto errMsgDuplicatedMethod        = "\n%s(%d:%d): error 103: Method '%s' already exists in the class\n";
   constexpr auto errMsgUnknownClass            = "\n%s(%d:%d): error 104: Class '%s' doesn't exist\n";
   constexpr auto errMsgDuplicatedLocal         = "\n%s(%d:%d): error 105: Variable '%s' already exists\n";
   constexpr auto errMsgUnknownObject           = "\n%s(%d:%d): error 106: Unknown object '%s'\n";
   constexpr auto errMsgInvalidOperation        = "\n%s(%d:%d): error 107: Invalid operation with '%s'\n";
   constexpr auto errMsgDuplicatedDictionary    = "\n%s(%d:%d): error 108: Dictionary '%s' already exists\n";
   constexpr auto errMsgDuplicatedField         = "\n%s(%d:%d): error 109: Field '%s' already exists in the class\n";
   constexpr auto errMsgUnknownVariableType     = "\n%s(%d:%d): error 110: Type of the variable '%s' doesn't exist\n";
   constexpr auto errMsgIllegalField            = "\n%s(%d:%d): error 111: Illegal field declaration '%s'\n";
   constexpr auto errMsgTooManyParameters       = "\n%s(%d:%d): error 113: Too many parameters for '%s' message\n";
   constexpr auto errMsgDuplicatedDefinition    = "\n%s(%d:%d): error 119: Duplicate definition: '%s' already declared\n";
   constexpr auto errMsgInvalidIntNumber        = "\n%s(%d:%d): error 130: Invalid integer value %s\n";
   constexpr auto errMsgCannotEval              = "\n%s(%d:%d): error 140: Cannot evaluate the expression %s\n";
   constexpr auto errMsgSealedParent            = "\n%s(%d:%d): error 141: parent class %s cannot be inherited\n";
   constexpr auto errMsgClosedParent            = "\n%s(%d:%d): error 142: new method cannot be declared\n";

   constexpr auto errMsgInvalidHint             = "\n%s(%d:%d): error 147: Invalid attribute '%s'\n";
   constexpr auto errMsgIllegalConstructor      = "\n%s(%d:%d): error 149: Constructor cannot be declared\n";
   constexpr auto errMsgClosedMethod            = "\n%s(%d:%d): error 150: sealed method cannot be overridden\n";
   constexpr auto errMsgIllegalStaticMethod     = "\n%s(%d:%d): error 151: Static method cannot be declared\n";
   constexpr auto errMsgIllegalMethod           = "\n%s(%d:%d): error 152: Illegal method declaration\n";
   constexpr auto errMsgIllegalOperation        = "\n%s(%d:%d): error 153: Illegal operation\n";
   constexpr auto errMsgTypeAlreadyDeclared     = "\n%s(%d:%d): error 158: type attribute cannot be overridden\n";
   constexpr auto errMsgAbstractMethods         = "\n%s(%d:%d): error 159: Class contains abstract methods\n";

   constexpr auto errMsgDispatcherInInterface   = "\n%s(%d:%d): error 160: Closed class contains a dispatcher method\n";
   constexpr auto errMsgAbstractMethodCode      = "\n%s(%d:%d): error 161: An abstract method cannot have an explicit body\n";
   constexpr auto errMsgNotAbstractClass        = "\n%s(%d:%d): error 164: An attribute '%s' cannot be declared in a non-abstract class\n";
   constexpr auto errMsgIllegalPrivate          = "\n%s(%d:%d): error 166: An attribute '%s' cannot be applied for an extension\n";

   constexpr auto errMsgDupPublicMethod         = "\n%s(%d:%d): error 167: A public method with the same name '%s' is already declared\n";
   constexpr auto errMsgEmptyStructure          = "\n%s(%d:%d): error 169: a structure class '%s' should have at least one field\n";
   constexpr auto errMsgInvalidType             = "\n%s(%d:%d): error 172: '%s' cannot be used in the declaration\n";
   constexpr auto errMsgDupInternalMethod       = "\n%s(%d:%d): error 173: An internal method with the same name '%s' is already declared\n";
   constexpr auto errMsgInvalidConstAttr        = "\n%s(%d:%d): error 174: A method '%s' cannot be compiled as a constant one\n";
   constexpr auto errMsgIllegalConstructorAbstract = "\n%s(%d:%d): error 177: An abstract class cannot have a public constructor\n";
   constexpr auto errMsgNoBodyMethod            = "\n%s(%d:%d): error 180: Only abstract method can have no body\n";
   constexpr auto errMsgUnknownTemplate         = "\n%s(%d:%d): error 181: Unknown template %s\n";
   constexpr auto errMsgDupPrivateMethod        = "\n%s(%d:%d): error 182: A private method with the same name '%s' is already declared\n";
   constexpr auto errMsgDupProtectedMethod      = "\n%s(%d:%d): error 183: A protected method with the same name '%s' is already declared\n";
   constexpr auto errMsgUnknownDefConstructor   = "\n%s(%d:%d): error 184: A constructor is not defined for the class\n";
   constexpr auto errMsgUnknownMessage          = "\n%s(%d:%d): error 185: Message '%s' does not handled by the object\n";

   constexpr auto errMsgUnknownModule           = "\nlinker: error 201: Unknown module '%s'\n";
   constexpr auto errMsgUnresovableLink         = "\nlinker: error 202: Link '%s' is not resolved\n";
   constexpr auto errMsgInvalidModule           = "\nlinker: error 203: Invalid module file '%s'\n";
   constexpr auto errMsgCannotCreate            = "\nlinker: error 204: Cannot create a file '%s'\n";
   constexpr auto errMsgInvalidFile             = "\nlinker: error 205: Invalid file '%s'\n";
   constexpr auto errMsgInvalidParserTarget     = "\nlinker: error 206: Invalid parser target '%s'\n";
   constexpr auto errMsgInvalidParserTargetType = "\nlinker: error 207: Invalid parser target type '%s'\n";
   constexpr auto errMsgInvalidModuleVersion    = "\nlinker: error 210: Obsolete module file '%s'\n";
   constexpr auto errMsgEmptyTarget             = "\nlinker: error 212: Target is not specified\n";

   constexpr auto errMsgParserNotInitialized = "\ninternal error 300: a parser cannot be initialized\n";
   constexpr auto errMsgProjectAlreadyLoaded = "\ninternal error 301: a project cannot be loaded: %s\n";

   constexpr auto wrnMsgUnknownHint          = "\n%s(%d:%d): warning 404: Unknown attribute '%s'\n";
   constexpr auto wrnMsgInvalidHint          = "\n%s(%d:%d): warning 406: Attribute '%s' cannot be applied here\n";
   constexpr auto wrnMsgUnknownMessage       = "\n%s(%d:%d): warning 407: Message '%s' does not handled by the object\n";
   constexpr auto wrnMsgUnknownFunction      = "\n%s(%d:%d): warning 408: Function message does not handled by the object '%s'\n";
   constexpr auto wrnMsgUnknownDefConstructor = "\n%s(%d:%d): warning 409: Explicit constructor is not defined in the object\n";
   constexpr auto wrnMsgUnknownModule        = "\n%s(%d:%d): warning 413: Unknown module '%s'\n";
   constexpr auto wrnMsgTypeInherited        = "\n%s(%d:%d): warning 420: Type attribute is inherited\n";
   constexpr auto wrnMsgDuplicateInclude     = "\n%s(%d:%d): warning 425: '%s': duplicate inclusion\n";
   constexpr auto wrnMsgUnknownTypecast      = "\n%s(%d:%d): warning 426: typecasting routine cannot be found\n";
   constexpr auto wrnMsgUnsupportedOperator  = "\n%s(%d:%d): warning 427: operator handler is not defined for %s\n";

   constexpr auto wrnMsgSyntaxFileNotFound   = "\nwarning 500: cannot open syntax file '%s'\n";
   constexpr auto wrnMsgInvalidConfig        = "\nwarning 501: invalid or unknown config file %s\n";

   constexpr auto errMsgCommandSetAbsent     = "\ninternal error 600: command set is not defined for %x\n";
   constexpr auto errMsgReadOnlyModule       = "\ninternal error 601: read-only module\n";
   constexpr auto errMsgNotDefinedBaseClass  = "\ninternal error 602: base class is not defined\n";
   constexpr auto errMsgReferenceOverflow    = "\ninternal error 603: Reference overflow\n";
   constexpr auto errMsgUnknownBaseClass     = "\ninternal error 604: Base class doesn't exist\n";
   constexpr auto errMsgNoDispatcher         = "\ninternal error 605: Default dispatch method is not declared in the base class\n";
   constexpr auto errMsgClosureError         = "\ninternal error 606: closure cannot be generated\n";

   constexpr auto errMsgFatalError           = "\nFatal error\n";
   constexpr auto errMsgUnrecognizedError    = "\nUnknown error\n";
   constexpr auto errMsgFatalLinkerError     = "\nFatal linker error\n";
   constexpr auto errMsgNotImplemented       = "\nNot implemented error\n";
   constexpr auto errMsgCorruptedVMT         = "\nVMT structure is corrupt\n";

   constexpr auto infoMsgNewMethod           = "\ninfo 701:   new method %s\n";
   constexpr auto infoMsgCurrentMethod       = "\ninfo 702:   compiling method %s\n";
   constexpr auto infoMsgCurrentClass        = "\ninfo 703: compiling class %s\n";
   constexpr auto infoMsgAbstractMetod       = "\ninfo 704:   abstract method %s\n";

} // _ELENA_

#endif // ERRORS_H
