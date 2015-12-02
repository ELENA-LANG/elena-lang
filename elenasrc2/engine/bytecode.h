//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//
//                                              (C)2009-2015, by Alexei Rakov
//------------------------------------------------------------------------------

#ifndef bytecodeH
#define bytecbcpopodeH 1

namespace _ELENA_
{

// --- Byte code command set ---
enum ByteCode
{
   // commands:
   bcNop             = 0x00,
   bcBreakpoint      = 0x01,
   bcPushB           = 0x02,
   bcPop             = 0x03,
   bcSNop            = 0x04,
   bcPushE           = 0x05,
   bcDCopyVerb       = 0x06,
   bcThrow           = 0x07,
   bcDCopyCount      = 0x08,
   bcOr              = 0x09,
   bcPushA           = 0x0A,
   bcPopA            = 0x0B,
   bcACopyB          = 0x0C,
   bcPopE            = 0x0D,
   bcBSRedirect      = 0x0E,
   bcDCopySubj       = 0x0F,

   bcNot             = 0x10,
   bcLen             = 0x11,
   bcBCopyA          = 0x12,
   bcDec             = 0x13,
   bcPopB            = 0x14,
   bcClose           = 0x15,
   bcSub             = 0x16,
   bcQuit            = 0x17,
   bcGet             = 0x18,
   bcSet             = 0x19,
   bcInc             = 0x1A,
   bcEQuit           = 0x1B,
   bcUnhook          = 0x1D,
   bcAdd             = 0x1E,
   bcCreate          = 0x1F,

   bcECopyD          = 0x20,
   bcDCopyE          = 0x21,
   bcPushD           = 0x22,
   bcPopD            = 0x23,
   bcDReserve        = 0x24,
   bcDRestore        = 0x25,
   bcExclude         = 0x26,   
   bcTryLock         = 0x27,
   bcFreeLock        = 0x28,
   bcESwap           = 0x2C,
   bcBSwap           = 0x2D,
   bcCopy            = 0x2E,           
   bcXSet            = 0x2F,

   bcXLen            = 0x30,
   bcBLen            = 0x31,
   bcWLen            = 0x32,
   bcFlag            = 0x33, 
   bcNLen            = 0x34,
   bcClass           = 0x36,
   bcMIndex          = 0x37,
   bcCall            = 0x38,
   bcACallVD         = 0x39,
   bcValidate        = 0x3A,

   bcNEqual          = 0x40,
   bcNLess           = 0x41,
   bcNCopy           = 0x42,
   bcNAdd            = 0x43,
   bcNSub            = 0x44,
   bcNMul            = 0x45,
   bcNDiv            = 0x46,
   bcNSave           = 0x47,
   bcNLoad           = 0x48,
   bcDCopyR          = 0x49,
   bcNAnd            = 0x4A,
   bcNOr             = 0x4B,
   bcNXor            = 0x4C,
   bcNShift          = 0x4D,
   bcNNot            = 0x4E,
   bcNCreate         = 0x4F,

   bcNCopyB          = 0x50,
   bcLCopyB          = 0x51,
   bcCopyB           = 0x52,

   bcWRead           = 0x59,
   bcWWrite          = 0x5A,
   bcNRead           = 0x5B,
   bcNWrite          = 0x5C,
   bcWCreate         = 0x5F,

   bcBReadW          = 0x60,
   bcBRead           = 0x61,
   bcBReadB          = 0x65,
   bcRSin            = 0x66,
   bcRCos            = 0x67,
   bcRArcTan         = 0x68,
   bcBWrite          = 0x69,
   bcBWriteB         = 0x6C,
   bcBWriteW         = 0x6D,
   bcBCreate         = 0x6F,

   bcLCopy           = 0x70,
   bcLEqual          = 0x72,
   bcLLess           = 0x73,
   bcLAdd            = 0x74,
   bcLSub            = 0x75,
   bcLMul            = 0x76,
   bcLDiv            = 0x77,
   bcLAnd            = 0x78,
   bcLOr             = 0x79,
   bcLXor            = 0x7A,
   bcLShift          = 0x7B,
   bcLNot            = 0x7C,

