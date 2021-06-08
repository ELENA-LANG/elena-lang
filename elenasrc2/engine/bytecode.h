//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//
//                                              (C)2009-2021, by Alexei Rakov
//------------------------------------------------------------------------------

#ifndef bytecodeH
#define bytecodeH 1

namespace _ELENA_
{

// --- Byte code command set ---
enum ByteCode
{
   // commands:
   bcNop             = 0x00,
   bcBreakpoint      = 0x01,
   bcCoalesce        = 0x02,
   bcPeek            = 0x03,
   bcSNop            = 0x04,
   bcPushVerb        = 0x05,
   bcLoadVerb        = 0x06,
   bcThrow           = 0x07,
   bcMCount          = 0x08,
   bcPush            = 0x09,
   bcPushA           = 0x0A,
   bcPopA            = 0x0B,
   bcXNew            = 0x0C,
   bcStoreV          = 0x0D,
   bcBSRedirect      = 0x0E,
   bcSetV            = 0x0F,

   bcNot             = 0x10,
   bcOpen            = 0x11,
   bcPop             = 0x12,
   bcSub             = 0x13,
   bcSwapD           = 0x14,
   bcClose           = 0x15,
   bcRExp            = 0x16,
   bcQuit            = 0x17,
   bcGet             = 0x18,
   bcSet             = 0x19,
   bcSwap            = 0x1A,
   bcMQuit           = 0x1B,
   bcCount           = 0x1C,
   bcUnhook          = 0x1D,
   bcRSin            = 0x1E,
   bcAllocD          = 0x1F,

   bcRCos            = 0x20,
   bcRArcTan         = 0x21,
   bcPushD           = 0x22,
   bcPopD            = 0x23,
   bcXTrans          = 0x24,
   bcInclude         = 0x25,     // should immediately follow exclude (after callextr)
   bcExclude         = 0x26,   
   bcTryLock         = 0x27,
   bcFreeLock        = 0x28,
   bcFreeD           = 0x29,
   bcLoadEnv         = 0x2A,
   bcStore           = 0x2B,
   bcRLn             = 0x2C,
   bcRead            = 0x2D,
   bcClone           = 0x2E,           
   bcXSet            = 0x2F,

   bcRAbs            = 0x30,
   bcLen             = 0x31,
   bcRLoad           = 0x32,
   bcFlag            = 0x33,
//   bcNLen            = 0x34,
   bcParent          = 0x35,
   bcClass           = 0x36,
   bcMIndex          = 0x37,
//   bcCheck           = 0x38,
//   bcACallVD         = 0x39,
//   bcValidate        = 0x3A,
//   bcDMoveVerb       = 0x3C,
   bcRRound          = 0x3D,
   bcEqual           = 0x3E,

   bcNEqual          = 0x40,
   bcNLess           = 0x41,
//   bcNCopy           = 0x42,
   bcLEqual          = 0x43,
   bcLLess           = 0x44,
   bcRSet            = 0x45,
   bcRSave           = 0x46,
   bcSave            = 0x47,
   bcLoad            = 0x48,
   bcRSaveN          = 0x49,
   bcRSaveL          = 0x4A,
   bcLSave           = 0x4B,
   bcLLoad           = 0x4C,
//   bcNShiftL         = 0x4D,
//   bcNNot            = 0x4E,
   bcRInt            = 0x4F,

   bcAddF            = 0x50,
   bcSubF            = 0x51,
   bcNXorF           = 0x52,
   bcNOrF            = 0x53,
   bcNAndF           = 0x54,
   bcMovFIPD         = 0x55,
   bcXSave           = 0x5A,
   bcDiv             = 0x5B,
   bcXWrite          = 0x5C,
   bcCopyTo          = 0x5D,
   bcNShlF           = 0x5E,
   bcNShrF           = 0x5F,

   bcMul             = 0x60,
   bcCheckSI         = 0x61,
   bcXRedirect       = 0x62,
   bcXVRedirect      = 0x63,

   bcLAddF           = 0x74,
   bcLSubF           = 0x75,
   bcLMulF           = 0x76,
   bcLDivF           = 0x77,
   bcLAndF           = 0x78,
   bcLOrF            = 0x79,
   bcLXorF           = 0x7A,
   bcLShlF           = 0x7B,
//   bcLNot            = 0x7C,
   bcLShrF           = 0x7D,

