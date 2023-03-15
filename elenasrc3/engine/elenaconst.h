//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Compiler Engine templates,
//		classes, structures, functions and constants
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENACONST_H
#define ELENACONST_H

namespace elena_lang
{
   // --- Common ELENA Engine constants ---
   #define ENGINE_MAJOR_VERSION           6                    // ELENA Engine version
   #define ENGINE_MINOR_VERSION           0

   constexpr auto LINE_LEN                = 0x1000;            // the maximal source line length
   constexpr auto IDENTIFIER_LEN          = 0x0300;            // the maximal identifier length
   constexpr auto MESSAGE_LEN             = 0x200;             // the maximal message length

  // --- ELENA Standart message constants ---
   constexpr auto ACTION_ORDER            = 9;

   constexpr auto ACTION_MASK             = 0x1C0u;
   constexpr auto MESSAGE_FLAG_MASK       = 0x1E0u;

   constexpr auto STATIC_MESSAGE          = 0x100u;
   constexpr auto FUNCTION_MESSAGE        = 0x020u;         // indicates it is an invoke message (without target variable in the call stack)
   constexpr auto CONVERSION_MESSAGE      = 0x040u;
   constexpr auto VARIADIC_MESSAGE        = 0x080u;
   constexpr auto PROPERTY_MESSAGE        = 0x0C0u;
   constexpr auto PREFIX_MESSAGE_MASK     = 0x0C0u;         // HOTFIX : is used to correctly identify VARIADIC_MESSAGE or PROPERTY_MESSAGE

   constexpr auto ARG_COUNT               = 0x01Eu;
   constexpr auto ARG_MASK                = 0x01Fu;

   // --- ELENA Module structure constants ---
   constexpr auto ELENA_SIGNITURE         = "ELENA.";          // the stand alone image
   constexpr auto ELENA_VM_SIGNITURE      = "VM.ELENA.";       // the stand alone image
   constexpr auto MODULE_SIGNATURE        = "ELENA.0601";      // the module version
   constexpr auto DEBUG_MODULE_SIGNATURE  = "ED.06";

  // --- ELENA core module names ---
   constexpr auto CORE_ALIAS                 = "core";            // Core functionality

   // --- ELENA predefined module names ---
   constexpr auto BINARY_MODULE              = "$binary";
   constexpr auto PREDEFINED_MODULE          = "system'predefined"; // NOTE : system'predefined module should preceed system one
   constexpr auto OPERATIONS_MODULE          = "system'operations"; // NOTE : system'predefined module should preceed system one
   constexpr auto STANDARD_MODULE            = "system";

   // --- ELENA special sections ---
   constexpr auto NAMESPACES_SECTION         = "$namespaces";
   constexpr auto IMPORTS_SECTION            = "$import";
   constexpr auto EXTENSION_SECTION          = "#extensions";

   constexpr auto NAMESPACE_REF              = "$namespace";

   // --- ELENA standard weak namespace
   constexpr auto RT_FORWARD                 = "$rt";
   constexpr auto ROOT_MODULE                = "$rootnamespace";  // The project namespace

   // --- ELENA standard forwards
   constexpr auto TEMPLATE_PREFIX_NS         = "'$auto'";
   constexpr auto TEMPLATE_PREFIX_NS_ENCODED = "@$auto@";
   constexpr auto FORWARD_PREFIX_NS          = "$forwards'";
   constexpr auto INLINE_CLASSNAME           = "$inline";          // nested class generic name

   constexpr auto PREDEFINED_MAP             = "$forwards'meta$predefined";
   constexpr auto ATTRIBUTES_MAP             = "$forwards'meta$attributes";
   constexpr auto OPERATION_MAP              = "$forwards'meta$statementTemplates";
   constexpr auto ALIASES_MAP                = "$forwards'meta$aliasTypes";
   constexpr auto STARTUP_LIST               = "$forwards'meta$startUpSymbols";
   constexpr auto STARTUP_ENTRY              = "$forwards'startUpSymbols";

   constexpr auto VM_TAPE                 = "$elena'meta$startUpTape";

   constexpr auto PROGRAM_ENTRY           = "$forwards'program";         // used by the linker to define the debug entry

   constexpr auto RETVAL_ARG              = "$retVal";
   constexpr auto PARENT_VAR              = "$parent";
   constexpr auto OWNER_VAR               = "$owner";

