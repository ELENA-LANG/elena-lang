//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Engine constants
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenaconstH
#define elenaconstH 1

namespace _ELENA_
{
  // --- Common ELENA Engine constants ---
   #define ENGINE_MAJOR_VERSION           5                 // ELENA Engine version
   #define ENGINE_MINOR_VERSION           10

   constexpr auto LINE_LEN                = 0x1000;         // the maximal source line length
   constexpr auto IDENTIFIER_LEN          = 0x0300;         // the maximal identifier length

  // --- ELENA Standart message constants ---
   constexpr int ACTION_ORDER             = 9;

   constexpr auto ACTION_MASK             = 0x1C0u;
   constexpr auto MESSAGE_FLAG_MASK       = 0x1E0u;

   constexpr auto FUNCTION_MESSAGE        = 0x020u;         // indicates it is an invoke message (without target variable in the call stack)
   constexpr auto VARIADIC_MESSAGE        = 0x080u;
   constexpr auto CONVERSION_MESSAGE      = 0x040u;
   constexpr auto PROPERTY_MESSAGE        = 0x0C0u;
   constexpr auto PREFIX_MESSAGE_MASK     = 0x0C0u;         // HOTFIX : is used to correctly identify VARIADIC_MESSAGE or PROPERTY_MESSAGE

   constexpr auto STATIC_MESSAGE          = 0x100u;
   constexpr auto ARG_COUNT               = 0x01Eu;
   constexpr auto ARG_MASK                = 0x01Fu;
   constexpr auto ARGX_MASK               = 0x000000000000001Fu;

   constexpr auto INVALID_REF             = (ref_t)0xFFFFFFFFu;
   constexpr auto INVALID_PTR             = (uintptr_t)-1;
   constexpr auto INVALID_VADDR           = (lvaddr_t)-1;

   constexpr auto EQUAL_OPERATOR_ID       = 0x0004;
   constexpr auto SET_OPERATOR_ID         = 0x0007;
   constexpr auto LESS_OPERATOR_ID        = 0x0008;
   constexpr auto IF_OPERATOR_ID          = 0x0009;
   constexpr auto AND_OPERATOR_ID         = 0x000A;
   constexpr auto OR_OPERATOR_ID          = 0x000B;
   constexpr auto XOR_OPERATOR_ID         = 0x000C;
   constexpr auto IFNOT_OPERATOR_ID       = 0x000D;
   constexpr auto NOTEQUAL_OPERATOR_ID    = 0x000E;
   constexpr auto NOTLESS_OPERATOR_ID     = 0x000F;
   constexpr auto NOTGREATER_OPERATOR_ID  = 0x0010;
   constexpr auto GREATER_OPERATOR_ID     = 0x0011;
   constexpr auto ADD_OPERATOR_ID         = 0x0012;
   constexpr auto SUB_OPERATOR_ID         = 0x0013;
   constexpr auto MUL_OPERATOR_ID         = 0x0014;
   constexpr auto DIV_OPERATOR_ID         = 0x0015;
   constexpr auto REFER_OPERATOR_ID       = 0x0016;
   constexpr auto APPEND_OPERATOR_ID      = 0x0017;
   constexpr auto REDUCE_OPERATOR_ID      = 0x0018;
   constexpr auto SET_REFER_OPERATOR_ID   = 0x0019;
   constexpr auto SHIFTR_OPERATOR_ID      = 0x001A;
   constexpr auto SHIFTL_OPERATOR_ID      = 0x001B;
   constexpr auto INCREASE_OPERATOR_ID    = 0x001C;
   constexpr auto SEPARATE_OPERATOR_ID    = 0x001D;
   constexpr auto FINALLY_OPERATOR_ID     = 0x001E;
   constexpr auto INVERTED_OPERATOR_ID    = 0x001F;
   constexpr auto NEGATIVE_OPERATOR_ID    = 0x0020;
   constexpr auto VALUE_OPERATOR_ID       = 0x0021;
   constexpr auto ISNIL_OPERATOR_ID       = 0x0022;
   constexpr auto CATCH_OPERATOR_ID       = 0x0023;
   constexpr auto ALT_OPERATOR_ID         = 0x0024;
   constexpr auto LEN_OPERATOR_ID         = 0x0025;
   constexpr auto BAND_OPERATOR_ID        = 0x0026;
   constexpr auto BOR_OPERATOR_ID         = 0x0027;
   constexpr auto BXOR_OPERATOR_ID        = 0x0028;
   constexpr auto BINVERTED_OPERATOR_ID   = 0x0029;
   constexpr auto BAPPEND_OPERATOR_ID     = 0x002A;
   constexpr auto BINCREASE_OPERATOR_ID   = 0x002B;

