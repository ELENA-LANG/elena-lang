//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Engine constants
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenaconstH
#define elenaconstH 1

namespace _ELENA_
{
  // --- Common ELENA Engine constants ---
   #define ENGINE_MAJOR_VERSION     3                 // ELENA Engine version
   #define ENGINE_MINOR_VERSION     1
   #define ENGINE_RELEASE_VERSION   1

   #define LINE_LEN                 0x1000            // the maximal source line length
   #define IDENTIFIER_LEN           0x0100            // the maximal identifier length

  // --- ELENA Standart message constants ---
   #define VERB_MASK               0x7F000000u
   #define SIGN_MASK               0x00FFFFF0u
   #define PARAM_MASK              0x0000000Fu
   #define MESSAGE_MASK            0x80000000u
   #define OPEN_ARG_COUNT          0x0F

   #define VERBX_MASK              0x7F00000000000000u
   #define SIGNX_MASK              0x00FFFFFFFFFF0000u
   #define PARAMX_MASK             0x000000000000FFFFu

   #define DISPATCH_MESSAGE_ID     0x0001             // NOTE : verb id should not be bigger than 0xE0 due to message constant implementation
   #define NEWOBJECT_MESSAGE_ID    0x0002             // NOTE : verb id should not be equal to 7, due to message reference constant implementation

   #define NEW_MESSAGE_ID          0x0003
   #define EQUAL_MESSAGE_ID        0x0004
   #define EVAL_MESSAGE_ID         0x0005
   #define GET_MESSAGE_ID          0x0006
   #define LESS_MESSAGE_ID         0x0008
   #define IF_MESSAGE_ID           0x0009
   #define AND_MESSAGE_ID          0x000A
   #define OR_MESSAGE_ID           0x000B
   #define XOR_MESSAGE_ID          0x000C
   #define IFNOT_MESSAGE_ID        0x000D
   #define RUN_MESSAGE_ID          0x000E
   #define NOTEQUAL_MESSAGE_ID     0x000F
   #define NOTLESS_MESSAGE_ID      0x0010
   #define NOTGREATER_MESSAGE_ID   0x0011
   #define GREATER_MESSAGE_ID      0x0012
   #define ADD_MESSAGE_ID          0x0013
   #define SUB_MESSAGE_ID          0x0014
   #define MUL_MESSAGE_ID          0x0015
   #define DIV_MESSAGE_ID          0x0016
   #define REFER_MESSAGE_ID        0x0017
   #define APPEND_MESSAGE_ID       0x0018
   #define REDUCE_MESSAGE_ID       0x0019
   #define INCREASE_MESSAGE_ID     0x001A
   #define SEPARATE_MESSAGE_ID     0x001B
   #define SET_REFER_MESSAGE_ID    0x001C
   #define SET_MESSAGE_ID          0x001D
   #define READ_MESSAGE_ID         0x001E
   #define WRITE_MESSAGE_ID        0x001F
   #define RAISE_MESSAGE_ID        0x0020
   #define SELECT_MESSAGE_ID       0x0021
   #define FIND_MESSAGE_ID         0x0022
   #define SEEK_MESSAGE_ID         0x0023
   #define STOP_MESSAGE_ID         0x0024
   #define REWIND_MESSAGE_ID       0x0025
   #define EXCHANGE_MESSAGE_ID     0x0026
   #define INDEXOF_MESSAGE_ID      0x0027
   #define CLOSE_MESSAGE_ID        0x0028
   #define CLEAR_MESSAGE_ID        0x0029
   #define DELETE_MESSAGE_ID       0x002A
   #define DO_MESSAGE_ID           0x002B
   #define INSERT_MESSAGE_ID       0x002C
   #define SAVE_MESSAGE_ID         0x002D
   #define RESET_MESSAGE_ID        0x002E
   #define SPLIT_MESSAGE_ID        0x002F
   #define CONVERT_MESSAGE_ID      0x0030
   #define FILL_MESSAGE_ID         0x0031
   #define LOAD_MESSAGE_ID         0x0032
   #define SHIFT_MESSAGE_ID        0x0033
   #define NOT_MESSAGE_ID          0x0034
   #define VALIDATE_MESSAGE_ID     0x0035
   #define INC_MESSAGE_ID          0x0036
   #define START_MESSAGE_ID        0x0037
   #define RETRIEVE_MESSAGE_ID     0x0038
   #define CAST_MESSAGE_ID         0x0039
   #define RESUME_MESSAGE_ID       0x003A
   #define OPEN_MESSAGE_ID         0x003B
   #define EXIT_MESSAGE_ID         0x003C
   #define SHOW_MESSAGE_ID         0x003D
   #define HIDE_MESSAGE_ID         0x003E
   #define CREATE_MESSAGE_ID       0x003F
   #define IS_MESSAGE_ID           0x0040
   #define ROLLBACK_MESSAGE_ID     0x0041
   #define REPLACE_MESSAGE_ID      0x0043  