   constexpr auto SYSTEM_FORWARD          = "$system_entry";   // the system entry
   constexpr auto SUPER_FORWARD           = "$super";          // the common class predecessor
   constexpr auto INTLITERAL_FORWARD      = "$int";            // the int literal
   constexpr auto LONGLITERAL_FORWARD     = "$long";           // the long literal
   constexpr auto REALLITERAL_FORWARD     = "$real";           // the real literal
   constexpr auto INT8LITERAL_FORWARD     = "$byte";           // the int literal
   constexpr auto INT16LITERAL_FORWARD    = "$short";          // the int literal
   constexpr auto LITERAL_FORWARD         = "$string";         // the string literal
   constexpr auto WIDELITERAL_FORWARD     = "$wide";           // the wide string literal
   constexpr auto CHAR_FORWARD            = "$char";           // the char literal
   constexpr auto BOOL_FORWARD            = "$boolean";        // the boolean class
   constexpr auto TRUE_FORWARD            = "$true";           // the true boolean value
   constexpr auto FALSE_FORWARD           = "$false";          // the false boolean value
   constexpr auto WRAPPER_FORWARD         = "$ref";            // the wrapper template
   constexpr auto ARRAY_FORWARD           = "$array";          // the array template
   constexpr auto VARIADIC_ARRAY_FORWARD  = "$varray";         // the array template 
   constexpr auto MESSAGE_FORWARD         = "$message";        // the message class
   constexpr auto EXT_MESSAGE_FORWARD     = "$ext_message";    // the extension message class
   constexpr auto CLOSURE_FORWARD         = "$closure";        // the closure template class
   constexpr auto DWORD_FORWARD           = "$dword";          // the dword wrapper
   constexpr auto LAZY_FORWARD            = "$lazy";

   // --- ELENA section prefixes
   constexpr auto META_PREFIX             = "meta$";
   constexpr auto INLINE_PREFIX           = "inline$";
   constexpr auto INLINE_PROPERTY_PREFIX  = "prop$";

   // --- ELENA class prefixes / postfixes ---
   constexpr auto PRIVATE_PREFIX_NS       = "'$private'";
   constexpr auto INTERNAL_PREFIX_NS      = "'$intern'";

   constexpr auto TEMPLATE_PREFIX         = "'$auto";
   constexpr auto PRIVATE_PREFIX          = "'$private";
   constexpr auto INTERNAL_PREFIX         = "'$intern";

   constexpr auto CLASSCLASS_POSTFIX      = "#class";
   constexpr auto CONST_POSTFIX           = "#const";
   constexpr auto STATICFIELD_POSTFIX     = "#static";
   constexpr auto GENERIC_PREFIX          = "#generic";
   constexpr auto PARAMETER_NAMES         = "parameter_names";

   // --- ELENA verb messages ---
   constexpr auto DISPATCH_MESSAGE        = "#dispatch";
   constexpr auto CONSTRUCTOR_MESSAGE     = "#constructor";
   constexpr auto CONSTRUCTOR_MESSAGE2    = "#constructor2";   // protected constructor
   constexpr auto CAST_MESSAGE            = "#cast";
   constexpr auto INVOKE_MESSAGE          = "#invoke";
   constexpr auto TRY_INVOKE_MESSAGE      = "#try_invoke";
   constexpr auto INIT_MESSAGE            = "#init";

   constexpr auto ADD_MESSAGE             = "add";
   constexpr auto SUB_MESSAGE             = "subtract";
   constexpr auto MUL_MESSAGE             = "multiply";
   constexpr auto DIV_MESSAGE             = "divide";
   constexpr auto BAND_MESSAGE            = "band";
   constexpr auto BOR_MESSAGE             = "bor";
   constexpr auto BXOR_MESSAGE            = "bxor";
   constexpr auto REFER_MESSAGE           = "at";
   constexpr auto SET_REFER_MESSAGE       = "setAt";
   constexpr auto IF_MESSAGE              = "if";
   constexpr auto IIF_MESSAGE             = "iif";
   constexpr auto EQUAL_MESSAGE           = "equal";
   constexpr auto NOT_MESSAGE             = "Inverted";
   constexpr auto NEGATE_MESSAGE          = "Negative";
   constexpr auto VALUE_MESSAGE           = "Value";
   constexpr auto NOTEQUAL_MESSAGE        = "notequal";
   constexpr auto LESS_MESSAGE            = "less";
   constexpr auto NOTLESS_MESSAGE         = "notless";
   constexpr auto GREATER_MESSAGE         = "greater";
   constexpr auto NOTGREATER_MESSAGE      = "notgreater";