   // built-in class attributes values
   constexpr auto CLASSNAME_CONST         = 0x62000001u;
   constexpr auto PACKAGE_CONST           = 0x62000002u;
//   constexpr auto PACKAGE_ATTR_INDEX      = -5;
//
////   // virtual operator
////   #define SETNIL_REFER_MESSAGE_ID 0x1019

   // ---- ELENAVM command masks ---
   constexpr auto VM_MASK                 = 0x0200;             // vm command mask
   constexpr auto LITERAL_ARG_MASK        = 0x0400;             // indicates that the command has a literal argument

   // ---- ELENAVM commands ---
   constexpr auto START_VM_MESSAGE_ID     = 0x02F1;             // restart VM
   constexpr auto MAP_VM_MESSAGE_ID       = 0x06F2;             // map forward reference
   constexpr auto USE_VM_MESSAGE_ID       = 0x06F3;             // set current package
   constexpr auto LOAD_VM_MESSAGE_ID      = 0x06F4;             // load template
   constexpr auto OPEN_VM_CONSOLE         = 0x02F5;             // open console
   constexpr auto EXT_DISPATCHER_ON       = 0x02F6;             // turn on with-ext-dispatcher-mode

   // ---- ELENAVM interpreter commands ---
   constexpr auto CALL_TAPE_MESSAGE_ID    = 0x05E0;             // call symbol
   constexpr auto ARG_TAPE_MESSAGE_ID     = 0x05E1;             // define the second parameter
//   constexpr auto PUSH_VAR_MESSAGE_ID     = 0x01E2;             // copy the data
//   constexpr auto ASSIGN_VAR_MESSAGE_ID   = 0x01E3;             // assign the data
   constexpr auto PUSH_TAPE_MESSAGE_ID    = 0x05E4;             // push constant
   constexpr auto PUSHS_TAPE_MESSAGE_ID   = 0x05E5;             // push literal constant
   constexpr auto PUSHN_TAPE_MESSAGE_ID   = 0x05E6;             // push integer constant
   constexpr auto PUSHR_TAPE_MESSAGE_ID   = 0x05E7;             // push floating numeric constant
   constexpr auto PUSHL_TAPE_MESSAGE_ID   = 0x05E8;             // push long integer constant
   constexpr auto PUSHM_TAPE_MESSAGE_ID   = 0x05E9;             // push message reference
   constexpr auto PUSHG_TAPE_MESSAGE_ID   = 0x05EA;             // push the subject reference
   constexpr auto POP_TAPE_MESSAGE_ID     = 0x01EB;             // free the stack content
   constexpr auto SEND_TAPE_MESSAGE_ID    = 0x05EC;             // send the message
   constexpr auto PUSHE_TAPE_MESSAGE_ID   = 0x05EE;             // push message reference

   constexpr auto NEW_TAPE_MESSAGE_ID     = 0x01F0;             // create an object

   constexpr auto VA_ALIGNMENT            = 0x08;
   constexpr auto VA_ALIGNMENT_POWER      = 0x03;

  // --- ELENA Reference masks ---
   enum ReferenceType : unsigned int
   {
      // masks
      mskAnyRef              = 0xFF000000u,
      mskImageMask           = 0xF0000000u,
      mskTypeMask            = 0x0F000000u,

