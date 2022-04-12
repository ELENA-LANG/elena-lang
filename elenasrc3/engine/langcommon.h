//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the language common constants
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LANGCOMMON_H
#define LANGCOMMON_H

namespace elena_lang
{
   enum class MethodHint
   {
      Mask           = 0x000000F,

      None           = 0x0000000,
      Sealed         = 0x0000001,
      Dispatcher     = 0x0000004,
      Constructor    = 0x0200400,
      Conversion     = 0x0200800,
      Static         = 0x0004000,
   };

   // === ELENA Error codes ===
   constexpr auto errInvalidSyntax        = 4;

   constexpr auto errDuplicatedSymbol     = 102;
   constexpr auto errDuplicatedMethod     = 103;
   constexpr auto errUnknownClass         = 104;
   constexpr auto errDuplicatedLocal      = 105;
   constexpr auto errUnknownObject        = 106;
   constexpr auto errInvalidOperation     = 107;
   constexpr auto errDuplicatedDictionary = 108;
   constexpr auto errDuplicatedField      = 109;
   constexpr auto errUnknownVariableType  = 110;
   constexpr auto errIllegalField         = 111;
   constexpr auto errTooManyParameters    = 113;
   constexpr auto errDuplicatedDefinition = 119;
   constexpr auto errInvalidIntNumber     = 130;
   constexpr auto errCannotEval           = 140;
   constexpr auto errSealedParent         = 141;
   constexpr auto errInvalidHint          = 147;
   constexpr auto errIllegalConstructor   = 149;
   constexpr auto errIllegalStaticMethod  = 151;
   constexpr auto errIllegalMethod        = 152;
   constexpr auto errEmptyStructure       = 169;
   constexpr auto errNoBodyMethod         = 180;
   constexpr auto errUnknownTemplate      = 181;

   constexpr auto errUnknownModule        = 201;
   constexpr auto errUnresovableLink      = 202;
   constexpr auto errInvalidModule        = 203;
   constexpr auto errCannotCreate         = 204;
   constexpr auto errInvalidFile          = 205;
   constexpr auto errInvalidModuleVersion = 210;
   constexpr auto errEmptyTarget          = 212;

   constexpr auto errParserNotInitialized = 300;
   constexpr auto errProjectAlreadyLoaded = 301;

   constexpr auto wrnUnknownHint          = 404;
   constexpr auto wrnInvalidHint          = 406;
   constexpr auto wrnUnknownModule        = 413;
   constexpr auto wrnDuplicateInclude     = 425;

   constexpr auto wrnSyntaxFileNotFound   = 500;
   constexpr auto wrnInvalidConfig        = 501;

   constexpr auto errCommandSetAbsent     = 600;
   constexpr auto errReadOnlyModule       = 601;
   constexpr auto errNotDefinedBaseClass  = 602;
   constexpr auto errReferenceOverflow    = 603;
   constexpr auto errUnknownBaseClass     = 604;
   constexpr auto errNoDispatcher         = 605;

   constexpr auto errFatalError       = -1;
   constexpr auto errFatalLinker      = -2;
   constexpr auto errNotImplemented   = -3;
   constexpr auto errCorruptedVMT     = -4;

   // --- Project warning levels
   constexpr int WARNING_LEVEL_1          = 1;
   constexpr int WARNING_LEVEL_2          = 2;
   constexpr int WARNING_LEVEL_3          = 4;

   constexpr int WARNING_MASK_0           = 0;
   constexpr int WARNING_MASK_1           = 1;
   constexpr int WARNING_MASK_2           = 3;
   constexpr int WARNING_MASK_3           = 7;

   // === Attributes / Predefined Types ===

   /// scope_accessors? modificator? visibility? scope_prefix? scope? type?
   constexpr auto V_CATEGORY_MASK         = 0x7FFFFF00u;
   constexpr auto V_CATEGORY_MAX          = 0x0000F000u;

   /// visibility:
   constexpr auto V_PUBLIC                = 0x80004001u;
   constexpr auto V_PRIVATE               = 0x80004002u;

   /// property:
   constexpr auto V_SEALED                = 0x80003001u;

   /// scope_prefix:
   constexpr auto V_CONST                 = 0x80002001u;
   constexpr auto V_EMBEDDABLE            = 0x80002002u;

   /// scope:
   constexpr auto V_CLASS                 = 0x80001001u;
   constexpr auto V_STRUCT                = 0x80001002u;
   constexpr auto V_SYMBOLEXPR            = 0x80001003u;
   constexpr auto V_CONSTRUCTOR           = 0x80001004u;
   constexpr auto V_SINGLETON             = 0x80001006u;
   constexpr auto V_METHOD                = 0x80001008u;
   constexpr auto V_FIELD                 = 0x80001009u;
   constexpr auto V_VARIABLE              = 0x8000100Du;
   constexpr auto V_STATIC                = 0x8000100Fu;
   constexpr auto V_CONVERSION            = 0x80001011u;
   constexpr auto V_DISPATCHER            = 0x80001013u;
   constexpr auto V_INTERN                = 0x80001016u;
   constexpr auto V_FORWARD               = 0x80001017u;
   constexpr auto V_IMPORT                = 0x80001018u;
   constexpr auto V_INLINE                = 0x80001025u;

   /// primitive type attribute
   constexpr auto V_STRINGOBJ             = 0x80000801u;
   constexpr auto V_INTBINARY             = 0x80000803u;
   constexpr auto V_SYMBOL                = 0x80000808u;

   /// primitive types
   constexpr auto V_STRING                = 0x80000001u;
   constexpr auto V_INT32                 = 0x80000002u;
   constexpr auto V_DICTIONARY            = 0x80000003u;
   constexpr auto V_NIL                   = 0x80000004u;
   constexpr auto V_OBJARRAY              = 0x80000005u;
   constexpr auto V_OBJECT                = 0x80000006u;
   constexpr auto V_BINARYARRAY           = 0x8000000Bu;
   constexpr auto V_OBJATTRIBUTES         = 0x80000012u;

   /// built-in variables
   constexpr auto V_SELF_VAR              = 0x80000081u;

   // === Operators ===
   constexpr auto OPERATOR_MAKS           = 0x1840;
   constexpr auto INDEX_OPERATOR_ID       = 0x0001;
   constexpr auto SET_OPERATOR_ID         = 0x0002;
   constexpr auto ADD_ASSIGN_OPERATOR_ID  = 0x0003;
   constexpr auto SET_INDEXER_OPERATOR_ID = 0x0201;

}

#endif
