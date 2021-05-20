//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains the ELENA Compiler error messages
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef jeterrorsH
#define jeterrorsH 1

namespace _ELENA_
{
  // --- Parser error messages ---
   #define errLineTooLong           "%s(%d:%d): error 001: Line too long\n"
   #define errInvalidChar           "%s(%d:%d): error 002: Invalid char %c\n"
   #define errInvalidSyntax         "%s(%d:%d): error 004: Invalid syntax near '%s'\n"
//   #define errDotExpectedSyntax     "%s(%d:%d): error 005: '.' expected\n"
//   #define errCBrExpectedSyntax     "%s(%d:%d): error 006: ')' expected\n"
////   #define errOBrExpectedSyntax     "%s(%d:%d): error 007: '(' expected\n"
////   #define errOActionExpectedSyntax "%s(%d:%d): error 008: '(' or '[' expected\n"
//   #define errCSBrExpectedSyntax    "%s(%d:%d): error 009: ']' expected\n"
//   #define errCBraceExpectedSyntax  "%s(%d:%d): error 010: '}' expected\n"
////   #define errVarNameExpectedSyntax "%s(%d:%d): error 011: public or private identifier expected\n"
////   #define errExtensionNotAllowed   "%s(%d:%d): error 012: role cannot have an extension\n"
//   #define errMethodNameExpected    "%s(%d:%d): error 013: method identifier expected\n"
////   #define errEqualExpected         "%s(%d:%d): error 014: '=' expected\n"
////   #define errSymbolOnlyExpected    "%s(%d:%d): error 015: '#symbol' or '#class' expected\n"
////   #define errCommaExpectedSyntax   "%s(%d:%d): error 016: ',' expected\n"
//   #define errObjectExpected        "%s(%d:%d): error 017: object expected\n"
//   #define errMessageExpected       "%s(%d:%d): error 018: message expected\n"

