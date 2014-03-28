//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//
//                                              (C)2009-2014, by Alexei Rakov
//------------------------------------------------------------------------------

#ifndef bytecodeH
#define bytecodeH 1

namespace _ELENA_
{

// --- Byte code command set ---
enum ByteCode
{
   // commands:
   bcNop            = 0x00,
   bcBreakpoint     = 0x01,
   bcPushB          = 0x02,
   bcPop            = 0x03,
//   bcDXCopyA        = 0x04,
   bcPushM          = 0x05,
   bcMCopyVerb      = 0x06,
   bcThrow          = 0x07,
   //bcSNop           = 0x08,
   bcMCopySubj      = 0x09,
   bcPushA          = 0x0A,
   bcPopA           = 0x0B,
   bcACopyB         = 0x0C,
   bcPopM           = 0x0D,
   bcBSRedirect     = 0x0E,
   bcUnbox          = 0x0F,

   bcBSGRedirect    = 0x10,
   bcGetLen         = 0x11,
   bcBCopyA         = 0x12,
   bcDDec           = 0x13,
   bcPopB           = 0x14,
   bcClose          = 0x15,
   bcQuit           = 0x17,
   bcGet            = 0x18,
   bcSet            = 0x19,
   bcDInc           = 0x1A,
   bcMQuit          = 0x1B,
   bcALoadD         = 0x1C,
   bcUnhook         = 0x1D,
   bcExclude        = 0x1E,
   bcInclude        = 0x1F,

   // all 2x command are push ones
   bcReserve        = 0x20,   // should be used only for unmanaged stack (stack may contains old references, which may break GC)
   bcPushN          = 0x21,
   bcPushR          = 0x22,
   bcPushBI         = 0x23,
   bcPushAI         = 0x24,
   bcPushFI         = 0x26,
   bcMSaveParams    = 0x29,
   bcPushSI         = 0x2A,
   bcPushF          = 0x2D,

   // all 3x commands are pop ones
   bcPopI           = 0x30,
   bcPopBI          = 0x31,
   bcPopFI          = 0x32,
   bcXPopAI         = 0x33,
   bcPopSI          = 0x34,
   bcPopAI          = 0x35,
   bcQuitN          = 0x39,

   bcCallExtR       = 0x40,
   bcEvalR          = 0x41,
   bcACallVI        = 0x42,
   bcCallR          = 0x43,
   bcMLoadAI        = 0x47,
   bcMLoadSI        = 0x48,
   bcMLoadFI        = 0x49,
   bcMSaveAI        = 0x4A, 
   bsMSetVerb       = 0x4B,
   bcMReset         = 0x4C,
   bcMAddAI         = 0x4D,
   bcMCopy          = 0x4E,
   bcMAdd           = 0x4F,

   bcDLoadSI        = 0x50,
   bcDSaveSI        = 0x51,
   bcDLoadFI        = 0x52,
   bcALoadR         = 0x53,
   bcALoadFI        = 0x54,
   bcALoadSI        = 0x55,
   bcDCopy          = 0x56,
   bcDLoadAI        = 0x57,
   bcDAddAI         = 0x59,
   bcDSubAI         = 0x5A,
   bcDAddSI         = 0x5B,
   bcDSubSI         = 0x5C,
   bcDSaveFI        = 0x5D,

   bcASaveBI        = 0x61,
   bcASaveSI        = 0x63,
   bcASaveFI        = 0x64,
   bcASaveR         = 0x65,
   bcDSaveAI        = 0x67,
   bcSwapSI         = 0x6C,
   bcASwapSI        = 0x6D,
   bcAXCopyR        = 0x6E,
   bcIAXLoadB       = 0x6F,

   bcSCopyF         = 0x72,
   bcACopyS         = 0x75,
   bcACopyR         = 0x78,
//   //bcACopyN         = 0x79,
   bcALoadAI        = 0x7A,
//   bcAXCopyF        = 0x7B,
   bcACopyF         = 0x7C,
//   //bcAAdd           = 0x7E,
//   //bcAMul           = 0x7F,

   bcRestore        = 0x82,
   bcOpen           = 0x88,

   bcJump           = 0xA0,
   bcAJumpVI        = 0xA1,
   bcHook           = 0xA6,
   bcDElse          = 0xA7,
   bcDThen          = 0xA8,
   bcAElse          = 0xA9,
   bcAThen          = 0xAA,

   bcNBox           = 0xB0,
   bcBox            = 0xB1,