      mskRelRef              = 0x10000000u,
      mskCodeRef             = 0x20000000u,
      mskRelCodeRef          = 0x30000000u,
      mskRDataRef            = 0x40000000u,
      mskRelRDataRef         = 0x50000000u,
      mskMetaRef             = 0x60000000u,
      mskStatRef             = 0x80000000u,
      mskRelStatRef          = 0x90000000u,
      mskDataRef             = 0xA0000000u,
      mskRelDataRef          = 0xB0000000u,
      mskTLSRef              = 0xC0000000u,
      mskImportRef           = 0xE0000000u,
      mskRelExternakRef      = 0xF0000000u, // in fact the same as mskRelImportRef, but should contain only image part

      mskRelImportRef        = 0xFF000000u,

      mskNativeCodeRef       = 0x28000000u,
      mskNativeRelCodeRef    = 0x38000000u,
      mskNativeRDataRef      = 0x48000000u,
      mskNativeDataRef       = 0xA8000000u,
      mskNativeRelDataRef    = 0xB8000000u,

      mskPreloadCodeRef      = 0x2C000000u,
      mskPreloadRelCodeRef   = 0x3C000000u,
      mskPreloadDataRef      = 0xAC000000u,
      mskPreloadRelDataRef   = 0xBC000000u,
//      mskNativeVariable      = 0xAD000000u,
//      mskConstVariable       = 0x4D000000u,
      mskLockVariable        = 0xAE000000u,   // HOTFIX : used to fool trylock opcode, adding virtual offset

      mskInternalRef         = 0x23000000u,   // internal code
      mskInternalRelRef      = 0x33000000u,   // internal code
      mskSymbolRef           = 0x22000000u,   // symbol code
      mskSymbolRelRef        = 0x32000000u,   // symbol code
      mskVMTRef              = 0x41000000u,   // class VMT
      mskClassRef            = 0x21000000u,   // class code
//      mskClassRelRef         = 0x31000000u,   // class relative code
      mskStatSymbolRef       = 0x82000000u,   // reference to static symbol
      mskStatSymbolRelRef    = 0x92000000u,
      mskEntryRef            = 0x24000000u,   // reference to the program entry
      mskEntryRelRef         = 0x34000000u,   // reference to the program entry

      mskVMTMethodAddress    = 0x43000000u,   // the method address, where the reference offset is a message id, reference values is VMT
      mskMetaRDataRef        = 0x44000000u,   // meta data
      mskVMTEntryOffset      = 0x45000000u,   // the message offset in VMT, where the reference offset is a message id, reference values is VMT
      mskSyntaxTreeRef       = 0x46000000u,   // template, declared in subject namespace
      mskAttributeRef        = 0x47000000u,
      mskVMTXMethodAddress   = 0x49000000u,   // VMTX method address, where the reference offset (64bit) is a message id, reference values is VMT
      mskVMTXEntryOffset     = 0x4A000000u,   // the message offset in VMTX, where the reference offset (64bit) is a message id, reference values is VMTX

      mskConstantRef         = 0x01000000u,   // reference to constant
      mskLiteralRef          = 0x02000000u,   // reference to constant literal
      mskInt32Ref            = 0x03000000u,   // reference to constant integer number
      mskInt64Ref            = 0x04000000u,   // reference to constant 64bit integer number
      mskRealRef             = 0x05000000u,   // reference to constant real number
      mskMessage             = 0x06000000u,   // message constant
      mskCharRef             = 0x07000000u,   // reference to character constant
      mskWideLiteralRef      = 0x08000000u,   // reference to constant wide literal
      mskMessageName         = 0x09000000u,   // message action constant
      mskExtMessage          = 0x0B000000u,   // external message verb constant
      mskPreloaded           = 0x0C000000u,   // preloaded mask, should be used in combination with image mask
      mskConstArray          = 0x0D000000u,   // constant array

      mskDebugRef            = 0x60000000u,
      mskMetaAttributes      = 0x61000000u,
      mskMessageTableRef     = 0x62000000u,
      mskMetaConstant        = 0x62000000u,
      mskSymbolAttributeRef  = 0x67000000u,
   };

   // --- ELENA Debug symbol constants ---
   enum DebugSymbol
   {
      dsNone                    = 0x0000,

