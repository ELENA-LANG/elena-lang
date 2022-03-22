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
      Dispatcher     = 0x0000004,
      Constructor    = 0x0200400,
      Static         = 0x0004000,
   };

   // === ELENA Error codes ===
   constexpr auto errNotDefinedBaseClass  = 602;
   constexpr auto errUnknownBaseClass     = 604;

   constexpr auto errUnknownModule        = 201;
   constexpr auto errUnresovableLink      = 202;
   constexpr auto errInvalidModule        = 203;
   constexpr auto errCannotCreate         = 204;
   constexpr auto errInvalidFile          = 205;
   constexpr auto errInvalidModuleVersion = 210;
   constexpr auto errEmptyTarget          = 212;

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

   /// scope:
   constexpr auto V_CLASS                 = 0x80001001u;
   constexpr auto V_SYMBOLEXPR            = 0x80001003u;
   constexpr auto V_CONSTRUCTOR           = 0x80001004u;
   constexpr auto V_METHOD                = 0x80001008u;
   constexpr auto V_STATIC                = 0x8000100Fu;
   constexpr auto V_DISPATCHER            = 0x80001013u;
   constexpr auto V_INTERN                = 0x80001016u;
   constexpr auto V_FORWARD               = 0x80001017u;
   constexpr auto V_INLINE                = 0x80001025u;

   /// primitive type attribute
   constexpr auto V_STRINGOBJ             = 0x80000801u;

   constexpr auto V_STRING                = 0x80000001u;
   constexpr auto V_INT32                 = 0x80000002u;
   constexpr auto V_DICTIONARY            = 0x80000003u;
   constexpr auto V_NIL                   = 0x80000004u;
   constexpr auto V_OBJARRAY              = 0x80000005u;
   constexpr auto V_OBJECT                = 0x80000006u;

   constexpr auto V_SELF_VAR              = 0x80000081u;

   // === Operators ===
   constexpr auto OPERATOR_MAKS           = 0x1840;
   constexpr auto INDEX_OPERATOR_ID       = 0x0001;
   constexpr auto SET_OPERATOR_ID         = 0x0002;
   constexpr auto ADD_ASSIGN_OPERATOR_ID  = 0x0003;
   constexpr auto SET_INDEXER_OPERATOR_ID = 0x0201;

}

#endif