   bcALoadBI        = 0xCE,

   bcFunc           = 0xD0,
   bcBSTest         = 0xDD,
   bcTest           = 0xDE,
   bcWSTest         = 0xDF,

   bcAElseR         = 0xE0,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcAThenR         = 0xE1,   // though in bytecode section they saved in the correct order (jump arg, label)
   bcMElse          = 0xE2,
   bcMThen          = 0xE3,
   bcDElseN         = 0xE4,
   bcDThenN         = 0xE5,
   bcAElseSI        = 0xE6,
   bcAThenSI        = 0xE7,
   bcMElseVerb      = 0xE8,
   bcMThenVerb      = 0xE9,
   bcMElseAI        = 0xEA,
   bcMThenAI        = 0xEB,
   bcElseFlag       = 0xEC,
   bcTestFlag       = 0xED,
   bcNext           = 0xEF,

   bcCreate         = 0xF0,
   bcCreateN        = 0xF1,
   bcIAXCopyR       = 0xF2,
   bcIAXLoadFI      = 0xF3,
   bcIAXLoadSI      = 0xF4,
   bcIAXLoadBI      = 0xF5,

   bcSCallVI        = 0xFC,
   bcXCallRM        = 0xFE,

   bcReserved       = 0xFF,

   // labels
   blLabelMask      = 0xC000,  // tape label mask
   blBegin          = 0xC001,  // meta command, declaring the structure
   blEnd            = 0xC002,  // meta command, closing the structure
   blLabel          = 0xC003,  // meta command, declaring the label

   // meta commands:
   bcAllocStack     = 0x8101,  // meta command, used to indicate that the previous command allocate number of items in the stack; used only for exec
   bcFreeStack      = 0x8102,  // meta command, used to indicate that the previous command release number of items from stack; used only for exec
   //blHint           = 0x204,  // meta command, compiler hint

   bcMatch          = 0xFFFE,  // used in optimization engine
   bcNone           = 0xFFFF,  // used in optimization engine

   blDeclare        = 0x8120,  // meta command, closing the structure
   blStatement      = 0x8121,