      dsStep                    = 0x0010,
      dsEOP                     = 0x0011,    // end of procedure
      dsVirtualEnd              = 0x0013,    // virtual end of expreession; it should be skipped by debugger
      dsProcedureStep           = 0x0014,    // check the step result
      dsAtomicStep              = 0x0018,    // "step into" is always treated as step over, used for external code
      dsAssemblyStep            = 0x0110,    // the step requires some "disassemblying"

      dsSymbol                  = 0x0001,
      dsClass                   = 0x0002,
      dsField                   = 0x0004,
      dsLocal                   = 0x0005,
      dsMessage                 = 0x0006,
      dsProcedure               = 0x0007,
      dsCodeInfo                = 0x0008,
//      dsStack                   = 0x0009,
      dsStatement               = 0x000A,
      dsVirtualBlock            = 0x000B,
      dsEnd                     = 0x000F,
      dsIntLocal                = 0x0105,
      dsLongLocal               = 0x0205,
      dsRealLocal               = 0x0305,
      dsParamsLocal             = 0x0405,
      dsByteArrayLocal          = 0x0505,
      dsShortArrayLocal         = 0x0605,
      dsIntArrayLocal           = 0x0705,

      // primitive variables
      dsIntLocalPtr             = 0x0805,
      dsLongLocalPtr            = 0x0905,
      dsRealLocalPtr            = 0x0A05,
      dsByteArrayLocalPtr       = 0x0B05,
      dsShortArrayLocalPtr      = 0x0C05,
      dsIntArrayLocalPtr        = 0x0D05,
      dsStructPtr               = 0x0E05,
      dsStructInfo              = 0x0F05,
      dsLocalPtr                = 0x1005,
      dsFrameOffset             = 0x0009,

      dsFieldInfo               = 0x0F04,

      dsDebugMask               = 0x00F0,
      dsTypeMask                = 0x1F00,
//      dsDebugTypeMask           = 0x1FFF,
   };

   // predefined debug module sections
   #define DEBUG_LINEINFO_ID      (pos_t)-1
   #define DEBUG_STRINGS_ID       (pos_t)-2

   // --- LoadResult enum ---
   enum LoadResult
   {
      lrSuccessful = 0,
      lrNotFound,
      lrWrongVersion,
      lrWrongStructure,
      lrDuplicate,
      lrCannotCreate
   };

  // --- ELENA Platform type ---
   enum PlatformType {
      // masks
      mtPlatformMask     = 0x000FF,
      mtWin32            = 0x00001,
      mtLinux32          = 0x00002,
      mtWin64            = 0x00081,

      mtTargetMask       = 0x00F00,
      mtStandalone       = 0x00000,
      mtVMClient         = 0x00100,

      mtUIMask           = 0x0F000,
      mtCUI              = 0x00000,
      mtGUI              = 0x01000,

      mtThreadMask       = 0xF0000,
      mtSingleThread     = 0x00000,
      mtMultyThread      = 0x10000,

      ptLibrary          = 0x00000,
      ptWin32Console     = 0x00001,
      ptWin64Console     = 0x00081,
      ptWin32GUI         = 0x01001,
      ptVMWin32Console   = 0x00101,
      ptVMWin64Console   = 0x00181,
      ptWin32ConsoleX    = 0x10001,
//      ptWin32GUIX        = 0x11001,
//      ptVMWin32GUI       = 0x01101,
      ptLinux32Console   = 0x00002,
      ptVMLinux32Console = 0x00102,
      ptLinux64Console   = 0x00082,
   };

//  // --- ELENA Debug Mode ---
//   enum DebugMode {
//      dbmNone       =  0,
//      dbmActive     = -1
//   };

   // --- ELENA Parse Table constants ---
   constexpr int mskTerminal           = 0x2000;
   constexpr int nsEps                 = 2;

   constexpr int cnHashSize            = 0x0100;              // the parse table hash size
   constexpr int cnTablePower          = 0x0010;
   constexpr int cnTableKeyPower       = cnTablePower + 1;
   constexpr int cnSyntaxPower         = 0x0008;