  // --- Compiler error messages ---
   #define errDuplicatedSymbol	   "%s(%d:%d): error 102: Class '%s' already exists\n"
   #define errDuplicatedMethod      "%s(%d:%d): error 103: Method '%s' already exists in the class\n"
   #define errUnknownClass          "%s(%d:%d): error 104: Class '%s' doesn't exist\n"
   #define errDuplicatedLocal       "%s(%d:%d): error 105: Variable '%s' already exists\n"
   #define errUnknownObject         "%s(%d:%d): error 106: Unknown object '%s'\n"
   #define errInvalidOperation	   "%s(%d:%d): error 107: Invalid operation with '%s'\n"
   #define errDuplicatedField       "%s(%d:%d): error 109: Field '%s' already exists in the class\n"
   #define errUnknownVariableType   "%s(%d:%d): error 104: Type of the variable '%s' doesn't exist\n"
   #define errIllegalField          "%s(%d:%d): error 111: Illegal field declaration '%s'\n"
   #define errTooManyParameters     "%s(%d:%d): error 113: Too many parameters for '%s' message\n"
   #define errDuplicatedDefinition  "%s(%d:%d): error 119: Duplicate definition: '%s' already declared\n"
//////   #define errInvalidProperty       "%s(%d:%d): error 121: Invalid or none-existing property '%s'\n"
//////   #define errInvalidRedirectMessage "%s(%d:%d): error 127: It is not possible to use redirect message in this case\n"
   #define errInvalidIntNumber      "%s(%d:%d): error 130: Invalid integer value %s\n"
   #define errUnknownMessage        "%s(%d:%d): error 131: Unknown message %s\n"
////   #define errUnknownSubject        "%s(%d:%d): error 132: Unknown attribute %s\n"
////   #define errInvalidSubject        "%s(%d:%d): error 132: Invalid attribute %s\n"
//////   #define errDuplicatedArgument    "%s(%d:%d): error 133: Argument '%s' already exists\n"
//////   #define errDuplicatedSubject     "%s(%d:%d): error 134: Subject '%s' already exists\n"
//////   #define errUnmappedArgument      "%s(%d:%d): error 135: Argument '%s' is not mapped\n"
   #define errInvalidLink           "%s(%d:%d): error 136: The link '%s' cannot be resolved\n"
   #define errInvalidInlineClass    "%s(%d:%d): error 137: Inline structure cannot have external dependencies\n"
//////   #define errUnknownExtRole        "%s(%d:%d): error 138: Unknown external role '%s' mapping\n"
//   #define errInvalidParent         "%s(%d:%d): error 139: Invalid parent class %s\n"
////////   #define errInvalidSync           "%s(%d:%d): error 140: The method %s cannot have a sync hint\n"
   #define errSealedParent          "%s(%d:%d): error 141: parent class %s cannot be inherited\n"
   #define errClosedParent          "%s(%d:%d): error 141: new method cannot be declared\n"
//////   #define errInvalidSymbolExpr     "%s(%d:%d): error 142: %s cannot be used inside a symbol\n"
//////   #define errInvalidRoleDeclr      "%s(%d:%d): error 143: %s cannot be used with a role\n"
////   #define errInvalidHintValue      "%s(%d:%d): error 144: Invalid hint value '%s'\n"
   #define errNotApplicable         "%s(%d:%d): error 145: Illegal declaration '%s'\n"
//////   #define errNotSupportedType      "%s(%d:%d): error 146: Class '%s' is not compatible with its type\n"
   #define errInvalidHint           "%s(%d:%d): error 147: Invalid attribute '%s'\n"
////   #define errStrongTypeNotAllowed  "%s(%d:%d): error 148: strong type '%s' cannot be used as a custom verb\n"
   #define errIllegalConstructor    "%s(%d:%d): error 149: Constructor cannot be declared\n"
   #define errClosedMethod          "%s(%d:%d): error 150: sealed method cannot be overridden\n"
////   #define errNoConstructorDefined  "%s(%d:%d): error 151: Class '%s' has no implicit or explicit constructors\n"
   constexpr auto errIllegalMethod              = "%s(%d:%d): error 152: Illegal method declaration\n";
   constexpr auto errIllegalOperation           = "%s(%d:%d): error 153: Illegal operation\n";
   constexpr auto errInvalidConstant            = "%s(%d:%d): error 154: Invalid constant '%s'\n";
////   #define errInvalidMultimethod    "%s(%d:%d): error 155: Invalid multimethod declararion in the class '%s'\n"
   #define errTypeNotAllowed        "%s(%d:%d): error 156: returning type attribute '%s' cannot be specified for this method\n"
   #define errNotCompatibleMulti    "%s(%d:%d): error 157: type attribute should be the same for the overloaded method '%s'\n"
   #define errTypeAlreadyDeclared   "%s(%d:%d): error 158: type attribute cannot be overridden\n"
   #define errAbstractMethods       "%s(%d:%d): error 159: Class contains abstract methods\n"
   #define errDispatcherInInterface "%s(%d:%d): error 160: Closed class contains a dispatcher method\n"
   #define errAbstractMethodCode    "%s(%d:%d): error 161: An abstract method cannot have an explicit body\n"
   #define errPedefineMethodCode    "%s(%d:%d): error 162: A predefined method cannot have an explicit body\n"
////   #define errUnknownTemplate       "%s(%d:%d): error 163: Unrecognized template %s\n"
   #define errNotAbstractClass      "%s(%d:%d): error 164: An attribute '%s' cannot be declared in a non-abstract class\n"
   #define errNoMethodOverload      "%s(%d:%d): error 165: An attribute '%s' cannot be applied for an already existing method\n"
   #define errIllegalPrivate        "%s(%d:%d): error 166: An attribute '%s' cannot be applied for an extension\n"
   constexpr auto errDupPublicMethod            = "%s(%d:%d): error 167: A public method with the same name '%s' is already declared\n";
//   //   #define errOneDefaultConstructor "%s(%d:%d): error 168: a class '%s' should have only one default constructor\n"
   constexpr auto errEmptyStructure             = "%s(%d:%d): error 169: a structure class '%s' should have at least one field\n";
   constexpr auto errReadOnlyField              = "%s(%d:%d): error 170: cannot assign a value to the read-only field '%s'\n";
   constexpr auto errDefaultConstructorNotFound = "%s(%d:%d): error 171: default or conversion constructor is not found for '%s'\n";
   constexpr auto errInvalidType                = "%s(%d:%d): error 172: '%s' cannot be used in the declaration\n";
   constexpr auto errDupInternalMethod          = "%s(%d:%d): error 173: An internal method with the same name '%s' is already declared\n";
   constexpr auto errInvalidConstAttr           = "%s(%d:%d): error 174: A method '%s' cannot be compiled as a constant one\n";
   constexpr auto errInvalidBoxing              = "%s(%d:%d): error 175: An expression at '%s' cannot be boxed\n";
   constexpr auto errIllegalStaticMethod        = "%s(%d:%d): error 176: Static method cannot be declared\n";
   constexpr auto errIllegalConstructorAbstract = "%s(%d:%d): error 177: An abstract class cannot have a public constructor\n";
   constexpr auto errUnresolvedDispatch         = "%s(%d:%d): error 178: a target does not handle dispatched message\n";
   constexpr auto errUnresolvedInterface        = "%s(%d:%d): error 179: a class does not contain an interface implementation\n";
   constexpr auto errNoBodyMethod               = "%s(%d:%d): error 180: only abstract method can have no body\n";
   constexpr auto errInvalidComlexMessageName   = "%s(%d:%d): error 103: Invalid complex message name '%s'\n";