   // debug info
   bdBreakpoint     = 0x8401,
   bdBreakcoord     = 0x8402,
   bdLocal          = 0x8403,
   bdSelf           = 0x8404,
   bdIntLocal       = 0x8413,
   bdLongLocal      = 0x8423,
   bdRealLocal      = 0x8433,
   bdParamsLocal      = 0x8443,
};

enum FunctionCode
{
   fnUnknown        = 0x0000,
   fnNCopy          = 0x0001,
   fnNCopyStr       = 0x0002,
   fnNLoad          = 0x0003,
   fnNEqual         = 0x0004,
   fnNLess          = 0x0005,
   fnNNotGreater    = 0x0006,
   fnNAdd           = 0x0007,
   fnNSub           = 0x0008,
   fnNMul           = 0x0009,
   fnNDiv           = 0x000A,
   fnNAnd           = 0x000B,
   fnNOr            = 0x000C,
   fnNXor           = 0x000D,
   fnNShift         = 0x000E,
   fnNNot           = 0x000F,
   fnNInc           = 0x0010,
   fnLCopy          = 0x0011,
   fnLCopyInt       = 0x0012,
   fnLCopyStr       = 0x0013,
   fnLLoad          = 0x0014,
   fnLEqual         = 0x0015,
   fnLLess          = 0x0016,
   fnLNotGreater    = 0x0017,
   fnLAdd           = 0x0018,
   fnLSub           = 0x0019,
   fnLMul           = 0x001A,
   fnLDiv           = 0x001B,
   fnLAnd           = 0x001C,
   fnLOr            = 0x001D,
   fnLXor           = 0x001E,
   fnLNot           = 0x001F,
   fnLShift         = 0x0020,
   fnRCopy          = 0x0021,
   fnRCopyInt       = 0x0022,
   fnRCopyLong      = 0x0023,
   fnRCopyStr       = 0x0024,
   fnREqual         = 0x0025,
   fnRLess          = 0x0026,
   fnRNotGreater    = 0x0027,
   fnRAdd           = 0x0028,
   fnRSub           = 0x0029,
   fnRMul           = 0x002A,
   fnRDiv           = 0x002B,
   fnRAddInt        = 0x002C,
   fnRSubInt        = 0x002D,
   fnRMulInt        = 0x002E,
   fnRDivInt        = 0x002F,
   fnRAddLong       = 0x0030,
   fnRSubLong       = 0x0031,
   fnRMulLong       = 0x0032,
   fnRDivLong       = 0x0033,
   fnWSGetLen       = 0x0034,
   fnWSSetLen       = 0x0035,
   fnWSCreate       = 0x0036,
   fnWSCopy         = 0x0037,
   fnWSCopyInt      = 0x0038,
   fnWSCopyLong     = 0x0039,
   fnWSCopyReal     = 0x003A,
   fnWSCopyBuf      = 0x003B,
   fnWSEqual        = 0x003C,
   fnWSLess         = 0x003D,
   fnWSNotGreater   = 0x003E,
   fnWSAdd          = 0x003F,
   fnWSGetAt        = 0x0040,
   fnWSSetAt        = 0x0041,
   fnWSIndexOfStr   = 0x0042,
   fnWSCopyStr      = 0x0043,
   fnWSAddStr       = 0x0044,
   fnWSLoadName     = 0x0045,
   fnBSSetBuf       = 0x0046,
   fnBSGetBuf       = 0x0047,
   fnBSCopyStr      = 0x0048,
   fnBSSetWord      = 0x0049,
   fnBSGetWord      = 0x004A,
   fnBSIndexOf      = 0x004B, 
   fnBSIndexOfWord  = 0x004C, 
   fnBSEval         = 0x004D,
   fnLRndNew        = 0x004E,
   fnLRndNext       = 0x004F,
   fnRAbs           = 0x0050,
   fnRRound         = 0x0051,
   fnRExp           = 0x0052,
   fnRLn            = 0x0053,
   fnRInt           = 0x0054,
   fnRCos           = 0x0055,
   fnRSin           = 0x0056,
   fnRArcTan        = 0x0057,
   fnRSqrt          = 0x0058,
   fnRPi            = 0x0059,
   fnRefGetLenZ     = 0x005A,
   fnRefCreate      = 0x005B,
   fnBSCreate       = 0x005C,
   fnNSave          = 0x005D,
   fnLSave          = 0x005E,
   fnWSSave         = 0x005F,
   fnWSReserve      = 0x0060,
   fnBSSave         = 0x0061,
   fnBSReserve      = 0x0062,
   fnBSGetLen       = 0x0063,
   fnBSSetLen       = 0x0064,
   fnWSLoad         = 0x0065,
   fnBSLoad         = 0x0066,
   fnBSGetInt       = 0x0067,
   fnNCopyWord      = 0x0068,
   fnGetAt          = 0x0069, 
};

#define EXTENSION_COUNT 0x69

enum PseudoArg
{
   baNone          = 0,
   baFirstLabel    = 1,
   baCurrentLabel  = 2,
   baPreviousLabel = 3,
   baPrev2Label    = 4, // before previous
};

enum Predicate
{
   bpNone  = 0,
   bpFrame = 1,
   bpBlock = 2
};

enum TapeStructure
{
   bsNone        = 0x0,
   bsSymbol      = 0x1,
   bsClass       = 0x2,
   bsMethod      = 0x3,
   bsBranch      = 0x5,
   bsImport      = 0x6,
};

struct ByteCommand
{
   ByteCode  code;
   int       argument;
   int       additional;
   Predicate predicate;

   int Argument() const { return argument; }

   operator ByteCode() const { return code; }

   ByteCommand()
   {
      code = bcNop;
      argument = 0;
      additional = 0;
      predicate = bpNone;
   }
   ByteCommand(ByteCode code)
   {
      this->code = code;
      this->argument = 0;
      this->additional = 0;
      this->predicate = bpNone;
   }
   ByteCommand(ByteCode code, int argument)
   {
      this->code = code;
      this->argument = argument;
      this->additional = 0;
      this->predicate = bpNone;
   }
   ByteCommand(ByteCode code, int argument, int additional)
   {
      this->code = code;
      this->argument = argument;
      this->additional = additional;
      this->predicate = bpNone;
   }
   ByteCommand(ByteCode code, int argument, int additional, Predicate predicate)
   {
      this->code = code;
      this->argument = argument;
      this->additional = additional;
      this->predicate = predicate;
   }

   void save(MemoryWriter* writer, bool commandOnly = false)
   {
      writer->writeByte((unsigned char)code);
      if (!commandOnly && (code >= 0x20)) {
         writer->writeDWord(argument);
      }
      if (!commandOnly && (code >= 0xF0)) {
         writer->writeDWord(additional);
      }
   }
};

// --- ByteCodeCompiler ---

class ByteCodeCompiler
{
public:
   static void loadVerbs(MessageMap& verbs);
   static void loadOperators(MessageMap& operators);