  // --- ELENA VMT flags ---
   constexpr int elStandartVMT         = 0x00000001;
   constexpr int elVirtualVMT          = 0x00000002;
   constexpr int elStateless           = 0x00000004;
   constexpr int elFinal               = 0x00000008;
   constexpr int elClosed              = 0x00000010;
   constexpr int elSealed              = 0x00000038;
   constexpr int elNestedClass         = 0x00000040;
   constexpr int elClassClass          = 0x00000080;
   constexpr int elRole                = 0x00000100;
   constexpr int elAbstract            = 0x00000200;
   constexpr int elNoCustomDispatcher  = 0x00000400;
   constexpr int elStructureRole       = 0x00000838;
   constexpr int elReadOnlyRole        = 0x00001000;
   constexpr int elNonStructureRole    = 0x00002000;
   constexpr int elWrapper             = 0x00004000;
   constexpr int elStructureWrapper    = 0x00004800;
   constexpr int elDynamicRole         = 0x00008000;
   constexpr int elExtension           = 0x0000110C;
   constexpr int elMessage             = 0x00200000;
   constexpr int elWithVariadics       = 0x00400000;
   constexpr int elWithCustomDispatcher= 0x00800000;
   constexpr int elWithYieldable       = 0x01000000;
   constexpr int elWithGenerics        = 0x02000000;
   constexpr int elSubject             = 0x04000000;
   constexpr int elExtMessage          = 0x08200000;
   constexpr int elGroup               = 0x10000000;
   constexpr int elSymbol              = 0x20000000;
   constexpr int elTemplatebased       = 0x40000000;
   constexpr int elExtendedVMT         = 0x80000000;   // indicates that the VMT is 64bit one

   constexpr int elDebugMask           = 0x001F0000;
   constexpr int elDebugDWORD          = 0x00010000;
   constexpr int elDebugReal64         = 0x00020000;
   constexpr int elDebugLiteral        = 0x00030000;
   constexpr int elDebugIntegers       = 0x00040000;
   constexpr int elDebugArray          = 0x00050000;
   constexpr int elDebugQWORD          = 0x00060000;
   constexpr int elDebugBytes          = 0x00070000;
   constexpr int elDebugShorts         = 0x00080000;
   constexpr int elDebugPTR            = 0x00090000;
   constexpr int elDebugWideLiteral    = 0x000A0000;
   constexpr int elDebugReference      = 0x000B0000;   // symbol reference
   constexpr int elDebugSubject        = 0x000C0000;
////////   const int elDebugReals          = 0x000D0000;
   constexpr int elDebugMessage        = 0x000E0000;
////////   const int elDebugDPTR           = 0x000F0000;
////   const int elEnumList            = 0x00100000;

  // --- ELENA Linker / ELENA VM constants ---
   constexpr int lnGCMGSize               = 0x00000001;
   constexpr int lnGCYGSize               = 0x00000002;
   constexpr int lnThreadCount            = 0x00000003;
   constexpr int lnObjectSize             = 0x00000004;
   constexpr int lnGCPERMSize             = 0x00000005;

  // ELENA run-time exceptions
   constexpr int ELENA_ERR_CRITICAL          = 0x100;
   constexpr int ELENA_ERR_ACCESS_VIOLATION  = 0x101;
   constexpr int ELENA_ERR_DIVIDE_BY_ZERO    = 0x102;
   constexpr int ELENA_ERR_OUT_OF_MEMORY     = 0x103;
   constexpr int ELENA_ERR_OUT_OF_PERMMEMORY = 0x104;

  // --- Project warning levels
   constexpr int WARNING_LEVEL_1          = 1;
   constexpr int WARNING_LEVEL_2          = 2;
   constexpr int WARNING_LEVEL_3          = 4;

   constexpr int WARNING_MASK_0           = 0;
   constexpr int WARNING_MASK_1           = 1;
   constexpr int WARNING_MASK_2           = 3;
   constexpr int WARNING_MASK_3           = 7;