   constexpr auto errUnknownBaseClass           = "internal error 500: Base class doesn't exist\n";
   constexpr auto errNotDefinedBaseClass        = "internal error 501: Base class doesn't defined\n";
   constexpr auto errNoDispatcher               = "internal error 502: Default dispatch method is not declared in the base class\n";
   constexpr auto errClosureError               = "internal error 503: closure cannot be generated";
////   #define errCrUnknownReference    "reference cannot be resolved"
   constexpr auto errCommandSetAbsent           = "internal error 600: command set is not defined";
   constexpr auto errFatalLinker                = "internal error 601: linker fatal error";

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
   #define errInvalidTargetOption   "linker: error 213: Invalid target option '%s'\n"

//  // --- Compiler internal error messages ---
////   #define errReferenceOverflow     "error 301: The section reference overflow\n"

  // --- Compiler warnings ---
//   #define wrnUnresovableLink       "%s(%d:%d): warning 401: Link %s is unresolvable\n"
   constexpr auto wrnUnknownHint          = "%s(%d:%d): warning 404: Unknown attribute '%s'\n";
   constexpr auto wrnInvalidHint          = "%s(%d:%d): warning 406: Attribute '%s' cannot be applied here\n";
   constexpr auto wrnUnknownMessage       = "%s(%d:%d): warning 407: Message '%s' does not handled by the object\n";
   constexpr auto wrnUnknownFunction      = "%s(%d:%d): warning 408: Function message does not handled by the object\n";
   constexpr auto wrnUnknownDefConstructor= "%s(%d:%d): warning 409: Explicit constructor is not defined in the object\n";
   constexpr auto wrnUnknownModule        = "%s(%d:%d): warning 413: Unknown module '%s'\n";
////   #define wrnOuterAssignment       "%s(%d:%d): warning 414: Outer assignment; the change will not be seen outside the nested code\n"
////   #define wrnTypeMismatch          "%s(%d:%d): warning 415: Type mismatch, direct typecast is invoked\n"
////   #define wrnBoxingCheck           "%s(%d:%d): warning 417: The object may be boxed\n"
////   #define wrnDuplicateExtension    "%s(%d:%d): warning 418: '%s': duplicate extension\n"
//////   #define wrnUnboxinging           "%s(%d:%d): warning 419: '%s' will be unboxed\n"
   constexpr auto wrnTypeInherited        = "%s(%d:%d): warning 420: Type attribute is inherited\n";
   constexpr auto wrnAmbiguousIdentifier  ="%s(%d:%d): warning 421: Identifier '%s' is ambiguous";
////   #define wrnAmbiguousMessageName  "%s(%d:%d): warning 422: Message name '%s' is ambiguous"
////   #define wrnAmbiguousVariable     "%s(%d:%d): warning 423: Variable '%s' is ambiguous"
   constexpr auto wrnDuplicateAttribute   ="%s(%d:%d): warning 424: '%s': duplicate attribute\n";
   constexpr auto wrnDuplicateInclude     ="%s(%d:%d): warning 425: '%s': duplicate inclusion\n";
   constexpr auto wrnExplicitExtension    ="%s(%d:%d): warning 426: an extension '%s' should not be used directly\n";
   constexpr auto wrnUnknownConversion    = "%s(%d:%d): warning 427: a conversion constructor is not found for '%s'\n";
   constexpr auto wrnUnknownEOPConversion = "%s(%d:%d): warning 428: a conversion constructor is not found for self\n";
   constexpr auto wrnUnresolvedDispatch   = "%s(%d:%d): warning 429: a target does not handle dispatched message\n";
   constexpr auto wrnUnresolvedInterface  = "%s(%d:%d): warning 430: a class does not contain an interface implementation\n";
   constexpr auto wrnAbstractMethodBody   = "%s(%d:%d): warning 431: an abstract method should have no body\n";
   constexpr auto wrnUnassignedVaiable    = "%s(%d:%d): warning 432: an unassined variable - '%s'\n";
   constexpr auto wrnInvalidModule        = "Invalid or unknown module %s\n";

   constexpr auto infoAbstractMetod       = "abstract method %s";
   constexpr auto infoNewMethod           = "new method %s";

////   #define wrnDuplicateInfo         "Duplicate extension - "

} // _ELENA_

#endif // jeterrors