   // --- constant string lengths ---
   constexpr auto TEMPLATE_PREFIX_NS_LEN = 7;

   // --- ELENA VMT flags ---
   constexpr ref_t elStandartVMT          = 0x00000001;
   constexpr ref_t elClassClass           = 0x00000002;
   constexpr ref_t elStateless            = 0x00000004;
   constexpr ref_t elFinal                = 0x00000008;
   constexpr ref_t elClosed               = 0x00000010;
   constexpr ref_t elSealed               = 0x00000038;
   constexpr ref_t elNestedClass          = 0x00000040;
   constexpr ref_t elAutoLoaded           = 0x00000080;
   constexpr ref_t elRole                 = 0x00000100;
   constexpr ref_t elAbstract             = 0x00000200;
   constexpr ref_t elNoCustomDispatcher   = 0x00000400;
   constexpr ref_t elStructureRole        = 0x00000838;
   constexpr ref_t elReadOnlyRole         = 0x00001000;
   constexpr ref_t elNonStructureRole     = 0x00002000;
   constexpr ref_t elWrapper              = 0x00004000;
   constexpr ref_t elStructureWrapper     = 0x00004800;
   constexpr ref_t elDynamicRole          = 0x00008000;
   constexpr ref_t elExtension            = 0x0000110C;
   constexpr ref_t elMessage              = 0x00200000;
   constexpr ref_t elWithVariadics        = 0x00400000;
   constexpr ref_t elWithCustomDispatcher = 0x00800000;
   constexpr ref_t elWithGenerics         = 0x02000000;
   constexpr ref_t elTemplatebased        = 0x40000000;

   constexpr ref_t elDebugMask            = 0x001F0000;
   constexpr ref_t elDebugDWORD           = 0x00010000;
   constexpr ref_t elDebugQWORD           = 0x00020000;
   constexpr ref_t elDebugFLOAT64         = 0x00030000;
   constexpr ref_t elDebugDWORDS          = 0x00040000;
   constexpr ref_t elDebugLiteral         = 0x00050000;
   constexpr ref_t elDebugWideLiteral     = 0x00060000;
   constexpr ref_t elDebugArray           = 0x00070000;

   // --- LoadResult enum ---
   enum class LoadResult
   {
      Successful = 0,
      NotFound,
      WrongVersion,
      WrongStructure
   };

   // --- ELENA Platform type ---
   enum class PlatformType {
      None           = 0x00000, 

      // masks
      PlatformMask   = 0x000FF,
      TargetTypeMask = 0xFF000,

      Win_x86        = 0x00011,
      Win_x86_64     = 0x00012,

      Linux_x86      = 0x00021,
      Linux_x86_64   = 0x00022,

      Linux_ARM32    = 0x00023,
      Linux_ARM64    = 0x00024,

      Linux_PPC64le  = 0x00025,
      Linux_PPC32le  = 0x00026,

      TargetMask     = 0x00F00,
      Standalone     = 0x00000,
      VMClient       = 0x00100,

      UIMask         = 0x0F000,
      CUI            = 0x01000,
      GUI            = 0x02000,

      ThreadMask     = 0xF0000,
      SingleThread   = 0x00000,
      MultiThread    = 0x10000,

      // target types
      Library        = 0x00000,
      Console        = 0x01000,
   };

   // --- ELENA Debug symbol constants ---
   enum class DebugSymbol
   {
      DebugMask            = 0xFFF0,    
      None                 = 0x0000,

      Symbol               = 0x0011,
      Class                = 0x0012,
      Procedure            = 0x0013,
      Statement            = 0x0014,
      Breakpoint           = 0x0020,
      VirtualBreakpoint    = 0x0021,
      End                  = 0x0040,
      EndOfStatement       = 0x0041,

      Field                = 0x0050,
      FieldAddress         = 0x0051,

      Local                = 0x0100,
      LocalAddress         = 0x0101,
      IntLocalAddress      = 0x0102,
      LongLocalAddress     = 0x0103,
      RealLocalAddress     = 0x0104,
      ByteArrayAddress     = 0x0105,
      ShortArrayAddress    = 0x0106,
      IntArrayAddress      = 0x0107,