   // --- ELENA Module structure constants ---
   constexpr auto ELENA_SIGNITURE         = "ELENA.";          // the stand alone image
   constexpr auto ELENACLIENT_SIGNITURE   = "VM.ELENA.";       // the ELENAVM client

   constexpr auto MODULE_SIGNATURE        = "ELENA.0510";      // the module version
   constexpr auto DEBUG_MODULE_SIGNATURE  = "ED!03";

  // --- ELENA core module names ---
   constexpr auto CORE_ALIAS              = "core";            // Core functionality

  // --- ELENA verb messages ---
   constexpr auto DISPATCH_MESSAGE        = "#dispatch";
   constexpr auto CAST_MESSAGE            = "#cast";
   constexpr auto CONSTRUCTOR_MESSAGE     = "#constructor";
   constexpr auto CONSTRUCTOR_MESSAGE2    = "#constructor2";   // protected constructor
   constexpr auto INVOKE_MESSAGE          = "#invoke";
   constexpr auto EQUAL_MESSAGE           = "equal";
   constexpr auto NOTEQUAL_MESSAGE        = "notequal";
   constexpr auto LESS_MESSAGE            = "less";
   constexpr auto AND_MESSAGE             = "and";
   constexpr auto OR_MESSAGE              = "or";
   constexpr auto XOR_MESSAGE             = "xor";
   constexpr auto GREATER_MESSAGE         = "greater";
   constexpr auto NOTLESS_MESSAGE         = "notless";
   constexpr auto NOTGREATER_MESSAGE      = "notgreater";
   constexpr auto ADD_MESSAGE             = "add";
   constexpr auto SUB_MESSAGE             = "subtract";
   constexpr auto MUL_MESSAGE             = "multiply";
   constexpr auto DIV_MESSAGE             = "divide";
   constexpr auto REFER_MESSAGE           = "at";
//  // #define APPEND_MESSAGE           "append"
//  // #define REDUCE_MESSAGE           "reduce"
   constexpr auto SET_REFER_MESSAGE       = "setAt";
//  // #define SET_MESSAGE              "set"
//   constexpr auto READ_MESSAGE         = "read";
//   constexpr auto WRITE_MESSAGE        = "write";
   constexpr auto IF_MESSAGE              = "if";
   constexpr auto IFNOT_MESSAGE           = "ifnot";
   constexpr auto ALT_MESSAGE             = "alt";
   constexpr auto SHIFTL_MESSAGE          = "shiftLeft";
   constexpr auto SHIFTR_MESSAGE          = "shiftRight";
   //  // #define IF_ELSE_MESSAGE          "if:else"
   constexpr auto INIT_MESSAGE            = "#init";
//  // #define ISNIL_MESSAGE            "#isnil"
   constexpr auto NEGATIVE_MESSAGE        = "Negative";
   constexpr auto INVERTED_MESSAGE        = "Inverted";
   constexpr auto VALUE_MESSAGE           = "Value";
   constexpr auto BAND_MESSAGE            = "band";
   constexpr auto BOR_MESSAGE             = "bor";
   constexpr auto BXOR_MESSAGE            = "bxor";
   constexpr auto BINVERTED_MESSAGE       = "BInverted";