   bcRAddNF          = 0x80, 
   bcRSubNF          = 0x81,
   bcRMulNF          = 0x82,
   bcREqual          = 0x83,
   bcRLess           = 0x84,
   bcRAddF           = 0x85,
   bcRSubF           = 0x86,
   bcRMulF           = 0x87,
   bcRDivF           = 0x88,
   bcRDivNF          = 0x89,
   bcRIntF           = 0x8E,

   bcDec             = 0x90,
   bcGetI            = 0x91,
   bcRestore         = 0x92,
   bcPeekR           = 0x93,
   bcPeekFI          = 0x94,
   bcPeekSI          = 0x95,
   bcIfHeap          = 0x96,
   bcXSetI           = 0x97,
   bcQuitN           = 0x99,
   bcCreate          = 0x9A,
   bcFillR           = 0x9B,
   bcMovF            = 0x9C,
   bcMovSIP          = 0x9D,
   bcMovR            = 0x9E,
   bcMovM            = 0x9F,

   bcJump            = 0xA0,
   bcJumpVI          = 0xA1,
   bcCallVI          = 0xA2,
   bcCallR           = 0xA3,
   bcJumpI           = 0xA4,
   bcSetFrame        = 0xA5,
   bcHook            = 0xA6,
   bcAddress         = 0xA7,
   bcCallI           = 0xA8,
//   bcLess            = 0xA9,
   bcNotLess         = 0xAA,
   bcNotGreater      = 0xAB,
   bcElseD           = 0xAC,
   bcIf              = 0xAD,
   bcElse            = 0xAE,
   bcIfCount         = 0xAF,

   bcPushN           = 0xB0,
   bcMovN            = 0xB1,
   bcPushR           = 0xB2,
   bcEqualFI         = 0xB3,
   bcPushAI          = 0xB4,
   bcLoadF           = 0xB5,
   bcPushFI          = 0xB6,
   bcLoadFI          = 0xB7,
   bcLoadSI          = 0xB8,
   bcSaveF           = 0xB9,
   bcPushSI          = 0xBA,
   bcSaveSI          = 0xBB,
   bcSaveFI          = 0xBC,
   bcPushF           = 0xBD,
   bcPushSIP         = 0xBE,
   bcReserve         = 0xBF,   // should be used only for unmanaged stack (stack may contains old references, which may break GC)

   bcSetI            = 0xC0,
   bcMovFIP          = 0xC1,
   bcPushFIP         = 0xC2,
   bcStoreSI         = 0xC3,
   bcStoreFI         = 0xC4,
   bcNAddF           = 0xC5,
   bcNMulF           = 0xC6,
   bcXSetR           = 0xC7,
   bcNSubF           = 0xC8,
   bcNDivF           = 0xC9,
   bcLoadI           = 0xCA,
   bcSaveI           = 0xCB,
   bcStoreR          = 0xCC,
   bcXOr             = 0xCD,
   bcCloneF          = 0xCE,
   bcXLoad           = 0xCF,

   bcFreeI           = 0xD0,
   bcAllocI          = 0xD1, 
   bcXCreate         = 0xD2,
   bcMovV            = 0xD3,
   bcShl             = 0xD4,
   bcAnd             = 0xD5,
   bcInc             = 0xD6,
   bcOr              = 0xD7,
   bcCoalesceR       = 0xD8,
   bcShr             = 0xD9,   

   bcXSaveLenF       = 0xDA,
   bcVJumpRM         = 0xDB,
   bcXSaveAI         = 0xDC,
   bcCopyAI          = 0xDD,
   bcMove            = 0xDE,
   bcMoveTo          = 0xDF,

   bcReadToF         = 0xE0,
   bcCreateN         = 0xE1,
   bcXSetFI          = 0xE2,
   bcCopyToAI        = 0xE3,
   bcCopyToFI        = 0xE4,
   bcCopyToF         = 0xE5,
   bcCopyFI          = 0xE6,
   bcCopyF           = 0xE7,
   bcMTRedirect      = 0xE8,
   bcXMTRedirect     = 0xE9,
   bcGreaterN        = 0xEA,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcNotGreaterN     = 0xEB,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcNotLessN        = 0xEC,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcXRSaveF         = 0xED,
   bcXAddF           = 0xEE,
   bcXSaveF          = 0xEF,

