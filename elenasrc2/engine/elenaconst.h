//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains the common ELENA Engine constants
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenaconstH
#define elenaconstH 1

namespace _ELENA_
{
  // --- Common ELENA Engine constants ---
   #define ENGINE_MAJOR_VERSION     0x0008            // ELENA Engine version
   #define ENGINE_MINOR_VERSION     0x0002

   #define LINE_LEN                 0x1000            // the maximal source line length
   #define IDENTIFIER_LEN           0x0100            // the maximal identifier length

  // --- ELENA Standart message constants ---
   #define VERB_MASK               0x7F000000
   #define SIGN_MASK               0x00FFFFF0
   #define PARAM_MASK              0x0000000F
   #define MESSAGE_MASK            0x80000000
   #define OPEN_ARG_COUNT          0x0F

   #define DISPATCH_MESSAGE_ID     0x0001
   #define TYPECAST_MESSAGE_ID     0x0002
   #define NEWOBJECT_MESSAGE_ID    0x0003

   #define NEW_MESSAGE_ID          0x0004
   #define EVAL_MESSAGE_ID         0x0005
   #define GET_MESSAGE_ID          0x0006
   #define EQUAL_MESSAGE_ID        0x0007
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
   #define IFFAILED_MESSAGE_ID     0x0021
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

//   // ---- ELENAVM command masks ---
//   #define VM_MASK                 0x0200             // vm command mask
//   #define LITERAL_ARG_MASK        0x0400             // indicates that the command has a literal argument

   // ---- ELENAVM commands ---
   #define START_VM_MESSAGE_ID     0x02F1             // restart VM
   #define MAP_VM_MESSAGE_ID       0x06F2             // map forward reference
   #define USE_VM_MESSAGE_ID       0x06F3             // set current package
   #define LOAD_VM_MESSAGE_ID      0x06F4             // load template

   // ---- ELENAVM interpreter commands ---
   #define CALL_TAPE_MESSAGE_ID    0x05E0             // call symbol
  // #define PUSH_EMPTY_MESSAGE_ID   0x01E1
   #define PUSH_VAR_MESSAGE_ID     0x01E2             // copy the data
  // #define POP_VAR_MESSAGE_ID      0x01E3             // copy the data
   #define PUSH_TAPE_MESSAGE_ID    0x05E4             // push constant
   #define PUSHS_TAPE_MESSAGE_ID   0x05E5             // push literal constant
   #define PUSHN_TAPE_MESSAGE_ID   0x05E6             // push integer constant
   #define PUSHR_TAPE_MESSAGE_ID   0x05E7             // push floating numeric constant
   #define PUSHL_TAPE_MESSAGE_ID   0x05E8             // push long integer constant
   #define PUSHM_TAPE_MESSAGE_ID   0x05E9             // push message reference
   #define PUSHG_TAPE_MESSAGE_ID   0x05EA             // push the subject reference
   #define POP_TAPE_MESSAGE_ID     0x01EB             // free the stack content
   #define SEND_TAPE_MESSAGE_ID    0x05EC             // send the message
   #define SENDR_TAPE_MESSAGE_ID   0x05ED             // send the message
  // #define START_TAPE_MESSAGE_ID   0x01EE             // mark the creating object
  // #define NEW_TAPE_MESSAGE_ID     0x05EE             // create an object (containing the last n objects)
   #define REVERSE_TAPE_MESSAGE_ID 0x01EF             // reverse the stack parameters 
   #define NEWS_TAPE_MESSAGE_ID    0x01F0             // create a dynamic structure
   #define NEWA_TAPE_MESSAGE_ID    0x01F1             // create a dynamic action

   #define VA_ALIGNMENT       0x08
   #define VA_ALIGNMENT_POWER 0x03

  // --- ELENA Reference masks ---
   enum ReferenceType
   {
      // masks
      mskAnyRef              = 0xFF000000,
      mskImageMask           = 0xE0000000,
      mskTypeMask            = 0x0F000000,

      mskCodeRef             = 0x00000000,
      mskRelCodeRef          = 0x20000000,
      mskRDataRef            = 0x40000000,
      mskStatRef             = 0x80000000,
      mskDataRef             = 0xA0000000,
      mskTLSRef              = 0xC0000000,
      mskImportRef           = 0xE0000000,

      mskNativeCodeRef       = 0x18000000,
      mskNativeRelCodeRef    = 0x38000000,
      mskNativeRDataRef      = 0x48000000,
      mskNativeDataRef       = 0xA8000000,
      mskPreloadCodeRef      = 0x1C000000,
      mskPreloadRelCodeRef   = 0x2C000000,
      mskPreloadDataRef      = 0xAC000000,
      mskNativeVariable      = 0xAD000000,