      Parameter            = 0x0200,
      IntParameterAddress  = 0x0202,
      LongParameterAddress = 0x0203,
      RealParameterAddress = 0x0204,
      ParameterAddress     = 0x0205,
      ByteArrayParameter   = 0x0206,
      ShortArrayParameter  = 0x0207,
      IntArrayParameter    = 0x0208,

      FrameInfo            = 0x0301,
      ClassInfo            = 0x0302,
      MessageInfo          = 0x0303,
   };

   // --- ClassAttribute ---
   enum class ClassAttribute
   {
      None              = 0x000,

      ReferenceMask     = 0x100,
      MessageMask       = 0x200,
      ReferenceKeyMask  = 0x400,
      MessageKeyMask    = 0x800,

      ProtectedAlias    = 0xA01,
      InternalAlias     = 0xA02,
      OverloadList      = 0x903,
      ConstantMethod    = 0x904,
      ExtensionRef      = 0x105,
      ParameterName     = 0x806,
      RuntimeLoadable   = 0x007,
      SealedStatic      = 0x908,
      SingleDispatch    = 0xA09,
      ExtOverloadList   = 0x90A,
   };

   // === Reference constants ====
   constexpr ref_t mskAnyRef              = 0xFF000000u;
   constexpr ref_t mskRefType             = 0xF0000000u;
   constexpr ref_t mskImageType           = 0x0F000000u;

   // --- Section reference types ---
   constexpr ref_t mskSymbolRef           = 0x01000000u;
   constexpr ref_t mskArrayRef            = 0x02000000u;
   constexpr ref_t mskExternalRef         = 0x03000000u;
   constexpr ref_t mskVMTRef              = 0x04000000u;
   constexpr ref_t mskMetaClassInfoRef    = 0x05000000u;
   constexpr ref_t mskClassRef            = 0x06000000u;
   constexpr ref_t mskAttributeMapRef     = 0x07000000u;
   constexpr ref_t mskTypeListRef         = 0x08000000u;
   constexpr ref_t mskLiteralListRef      = 0x09000000u;
   constexpr ref_t mskSyntaxTreeRef       = 0x0A000000u;
   constexpr ref_t mskProcedureRef        = 0x0B000000u;
   constexpr ref_t mskIntLiteralRef       = 0x0C000000u;
   constexpr ref_t mskTypeMapRef          = 0x0D000000u;
   constexpr ref_t mskLiteralRef          = 0x0E000000u;   // reference to constant literal
   constexpr ref_t mskVMTMethodAddress    = 0x0F000000u;
   constexpr ref_t mskVMTMethodOffset     = 0x10000000u;
   constexpr ref_t mskConstArray          = 0x11000000u;
   constexpr ref_t mskMessageBodyRef      = 0x12000000u;
   constexpr ref_t mskMetaSymbolInfoRef   = 0x13000000u;
   constexpr ref_t mskDeclAttributesRef   = 0x14000000u;
   constexpr ref_t mskMetaExtensionRef    = 0x15000000u;
   constexpr ref_t mskStaticRef           = 0x16000000u;
   constexpr ref_t mskCharacterRef        = 0x17000000u;   // reference to character literal
   constexpr ref_t mskConstant            = 0x18000000u;
   constexpr ref_t mskStaticVariable      = 0x19000000u;
   constexpr ref_t mskNameLiteralRef      = 0x1A000000u;
   constexpr ref_t mskPathLiteralRef      = 0x1B000000u;
   constexpr ref_t mskMssgLiteralRef      = 0x1C000000u;
   constexpr ref_t mskLabelRef            = 0x1D000000u;
   constexpr ref_t mskWideLiteralRef      = 0x1E000000u;   // reference to wide literal constant
   constexpr ref_t mskStringMapRef        = 0x1F000000u;
   constexpr ref_t mskLongLiteralRef      = 0x20000000u;
   constexpr ref_t mskRealLiteralRef      = 0x21000000u;
   constexpr ref_t mskExtMssgLiteralRef   = 0x22000000u;
   constexpr ref_t mskPSTRRef             = 0x23000000u;

   // --- Image reference types ---
   constexpr ref_t mskCodeRef             = 0x01000000u;
   constexpr ref_t mskRDataRef            = 0x02000000u;
   constexpr ref_t mskDataRef             = 0x03000000u;
   constexpr ref_t mskImportRef           = 0x04000000u;
   constexpr ref_t mskMBDataRef           = 0x05000000u;
   constexpr ref_t mskMDataRef            = 0x06000000u;
   constexpr ref_t mskStatDataRef         = 0x07000000u;

