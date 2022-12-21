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
   enum class MethodHint : ref_t
   {
      Mask           = 0x0000000F,

      None           = 0x00000000,
      Normal         = 0x00000001,
      Sealed         = 0x00000003,
      Virtual        = 0x00000005,
      Dispatcher     = 0x00000007,

      Function       = 0x00000080,
      Generic        = 0x00000100,
      RetOverload    = 0x00000200,
      Multimethod    = 0x00001000,
      Static         = 0x00004000,
      GetAccessor    = 0x00008000,
      Abstract       = 0x00020000,
      Internal       = 0x00040000,
      Predefined     = 0x00080000, // virtual class declaration
      Stacksafe      = 0x00100000,
      Constructor    = 0x00200400,
      Conversion     = 0x00200800,
      SetAccessor    = 0x00400000,
      VirtualReturn  = 0x01000000,  // used for MutliRet with non-embeddable return type
      Constant       = 0x02000000,
      Protected      = 0x04000000,
      Private        = 0x08000000,
      Extension      = 0x10000000,
      Initializer    = 0x20000000,
      Autogenerated  = 0x40000000,
   };

   // === ELENA Error codes ===
   constexpr auto errInvalidSyntax           = 4;

   constexpr auto errDuplicatedSymbol        = 102;
   constexpr auto errDuplicatedMethod        = 103;
   constexpr auto errUnknownClass            = 104;
   constexpr auto errDuplicatedLocal         = 105;
   constexpr auto errUnknownObject           = 106;
   constexpr auto errInvalidOperation        = 107;
   constexpr auto errDuplicatedDictionary    = 108;
   constexpr auto errDuplicatedField         = 109;
   constexpr auto errUnknownVariableType     = 110;
   constexpr auto errIllegalField            = 111;
   constexpr auto errTooManyParameters       = 113;
   constexpr auto errDuplicatedDefinition    = 119;
   constexpr auto errInvalidIntNumber        = 130;
   constexpr auto errCannotEval              = 140;
   constexpr auto errSealedParent            = 141;
   constexpr auto errClosedParent            = 142;
   constexpr auto errInvalidHint             = 147;
   constexpr auto errIllegalConstructor      = 149;
   constexpr auto errClosedMethod            = 150;
   constexpr auto errIllegalStaticMethod     = 151;
   constexpr auto errIllegalMethod           = 152;
   constexpr auto errIllegalOperation        = 153;
   constexpr auto errTypeAlreadyDeclared     = 158;
   constexpr auto errAbstractMethods         = 159;
   constexpr auto errDispatcherInInterface   = 160;
   constexpr auto errAbstractMethodCode      = 161;
   constexpr auto errNotAbstractClass        = 164;
   constexpr auto errIllegalPrivate          = 166;
   constexpr auto errDupPublicMethod         = 167;
   constexpr auto errEmptyStructure          = 169;
   constexpr auto errInvalidType             = 172;
   constexpr auto errDupInternalMethod       = 173;
   constexpr auto errInvalidConstAttr        = 174;
   constexpr auto errIllegalConstructorAbstract = 177;
   constexpr auto errNoBodyMethod            = 180;
   constexpr auto errUnknownTemplate         = 181;
   constexpr auto errDupPrivateMethod        = 182;
   constexpr auto errDupProtectedMethod      = 183;
   constexpr auto errUnknownDefConstructor   = 184;
   constexpr auto errUnknownMessage          = 185;

   constexpr auto errUnknownModule           = 201;
   constexpr auto errUnresovableLink         = 202;
   constexpr auto errInvalidModule           = 203;
   constexpr auto errCannotCreate            = 204;
   constexpr auto errInvalidFile             = 205;
   constexpr auto errInvalidModuleVersion    = 210;
   constexpr auto errEmptyTarget             = 212;

   constexpr auto errParserNotInitialized    = 300;
   constexpr auto errProjectAlreadyLoaded    = 301;

   constexpr auto wrnUnknownHint             = 404;
   constexpr auto wrnInvalidHint             = 406;
   constexpr auto wrnUnknownMessage          = 407;
   constexpr auto wrnUnknownFunction         = 408;
   constexpr auto wrnUnknownDefConstructor   = 409;
   constexpr auto wrnUnknownModule           = 413;
   constexpr auto wrnTypeInherited           = 420;
   constexpr auto wrnDuplicateInclude        = 425;
   constexpr auto wrnUnknownTypecast         = 426;
   constexpr auto wrnUnsupportedOperator     = 427;

   constexpr auto wrnSyntaxFileNotFound      = 500;
   constexpr auto wrnInvalidConfig           = 501;

   constexpr auto errCommandSetAbsent        = 600;
   constexpr auto errReadOnlyModule          = 601;
   constexpr auto errNotDefinedBaseClass     = 602;
   constexpr auto errReferenceOverflow       = 603;
   constexpr auto errUnknownBaseClass        = 604;
   constexpr auto errNoDispatcher            = 605;
   constexpr auto errClosureError            = 606;

   constexpr auto infoNewMethod              = 701;
   constexpr auto infoCurrentMethod          = 702;
   constexpr auto infoCurrentClass           = 703;
   constexpr auto infoAbstractMetod          = 704;

   constexpr auto errVMBroken                = 800;
   constexpr auto errVMNotInitialized        = 801;
   constexpr auto errVMNotExecuted           = 802;
   constexpr auto errVMReferenceNotFound     = 803;

   constexpr auto errFatalError       = -1;
   constexpr auto errFatalLinker      = -2;
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

   /// accessors:
   constexpr auto V_GETACCESSOR           = 0x80005001u;
   constexpr auto V_SETACCESSOR           = 0x80005002u;

   /// visibility:
   constexpr auto V_PUBLIC                = 0x80004001u;
   constexpr auto V_PRIVATE               = 0x80004002u;
   constexpr auto V_INTERNAL              = 0x80004003u;
   constexpr auto V_PROTECTED             = 0x80004004u;

   /// property:
   constexpr auto V_SEALED                = 0x80003001u;
   constexpr auto V_ABSTRACT              = 0x80003002u;
   constexpr auto V_PREDEFINED            = 0x80003005u;

   /// scope_prefix:
   constexpr auto V_CONST                 = 0x80002001u;
   constexpr auto V_EMBEDDABLE            = 0x80002002u;
   constexpr auto V_WRAPPER               = 0x80002003u;
   constexpr auto V_OVERLOADRET           = 0x8000200Au;
   constexpr auto V_VARIADIC              = 0x8000200Bu;

   /// scope:
   constexpr auto V_CLASS                 = 0x80001001u;
   constexpr auto V_STRUCT                = 0x80001002u;
   constexpr auto V_SYMBOLEXPR            = 0x80001003u;
   constexpr auto V_CONSTRUCTOR           = 0x80001004u;
   constexpr auto V_EXTENSION             = 0x80001005u;
   constexpr auto V_SINGLETON             = 0x80001006u;
   constexpr auto V_LIMITED               = 0x80001007u;
   constexpr auto V_METHOD                = 0x80001008u;
   constexpr auto V_FIELD                 = 0x80001009u;
   constexpr auto V_NONESTRUCT            = 0x8000100Au;
   constexpr auto V_GENERIC               = 0x8000100Bu;
   constexpr auto V_FUNCTION              = 0x8000100Cu;     // a closure attribute
   constexpr auto V_VARIABLE              = 0x8000100Du;
   constexpr auto V_MEMBER                = 0x8000100Eu;
   constexpr auto V_STATIC                = 0x8000100Fu;
   constexpr auto V_CONVERSION            = 0x80001011u;
   constexpr auto V_NEWOP                 = 0x80001012u;
   constexpr auto V_DISPATCHER            = 0x80001013u;
   constexpr auto V_EXTERN                = 0x80001015u;
   constexpr auto V_INTERN                = 0x80001016u;
   constexpr auto V_FORWARD               = 0x80001017u;
   constexpr auto V_IMPORT                = 0x80001018u;
   constexpr auto V_AUTO                  = 0x8000101Cu;
   constexpr auto V_NAMESPACE             = 0x80001021u;
   constexpr auto V_SUPERIOR              = 0x80001024u;
   constexpr auto V_INLINE                = 0x80001025u;
   constexpr auto V_PROBEMODE             = 0x80001026u;
   constexpr auto V_TEMPLATE              = 0x80001027u;

   /// primitive type attribute
   constexpr auto V_STRINGOBJ             = 0x80000801u;
   constexpr auto V_FLOATBINARY           = 0x80000802u;
   constexpr auto V_INTBINARY             = 0x80000803u;
   //constexpr auto V_DECLOBJ               = 0x80000804u;
   constexpr auto V_WORDBINARY            = 0x80000805u;
   constexpr auto V_MSSGNAME              = 0x80000806u;
   constexpr auto V_SYMBOL                = 0x80000808u;
   constexpr auto V_MSSGBINARY            = 0x80000809u;

   /// primitive types
   constexpr auto V_STRING                = 0x80000001u;
   constexpr auto V_INT32                 = 0x80000002u;
   constexpr auto V_DICTIONARY            = 0x80000003u;
   constexpr auto V_NIL                   = 0x80000004u;
   constexpr auto V_OBJARRAY              = 0x80000005u;
   constexpr auto V_OBJECT                = 0x80000006u;
   constexpr auto V_FLAG                  = 0x80000007u;
   constexpr auto V_WORD32                = 0x80000008u;
   constexpr auto V_INT8                  = 0x80000009u;
   constexpr auto V_INT8ARRAY             = 0x8000000Au;
   constexpr auto V_BINARYARRAY           = 0x8000000Bu;
   constexpr auto V_ELEMENT               = 0x8000000Cu;
   constexpr auto V_MESSAGE               = 0x8000000Du;
   constexpr auto V_MESSAGENAME           = 0x8000000Eu;
   constexpr auto V_INT16                 = 0x8000000Fu;
   constexpr auto V_INT16ARRAY            = 0x80000010u;
   constexpr auto V_WIDESTRING            = 0x80000011u;
   constexpr auto V_OBJATTRIBUTES         = 0x80000012u;
   constexpr auto V_CLOSURE               = 0x80000013u;
   constexpr auto V_DECLARATION           = 0x80000014u;
   constexpr auto V_DECLATTRIBUTES        = 0x80000015u;
   constexpr auto V_ARGARRAY              = 0x80000016u;
   constexpr auto V_INT64                 = 0x80000017u;
   constexpr auto V_FLOAT64               = 0x80000018u;

   /// built-in variables
   constexpr auto V_SELF_VAR              = 0x80000081u;
   constexpr auto V_DECL_VAR              = 0x80000082u;
   constexpr auto V_SUPER_VAR             = 0x80000083u;

   // === Operators ===
   constexpr auto OPERATOR_MAKS              = 0x1840;
   constexpr auto INDEX_OPERATOR_ID          = 0x0001;
   constexpr auto SET_OPERATOR_ID            = 0x0002;
   constexpr auto ADD_ASSIGN_OPERATOR_ID     = 0x0003;
   constexpr auto ADD_OPERATOR_ID            = 0x0004;
   constexpr auto SUB_OPERATOR_ID            = 0x0005;
   constexpr auto LEN_OPERATOR_ID            = 0x0006;
   constexpr auto IF_OPERATOR_ID             = 0x0007;
   constexpr auto LESS_OPERATOR_ID           = 0x0008;
   constexpr auto NAME_OPERATOR_ID           = 0x0009;
   constexpr auto EQUAL_OPERATOR_ID          = 0x000A;
   constexpr auto NOT_OPERATOR_ID            = 0x000B;
   constexpr auto NOTEQUAL_OPERATOR_ID       = 0x000C;
   constexpr auto ELSE_OPERATOR_ID           = 0x000E;
   constexpr auto IF_ELSE_OPERATOR_ID        = 0x000F;
   constexpr auto MUL_OPERATOR_ID            = 0x0010;
   constexpr auto DIV_OPERATOR_ID            = 0x0011;
   constexpr auto NOTLESS_OPERATOR_ID        = 0x0012;
   constexpr auto GREATER_OPERATOR_ID        = 0x0013;
   constexpr auto NOTGREATER_OPERATOR_ID     = 0x0014;
   constexpr auto NEGATE_OPERATOR_ID         = 0x0016;
   constexpr auto VALUE_OPERATOR_ID          = 0x0017;
   constexpr auto BAND_OPERATOR_ID           = 0x0018;
   constexpr auto BOR_OPERATOR_ID            = 0x0019;
   constexpr auto BXOR_OPERATOR_ID           = 0x001A;
   constexpr auto BNOT_OPERATOR_ID           = 0x001B;
   constexpr auto SHL_OPERATOR_ID            = 0x001C;
   constexpr auto SHR_OPERATOR_ID            = 0x001D;
   constexpr auto SUB_ASSIGN_OPERATOR_ID     = 0x001E;
   constexpr auto MUL_ASSIGN_OPERATOR_ID     = 0x001F;
   constexpr auto DIV_ASSIGN_OPERATOR_ID     = 0x0020;
   constexpr auto AND_OPERATOR_ID            = 0x0021;
   constexpr auto OR_OPERATOR_ID             = 0x0022;
   constexpr auto XOR_OPERATOR_ID            = 0x0023;

   constexpr auto ISNIL_OPERATOR_ID          = 0x003E;
   constexpr auto CLASS_OPERATOR_ID          = 0x003F;
   constexpr auto SET_INDEXER_OPERATOR_ID    = 0x0201;

   // === Conversion Routines ===
   constexpr auto INT32_64_CONVERSION        = 0x001;
   constexpr auto INT32_FLOAT64_CONVERSION   = 0x002;

   // === VM Command ===
   constexpr pos_t VM_STR_COMMAND_MASK       = 0x100;

   constexpr pos_t VM_ENDOFTAPE_CMD          = 0x001;
   constexpr pos_t VM_LOADSYMBOLARRAY_CMD    = 0x102;
   constexpr pos_t VM_SETNAMESPACE_CMD       = 0x103;
   constexpr pos_t VM_SETPACKAGEPATH_CMD     = 0x104;
   constexpr pos_t VM_INIT_CMD               = 0x005;
   constexpr pos_t VM_FORWARD_CMD            = 0x106;

   // --- Configuration xpaths ---
   constexpr auto WIN_X86_KEY = "Win_x86";
   constexpr auto WIN_X86_64_KEY = "Win_x64";
   constexpr auto LINUX_X86_KEY = "Linux_I386";
   constexpr auto LINUX_X86_64_KEY = "Linux_AMD64";
   constexpr auto LINUX_PPC64le_KEY = "Linux_PPC64le";
   constexpr auto LINUX_ARM64_KEY = "Linux_ARM64";
   constexpr auto LIBRARY_KEY = "Library";
   constexpr auto CONSOLE_KEY = "STA Console";
   constexpr auto VM_CONSOLE_KEY = "VM STA Console";

   constexpr auto CONFIG_ROOT = "configuration";
   constexpr auto PLATFORM_CATEGORY = "configuration/platform";
   constexpr auto TEMPLATE_CATEGORY = "templates/*";
   constexpr auto PRIMITIVE_CATEGORY = "primitives/*";
   constexpr auto FORWARD_CATEGORY = "forwards/*";
   constexpr auto EXTERNAL_CATEGORY = "externals/*";
   constexpr auto WINAPI_CATEGORY = "winapi/*";
   constexpr auto REFERENCE_CATEGORY = "references/*";
   constexpr auto MODULE_CATEGORY = "files/*";
   constexpr auto FILE_CATEGORY = "include/*";

   constexpr auto LIB_PATH = "project/libpath";
   constexpr auto OUTPUT_PATH = "project/output";
   constexpr auto TARGET_PATH = "project/executable";
   constexpr auto PROJECT_TEMPLATE = "project/template";
   constexpr auto NAMESPACE_KEY = "project/namespace";
   constexpr auto DEBUGMODE_PATH = "project/debuginfo";
   constexpr auto FILE_PROLOG = "project/prolog";
   constexpr auto FILE_EPILOG = "project/epilog";

   constexpr auto PLATFORMTYPE_KEY = "system/platform";

   constexpr auto MGSIZE_PATH = "linker/mgsize";
   constexpr auto YGSIZE_PATH = "linker/ygsize";

   inline ustr_t getPlatformName(PlatformType type)
   {
      switch (type) {
      case PlatformType::Win_x86:
         return WIN_X86_KEY;
      case PlatformType::Win_x86_64:
         return WIN_X86_64_KEY;
      case PlatformType::Linux_x86:
         return LINUX_X86_KEY;
      case PlatformType::Linux_x86_64:
         return LINUX_X86_64_KEY;
      case PlatformType::Linux_PPC64le:
         return LINUX_PPC64le_KEY;
      case PlatformType::Linux_ARM64:
         return LINUX_ARM64_KEY;
      default:
         return nullptr;
      }
   }

}

#endif