   bcRCopy           = 0x80,
   bcRSave           = 0x82,
   bcREqual          = 0x83,
   bcRLess           = 0x84,
   bcRAdd            = 0x85,
   bcRSub            = 0x86,
   bcRMul            = 0x87,
   bcRDiv            = 0x88,
   bcRExp            = 0x8A,
   bcRLn             = 0x8B,
   bcRAbs            = 0x8C,
   bcRRound          = 0x8D,
   bcRInt            = 0x8E,
   bcRLoad           = 0x8F,

   bcDCopy           = 0x90,
   bcECopy           = 0x91,
   bcRestore         = 0x92,
   bcALoadR          = 0x93,
   bcALoadFI         = 0x94,
   bcALoadSI         = 0x95,
   bcIfHeap          = 0x96,
   bcBCopyS          = 0x97,
   bcOpen            = 0x98,
   bcQuitN           = 0x99,
   bcBCopyR          = 0x9A,
   bcBCopyF          = 0x9B,
   bcACopyF          = 0x9C,
   bcACopyS          = 0x9D,
   bcACopyR          = 0x9E,
   bcCopyM           = 0x9F,

   bcJump            = 0xA0,
   bcAJumpVI         = 0xA1,
   bcACallVI         = 0xA2,
   bcCallR           = 0xA3,
   bcCallExtR        = 0xA5,
   bcHook            = 0xA6,
   bcAddress         = 0xA7,
   bcLess            = 0xA9,
   bcNotLess         = 0xAA,
   bcIfB             = 0xAB,
   bcElseB           = 0xAC,
   bcIf              = 0xAD,
   bcElse            = 0xAE,
   bcNext            = 0xAF,

   bcPushN           = 0xB0,
   bcELoadFI         = 0xB1,
   bcPushR           = 0xB2,
   bcPushAI          = 0xB4,
   bcESaveFI         = 0xB5,
   bcPushFI          = 0xB6,
   bcDLoadFI         = 0xB7,
   bcDLoadSI         = 0xB8,
   bcDSaveFI         = 0xB9,
   bcPushSI          = 0xBA,
   bcDSaveSI         = 0xBB,
   bcELoadSI         = 0xBC,
   bcPushF           = 0xBD,
   bcESaveSI         = 0xBE,
   bcReserve         = 0xBF,   // should be used only for unmanaged stack (stack may contains old references, which may break GC)

   bcASaveBI         = 0xC0,
   bcASwapSI         = 0xC2,
   bcASaveSI         = 0xC3,
   bcASaveFI         = 0xC4,
   bcBSwapSI         = 0xC5,
   bcESwapSI         = 0xC6,
   bcDSwapSI         = 0xC7,
   bcBLoadFI         = 0xC8,
   bcBLoadSI         = 0xC9,
   bcNLoadI          = 0xCA,
   bcNSaveI          = 0xCB,
   bcASaveR          = 0xCC,
   bcALoadAI         = 0xCD,
   bcALoadBI         = 0xCE,
   bcAXSaveBI        = 0xCF,

   bcPopI            = 0xD0,
   bcSCopyF          = 0xD2,
   bcSetVerb         = 0xD3,
   bcSetSubj         = 0xD4,
   bcAndN            = 0xD5,
   bcAddN            = 0xD6,
   bcOrN             = 0xD7,
   bcEAddN           = 0xD8,
   bcShiftN          = 0xD9,
   bcMulN            = 0xDA,

   bcNew             = 0xF0,
   bcNewN            = 0xF1,
   bcXSelectR        = 0xF3,
   bcXIndexRM        = 0xF4,
   bcXJumpRM         = 0xF5,
   bcSelectR         = 0xF6,
   bcLessN           = 0xF7,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcIfM             = 0xF8,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcElseM           = 0xF9,   // though in bytecode section they saved in the correct order (jump arg, label)
   bcIfR             = 0xFA,
   bcElseR           = 0xFB,   
   bcIfN             = 0xFC,
   bcElseN           = 0xFD,   
   bcXCallRM         = 0xFE,
   bcReserved        = 0xFF,