   // --- Address reference types ---
   // NOTE: Address reference types and Image reference types should not intersect
   constexpr ref_t mskRef32               = 0x80000000u;
   constexpr ref_t mskRelRef32            = 0x40000000u;
   constexpr ref_t mskRef64               = 0xC0000000u;
   constexpr ref_t mskRef32Hi             = 0x20000000u;         // <32 bit address> >> 16     
   constexpr ref_t mskRef32Lo             = 0xA0000000u;         // <32 bit address> & 0xFFFFF 
   constexpr ref_t mskDisp32Hi            = 0x60000000u;
   constexpr ref_t mskDisp32Lo            = 0xE0000000u;
   constexpr ref_t mskRelRef32Hi4k        = 0x10000000u;          // <32 bit address> >> 12    ; for ARM64 : it should be split: hi 2 bits goes to b30:29, the rest b23:5
   constexpr ref_t mskRef32Lo12           = 0x90000000u;          // <32 bit address> & 0xFFF  ; for ARM64 : it shoulb be b21:10
   constexpr ref_t mskRef32Lo12_8         = 0x50000000u;          // <32 bit address> & 0xFFF  ; for ARM64 : it shoulb be b21:10
   constexpr ref_t mskXDisp32Hi           = 0x30000000u;
   constexpr ref_t mskXDisp32Lo           = 0x70000000u;

   // --- VAddress reference types ----
   constexpr ref_t mskCodeRef32           = 0x81000000u;
   constexpr ref_t mskCodeRelRef32        = 0x41000000u;
   constexpr ref_t mskCodeRef64           = 0xC1000000u;
   constexpr ref_t mskCodeDisp32Hi        = 0x61000000u;
   constexpr ref_t mskCodeDisp32Lo        = 0xE1000000u;
   constexpr ref_t mskCodeRef32Hi         = 0x21000000u;
   constexpr ref_t mskCodeRef32Lo         = 0xA1000000u;
   constexpr ref_t mskCodeXDisp32Hi       = 0x31000000u;
   constexpr ref_t mskCodeXDisp32Lo       = 0x71000000u;

   constexpr ref_t mskRDataRef32          = 0x82000000u;
   constexpr ref_t mskRDataRef64          = 0xC2000000u;
   constexpr ref_t mskRDataRef32Hi        = 0x22000000u;
   constexpr ref_t mskRDataRef32Lo        = 0xA2000000u;
   constexpr ref_t mskRDataDisp32Hi       = 0x62000000u;
   constexpr ref_t mskRDataDisp32Lo       = 0xE2000000u;
   constexpr ref_t mskRDataXDisp32Hi      = 0x32000000u;
   constexpr ref_t mskRDataXDisp32Lo      = 0x72000000u;

   constexpr ref_t mskImportRef32         = 0x84000000u;
   constexpr ref_t mskImportRelRef32      = 0x44000000u;
   constexpr ref_t mskImportRef64         = 0xC4000000u;
   constexpr ref_t mskImportDisp32Hi      = 0x64000000u;
   constexpr ref_t mskImportDisp32Lo      = 0xE4000000u;
   constexpr ref_t mskImportRelRef32Hi4k  = 0x14000000u;
   constexpr ref_t mskImportRef32Lo12     = 0x94000000u;
   constexpr ref_t mskImportRef32Lo12_8   = 0x54000000u;
   constexpr ref_t mskImportRef32Hi       = 0x24000000u;
   constexpr ref_t mskImportRef32Lo       = 0xA4000000u;

   constexpr ref_t mskDataRef32           = 0x83000000u;
   constexpr ref_t mskDataRelRef32        = 0x43000000u;
   constexpr ref_t mskDataRef64           = 0xC3000000u;
   constexpr ref_t mskDataRef32Hi         = 0x23000000u;
   constexpr ref_t mskDataRef32Lo         = 0xA3000000u;
   constexpr ref_t mskDataDisp32Hi        = 0x63000000u;
   constexpr ref_t mskDataDisp32Lo        = 0xE3000000u;
   constexpr ref_t mskDataXDisp32Hi       = 0x33000000u;
   constexpr ref_t mskDataXDisp32Lo       = 0x73000000u;

   constexpr ref_t mskMBDataRef32         = 0x85000000u;
   constexpr ref_t mskMBDataRef64         = 0xC5000000u;