   #define VALUES_MESSAGE_ID       0x007E             // virtual method returning the list of enum values
   #define PRIVATE_MESSAGE_ID      0x007F

   // ---- ELENAVM command masks ---
   #define VM_MASK                 0x0200             // vm command mask
   #define LITERAL_ARG_MASK        0x0400             // indicates that the command has a literal argument

   // ---- ELENAVM commands ---
   #define START_VM_MESSAGE_ID     0x02F1             // restart VM
   #define MAP_VM_MESSAGE_ID       0x06F2             // map forward reference
   #define USE_VM_MESSAGE_ID       0x06F3             // set current package
   #define LOAD_VM_MESSAGE_ID      0x06F4             // load template

   // ---- ELENAVM interpreter commands ---
   #define CALL_TAPE_MESSAGE_ID    0x05E0             // call symbol
   #define ARG_TAPE_MESSAGE_ID     0x05E1             // define the second parameter
   #define PUSH_VAR_MESSAGE_ID     0x01E2             // copy the data
   #define ASSIGN_VAR_MESSAGE_ID   0x01E3             // assign the data
   #define PUSH_TAPE_MESSAGE_ID    0x05E4             // push constant
   #define PUSHS_TAPE_MESSAGE_ID   0x05E5             // push literal constant
   #define PUSHN_TAPE_MESSAGE_ID   0x05E6             // push integer constant
   #define PUSHR_TAPE_MESSAGE_ID   0x05E7             // push floating numeric constant
   #define PUSHL_TAPE_MESSAGE_ID   0x05E8             // push long integer constant
   #define PUSHM_TAPE_MESSAGE_ID   0x05E9             // push message reference
   #define PUSHG_TAPE_MESSAGE_ID   0x05EA             // push the subject reference
   #define POP_TAPE_MESSAGE_ID     0x01EB             // free the stack content
   #define SEND_TAPE_MESSAGE_ID    0x05EC             // send the message
   #define REVERSE_TAPE_MESSAGE_ID 0x01ED             // reverse the stack
   #define PUSHE_TAPE_MESSAGE_ID   0x05EE             // push message reference

   #define NEW_TAPE_MESSAGE_ID     0x01F0             // create a dynamic object

   #define VA_ALIGNMENT       0x08
   #define VA_ALIGNMENT_POWER 0x03

  // --- ELENA Reference masks ---
   enum ReferenceType
   {
      // masks
      mskAnyRef              = 0xFF000000u,
      mskImageMask           = 0xE0000000u,
      mskTypeMask            = 0x0F000000u,

      mskCodeRef             = 0x00000000u,
      mskRelCodeRef          = 0x20000000u,
      mskRDataRef            = 0x40000000u,
      mskDebugRef            = 0x60000000u,
      mskStatRef             = 0x80000000u,
      mskDataRef             = 0xA0000000u,
      mskTLSRef              = 0xC0000000u,
      mskImportRef           = 0xE0000000u,

      mskRelImportRef        = 0xEF000000u,

      mskNativeCodeRef       = 0x18000000u,
      mskNativeRelCodeRef    = 0x38000000u,
      mskNativeRDataRef      = 0x48000000u,
      mskNativeDataRef       = 0xA8000000u,
      mskPreloadCodeRef      = 0x1C000000u,
      mskPreloadRelCodeRef   = 0x3C000000u,
      mskPreloadDataRef      = 0xAC000000u,
      mskNativeVariable      = 0xAD000000u,
      mskLockVariable        = 0xAE000000u,   // HOTFIX : used to fool trylock opcode, adding virtual offset

      mskInternalRef         = 0x13000000u,   // internal code
      mskInternalRelRef      = 0x33000000u,   // internal code
      mskSymbolRef           = 0x12000000u,   // symbol code
      mskSymbolRelRef        = 0x32000000u,   // symbol code
      mskVMTRef              = 0x41000000u,   // class VMT
      mskClassRef            = 0x11000000u,   // class code
      mskClassRelRef         = 0x31000000u,   // class relative code
      mskStatSymbolRef       = 0x82000000u,   // reference to static symbol

