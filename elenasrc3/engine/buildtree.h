//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Byte code Build Tree classes
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BUILDTREE_H
#define BUILDTREE_H

#include "elena.h"
#include "tree.h"

namespace elena_lang
{
   enum class BuildKey
   {
      None                 = 0x0000,

      CollectionMask       = 0x1000,

      Root                 = 0x1001,
      Symbol               = 0x1002,
      Class                = 0x1003,
      Method               = 0x1004,
      Tape                 = 0x1005,
      AbstractMethod       = 0x1006,

      OpenFrame            = 0x0001,
      CloseFrame           = 0x0002,
      NilReference         = 0x0003,
      SymbolCall           = 0x0004,
      ClassReference       = 0x0005,
      CallOp               = 0x0006,      // NOTE : the stack for the argument list should be preallocated
      Exit                 = 0x0007,
      SavingInStack        = 0x0008,
      Assigning            = 0x0009,
      Local                = 0x000A,
      CreatingClass        = 0x000B,
      OpenStatement        = 0x000C,
      EndStatement         = 0x000D,
      Breakpoint           = 0x000E,
      EOPBreakpoint        = 0x000F,
      CreatingStruct       = 0x0010,
      IntLiteral           = 0x0011,
      StringLiteral        = 0x0012,
      goingToEOP           = 0x0013,
      LocalAddress         = 0x0014,
      Copying              = 0x0015,
      Allocating           = 0x0016,
      Freeing              = 0x0017,
      SavingNInStack       = 0x0018,
      ExtCallOp            = 0x0019,
      SavingIndex          = 0x001A,
      DirectCallOp         = 0x001B,
      DispatchingOp        = 0x001C,
      IntOp                = 0x001D,
      ByteArraySOp         = 0x001E,
      CopyingToAcc         = 0x001F,
      Argument             = 0x0020,
      BranchOp             = 0x0021,
      StrongRedirectOp     = 0x0022,
      ResendOp             = 0x0023,
      SealedDispatchingOp  = 0x0024,
      BoolSOp              = 0x0025,
      IntCondOp            = 0x0026,
      CharLiteral          = 0x0027,
      FieldAssigning       = 0x0028,
      Field                = 0x0029,
      OpenStatic           = 0x002A,
      CloseStatic          = 0x002B,
      ClassOp              = 0x002C,
      ByteArrayOp          = 0x002D,
      NewArrayOp           = 0x002E,
      Swapping             = 0x002F,
      MssgLiteral          = 0x0030,
      AccSwapping          = 0x0031,
      RedirectOp           = 0x0032,
      ShortArraySOp        = 0x0033,
      WideStringLiteral    = 0x0034,
      ByteOp               = 0x0035,
      ShortOp              = 0x0036,
      ByteCondOp           = 0x0037,
      ShortCondOp          = 0x0038,
      CopyingToAccField    = 0x0039,
      CopyingAccField      = 0x003A,
      LocalReference       = 0x003B,
      RefParamAssigning    = 0x003C,
      StaticVar            = 0x003D,
      LoadingIndex         = 0x003E,
      NilOp                = 0x003F,
      IntSOp               = 0x0040,
      ByteSOp              = 0x0041,
      ShortSOp             = 0x0042,
      LongLiteral          = 0x0043,
      LongOp               = 0x0044,
      LongSOp              = 0x0045,
      LongCondOp           = 0x0046,
      RealLiteral          = 0x0047,
      RealOp               = 0x0048,
      RealCondOp           = 0x0049,
      VirtualBreakoint     = 0x004A,
      ConversionOp         = 0x004B,
      SemiResendOp         = 0x004C,
      NilCondOp            = 0x004D,
      AssignLocalToStack   = 0x004E,
      SetImmediateField    = 0x004F,
      GenericDispatchingOp = 0x0050,
      BinaryArraySOp       = 0x0051,
      BinaryArrayOp        = 0x0052,
      ShortArrayOp         = 0x0053,
      BreakOp              = 0x0054,
      ConstantReference    = 0x0055,
      ObjArrayOp           = 0x0056,
      IntArrayOp           = 0x0057,
      IntArraySOp          = 0x0058,
      ObjArraySOp          = 0x0059,
      CopyingArr           = 0x005A,
      ExtMssgLiteral       = 0x005B,
      LoadingBinaryLen     = 0x005C,
      UnboxMessage         = 0x005D,
      LoadingSubject       = 0x005E,
      peekArgument         = 0x005F,
      TerminatorReference  = 0x0060,
      CopyingItem          = 0x0061,
      SavingLongIndex      = 0x0062,
      LongIntCondOp        = 0x0063,
      ConstArrayReference  = 0x0064,
      StaticAssigning      = 0x0065,
      SavingLInStack       = 0x0066,
      UIntCondOp           = 0x0067,
      UIntOp               = 0x0068,
      MssgNameLiteral      = 0x0069,
      VArgSOp              = 0x006A,
      LoadArgCount         = 0x006B, // argument contains the argument offset
      IncIndex             = 0x006C,
      FreeVarStack         = 0x006D,
      FillOp               = 0x006E, // if the argument is 0 - the size is in sp[0]
      StrongResendOp       = 0x006F,
      CopyingToAccExact    = 0x0070,
      SavingInt            = 0x0071,
      AddingInt            = 0x0072,
      LoadingAccToIndex    = 0x0073,
      IndexOp              = 0x0074,
      SavingIndexToAcc     = 0x0075,
      ContinueOp           = 0x0076,
      SemiDirectCallOp     = 0x0077,

      MaxOperationalKey    = 0x0077,