   // ELENA verb operators
   constexpr auto EQUAL_OPERATOR          = "==";
   constexpr auto NOTEQUAL_OPERATOR		   = "!=";
   constexpr auto NOTLESS_OPERATOR        = ">=";
   constexpr auto NOTGREATER_OPERATOR     = "<=";
   constexpr auto GREATER_OPERATOR        = ">";
   constexpr auto LESS_OPERATOR           = "<";
   constexpr auto IF_OPERATOR             = "?";
   constexpr auto IFNOT_OPERATOR          = "!";
   constexpr auto AND_OPERATOR            = "&&";
   constexpr auto OR_OPERATOR             = "||";
   constexpr auto XOR_OPERATOR            = "^^";
   constexpr auto BAND_OPERATOR           = "&";
   constexpr auto BOR_OPERATOR            = "|";
   constexpr auto BXOR_OPERATOR           = "^";
   constexpr auto ADD_OPERATOR            = "+";
   constexpr auto SUB_OPERATOR            = "-";
   constexpr auto MUL_OPERATOR            = "*";
   constexpr auto DIV_OPERATOR            = "/";
   constexpr auto APPEND_OPERATOR         = "+=";
   constexpr auto BAPPEND_OPERATOR        = "|=";
   constexpr auto REDUCE_OPERATOR         = "-=";
   constexpr auto INCREASE_OPERATOR       = "*=";
   constexpr auto BINCREASE_OPERATOR      = "&=";
   constexpr auto SEPARATE_OPERATOR       = "/=";
   constexpr auto SHIFTL_OPERATOR         = "$shl";
   constexpr auto SHIFTR_OPERATOR         = "$shr";
   constexpr auto LEN_OPERATOR            = "$sizeof";
   constexpr auto ISNIL_OPERATOR          = "??";
   constexpr auto CATCH_OPERATOR          = "\\\\";
   constexpr auto ALT_OPERATOR            = "\\";
   constexpr auto FINALLY_OPERATOR        = "$fnl";

   constexpr auto INVERTED_OPERATOR       = "!";
   constexpr auto NEGATIVE_OPERATOR       = "-";
   constexpr auto VALUE_OPERATOR          = "*";
   constexpr auto BINVERTED_OPERATOR      = "~";

  // operator templates
   constexpr auto DOIFNOTNIL_OPERATOR        = "if#1#1";
   constexpr auto TRYORRETURN_OPERATOR       = "tryOrReturn#1#1";

  // --- ELENA explicit variables ---
   constexpr auto GROUP_VAR                  = "__target";         // the current method target / closure owner method target
   constexpr auto SELF_VAR                   = "self";             // the current method class reference / closure owner class reference
   constexpr auto SUPER_VAR                  = "super";            // the predecessor class
   constexpr auto MESSAGE_VAR                = "__received";       // the current message
   constexpr auto NIL_VAR                    = "nil";              // the nil pseudo symbol - representing the null value
   constexpr auto RETVAL_VAR                 = "$$ret";            // the closure returning value
   constexpr auto OWNER_VAR                  = "$$owner";          // the nested class / closure owner
   constexpr auto PARENT_VAR                 = "$$parent";         // the closure parent
   constexpr auto RETVAL_ARG                 = "$$retarg";         // the embedded returning arg

  // --- Meta fields ---
   constexpr auto META_INFO_NAME             = "info";

  // --- ELENA meta variables ---
   constexpr auto CLASSNAME_VAR              = "@classname";       // the class name
   constexpr auto PACKAGE_VAR                = "@package";         // the package info
   constexpr auto SUBJECT_VAR                = "@subject";

   constexpr auto INLINE_CLASSNAME           = "$inline";          // nested class generic name

   constexpr auto VIRTUAL_FIELD              = "$virtual#";

  // --- ELENA special sections ---
   constexpr auto METAINFO_SECTION           = "#metainfo";
   constexpr auto ATTRIBUTE_SECTION          = "#attributes";
   constexpr auto EXTENSION_SECTION          = "#extensions";
   constexpr auto INITIALIZER_SECTION        = "#initializer";
   constexpr auto EXT_INITIALIZER_SECTION    = "#ext_initializer";
   constexpr auto PACKAGE_SECTION            = "#package";
   constexpr auto IMPORTS_SECTION            = "#import";
   constexpr auto NAMESPACES_SECTION         = "#namespaces";
   constexpr auto NAMESPACE_REF              = "#namespace";

  // --- ELENA class prefixes / postfixes ---
   constexpr auto PRIVATE_PREFIX_NS          = "'$private'";
   constexpr auto INTERNAL_PREFIX_NS         = "'$intern'";

   constexpr auto TEMPLATE_PREFIX_NS         = "'$auto'";
   constexpr auto TEMPLATE_PREFIX_NS_ENCODED = "@$auto@";
   constexpr auto TEMPLATE_PREFIX_NS_LEN     = 7;

