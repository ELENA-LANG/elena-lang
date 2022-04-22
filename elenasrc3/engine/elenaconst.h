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

   constexpr auto CONVERSION_MESSAGE      = 0x040u;
   constexpr auto STATIC_MESSAGE          = 0x100u;

   constexpr auto ARG_COUNT               = 0x01Eu;
   constexpr auto ARG_MASK                = 0x01Fu;

   // --- ELENA Module structure constants ---
   constexpr auto ELENA_SIGNITURE         = "ELENA.";          // the stand alone image
   constexpr auto MODULE_SIGNATURE        = "ELENA.0601";      // the module version
   constexpr auto DEBUG_MODULE_SIGNATURE  = "ED.06";

  // --- ELENA core module names ---
   constexpr auto CORE_ALIAS              = "core";            // Core functionality

   // --- ELENA predefined module names ---
   constexpr auto BINARY_MODULE           = "$binary";
   constexpr auto PREDEFINED_MODULE       = "system'predefined"; // NOTE : system'predefined module should preceed system one
   constexpr auto STANDARD_MODULE         = "system";

   // --- ELENA special sections ---
   constexpr auto NAMESPACES_SECTION      = "$namespaces";
   constexpr auto IMPORTS_SECTION         = "$import";
   constexpr auto NAMESPACE_REF           = "$namespace";

   // --- ELENA standard weak namespace
   constexpr auto RT_FORWARD              = "$rt";
   constexpr auto ROOT_MODULE             = "$rootnamespace";  // The project namespace

   // --- ELENA standard forwards
   constexpr auto TEMPLATE_PREFIX_NS      = "'$auto'";
   constexpr auto FORWARD_PREFIX_NS       = "$forwards'";
   constexpr auto INLINE_CLASSNAME        = "$inline";          // nested class generic name

   constexpr auto PREDEFINED_FORWARD      = "$forwards'meta$predefined";
   constexpr auto ATTRIBUTES_FORWARD      = "$forwards'meta$attributes";
   constexpr auto ALIASES_FORWARD         = "$forwards'meta$aliasTypes";
   constexpr auto SYSTEM_ENTRY            = "$forwards'$system_entry";   // the system entry
   constexpr auto PROGRAM_ENTRY           = "$forwards'program";         // used by the linker to define the debug entry

   constexpr auto SUPER_FORWARD           = "$forwards'$super";          // the common class predecessor
   constexpr auto INTLITERAL_FORWARD      = "$forwards'$int";          // the common class predecessor
   constexpr auto LITERAL_FORWARD         = "$forwards'$string";          // the common class predecessor

   // --- ELENA section prefixes
   constexpr auto META_PREFIX             = "meta$";
   constexpr auto INLINE_PREFIX           = "inline$";

   // --- ELENA class prefixes / postfixes ---
   constexpr auto PRIVATE_PREFIX_NS       = "'$private'";
   constexpr auto INTERNAL_PREFIX_NS      = "'$intern'";

   constexpr auto CLASSCLASS_POSTFIX      = "#class";

   // --- ELENA verb messages ---
   constexpr auto DISPATCH_MESSAGE        = "#dispatch";
   constexpr auto CONSTRUCTOR_MESSAGE     = "#constructor";
   constexpr auto CAST_MESSAGE            = "#cast";

   // --- constant string lengths ---
   constexpr auto TEMPLATE_PREFIX_NS_LEN = (sizeof(TEMPLATE_PREFIX_NS) - 1);

   // --- ELENA VMT flags ---
   constexpr ref_t elStandartVMT          = 0x00000001;
   constexpr ref_t elClassClass           = 0x00000002;
   constexpr ref_t elStateless            = 0x00000004;
   constexpr ref_t elFinal                = 0x00000008;
   constexpr ref_t elClosed               = 0x00000010;
   constexpr ref_t elSealed               = 0x00000038;
   constexpr ref_t elRole                 = 0x00000100;
   constexpr ref_t elAbstract             = 0x00000200;
   constexpr ref_t elNoCustomDispatcher   = 0x00000400;
   constexpr ref_t elStructureRole        = 0x00000838;
   constexpr ref_t elReadOnlyRole         = 0x00001000;
   constexpr ref_t elNonStructureRole     = 0x00002000;
   constexpr ref_t elWrapper              = 0x00004000;
   constexpr ref_t elStructureWrapper     = 0x00004800;
   constexpr ref_t elDynamicRole          = 0x00008000;

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
      TargetTypeMask = 0xFFF00,

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
      None           = 0x0000,

      Symbol,
      Class,
      Procedure,
      Statement,
      Breakpoint,
      End
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
   constexpr ref_t mskMetaDictionaryRef   = 0x07000000u;
   constexpr ref_t mskMetaArrayRef        = 0x08000000u;
   constexpr ref_t mskStrMetaArrayRef     = 0x09000000u;
   constexpr ref_t mskSyntaxTreeRef       = 0x0A000000u;
   constexpr ref_t mskProcedureRef        = 0x0B000000u;
   constexpr ref_t mskIntLiteralRef       = 0x0C000000u;
   constexpr ref_t mskMetaAttributesRef   = 0x0D000000u;
   constexpr ref_t mskLiteralRef          = 0x0E000000u;   // reference to constant literal
   constexpr ref_t mskVMTMethodAddress    = 0x0F000000u;
   constexpr ref_t mskVMTMethodOffset     = 0x10000000u;
   constexpr ref_t mskConstArray          = 0x11000000u;

   // --- Image reference types ---
   constexpr ref_t mskCodeRef             = 0x01000000u;
   constexpr ref_t mskRDataRef            = 0x02000000u;
   constexpr ref_t mskDataRef             = 0x03000000u;
   constexpr ref_t mskImportRef           = 0x04000000u;
   constexpr ref_t mskMessageBodyRef      = 0x05000000u;

   // --- Address reference types ---
   // NOTE: Address reference types and Image reference types should not intersect
   constexpr ref_t mskRef32               = 0x80000000u;
   constexpr ref_t mskRelRef32            = 0x40000000u;
   constexpr ref_t mskRef64               = 0xC0000000u;
   constexpr ref_t mskRef32Hi             = 0x20000000u;         // <32 bit address> >> 12    ; for ARM64 : it should be b20:5
   constexpr ref_t mskRef32Lo             = 0xA0000000u;         // <32 bit address> & 0xFFFF ; for ARM64 : it should be b20:5   
   constexpr ref_t mskDisp32Hi            = 0x60000000u;
   constexpr ref_t mskDisp32Lo            = 0xE0000000u;
   constexpr ref_t mskRelRef32Hi4k        = 0x10000000u;          // <32 bit address> >> 12    ; for ARM64 : it should be split: hi 2 bits goes to b30:29, the rest b23:5
   constexpr ref_t mskRef32Lo12           = 0x90000000u;          // <32 bit address> & 0xFFF  ; for ARM64 : it shoulb be b21:10
   constexpr ref_t mskRef32Lo12_8         = 0x50000000u;          // <32 bit address> & 0xFFF  ; for ARM64 : it shoulb be b21:10

   // --- VAddress reference types ----
   constexpr ref_t mskCodeRef32           = 0x81000000u;
   constexpr ref_t mskCodeRelRef32        = 0x41000000u;
   constexpr ref_t mskCodeRef64           = 0xC1000000u;
   constexpr ref_t mskCodeDisp32Hi        = 0x61000000u;
   constexpr ref_t mskCodeDisp32Lo        = 0xE1000000u;
   constexpr ref_t mskCodeRef32Hi         = 0x21000000u;
   constexpr ref_t mskCodeRef32Lo         = 0xA1000000u;

   constexpr ref_t mskRDataRef32          = 0x82000000u;
   constexpr ref_t mskRDataRef64          = 0xC2000000u;
   constexpr ref_t mskRDataRef32Hi        = 0x22000000u;
   constexpr ref_t mskRDataRef32Lo        = 0xA2000000u;
   constexpr ref_t mskRDataDisp32Hi       = 0x62000000u;
   constexpr ref_t mskRDataDisp32Lo       = 0xE2000000u;

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
   constexpr ref_t NARGHI_1               = 0x00000020u;
   constexpr ref_t RELPTR32_2             = 0x00000021u;

   // predefined debug module sections
   constexpr ref_t DEBUG_LINEINFO_ID      = -1;
   constexpr ref_t DEBUG_STRINGS_ID       = -2;

   // === ELENA Error codes ===
   constexpr auto errNotImplemented = -3;

} // _ELENA_

#endif // ELENACONST_H