      Import               = 0x0080,
      DictionaryOp         = 0x0081,
      ObjOp                = 0x0082,
      AttrDictionaryOp     = 0x0083,
      DeclOp               = 0x0084,
      DeclDictionaryOp     = 0x0085,
      LoopOp               = 0x0086,
      CatchOp              = 0x0087,
      ExternOp             = 0x0088,
      AltOp                = 0x0089,
      ShortCircuitOp       = 0x008A,
      Switching            = 0x008B,
      SwitchOption         = 0x008C,
      ElseOption           = 0x008D,
      FinalOp              = 0x008E,
      IntBranchOp          = 0x008F,
      IntConstBranchOp     = 0x0090,
      RealBranchOp         = 0x0091,

      VariableInfo         = 0x00A0,
      Variable             = 0x00A1,
      VariableAddress      = 0x00A2,
      IntVariableAddress   = 0x00A3,
      LongVariableAddress  = 0x00A4,
      RealVariableAddress  = 0x00A5,
      ByteArrayAddress     = 0x00A6,
      ShortArrayAddress    = 0x00A7,
      IntArrayAddress      = 0x00A8,
      UIntVariableAddress  = 0x00A9,

      ParameterInfo        = 0x00B0,
      Parameter            = 0x00B1,
      IntParameterAddress  = 0x00B2,
      LongParameterAddress = 0x00B3,
      RealParameterAddress = 0x00B4,
      ParameterAddress     = 0x00B5,
      MethodName           = 0x00B6,
      ShortArrayParameter  = 0x00B7,
      ByteArrayParameter   = 0x00B8,
      IntArrayParameter    = 0x00B9,

      BinaryArray          = 0x00C0,

      ByRefOpMark          = 0x4001,

      Value                = 0x8001,
      Reserved             = 0x8002,      // reserved managed
      ReservedN            = 0x8003,      // reserved unmanaged
      Index                = 0x8004,
      Type                 = 0x8005,
      Column               = 0x8006,
      Row                  = 0x8007,
      Size                 = 0x8008,
      Count                = 0x8009,
      Message              = 0x800A,
      Const                = 0x800B,
      TrueConst            = 0x800C,
      FalseConst           = 0x800D,
      Path                 = 0x800E,
      ClassName            = 0x800F,
      Special              = 0x8010,

      Idle                 = 0x8FFF,

      //MetaDictionary    = 0x0022,
      //MetaArray         = 0x1023,
   };

   typedef Map<ustr_t, BuildKey, allocUStr, freeUStr> BuildKeyMap;

   // --- BuildTree ---
   class BuildTree : public Tree<BuildKey, BuildKey::None>
   {
   public:
      static pos_t countChildren(Tree<BuildKey, BuildKey::None>::Node node, BuildKey mask)
      {
         pos_t counter = 0;
         auto current = node.firstChild(mask);
         while (current != BuildKey::None) {
            counter++;

            current = current.nextNode(mask);
         }

         return counter;
      }

      static void loadBuildKeyMap(BuildKeyMap& map)
      {
         map.add("breakpoint", BuildKey::Breakpoint);
         map.add("byrefmark", BuildKey::ByRefOpMark);
         map.add("int_literal", BuildKey::IntLiteral);
         map.add("copying", BuildKey::Copying);
         map.add("local_address", BuildKey::LocalAddress);
         map.add("saving_stack", BuildKey::SavingInStack);
         map.add("intop", BuildKey::IntOp);
         map.add("create_struct", BuildKey::CreatingStruct);
         map.add("assigning", BuildKey::Assigning);
         map.add("copying_to_acc", BuildKey::CopyingToAcc);
         map.add("local", BuildKey::Local);
         map.add("intcondop", BuildKey::IntCondOp);
         map.add("realcondop", BuildKey::RealCondOp);
         map.add("branchop", BuildKey::BranchOp);
         map.add("intbranchop", BuildKey::IntBranchOp);
         map.add("conversion_op", BuildKey::ConversionOp);
      }
   };

   // --- BuildTreeWriter ---
   typedef Tree<BuildKey, BuildKey::None>::Writer BuildTreeWriter;
   typedef Tree<BuildKey, BuildKey::None>::Node   BuildNode;

   constexpr int BuildKeyNoArg = INT_MAX;

   // --- BuildKeyPattern ---
   struct BuildKeyPattern
   {
      BuildKey type;

      int      argument;
      int      patternId;

      bool operator ==(BuildKey type) const
      {
         return (this->type == type);
      }

      bool operator !=(BuildKey type) const
      {
         return (this->type != type);
      }

      bool operator ==(BuildKeyPattern pattern)
      {
         return (type == pattern.type && argument == pattern.argument);
      }

      bool operator !=(BuildKeyPattern pattern)
      {
         return !(*this == pattern);
      }

      bool match(BuildNode node)
      {
         return node.key == type && (argument == BuildKeyNoArg || node.arg.value == argument);
      }

      BuildKeyPattern()
         : type(BuildKey::None), argument(BuildKeyNoArg), patternId(0)
      {
         
      }
      BuildKeyPattern(BuildKey type)
         : type(type), argument(BuildKeyNoArg), patternId(0)
      {
      }
   };

   typedef MemoryTrieBuilder<BuildKeyPattern>            BuildCodeTrie;
   typedef MemoryTrieNode<BuildKeyPattern>               BuildCodeTrieNode;
   typedef CachedList<BuildCodeTrieNode, 10>             BuildPatterns;

   // --- BuildTreeTransformer ---
   struct BuildTreeTransformer
   {
      typedef MemoryTrie<BuildKeyPattern>     MemoryBuildCodeTrie;

      MemoryBuildCodeTrie  trie;
      bool                 loaded;

      BuildTreeTransformer()
         : trie({ })
      {
         loaded = false;
      }
   };


}

#endif
