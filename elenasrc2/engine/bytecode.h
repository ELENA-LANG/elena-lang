//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//		
//                                              (C)2009-2013, by Alexei Rakov
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
   bcPushM          = 0x05,
   bcMCopyVerb      = 0x06,
   bcThrow          = 0x07,
   bcMCopySubj      = 0x09,
   bcPushA          = 0x0A,
   bcPopA           = 0x0B,
   bcACopyB         = 0x0C,
   bcPopM           = 0x0D,
   bcBSRedirect     = 0x0E, 

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
   bcReserve        = 0x20,
   bcPushN          = 0x21,
   bcPushR          = 0x22,
   bcPushBI         = 0x23,
   bcPushAI         = 0x24,
   bcPushFI         = 0x26,
   bcMSaveParams    = 0x29,
   bcPushSI         = 0x2A,
   bcPushF          = 0x2D,
   bcXPushF         = 0x2E,

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
   bcMAddAI         = 0x4D,
   bcMCopy          = 0x4E,     
   bcMAdd           = 0x4F,

   bcDLoadSI        = 0x50,
   bcDSaveSI        = 0x51,
   bcDLoadFI        = 0x52,
   bcALoadR         = 0x53,
   bcALoadFI        = 0x54,
   bcALoadSI        = 0x55,
   bcDCopyI         = 0x56,
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
   bcAXSetR         = 0x6E,

   bcSCopyF         = 0x72,
   bcACopyS         = 0x75,
   bcACopyR         = 0x78,
   //bcACopyN         = 0x79,
   bcALoadAI        = 0x7A,
   bcAXCopyF        = 0x7B,
   bcACopyF         = 0x7C,
   //bcAAdd           = 0x7E,
   //bcAMul           = 0x7F,

   bcRestore        = 0x82, 
   bcOpen           = 0x88,

   bcJump           = 0xA0,
   bcAJumpVI        = 0xA1,
   bcHook           = 0xA6,
   bcDElse          = 0xA7,
   bcDThen          = 0xA8,
   bcAElse          = 0xA9,
   bcAThen          = 0xAA,

   bcALoadBI        = 0xCE,

   bcFunc           = 0xD0,
   bcNFunc          = 0xD1,
   bcLFunc          = 0xD2,
   bcWSFunc         = 0xD3,
   bcBSFunc         = 0xD4,
   bcRFunc          = 0xD5,
   bcBSTest         = 0xDD,
   bcTest           = 0xDE,
   bcWSTest         = 0xDF,

   bcAElseR         = 0xE0,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcAThenR         = 0xE1,   // though in bytecode section they saved in the correct order (jump arg, label)
   bcMElse          = 0xE2, 
   bcMThen          = 0xE3,
   bcAElseSI        = 0xE6,
   bcAThenSI        = 0xE7,
   bcMElseAI        = 0xEA,
   bcMThenAI        = 0xEB,
   bcElseFlag       = 0xEC,
   bcTestFlag       = 0xED,
   bcNext           = 0xEF,   

   bcCreate         = 0xF0,
   bcCreateN        = 0xF1,
   bcIAXCopyR       = 0xF2,
   bcBoxN           = 0xF6,
   bcSCallVI        = 0xFC,
   bcXCallRM        = 0xFE,


//   bcWriteAcc       = 0x304, 
//   //bcSNop           = 0x08,
//   //bcMccCopyAcc     = 0x0F,
//
//   bcMccReverse     = 0x310,
//   bcJumpAcc        = 0x316,

//   bcPushI          = 0x325,
//   bcPushSPI        = 0x32F,
//
//   //bcAccTryN        = 0x36,
//   //bcAccTryR        = 0x37,
//
//   //bcSendVMTR       = 0x46,
//   bcXMccCopyM      = 0x34C,
//
//   //bcAccTestFlagN   = 0x58,
//
//   bcXAccSaveFI     = 0x6F,
//
//   //bcAccCopyM       = 0x7B,
//
//   bcRethrow        = 0x80,

//   //bcTryLock        = 0x90,
//   //bcFreeLock       = 0x91,
//   //bcSPTryLock      = 0x92,
//   //bcAccFreeLock    = 0x93,
//
   //bcJumpR          = 0xA3,