   bcNew             = 0xF0,
   bcNewN            = 0xF1,
   bcFillRI          = 0xF2,
   bcXSelectR        = 0xF3,
   bcVCallRM         = 0xF4,
   bcJumpRM          = 0xF5,
   bcSelect          = 0xF6,
   bcLessN           = 0xF7,   // note that for code simplicity reverse order is used for jump parameters (jump label, arg)
   bcAllocN          = 0xF8,
   bcXSaveSI         = 0xF9,
   bcIfR             = 0xFA,
   bcElseR           = 0xFB,   
   bcIfN             = 0xFC,
   bcElseN           = 0xFD,   
   bcCallRM          = 0xFE,
   bcCallExtR        = 0xFF,

   // labels
   blLabelMask       = 0xC000,  // tape label mask
   blBegin           = 0xC001,  // meta command, declaring the structure
   blEnd             = 0xC002,  // meta command, closing the structure
   blLabel           = 0xC003,  // meta command, declaring the label
   blBreakLabel      = 0xC004,  // meta command, breaking the optimization rules

   // meta commands:
   //bcAllocStack     = 0x8101,  // meta command, used to indicate that the previous command allocate number of items in the stack; used only for exec
   //bcFreeStack      = 0x8102,  // meta command, used to indicate that the previous command release number of items from stack; used only for exec
   //bcResetStack     = 0x8103,  // meta command, used to indicate that the previous command release number of items from stack; used only for exec

   bcMatch          = 0x8FFE,  // used in optimization engine
   bcNone           = 0x8FFF,  // used in optimization engine

   //blDeclare        = 0x8120,  // meta command, closing the structure
   blStatement      = 0x8121,  // meta command, declaring statement
   blBlock          = 0x8122,  // meta command, declaring sub code

   // debug info
   //bdDebugInfo      = 0x8400,
   bdBreakpoint     = 0x8401,
   bdBreakcoord     = 0x8402,
   bdLocal          = 0x8403,
   bdSelf           = 0x8404,
   bdMessage        = 0x8405,
   bdLocalInfo      = 0x8406,
   bdSourcePath     = 0x8407,

   bdIntLocal       = 0x8413,
   bdIntLocalPtr    = 0x8418,
   bdLongLocal      = 0x8423,
   bdLongLocalPtr   = 0x8428,
   bdRealLocal      = 0x8433,
   bdRealLocalPtr   = 0x8438,
   bdParamsLocal    = 0x8443,
   bdByteArrayLocal = 0x8453,
   bdShortArrayLocal= 0x8463,
   bdIntArrayLocal  = 0x8473,
   bdStruct         = 0x8486,
   bdStructSelf     = 0x8484,

   baCallArgsMask   = 0x00FF,
   baReleaseArgs    = 0x0100,
   baExternalCall   = 0x0200,
   baLongCall       = 0x0400,
};

#define MAX_SINGLE_ECODE 0x4F
#define MAX_DOUBLE_ECODE 0xD8

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
   bsNone            = 0x0,
   bsSymbol          = 0x1,
   bsClass           = 0x2,
   bsMethod          = 0x3,
   bsAbstractMethod  = 0x4,
   bsImport          = 0x6,
   bsInitializer     = 0x7,
};

struct ByteCommand
{
   ByteCode  code;
   int       argument;
   int       additional;

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
      if (!commandOnly && (code > MAX_SINGLE_ECODE)) {
         writer->writeDWord(argument);
      }
      if (!commandOnly && (code > MAX_DOUBLE_ECODE)) {
         writer->writeDWord(additional);
      }
   }

   // save additional argument if required
   void saveAditional(MemoryWriter* writer)
   {
      if (code > MAX_DOUBLE_ECODE) {
         writer->writeDWord(additional);
      }
   }
};

// --- ByteCodeCompiler ---

class ByteCodeCompiler
{
public:
   //static void loadVerbs(MessageMap& verbs);
   static void loadOperators(MessageMap& operators, MessageMap& unaryOperators);

   static ByteCode code(ident_t s);

   static ident_t decode(ByteCode code, char* s);