   // labels
   blLabelMask      = 0xC000,  // tape label mask
   blBegin          = 0xC001,  // meta command, declaring the structure
   blEnd            = 0xC002,  // meta command, closing the structure
   blLabel          = 0xC003,  // meta command, declaring the label

   // meta commands:
   bcAllocStack     = 0x8101,  // meta command, used to indicate that the previous command allocate number of items in the stack; used only for exec
   bcFreeStack      = 0x8102,  // meta command, used to indicate that the previous command release number of items from stack; used only for exec
   bcResetStack     = 0x8103,  // meta command, used to indicate that the previous command release number of items from stack; used only for exec

   bcMatch          = 0x8FFE,  // used in optimization engine
   bcNone           = 0x8FFF,  // used in optimization engine

   blDeclare        = 0x8120,  // meta command, closing the structure
   blStatement      = 0x8121,  // meta command, declaring statement
   blBlock          = 0x8122,  // meta command, declaring sub code

   // debug info
   bdDebugInfo      = 0x8400,
   bdBreakpoint     = 0x8401,
   bdBreakcoord     = 0x8402,
   bdLocal          = 0x8403,
   bdSelf           = 0x8404,
   bdMessage        = 0x8405,
   bdLocalInfo      = 0x8406,
   bdIntLocal       = 0x8413,
   bdLongLocal      = 0x8423,
   bdRealLocal      = 0x8433,
   bdParamsLocal    = 0x8443,
   bdByteArrayLocal = 0x8453,
   bdShortArrayLocal= 0x8463,
   bdIntArrayLocal  = 0x8473,
   bdStruct         = 0x8406,
};

#define MAX_SINGLE_ECODE 0x8F
#define MAX_DOUBLE_ECODE 0xEF

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
      if (!commandOnly && (code > MAX_SINGLE_ECODE)) {
         writer->writeDWord(argument);
      }
      if (!commandOnly && (code > MAX_DOUBLE_ECODE)) {
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

   static ByteCode code(ident_t s);
//   static FunctionCode codeFunction(const wchar16_t* s);

   static ident_t decode(ByteCode code, ident_c* s);
//   static const wchar16_t* decodeFunction(FunctionCode code, wchar16_t* s);

   static bool IsJump(ByteCode code)
   {
      switch(code) {
         case bcJump:
         case bcIfR:
         case bcElseR:
         case bcIfB:
         case bcElseB:
         case bcIf:
         case bcElse:
         case bcLess:
         case bcNotLess:
         case bcIfN:
         case bcElseN:
         case bcLessN:
         case bcIfM:
         case bcElseM:
         case bcNext:
         case bcIfHeap:
         case bcHook:
         case bcAddress:
         case bcXJumpRM:
            return true;
         default:
            return false;
         }
   }

   static bool IsRCode(ByteCode code)
   {
      switch(code) {
         case bcPushR:
         //case bcEvalR:
         case bcCallR:
         case bcALoadR:
         case bcASaveR:
         case bcACopyR:
         case bcNew:
         case bcNewN:
         case bcBCopyR:
         case bcXCallRM:
         case bcCallExtR:
         case bcSelectR:
         case bcXJumpRM:
         case bcXIndexRM:
            return true;
         default:
            return false;
      }
   }

   static bool IsR2Code(ByteCode code)
   {
      switch(code) {
         case bcIfR:
         case bcElseR:
         case bcSelectR:
            return true;
         default:
            return false;
      }
   }

   static bool IsPush(ByteCode code)
   {
      switch(code) {
         case bcPushA:
         case bcPushB:
         case bcPushFI:
         case bcPushN:
         case bcPushR:
         case bcPushSI:
         case bcPushAI:
         case bcPushF:
         case bcPushE:
         case bcPushD:
            return true;
         default:
            return false;
      }
   }

   static bool IsPop(ByteCode code)
   {
      switch(code) {
         case bcPop:
         case bcPopA:
         case bcPopI:
         case bcPopB:
         case bcPopE:
         case bcPopD:
            return true;
         default:
            return false;
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

   static bool optimizeIdleBreakpoints(CommandTape& tape);
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
   int             argument;

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