      mskVMTMethodAddress    = 0x43000000u,   // the method address, where the reference offset is a message id, reference values is VMT
      mskMetaRDataRef        = 0x44000000u,   // meta data
      mskVMTEntryOffset      = 0x45000000u,   // the message offset in VMT, where the reference offset is a message id, reference values is VMT
      mskSyntaxTreeRef       = 0x46000000u,   // template, declared in subject namespace
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
      mskSignature           = 0x09000000u,   // message signature constant
      mskVerb                = 0x0A000000u,   // message verb constant
      mskExtMessage          = 0x0B000000u,   // external message verb constant
      mskPreloaded           = 0x0C000000u,   // preloaded mask, should be used in combination with image mask
      mskConstArray          = 0x0D000000u    // constant array
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
      dsConstructor             = 0x0008,
      dsStack                   = 0x0009,
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

      dsDebugMask               = 0x00F0,
      dsTypeMask                = 0x1F00,
      dsDebugTypeMask           = 0x1FFF,
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
      ptWin32ConsoleX    = 0x10001,
      ptWin32GUIX        = 0x11001,
      ptLinux32Console   = 0x00002,
   };

//  // --- ELENA Debug Mode ---
//   enum DebugMode {
//      dbmNone       =  0,
//      dbmActive     = -1
//   };

   // --- ELENA Parse Table constants ---
   const int cnHashSize            = 0x0100;              // the parse table hash size
   const int cnTablePower          = 0x0010;
   const int cnTableKeyPower       = cnTablePower + 1;
   const int cnSyntaxPower         = 0x0008;

  // --- ELENA VMT flags ---
   const int elStandartVMT         = 0x00000001;
   const int elNestedClass         = 0x00000002;
   const int elDynamicRole         = 0x00000004;
   const int elStructureRole       = 0x00000008;
   const int elEmbeddable          = 0x00000010;
   const int elClosed              = 0x00000020;
   const int elWrapper             = 0x00000040;
   const int elStructureWrapper    = 0x00000048;
   const int elStateless           = 0x00000080;
   const int elSealed              = 0x00000120;
   const int elGroup               = 0x00000200;
   const int elWithGenerics        = 0x00000400;
   const int elReadOnlyRole        = 0x00000800;
   const int elNonStructureRole    = 0x00001000;
   const int elSignature           = 0x00002000;
   const int elRole                = 0x00004080;
   const int elExtension           = 0x00004980;
   const int elMessage             = 0x00008000;
   const int elSymbol              = 0x00100000;
   const int elExtMessage          = 0x00208000;
   const int elEmbeddableWrapper   = 0x00400040;   // wrapper containing embeddable field
   const int elWithCustomDispatcher= 0x00800000;
   const int elWithArgGenerics     = 0x01000000;
   const int elTapeGroup           = 0x02000200;

   const int elExtendedVMT         = 0x80000000;   // indicates that the VMT is 64bit one

   const int elDebugMask           = 0x001F0000;
   const int elDebugDWORD          = 0x00010000;
   const int elDebugReal64         = 0x00020000;
   const int elDebugLiteral        = 0x00030000;
   const int elDebugIntegers       = 0x00040000;
   const int elDebugArray          = 0x00050000;
   const int elDebugQWORD          = 0x00060000;
   const int elDebugBytes          = 0x00070000;
   const int elDebugShorts         = 0x00080000;
   const int elDebugPTR            = 0x00090000;
   const int elDebugWideLiteral    = 0x000A0000;
   const int elDebugReference      = 0x000B0000;   // symbol reference
   const int elDebugSubject        = 0x000C0000;
//   const int elDebugReals          = 0x000D0000;
   const int elDebugMessage        = 0x000E0000;
//   const int elDebugDPTR           = 0x000F0000;
   const int elEnumList            = 0x00100000;

  // --- ELENA Linker / ELENA VM constants ---
   const int lnGCMGSize            = 0x00000001;
   const int lnGCYGSize            = 0x00000002;
   const int lnThreadCount         = 0x00000003;
   const int lnObjectSize          = 0x00000004;

   const int lnVMAPI_Instance      = 0x00001001;   // reference to VM;