   static bool IsJump(ByteCode code)
   {
      switch(code) {
         case bcJump:
         case bcIfR:
         case bcElseR:
         //case bcIfB:
         case bcElseD:
         case bcIf:
         case bcIfCount:
         case bcElse:
         //case bcLess:
         case bcNotLess:
         case bcNotGreater:
         case bcIfN:
         case bcElseN:
         case bcLessN:
         case bcNotLessN:
         case bcGreaterN:
         case bcNotGreaterN:
         //case bcIfM:
         //case bcElseM:
         //case bcNext:
         case bcIfHeap:
         case bcHook:
         case bcAddress:
            return true;
         default:
            return false;
         }
   }

   static bool IsRCode(ByteCode code)
   {
      switch(code) {
         case bcPushR:
         ////case bcEvalR:
         case bcCallR:
         case bcPeekR:
         case bcStoreR:
         case bcMovR:
         case bcNew:
         case bcFillRI:
         case bcFillR:
         case bcNewN:
         case bcAllocN:
         case bcCallRM:
         case bcCallExtR:
         case bcSelect:
         case bcJumpRM:
         case bcVJumpRM:
         case bcVCallRM:
         //case bcBLoadR:
         case bcCreate:
         case bcXCreate:
         case bcCreateN:
         case bcCoalesceR:
         case bcXSelectR:
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
         case bcSelect:
         case bcXSelectR:
            return true;
         default:
            return false;
      }
   }

   static bool IsM2Code(ByteCode code)
   {
      switch (code) {
         case bcVJumpRM:
         case bcVCallRM:
         case bcCallRM:
         //case bcIfM:
         //case bcElseM:
            return true;
         default:
            return false;
      }
   }

   static bool IsMCode(ByteCode code)
   {
      switch (code) {
         case bcMovM:
            return true;
         default:
            return false;
      }
   }

   static bool IsMNCode(ByteCode code)
   {
      switch (code) {
         case bcMovV:
            return true;
         default:
            return false;
      }
   }

   static bool IsPush(ByteCode code)
   {
      switch(code) {
         case bcPushA:
         //case bcPushB:
         case bcPushFI:
         case bcPushN:
         case bcPushR:
         case bcAllocI:
         case bcPushSI:
         case bcPushSIP:
         case bcPushAI:
         case bcPushF:
         case bcPushFIP:
         case bcPushD:
            return true;
         default:
            return false;
      }
   }

   static bool IsPop(ByteCode code)
   {
      switch(code) {
         //case bcPop:
         case bcPopA:
         case bcFreeI:
         //case bcPopB:
         //case bcPopE:
         case bcPopD:
            return true;
         default:
            return false;
      }
   }

   static bool resolveMessageName(IdentifierString& messageName, _Module* module, mssg_t messageRef);
};

// --- CommandTape ---
typedef BList<ByteCommand>::Iterator ByteCodeIterator;

struct CommandTape
{
   BList<ByteCommand> tape;   // !! should we better use an array?

   int            labelSeed;
   Stack<int>     labels;
   
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

   int exchangeFirstsLabel(int newLabel)
   {
      auto it = labels.end();

      int oldLabel = *it;
      *it = newLabel;

      return oldLabel;
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

//   ByteCommand extract()
//   {
//      ByteCommand command = *tape.end();
//      tape.cut(tape.end());
//
//      return command;
//   }

   void import(_Memory* section, bool withHeader = false, bool withBreakpoints = false);

   static bool optimizeIdleBreakpoints(CommandTape& tape);
   static bool optimizeJumps(CommandTape& tape);
   static bool importReference(ByteCommand& command, _Module* sour, _Module* dest);

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
   braAditionalValue,
   braAdd,
   braCopy,
   braMatch,
   braSame,          // TransformTape should perform xor operation with the argument (1 if same, 0 if different)
   braAdditionalSame // TransformTape should perform xor operation with the argument (1 if same, 0 if different)
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
      if (this->code == command.code) {
         if (argumentType == braSame || argumentType == braAdditionalSame) {
            return argument == 0;
         }
         else return argumentType != braMatch || argument == command.argument;
      }
      else return false;
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

   bool makeStep(Node& step, ByteCommand& command, int previousArg);

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
