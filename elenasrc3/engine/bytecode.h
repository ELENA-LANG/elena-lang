//------------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains common ELENA byte code classes and constants
//
//                                                (C)2021-2023, by Aleksey Rakov
//------------------------------------------------------------------------------

#ifndef BYTECODE_H
#define BYTECODE_H

#include "elena.h"

namespace elena_lang
{

   // --- ByteCode ---
   enum class ByteCode : unsigned int
   {
      // commands:
      Nop            = 0x00,
      Breakpoint     = 0x01,
      Redirect       = 0x03,
      Quit           = 0x04,
      MovEnv         = 0x05,
      Load           = 0x06,
      Len            = 0x07,
      Class          = 0x08,
      Save           = 0x09,
      Throw          = 0x0A,  
      Unhook         = 0x0B,
      LoadV          = 0x0C,
      XCmp           = 0x0D,
      BLoad          = 0x0E,
      WLoad          = 0x0F,

      Exclude        = 0x10,
      Include        = 0x11,
      Assign         = 0x12,
      MovFrm         = 0x13,
      LoadS          = 0x14,
      MLen           = 0x15,
      DAlloc         = 0x16,
      XAssignSP      = 0x17,
      DTrans         = 0x18,
      XAssign        = 0x19,
      LLoad          = 0x1A,
      ConvL          = 0x1B,
      XLCmp          = 0x1C,
      XLoad          = 0x1D,
      XLLoad         = 0x1E,

      Coalesce       = 0x20,
      Not            = 0x21,
      Neg            = 0x22,
      BRead          = 0x23,
      LSave          = 0x24,
      FSave          = 0x25,
      WRead          = 0x26,
      XJump          = 0x27,
      XGet           = 0x2E,
      XCall          = 0x2F,

      MaxSingleOp    = 0x77,

      FAbsDP         = 0x78,

      SetR           = 0x80,
      SetDP          = 0x81,
      NLen           = 0x82,
      XAssignI       = 0x83,
      PeekR          = 0x84,
      StoreR         = 0x85,
      XSwapSI        = 0x86,
      SwapSI         = 0x87,
      MovM           = 0x88,
      MovN           = 0x89,
      LoadDP         = 0x8A,
      XCmpDP         = 0x8B,
      SubN           = 0x8C,
      AddN           = 0x8D,
      SetFP          = 0x8E,
      CreateR        = 0x8F,

      Copy           = 0x90,
      CloseN         = 0x91,
      AllocI         = 0x92,
      FreeI          = 0x93,
      AndN           = 0x94,
      ReadN          = 0x95,
      WriteN         = 0x96,
      CmpN           = 0x97,
      NConvFDP       = 0x98,
      FTruncDP       = 0x99,
      DCopy          = 0x9A,
      OrN            = 0x9B,
      MulN           = 0x9C,
      XAddDP         = 0x9D,
      XSetFP         = 0x9E,
      FRoundDP       = 0x9F,

      SaveDP         = 0xA0,
      StoreFI        = 0xA1,
      SaveSI         = 0xA2,
      StoreSI        = 0xA3,
      XFlushSI       = 0xA4,
      GetI           = 0xA5,
      AssignI        = 0xA6,
      XRefreshSI     = 0xA7,
      PeekFI         = 0xA8,
      PeekSI         = 0xA9,
      LSaveDP        = 0xAA,
      LSaveSI        = 0xAB,
      LLoadDP        = 0xAC,

      CallR          = 0xB0,
      CallVI         = 0xB1,
      Jump           = 0xB2,
      Jeq            = 0xB3,
      Jne            = 0xB4,
      JumpVI         = 0xB5,
      XRedirectM     = 0xB6,
      Jlt            = 0xB7,
      Jge            = 0xB8,
      Jgr            = 0xB9,
      Jle            = 0xBA,
      Jult           = 0xBB,

      CmpR           = 0xC0,
      FCmpN          = 0xC1,
      ICmpN          = 0xC2,
      TstFlag        = 0xC3,
      TstN           = 0xC4,
      TstM           = 0xC5,
      CmpFI          = 0xC8,
      CmpSI          = 0xC9,

      XLoadArgSI     = 0xCD,
      XCreateR       = 0xCE,
      System         = 0xCF,

      MaxDoubleOp    = 0xCF,

      FAddDPN        = 0xD0,
      FSubDPN        = 0xD1,
      FMulDPN        = 0xD2,
      FDivDPN        = 0xD3,
      UDivDPN        = 0xD4,