      mskSymbolRef           = 0x12000000,   // symbol code
      mskSymbolRelRef        = 0x32000000,   // symbol code
      mskVMTRef              = 0x41000000,   // class VMT
      mskClassRef            = 0x11000000,   // class code
//      mskClassRelRef         = 0x31000000,   // class relative code
      mskStatSymbolRef       = 0x82000000,   // reference to static symbol

      mskVMTEntryOffset      = 0x43000000,   // the message offset in VMT, where the reference offset is a message id, reference values is VMT
      mskMetaRDataRef        = 0x44000000,   // meta data

      mskConstantRef         = 0x01000000,   // reference to constant
      mskLiteralRef          = 0x02000000,   // reference to constant literal
      mskInt32Ref            = 0x03000000,   // reference to constant integer number
      mskInt64Ref            = 0x04000000,   // reference to constant 64bit integer number
      mskRealRef             = 0x05000000,   // reference to constant real number
      mskMessage             = 0x06000000,   // message constant
//      mskLinkerConstant      = 0x07000000,   // linker constant
      mskSymbolLoaderRef     = 0x08000000,   // reference to symbol loader
      mskSignature           = 0x09000000,   // message signature constant
      mskPreloaded           = 0x0C000000,   // prelooded mask, should be used in combination with image mask
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

      dsSymbol                  = 0x0001,
      dsClass                   = 0x0002,
      dsBase                    = 0x0003,
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

      dsDebugMask               = 0x00F0,
      dsDebugTypeMask           = 0x0FFF,
   };

   // predefined debug module sections
   #define DEBUG_LINEINFO_ID      (size_t)-1
   #define DEBUG_STRINGS_ID       (size_t)-2

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
      ptVMWin32GUI       = 0x01001,
      ptVMWin32Console   = 0x00101,
      ptWin32ConsoleMT   = 0x10001,
   };

  // --- ELENA Debug Mode ---
   enum DebugMode {
      dbmNone       =  0,
      dbmStandalone = -1,
      dbmElenaVM    = -2
   };

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
//// const int elClassClass          = 0x00000010;
   const int elVMTCustomDispatcher = 0x00000040;
   const int elStateless           = 0x00000080;
   const int elSealed              = 0x00000100;
   const int elGroup               = 0x00000200;
   const int elWithGenerics        = 0x00000440;
//// const int elCastGroup           = 0x00000600;
//// const int elUnion               = 0x00000A00;
//// const int elMethodHandler       = 0x00001000; 
   const int elSignature           = 0x00002000;
   const int elRole                = 0x00004000;
   const int elMessage             = 0x00008000;
//  // const int elDynamicSubjectRole  = 0x0000B080;
   const int elConstantSymbol      = 0x00000082;
//   const int elOperation           = 0x00204000;
//// const int elWithLocker          = 0x00100000;

   const int elDebugMask           = 0x000F0000;
   const int elDebugDWORD          = 0x00010000;
   const int elDebugReal64         = 0x00020000;
   const int elDebugLiteral        = 0x00030000;
   const int elDebugArray          = 0x00050000;
   const int elDebugQWORD          = 0x00060000;
//   const int elDebugBytes          = 0x00070000;

  // --- ELENA Linker / ELENA VM constants ---
   const int lnGCMGSize            = 0x00000001;
   const int lnGCYGSize            = 0x00000002;
   const int lnThreadCount         = 0x00000003;
   const int lnObjectSize          = 0x00000004;
   // used only for VM
   const int lnVMAPI_Instance      = 0x00001001;   // reference to VM;
   const int lnVMAPI_LoadSymbol    = 0x00001002;   // reference to LoadSymbol function;
   const int lnVMAPI_LoadName      = 0x00001003;   // reference to LoadClassName function;
   const int lnVMAPI_Interprete    = 0x00001005;   // reference to Interprete function;
  // const int lnVMAPI_GetLastError  = 0x00001006;   // reference to GetLastError function;

  // ELENA run-time exceptions
   #define ELENA_ERR_OUTOF_MEMORY  0x190

  // --- ELENA Module structure constants ---
   #define ELENA_SIGNITURE          "ELENA.8.02"       // the stand alone image
   #define ELENACLIENT_SIGNITURE    "ELENAVMC.8.02"    // the ELENAVM client
   #define MODULE_SIGNATURE         "EN!8.02"         // the language version
   #define DEBUG_MODULE_SIGNATURE   "ED!1.2"

  // --- ELENA core module names ---
   #define CORE_MODULE              "core"          // core GC functionality
   #define COMMANDSET_MODULE        "commands"      // core predefined command set
   #define CORE_VM_MODULE           "core_vm"       // core vm client functionality
   #define INLINE_MODULE            "inline"        // inline module alias

  // --- ELENA verb messages ---
   #define NEW_MESSAGE              "new"
   #define GET_MESSAGE              "get"
   #define EVAL_MESSAGE             "eval"
   #define EVALUATE_MESSAGE         "evaluate"
   #define EQUAL_MESSAGE            "equal"
   #define LESS_MESSAGE             "less"
   #define AND_MESSAGE              "and"
   #define OR_MESSAGE               "or"
   #define XOR_MESSAGE              "xor"
   #define DO_MESSAGE               "do"
   #define STOP_MESSAGE             "stop"
   #define GREATER_MESSAGE          "greater"
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
   #define IFFAILED_MESSAGE         "ifFailed"
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
   #define SELF_VAR                "self"             // the current object group
   #define THIS_VAR                "$self"            // the current class instance
   #define SUPER_VAR               "$super"           // the predecessor class
  // #define NEXT_VAR                "$next"            // the next group member

  // --- ELENA special sections ---
   #define TYPE_SECTION             "#types"

  // --- ELENA class prefixes / postfixes ---
   #define INLINE_POSTFIX           "#inline"
   #define CLASSCLASS_POSTFIX       "#class"
   #define GENERIC_POSTFIX          "#generic"

  // --- ELENA hints ---
   #define HINT_CONSTANT           "const"
   #define HINT_ROLE               "role"
   #define HINT_TYPE               "type"              // type hint
   #define HINT_SIZE               "size"
   #define HINT_ITEMSIZE           "itemsize"
   #define HINT_ITEM               "item"