  // ELENA run-time exceptions
   #define ELENA_ERR_OUTOF_MEMORY  0x190

  // --- Project warning levels
   const int WARNING_LEVEL_1 = 1;
   const int WARNING_LEVEL_2 = 2;
   const int WARNING_LEVEL_3 = 4;

   const int WARNING_MASK_0 = 0;
   const int WARNING_MASK_1 = 1;
   const int WARNING_MASK_2 = 3;
   const int WARNING_MASK_3 = 7;

   // --- ELENA Module structure constants ---
   #define ELENA_SIGNITURE          "ELENA.6."       // the stand alone image
   #define ELENACLIENT_SIGNITURE    "VM.ELENA.6."    // the ELENAVM client

   #define MODULE_SIGNATURE         "ELENA.6.0"      // the module version
   #define DEBUG_MODULE_SIGNATURE   "ED!2"

  // --- ELENA core module names ---
   #define CORE_ALIAS                "core"          // Core functionality
  
  // --- ELENA verb messages ---
   #define DISPATCH_MESSAGE         "dispatch"
   #define NEW_MESSAGE              "new"
   #define GET_MESSAGE              "get"
   #define EVAL_MESSAGE             "eval"
   #define EVALUATE_MESSAGE         "evaluate"
   #define EQUAL_MESSAGE            "equal"
   #define NOTEQUAL_MESSAGE         "notequal"
   #define LESS_MESSAGE             "less"
   #define AND_MESSAGE              "and"
   #define OR_MESSAGE               "or"
   #define XOR_MESSAGE              "xor"
   #define DO_MESSAGE               "do"
   #define STOP_MESSAGE             "stop"
   #define GREATER_MESSAGE          "greater"
   #define NOTLESS_MESSAGE          "notless"
   #define NOTGREATER_MESSAGE       "notgreater"
   #define ADD_MESSAGE              "add"
   #define SUB_MESSAGE              "subtract"
   #define MUL_MESSAGE              "multiply"
   #define DIV_MESSAGE              "divide"
   #define REFER_MESSAGE            "getAt"
   #define APPEND_MESSAGE           "append"
   #define REDUCE_MESSAGE           "reduce"
   #define INCREASE_MESSAGE         "multiplyBy"
   #define SEPARATE_MESSAGE         "divideInto"
   #define SET_REFER_MESSAGE        "setAt"
   #define SET_MESSAGE              "set"
   #define READ_MESSAGE             "read"
   #define WRITE_MESSAGE            "write"
   #define RAISE_MESSAGE            "raise"
   #define IF_MESSAGE               "if"
   #define IFNOT_MESSAGE            "ifnot"
   #define FIND_MESSAGE             "find"
   #define SEEK_MESSAGE             "seek"
   #define REWIND_MESSAGE           "rewind"
   #define EXCHANGE_MESSAGE         "exchange"
   #define INDEXOF_MESSAGE          "indexOf"
   #define CLOSE_MESSAGE            "close"
   #define CLEAR_MESSAGE            "clear"
   #define DELETE_MESSAGE           "delete"
   #define RUN_MESSAGE              "run"
   #define INSERT_MESSAGE           "insert"
   #define SAVE_MESSAGE             "save"
   #define RESET_MESSAGE            "reset"
   #define SPLIT_MESSAGE            "split"
   #define CONVERT_MESSAGE          "convert"
   #define FILL_MESSAGE             "fill"
   #define LOAD_MESSAGE             "load"
   #define SHIFT_MESSAGE            "shift"
   #define NOT_MESSAGE              "invert"
   #define VALIDATE_MESSAGE         "validate"
   #define INC_MESSAGE              "next"
   #define START_MESSAGE            "start"
   #define RETRIEVE_MESSAGE         "retrieve"
   #define CAST_MESSAGE             "cast"
   #define RESUME_MESSAGE           "resume"
   #define OPEN_MESSAGE             "open"
   #define EXIT_MESSAGE             "exit"
   #define SHOW_MESSAGE             "show"
   #define HIDE_MESSAGE             "hide"
   #define CREATE_MESSAGE           "create"
   #define IS_MESSAGE               "is"
   #define ROLLBACK_MESSAGE         "rollback"
   #define SELECT_MESSAGE           "select"
   #define REPLACE_MESSAGE          "replace"

