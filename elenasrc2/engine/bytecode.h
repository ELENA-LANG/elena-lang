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
   bcPushSelf       = 0x02,
   bcPop            = 0x03, 
   bcWriteAcc       = 0x04, 
   bcPushMcc        = 0x05,
   bcMccCopyVerb    = 0x06,
   bcThrow          = 0x07,
   //bcSNop           = 0x08,
   bcMccCopySubj    = 0x09,
   bcPushAcc        = 0x0A,
   bcPopAcc         = 0x0B,
   bcAccCopySelf    = 0x0C, 
   bcPopMcc         = 0x0D,
   bcBSRedirect     = 0x0E, 
   //bcMccCopyAcc     = 0x0F,

   bcInit           = 0x12,
   bcPopSelf        = 0x14,
   bcClose          = 0x15,
   bcJumpAcc        = 0x16,
   bcQuit           = 0x17, 
   bcGet            = 0x18,
   bcSet            = 0x19,
   bcQuitMcc        = 0x1B,
   bcUnhook         = 0x1D,
   bcExclude        = 0x1E, 
   bcInclude        = 0x1F, 
 
   // all 2x command are push ones
   bcReserve        = 0x20,
   bcPushN          = 0x21,
   bcPushR          = 0x22,
   bcPushSelfI      = 0x23,
   bcPushAccI       = 0x24,
   bcPushI          = 0x25,
   bcPushFI         = 0x26,
   bcMccCopyPrmFI   = 0x29,
   bcPushSI         = 0x2A,
   bcPushFPI        = 0x2D,
   bcXPushFPI       = 0x2E,
   bcPushSPI        = 0x2F,

   // all 3x commands are pop ones
   bcPopN           = 0x30,
   bcPopSelfI       = 0x31,
   bcPopFI          = 0x32,
   bcXPopAccI       = 0x33,
   bcPopSI          = 0x34,
   bcPopAccI        = 0x35,
   //bcAccTryN        = 0x36,
   //bcAccTryR        = 0x37,
   bcQuitN          = 0x39, 

   bcCallExtR       = 0x40,
   bcEvalR          = 0x41,
   bcCallAcc        = 0x42,
   bcCallR          = 0x43,
   //bcSendVMTR       = 0x46,
   bcMccCopyAccI    = 0x47,
   bcMccCopySI      = 0x48,
   bcMccCopyFI      = 0x49,
   bcMccAddAccI     = 0x4D,
   bcMccCopyM       = 0x4E,     
   bcMccAddM        = 0x4F,

   bcIncSI          = 0x50,
   bcIncFI          = 0x51,
   //bcAccInc         = 0x52,
   bcAccLoadR       = 0x53,
   bcAccLoadFI      = 0x54,
   bcAccLoadSI      = 0x55,
   //bcAccTestFlagN   = 0x58,

   bcAccSaveSelfI   = 0x61,
   bcAccSaveSI      = 0x63,
   bcAccSaveFI      = 0x64,
   bcAccSaveR       = 0x65,
   bcAccSaveDstSI   = 0x67,
   bcSwapSI         = 0x6C,
   bcAccSwapSI      = 0x6D,
   bcXAccSaveFI     = 0x6F,

   bcCopyFPI        = 0x72,
   //bcAccCopySPI     = 0x75,
   bcAccCopyR       = 0x78,
   bcAccCopyN       = 0x79,
   bcAccLoadAccI    = 0x7A,
   //bcAccCopyM       = 0x7B,
   bcXAccCopyFPI    = 0x7B,
   bcAccCopyFPI     = 0x7C,
   bcAccAddN        = 0x7E,

   bcRethrow        = 0x80,
   bcRestore        = 0x82, 
   bcOpen           = 0x88,

   //bcTryLock        = 0x90,
   //bcFreeLock       = 0x91,
   //bcSPTryLock      = 0x92,
   //bcAccFreeLock    = 0x93,

   bcJump           = 0xA0,
   bcJumpAccN       = 0xA1,
   //bcJumpR          = 0xA3,
   bcHook           = 0xA8,
   bcElse           = 0xA9,
   bcThen           = 0xAA,
   bcMccElseAcc     = 0xAB,
   bcMccThenAcc     = 0xAC,
   //bcElseLocal      = 0xAF,

   bcNWrite         = 0xB0,
   bcGetLen         = 0xB1,

   bcAccGetSI       = 0xC0,
   bcAccGetFI       = 0xC1,
   //bcAccGetAccSI    = 0xC2,
   bcAccCreate      = 0xC3,
   //bcAccMergeR      = 0xC8,

   bcElseR          = 0xE0,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcThenR          = 0xE1,   // though in bytecode section they saved in the correct order (jump arg, label)
   bcMccElse        = 0xE2, 
   bcMccThen        = 0xE3,
   //bcElseN          = 0xE4,
   //bcThenN          = 0xE5,
   bcElseSI         = 0xE6,
   bcThenSI         = 0xE7,
   //bcMccElseSI      = 0xE8,
   //bcMccThenSI      = 0xE9,
   bcMccElseAccI    = 0xEA,
   bcMccThenAccI    = 0xEB,
   bcElseFlag       = 0xEC,
   bcThenFlag       = 0xED,
   bcAccLoadSelfI   = 0xEE,

   bcCreate         = 0xF0,
   bcCreateN        = 0xF1,
   bcIAccCopyR      = 0xF2,
   bcIAccFillR      = 0xF3,
   //bcIAccCopyN      = 0xF4,
   bcAccCreateN     = 0xF5,
   bcAccBoxN        = 0xF6,

   bcCallSI         = 0xFC,
   bcRCallN         = 0xFD,
   bcRCallM         = 0xFE,

   bcReserved       = 0xFF,

   // labels
   blLabelMask     = 0x8000,  // tape label mask
   blBegin         = 0x8001,  // meta command, declaring the structure
   blEnd           = 0x8002,  // meta command, closing the structure
   blLabel         = 0x8003,  // meta command, declaring the label

   // meta commands:
   bcAllocStack     = 0x101,  // meta command, used to indicate that the previous command allocate number of items in the stack; used only for exec
   bcFreeStack      = 0x102,  // meta command, used to indicate that the previous command release number of items from stack; used only for exec
   //blHint           = 0x204,  // meta command, compiler hint

   // pseudo block commands
   bcPushBI         = 0x326, 
   bcAccLoadBI      = 0x354, 
   bcAccSaveBI      = 0x364, 
   bcPushBPI        = 0x32E, 

   bcMatch          = 0xFFE,  // used in optimization engine
   bcNone           = 0xFFF,  // used in optimization engine

   blDeclare        = 0x120,  // meta command, closing the structure
   blStatement      = 0x121,

   // debug info
   bdBreakpoint   = 0x401,
   bdBreakcoord   = 0x402,
   bdLocal        = 0x403,
   bdSelf         = 0x404,
   bdIntLocal     = 0x413,
   bdLongLocal    = 0x423,
   bdRealLocal    = 0x433,
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
   