      IAndDPN        = 0xD8,
      IOrDPN         = 0xD9,
      IXorDPN        = 0xDA,
      INotDPN        = 0xDB,
      IShlDPN        = 0xDC,
      IShrDPN        = 0xDD,
      SelULtRR       = 0xDF,

      CopyDPN        = 0xE0,
      IAddDPN        = 0xE1,
      ISubDPN        = 0xE2,
      IMulDPN        = 0xE3,
      IDivDPN        = 0xE4,
      NSaveDPN       = 0xE5,
      XHookDPR       = 0xE6,
      XNewNR         = 0xE7,
      NAddDPN        = 0xE8,
      DCopyDPN       = 0xE9,
      XWriteON       = 0xEA,
      XCopyON        = 0xEB,
      VJumpMR        = 0xEC,
      JumpMR         = 0xED,
      SelEqRR        = 0xEE,
      SelLtRR        = 0xEF,

      OpenIN         = 0xF0,
      XStoreSIR      = 0xF1,
      OpenHeaderIN   = 0xF2,
      MovSIFI        = 0xF3,
      NewIR          = 0xF4,
      NewNR          = 0xF5,
      XMovSISI       = 0xF6,
      CreateNR       = 0xF7,
      XStoreFIR      = 0xF9,
      XDispatchMR    = 0xFA,
      DispatchMR     = 0xFB,
      VCallMR        = 0xFC,
      CallMR         = 0xFD,
      CallExtR       = 0xFE,

      Label          = 0x1001,
//      BreakLabel     = 0x1011,  // meta command, breaking the optimization rules

      ImportOn       = 0x2002,
      ImportOff      = 0x2003,

      Match          = 0x8FFE,  // used in optimization engine
      None           = 0x8FFF,  // used in optimization engine
   };

   enum class PseudoArg
   {
      None           = 0,
      FirstLabel     = 1,
      CurrentLabel   = 2,
      PreviousLabel  = 3,
      Prev2Label     = 4, // before previous
   };

   // --- ByteCommand ---
   struct ByteCommand
   {
      ByteCode code;
      arg_t    arg1;
      arg_t    arg2;

      ByteCommand()
      {
         code = ByteCode::Nop;
         arg1 = 0;
         arg2 = 0;
      }
      ByteCommand(ByteCode code)
      {
         this->code = code;
         this->arg1 = 0;
         this->arg2 = 0;
      }
      ByteCommand(ByteCode code, arg_t arg1)
      {
         this->code = code;
         this->arg1 = arg1;
         this->arg2 = 0;
      }
      ByteCommand(ByteCode code, arg_t arg1, arg_t arg2)
      {
         this->code = code;
         this->arg1 = arg1;
         this->arg2 = arg2;
      }
   };

   typedef MemoryList<ByteCommand> ByteCodeList;
   typedef ByteCodeList::Iterator  ByteCodeIterator;

   struct CommandTape
   {
      ByteCodeList   tape;

      int            labelSeed;
      Stack<int>     labels;

      ByteCodeIterator start() const
      {
         return tape.start();
      }

      int resolvePseudoArg(PseudoArg argument);

      int newLabel()
      {
         labelSeed++;

         labels.push(labelSeed);

         return labelSeed;
      }

      void setLabel(bool persist = false)
      {
         if (persist) {
            write(ByteCode::Label, labels.peek());
         }
         else write(ByteCode::Label, labels.pop());
      }

      // to resolve possible conflicts the predefined labels should be negative
      void setPredefinedLabel(int label)
      {
         write(ByteCode::Label, label);
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

      void write(ByteCode code);
      void write(ByteCode code, arg_t arg1);
      void write(ByteCode code, arg_t arg1, arg_t arg2);
      void write(ByteCode code, PseudoArg arg);
      void write(ByteCode code, arg_t arg1, PseudoArg arg2);
      void write(ByteCode code, arg_t arg1, PseudoArg arg2, ref_t mask);

      void import(ModuleBase* sourceModule, MemoryBase* source, bool withHeader, SectionScopeBase* target);

      void saveTo(MemoryWriter* writer);

      //static bool optimizeIdleBreakpoints(CommandTape& tape);
      static bool optimizeJumps(CommandTape& tape);

      CommandTape()
         : tape({}), labelSeed(0), labels(0)
      {
      }
   };

   // --- ByteCodeUtil ---
   class ByteCodeUtil
   {
   public:
      static ByteCode code(ustr_t command);
      static void decode(ByteCode code, IdentifierString& target);

      static bool isDoubleOp(ByteCode code)
      {
         return code > ByteCode::MaxSingleOp && code <= ByteCode::MaxDoubleOp;
      }