   // ELENA verb operators
   #define EQUAL_OPERATOR		      "=="
   #define NOTEQUAL_OPERATOR		   "!="
   #define NOTLESS_OPERATOR		   ">="
   #define NOTGREATER_OPERATOR      "<="
   #define GREATER_OPERATOR		   ">"
   #define LESS_OPERATOR            "<"
   #define IF_OPERATOR			      "?"
   #define IFNOT_OPERATOR		      "!"
   #define AND_OPERATOR             "&&"
   #define OR_OPERATOR              "||"
   #define XOR_OPERATOR             "^^"
   #define ADD_OPERATOR             "+"
   #define SUB_OPERATOR             "-"
   #define MUL_OPERATOR             "*"
   #define DIV_OPERATOR             "/"
   #define REFER_OPERATOR			   "@"
   #define APPEND_OPERATOR			   "+="
   #define REDUCE_OPERATOR			   "-="
   #define INCREASE_OPERATOR			"*="
   #define SEPARATE_OPERATOR			"/="
   #define WRITE_OPERATOR           "<<"
   #define READ_OPERATOR            ">>"

  // --- ELENA explicit variables ---
   #define SELF_VAR                "self"             // the current method target / closure owner method target
   #define THIS_VAR                "$self"            // the current method class reference / closure owner class reference
   #define OWNER_VAR               "$owner"           // the nested class owner
   //#define CLOSURE_THIS_VAR        "$closure"         // the closure class reference
   //#define CLOSURE_SELF_VAR        "closure"          // the closure target

   #define SUPER_VAR               "$super"           // the predecessor class
   #define SUBJECT_VAR             "$subject"         // the current message
   #define NIL_VAR                 "$nil"             // the nil symbol
   #define RETVAL_VAR              "$$ret"            // the closure returning value

  // --- ELENA special sections ---
   #define ATTRIBUTE_SECTION        "#attributes"
   #define EXTENSION_SECTION        "#extensions"
   #define ACTION_SECTION           "#actions"
   #define INITIALIZER_SECTION      "#initializer"
   #define PACKAGE_SECTION          "#package"

  // --- ELENA class prefixes / postfixes ---
   #define INLINE_POSTFIX           "#inline"
   #define CLASSCLASS_POSTFIX       "#class"
   #define GENERIC_PREFIX           "#generic"
//   #define EMBEDDED_PREFIX          "#embedded"
//   #define TARGET_POSTFIX           "##"
   #define STATICFIELD_POSTFIX      "#static"

  // --- ELENA Standard module references ---
   #define DLL_NAMESPACE            "$dlls"
   #define RTDLL_FORWARD            "$rt"

   #define STANDARD_MODULE_LEN      6
   #define INTERNAL_MASK_LEN        12
   #define COREAPI_MASK_LEN         5 

   #define CORE_MODULE              "coreapi"
   #define STANDARD_MODULE          "system"                         // the standard module name
   #define EXTERNAL_MODULE          "system'external"                // external pseudo symbol
   #define COREAPI_MASK             "core_"                          // core api mask : any function starting with it
                                                                     // will be treated like internal core api one
   #define INTERNAL_MASK            "system'core_"                   // primitive module mask

   #define NATIVE_MODULE            "$native"

  // VM temporal code
   #define TAPE_SYMBOL              "$tape"

   #define GC_THREADTABLE           "$elena'@gcthreadroot"           // thread table
   #define TLS_KEY                  "$elena'@tlskey"                 // TLS key
   #define NAMESPACE_KEY            "$elena'@rootnamespace"          // The project namespace

   // predefined system forwards
   #define SUPER_FORWARD            "'$super"                        // the common class predecessor
   #define LAZYEXPR_FORWARD         "'$lazyexpression"               // the base lazy expression class
   #define INT_FORWARD              "'$int"
   #define LONG_FORWARD             "'$long"
   #define REAL_FORWARD             "'$real"
   #define STR_FORWARD              "'$literal"
   #define WIDESTR_FORWARD          "'$wideliteral"
   #define CHAR_FORWARD             "'$char"
   #define MESSAGE_FORWARD          "'$message"
   #define EXT_MESSAGE_FORWARD      "'$ext_message"
   #define SIGNATURE_FORWARD        "'$signature"
   #define VERB_FORWARD             "'$verb"
   #define ARRAY_FORWARD            "'$array"
   #define BOOL_FORWARD             "'$bool"

   #define PARAMS_SUBJFORWARD       "'$$params"

   #define STARTUP_CLASS            "'startUp"

} // _ELENA_

#endif // elenaconstH