//  // #define HINT_SAFEPOINT          "safepoint"
//  // #define HINT_LOCK               "sync"
   #define HINT_EXTENSION          "extension"
   #define HINT_SEALED             "sealed"

   #define HINT_DBG                "dbg"               // debugger watch hint values
   #define HINT_DBG_INT            "int"              
   #define HINT_DBG_LONG           "long"              
   #define HINT_DBG_LITERAL        "literal"
   #define HINT_DBG_REAL           "real"              
   #define HINT_DBG_ARRAY          "array"              

//   #define HINT_ARRAY              "array"
//   #define HINT_REAL               "real"
//   #define HINT_SHORT              "short"
////   #define HINT_MEM                "mem"
   #define HINT_MESSAGE            "message"
   #define HINT_SIGNATURE          "signature"
//   #define HINT_OPERATION          "operation"
   #define HINT_GROUP              "group"
   
  // --- ELENA Standard module references ---
   #define DLL_NAMESPACE            "$dlls"

   #define STANDARD_MODULE          "system"        // the standard module name
   #define PACKAGE_MODULE           "$package"
   #define EXTERNAL_MODULE          "system'external"

//  // VM temporal code
//   #define TAPE_SYMBOL              "$tape"

  // #define GC_ROOT                  "$elena'@gcroot"               // static roots
   #define GC_THREADTABLE           "$elena'@gcthreadroot"           // thread table
   #define TLS_KEY                  "$elena'@tlskey"                 // TLS key

   // predefined classes
   #define SUPER_CLASS              "system'Object"                  // the common class predecessor
   #define ACTION_CLASS             "system'Action"                  // the base action class
   #define NIL_CLASS                "system'nil"                     // the nil reference
   #define BREAK_EXCEPTION_CLASS    "system'BreakException"          // break class
   #define NOMETHOD_EXCEPTION_CLASS "system'MethodNotFoundException"          
   #define TRUE_CLASS               "system'true"
   #define FALSE_CLASS              "system'false"
   #define INT_CLASS                "system'IntNumber" 
   #define LONG_CLASS               "system'LongNumber" 
   #define REAL_CLASS               "system'RealNumber" 
   #define WSTR_CLASS               "system'LiteralValue" 
//   #define WCHR_CLASS               "system'CharValue" 
   #define ARRAY_CLASS              "system'Array" 
////   #define ROLES_CLASS              "system'dynamic'RoleList"        // the role list handler
//   #define TAPE_CLASS               "system'dynamic'Tape"            // the role list handler
//   #define TAPECONTROL_CLASS        "system'dynamic'tapeControl"     // the role list handler
   #define SYMBOL_CLASS             "system'dynamic'Symbol"          // the special role class
   #define MESSAGE_CLASS            "system'dynamic'Message"         // the special role class
   #define SIGNATURE_CLASS          "system'dynamic'Signature"       // the special role class
   #define GETPROPERTY_CLASS        "system'dynamic'GetProperty"     
//   #define STRUCT_CLASS             "system'dynamic'Struct"     

   // predefined types
   #define PARAMS_SUBJECT           "params"
//   #define ACTION_SUBJECT           "action"

   #define STARTUP_CLASS            "'program"

} // _ELENA_

#endif // elenaconstH