      static bool isSingleOp(ByteCode code)
      {
         return code <= ByteCode::MaxSingleOp;
      }

      static bool isMCommand(ByteCode code)
      {
         switch (code) {
            case ByteCode::MovM:
            case ByteCode::CallMR:
            case ByteCode::VCallMR:
            case ByteCode::JumpMR:
            case ByteCode::VJumpMR:
            case ByteCode::DispatchMR:
            case ByteCode::XDispatchMR:
            case ByteCode::TstM:
            case ByteCode::XRedirectM:
               return true;
            default:
               return false;
         }
      }

      static bool isRCommand(ByteCode code)
      {
         switch (code) {
         case ByteCode::SetR:
         case ByteCode::CallR:
         case ByteCode::CallExtR:
         case ByteCode::CmpR:
         case ByteCode::SelEqRR:
         case ByteCode::SelLtRR:
         case ByteCode::SelULtRR:
         case ByteCode::PeekR:
         case ByteCode::StoreR:
         case ByteCode::CreateR:
         case ByteCode::XCreateR:
            return true;
         default:
            return false;
         }
      }

      static bool isR2Command(ByteCode code)
      {
         switch (code) {
            case ByteCode::XStoreSIR:
            case ByteCode::XStoreFIR:
            case ByteCode::NewIR:
            case ByteCode::NewNR:
            case ByteCode::XNewNR:
            case ByteCode::CallMR:
            case ByteCode::VCallMR:
            case ByteCode::JumpMR:
            case ByteCode::VJumpMR:
            case ByteCode::DispatchMR:
            case ByteCode::XDispatchMR:
            case ByteCode::SelEqRR:
            case ByteCode::SelLtRR:
            case ByteCode::SelULtRR:
            case ByteCode::XHookDPR:
            case ByteCode::CreateNR:
               return true;
            default:
               return false;
         }
      }

      static void write(MemoryWriter& writer, ByteCommand& command)
      {
         writer.writeByte((char)command.code);
         if (command.code > ByteCode::MaxSingleOp) {
            writer.write(&command.arg1, sizeof(arg_t));
            if (command.code > ByteCode::MaxDoubleOp)
               writer.write(&command.arg2, sizeof(arg_t));
         }
      }

      static void write(MemoryWriter& writer, ByteCode code)
      {
         ByteCommand command(code);
         write(writer, command);
      }
      static void write(MemoryWriter& writer, ByteCode code, ref_t reference)
      {
         ByteCommand command(code, reference);
         write(writer, command);
      }
      static void write(MemoryWriter& writer, ByteCode code, int index, ref_t reference)
      {
         ByteCommand command(code, index, reference);
         write(writer, command);
      }

      static void read(MemoryReader& reader, ByteCommand& command)
      {
         command.code = (ByteCode)reader.getUChar();
         if (command.code > ByteCode::MaxSingleOp) {
            reader.read(&command.arg1, sizeof(arg_t));
            if (command.code > ByteCode::MaxDoubleOp)
               reader.read(&command.arg2, sizeof(arg_t));
         }            
      }

      static void importCommand(ByteCommand& command, SectionScopeBase* target, ModuleBase* importer);

      static void formatMessageName(IdentifierString& messageName, ModuleBase* module, ustr_t actionName,
         ref_t* references, size_t len, pos_t argCount, ref_t flags);
      static bool resolveMessageName(IdentifierString& messageName, ModuleBase* module, mssg_t message);

      static void parseMessageName(ustr_t messageName, IdentifierString& actionName, ref_t& flags, pos_t& argCount);

      static mssg_t resolveMessage(ustr_t messageName, ModuleBase* module, bool readOnlyMode);
   };

   // --- ByteCodePattern ---
   struct ByteCodePattern
   {
      ByteCode        code;

      bool operator ==(ByteCode code) const
      {
         return (this->code == code);
      }

      bool operator !=(ByteCode code) const
      {
         return (this->code != code);
      }

      bool operator ==(ByteCodePattern pattern)
      {
         return (code == pattern.code/* && argumentType == pattern.argumentType && argument == pattern.argument*/);
      }

      bool operator !=(ByteCodePattern pattern)
      {
         return !(*this == pattern);
      }

   };

   // --- ByteCodeTransformer ---
   struct ByteCodeTransformer
   {
      typedef MemoryTrie<ByteCodePattern>     MemoryByteTrie;

      MemoryByteTrie trie;
      bool           loaded;

      ByteCodeTransformer()
         : trie({ ByteCode::None })
      {
         loaded = false;
      }
   };

}

#endif