   constexpr ref_t mskMDataRef32          = 0x86000000u;
   constexpr ref_t mskMDataRef64          = 0xC6000000u;
   constexpr ref_t mskMDataRef32Hi        = 0x26000000u;
   constexpr ref_t mskMDataRef32Lo        = 0xA6000000u;
   constexpr ref_t mskMDataDisp32Hi       = 0x66000000u;
   constexpr ref_t mskMDataDisp32Lo       = 0xE6000000u;

   constexpr ref_t mskStatDataRef32       = 0x87000000u;
   constexpr ref_t mskStatDataRelRef32    = 0x47000000u;
   constexpr ref_t mskStatDataRef64       = 0xC7000000u;
   constexpr ref_t mskStatDataRef32Hi     = 0x27000000u;
   constexpr ref_t mskStatDataRef32Lo     = 0xA7000000u;
   constexpr ref_t mskStatXDisp32Hi       = 0x37000000u;
   constexpr ref_t mskStatXDisp32Lo       = 0x77000000u;

   // --- Address predefined references ---
   constexpr ref_t INV_ARG                = 0x00000100u;

   constexpr ref_t ARG32_1                = 0x00000001u;
   constexpr ref_t PTR32_1                = 0x00000002u;
   constexpr ref_t PTR64_1                = 0x00000003u;
   constexpr ref_t NARG_1                 = 0x00000004u;
   constexpr ref_t RELPTR32_1             = 0x00000005u;
   constexpr ref_t ARG16_1                = 0x00000006u;
   constexpr ref_t INV_ARG16_1            = 0x00000007u;
   constexpr ref_t DISP32HI_1             = 0x00000008u;
   constexpr ref_t DISP32LO_1             = 0x00000009u;
   constexpr ref_t ARG12_1                = 0x0000000Au;
   constexpr ref_t PTR32HI_1              = 0x0000000Bu;
   constexpr ref_t PTR32LO_1              = 0x0000000Cu;
   constexpr ref_t PTR32_2                = 0x0000000Du;
   constexpr ref_t NARG_2                 = 0x0000000Eu;
   constexpr ref_t ARG32_2                = 0x0000000Fu;
   constexpr ref_t PTR64_2                = 0x00000010u;
   constexpr ref_t INV_NARG16_2           = 0x00000011u;
   constexpr ref_t ARG16_2                = 0x00000012u;
   constexpr ref_t NARG16_2               = 0x00000013u;
   constexpr ref_t DISP32HI_2             = 0x00000014u;
   constexpr ref_t DISP32LO_2             = 0x00000015u;
   constexpr ref_t NARG12_2               = 0x00000016u;
   constexpr ref_t ARG12_2                = 0x00000017u;
   constexpr ref_t ARG9_1                 = 0x00000018u;
   constexpr ref_t INV_ARG12_1            = 0x00000019u;
   constexpr ref_t PTR32HI_2              = 0x0000001Au;
   constexpr ref_t PTR32LO_2              = 0x0000001Bu;
   constexpr ref_t ARG32HI_1              = 0x0000001Cu;
   constexpr ref_t ARG32LO_1              = 0x0000001Du;
   constexpr ref_t ARG64_2                = 0x0000001Eu;
   constexpr ref_t NARG16_1               = 0x0000001Fu;
   constexpr ref_t RELPTR32_2             = 0x00000021u;
   constexpr ref_t NARG12_1               = 0x00000022u;
   constexpr ref_t XDISP32HI_1            = 0x00000023u;
   constexpr ref_t XDISP32LO_1            = 0x00000024u;
   constexpr ref_t XDISP32HI_2            = 0x00000025u;
   constexpr ref_t XDISP32LO_2            = 0x00000026u;
   constexpr ref_t NARG16HI_1             = 0x00000027u;
   constexpr ref_t NARG16LO_1             = 0x00000028u;
   constexpr ref_t NARG16HI_2             = 0x00000029u;
   constexpr ref_t INV_ARG12_2            = 0x0000002Au;

   // predefined debug module sections
   constexpr ref_t DEBUG_LINEINFO_ID      = -1;
   constexpr ref_t DEBUG_STRINGS_ID       = -2;

   // === ELENA Error codes ===
   constexpr auto errNotImplemented = -3;
   constexpr auto errAborted        = -4;

} // _ELENA_

#endif // ELENACONST_H