   static ByteCode code(const wchar16_t* s);
   static FunctionCode codeFunction(const wchar16_t* s);

   static const wchar16_t* decode(ByteCode code, wchar16_t* s);
   static const wchar16_t* decodeFunction(FunctionCode code, wchar16_t* s);
};

// --- CommandTape ---
typedef BList<ByteCommand>::Iterator ByteCodeIterator;

struct CommandTape
{
   BList<ByteCommand> tape;   // !! should we better use an array?

   int        labelSeed;
   Stack<int> labels;

   ByteCodeIterator start() { return tape.start(); }

   ByteCodeIterator end() { return tape.end(); }

   int newLabel()
   {
      labelSeed++;

      labels.push(labelSeed);

      return labelSeed;
   }

   void setLabel(bool persist = false)
   {
      if (persist) {
         write(blLabel, labels.peek());
      }
      else write(blLabel, labels.pop());
   }

   void setPreviousLabel()
   {
      int lastLabel = labels.pop();

      write(blLabel, labels.pop());

      labels.push(lastLabel);
   }

   // to resolve possible conflicts the predefined labels should be negative
   void setPredefinedLabel(int label)
   {
      write(blLabel, label);
   }

   void releaseLabel()
   {
      labels.pop();
   }

   ByteCodeIterator find(ByteCode code);
   ByteCodeIterator find(ByteCode code, int argument);

   int resolvePseudoArg(PseudoArg argument);

   void write(ByteCode code);
   void write(ByteCode code, int argument);
   void write(ByteCode code, PseudoArg argument);
   void write(ByteCode code, int argument, int additional);
   void write(ByteCode code, PseudoArg argument, int additional);
   void write(ByteCode code, TapeStructure argument, int additional);
   void write(ByteCode code, int argument, int additional, Predicate predicate);
   void write(ByteCode code, int argument, Predicate predicate);
   void write(ByteCommand command);
   void insert(ByteCodeIterator& it, ByteCommand command);

//   ByteCommand extract()
//   {
//      ByteCommand command = *tape.end();
//      tape.cut(tape.end());
//
//      return command;
//   }

   void import(_Memory* section, bool withHeader = false);

   static bool optimizeJumps(CommandTape& tape);
   static bool import(ByteCommand& command, _Module* sour, _Module* dest);

   CommandTape()
   {
      labelSeed = 0;
   }

   void clear()
   {
      tape.clear();
      labelSeed = 0;
      labels.clear();
   }
};

// --- ByteRule ---

enum PatternArgument
{
   braNone = 0,
   braValue,
   braAdd,
   braCopy,
   braMatch
};

struct ByteCodePattern
{
   ByteCode        code;
   PatternArgument argumentType;
   size_t          argument;

   bool operator ==(ByteCode code) const
   {
      return (this->code == code);
   }

   bool operator !=(ByteCode code) const
   {
      return (this->code != code);
   }

   bool operator ==(ByteCommand command) const
   {
      return this->code == command.code && (argumentType != braMatch || argument == command.argument);
   }

   bool operator !=(ByteCommand command) const
   {
      return !(*this == command);
   }

   bool operator ==(ByteCodePattern pattern)
   {
      return (code == pattern.code && argumentType == pattern.argumentType && argument == pattern.argument);
   }

   bool operator !=(ByteCodePattern pattern)
   {
      return !(*this == pattern);
   }

   ByteCodePattern()
   {
      code = bcNone;
      argumentType = braNone;
      argument = 0;
   }
   ByteCodePattern(ByteCode code)
   {
      this->code = code;
      this->argumentType = braNone;
      this->argument = 0;
   }
};

// --- TransformTape ---

struct TransformTape
{
   typedef MemoryTrie<ByteCodePattern>     MemoryByteTrie;
   typedef MemoryTrieNode<ByteCodePattern> Node;

   MemoryByteTrie trie;
   bool           loaded;

   bool apply(CommandTape& tape);
   void transform(ByteCodeIterator& trans_it, Node replacement);

   bool makeStep(Node& step, ByteCommand& command);

   void load(StreamReader* optimization)
   {
      loaded = true;
      trie.load(optimization);
   }

   TransformTape()
      : trie(ByteCodePattern(bcNone))
   {
      loaded = false;
   }
};

} // _ELENA_

#endif // bytecodeH