   constexpr auto FORWARD_PREFIX_NS          = "$forwards'";
   constexpr auto FORWARD_PREFIX_NS_LEN      = 10;

   constexpr auto CLASSCLASS_POSTFIX         = "#class";
   constexpr auto GENERIC_PREFIX             = "#generic";
//      ////   #define EMBEDDED_PREFIX          "#embedded"
   constexpr auto CONSTANT_POSTFIX           = "#";
   constexpr auto STATICFIELD_POSTFIX        = "#static";

   constexpr auto EMBEDDAMLE_PREFIX          = "auto#";

  // --- ELENA Standard module references ---
   constexpr auto DLL_NAMESPACE              = "$dlls";
      constexpr auto RTDLL_FORWARD           = "$rt";

////   #define STANDARD_MODULE_LEN      6
//   #define INTERNAL_MASK_LEN        12
  constexpr auto COREAPI_MASK_LEN            = 5;

   constexpr auto CORE_MODULE                = "coreapi";
   constexpr auto STANDARD_MODULE            = "system";               // the standard module name
   constexpr auto FORWARD_MODULE             = "$forwards";
   constexpr auto RT_MODULE                  = "elenart";              // ELENART / ELENAVM dll
   constexpr auto COREAPI_MASK               = "core_";                // core api mask : any function starting with it
//                                                                     // will be treated like internal core api one
//   #define INTERNAL_MASK            "system'core_"                   // primitive module mask

   constexpr auto NATIVE_MODULE              = "$native";

   constexpr auto META_MODULE                = "$meta";
   constexpr auto MESSAGE_TABLE              = "$meta'$messages";
   constexpr auto MESSAGEBODY_TABLE          = "$meta'$messagebody";
   constexpr auto MATTRIBUTE_TABLE           = "$meta'$attributes";

  // VM temporal code
   constexpr auto TAPE_SYMBOL                = "$tape";

   constexpr auto GC_THREADTABLE             = "$elena'@gcthreadroot";           // thread table
   constexpr auto TLS_KEY                    = "$elena'@tlskey";                 // TLS key
   constexpr auto TAPE_KEY                   = "$elena'@tapekey";                // VM Tape key
   constexpr auto NAMESPACE_KEY              = "$elena'@rootnamespace";          // The project namespace

   // predefined system forwards
   constexpr auto SUPER_FORWARD              = "$super";                         // the common class predecessor
   constexpr auto LAZYEXPR_FORWARD           = "$lazyexpression";                // the base lazy expression class
   constexpr auto INT_FORWARD                = "$int";
   constexpr auto LONG_FORWARD               = "$long";
   constexpr auto REAL_FORWARD               = "$real";
   constexpr auto STR_FORWARD                = "$literal";
   constexpr auto WIDESTR_FORWARD            = "$wideliteral";
   constexpr auto CHAR_FORWARD               = "$char";
   constexpr auto MESSAGE_FORWARD            = "$message";
   constexpr auto EXT_MESSAGE_FORWARD        = "$ext_message";
   constexpr auto MESSAGENAME_FORWARD        = "$messagename";
   constexpr auto REFTEMPLATE_FORWARD        = "$reference";
   constexpr auto CLOSURETEMPLATE_FORWARD    = "$closure";
   constexpr auto ARRAYTEMPLATE_FORWARD      = "$parray";
   constexpr auto ARGARRAYTEMPLATE_FORWARD   = "$varray";
   constexpr auto BOOL_FORWARD               = "$bool";
   constexpr auto TRUE_FORWARD               = "$true";
   constexpr auto FALSE_FORWARD              = "$false";
//   constexpr auto WRAP_FORWARD            = "$wrap";

   constexpr auto PROGRAM_ENTRY              = "$program";                     // the program entry
   constexpr auto SYSTEM_ENTRY               = "$system_entry";                // the system entry

   // --- miscellaneous routines ---
   inline bool isWeakReference(ident_t referenceName)
   {
      return (referenceName != nullptr && referenceName[0] != 0 && referenceName[0] == '\'');
   }

} // _ELENA_

#endif // elenaconstH