//   bcMccElseAcc     = 0xAB,
//   bcMccThenAcc     = 0xAC,
//   //bcElseLocal      = 0xAF,
//
//   bcNWrite         = 0xB0,
//
//   bcAccGetSI       = 0xC0,
//   bcAccGetFI       = 0xC1,
//   //bcAccGetAccSI    = 0xC2,
//   bcAccCreate      = 0xC3,
//   bcAccFillR       = 0xC4,
//   //bcAccMergeR      = 0xC8,
//
//   //bcElseN          = 0xE4,
//   //bcThenN          = 0xE5,
//   //bcMccElseSI      = 0xE8,
//   //bcMccThenSI      = 0xE9,
//
//   bcIAccFillR      = 0xF3,
//   //bcIAccCopyN      = 0xF4,
//
//   bcRCallN         = 0xFD,

   bcReserved       = 0xFF,

   // labels
   blLabelMask      = 0x8000,  // tape label mask
   blBegin          = 0x8001,  // meta command, declaring the structure
   blEnd            = 0x8002,  // meta command, closing the structure
   blLabel          = 0x8003,  // meta command, declaring the label

   // meta commands:
   bcAllocStack     = 0x8101,  // meta command, used to indicate that the previous command allocate number of items in the stack; used only for exec
   bcFreeStack      = 0x8102,  // meta command, used to indicate that the previous command release number of items from stack; used only for exec
//   //blHint           = 0x204,  // meta command, compiler hint

   // pseudo block commands
   bcPushBlockI     = 0x8326, 
   bcALoadBlockI    = 0x8354, 
   bcASaveBlockI    = 0x8364, 
   bcPushBlockPI    = 0x832E, 
   bcALoadBlockPI   = 0x837C,

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
};

enum TypeCode
{
   tRef    = 0,
   tInt    = 1,
   tLong   = 2,
   tStr    = 3,
   tBytes  = 4,
   tReal   = 5,
   tLen    = 6,
   tShort  = 7,
   tParams = 8,
};

enum FunctionCode
{
   fnUnknown        = 0x0000,
   fnCreate         = 0x0004,
   fnEqual          = 0x0007,
   fnLess           = 0x0008,
   fnAnd            = 0x000A,
   fnOr             = 0x000B,
   fnXor            = 0x000C,
   fnNotGreater     = 0x0011,
   fnAdd            = 0x0013,
   fnSub            = 0x0014,
   fnMul            = 0x0015,
   fnDiv            = 0x0016,
   fnGetAt          = 0x0017,
   fnSetAt          = 0x001C,
   fnCopy           = 0x001F,
   fnIndexOf        = 0x0027,
   fnSave           = 0x002D,
   fnReserve        = 0x0031,
   fnLoad           = 0x0032,
   fnShift          = 0x0033,
   fnNot            = 0x0034,
   fnInc            = 0x0036,
   fnAddInt         = 0x0113,
   fnSubInt         = 0x0114,
   fnMulInt         = 0x0115,
   fnDivInt         = 0x0116,
   fnGetInt         = 0x0117,
   fnSetInt         = 0x011C,
   fnCopyInt        = 0x011F,
   fnAddLong        = 0x0213,
   fnSubLong        = 0x0214,
   fnMulLong        = 0x0215,
   fnDivLong        = 0x0216,
   fnCopyLong       = 0x021F,
   fnAddStr         = 0x0313,
   fnCopyStr        = 0x031F,
   fnCopyBuf        = 0x041F,
   fnGetWord        = 0x0717,
   fnGetBuf         = 0x0417,
   fnSetBuf         = 0x041C,
   fnSetWord        = 0x071C,
   fnIndexOfStr     = 0x0327,
   fnDeleteStr      = 0x032A,
   fnSaveStr        = 0x032D,
   fnLoadStr        = 0x0332,
   fnIndexOfWord    = 0x0727,
   fnGetLen         = 0x0606,
   fnSetLen         = 0x061D,
   fnCopyReal       = 0x051F,

   fnRndNew         = 0x0681,
   fnRndNext        = 0x0682,
   fnLn             = 0x0683,
   fnExp            = 0x0684,
   fnAbs            = 0x0685,
   fnRound          = 0x0686,
};

enum PseudoArg
{
   baNone          = 0,
   baFirstLabel    = 1,
   baCurrentLabel  = 2,
   baPreviousLabel = 3,
   baPrev2Label    = 4, // before previous
};

enum TapeStructure
{
   bsNone        = 0x0,
   bsSymbol      = 0x1,
   bsClass       = 0x2,
   bsMethod      = 0x3,
   bsBranch      = 0x5
};

struct ByteCommand
{
   ByteCode code;
   int      argument;
   int      additional;

   int Argument() const { return argument; }

   operator ByteCode() const { return code; }

   ByteCommand()
   {
      code = bcNop;
      argument = 0;
      additional = 0;
   }
   ByteCommand(ByteCode code)
   {
      this->code = code;
      this->argument = 0;
      this->additional = 0;
   }
   ByteCommand(ByteCode code, int argument)
   {
      this->code = code;
      this->argument = argument;
      this->additional = 0;
   }
   ByteCommand(ByteCode code, int argument, int additional)
   {
      this->code = code;
      this->argument = argument;
      this->additional = additional;
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
   void write(ByteCommand command);
   void insert(ByteCodeIterator& it, ByteCommand command);

   void import(_Memory* section);

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

   Node makeStep(Node& step, ByteCommand& command);

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