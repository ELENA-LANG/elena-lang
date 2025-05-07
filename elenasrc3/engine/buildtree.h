//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Byte code Build Tree classes
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef BUILDTREE_H
#define BUILDTREE_H

#include "elena.h"
#include "tree.h"
#include <climits>

#ifdef _MSC_VER

#pragma warning( push )
#pragma warning( disable : 4458 )

#endif

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
      Procedure            = 0x1007,

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
      //ResendOp             = 0x0023,
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
      VirtualBreakpoint    = 0x004A,
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
      IntRealOp            = 0x0078,
      RealIntOp            = 0x0079,
      CopyingArrTo         = 0x007A,
      LoadingStackDump     = 0x007B,
      SavingStackDump      = 0x007C,
      SavingFloatIndex     = 0x007D,
      IntCopyingToAccField = 0x007E,
      IntConstOp           = 0x007F,
      UByteCondOp          = 0x0080,
      UShortCondOp         = 0x0081,
      IntLongOp            = 0x0082,
      DistributedTypeList  = 0x0083,
      UnboxAndCallMessage  = 0x0084,
      ThreadVar            = 0x0085,
      ThreadVarAssigning   = 0x0086,
      OpenThreadVar        = 0x0087,
      CloseThreadVar       = 0x0088,
      LoadingLIndex        = 0x0089,
      SavingLIndex         = 0x008A,
      RealIntXOp           = 0x008B,
      OpenExtFrame         = 0x008C,
      LoadExtArg           = 0x008D,
      CloseExtFrame        = 0x008E,
      ExtExit              = 0x008F,
      ProcedureReference   = 0x0090,
      LoadingAccToLongIndex = 0x0091,
      ExternalVarReference = 0x0092,
      ByteConstOp          = 0x0093,

      MaxOperationalKey    = 0x0093,      
      
      DeclOp               = 0x0094,
      DeclDictionaryOp     = 0x0095,
      LoopOp               = 0x0096,
      CatchOp              = 0x0097,
      ExternOp             = 0x0098,
      AltOp                = 0x0099,
      ShortCircuitOp       = 0x009A,
      Switching            = 0x009B,
      SwitchOption         = 0x009C,
      ElseOption           = 0x009D,
      FinalOp              = 0x009E,
      IntBranchOp          = 0x009F,
      IntConstBranchOp     = 0x00A0,
      RealBranchOp         = 0x00A1,
      YieldingOp           = 0x00A2,
      StackCondOp          = 0x00A3,
      YieldDispatch        = 0x00A4,
      TernaryOp            = 0x00A5,
      NilRefBranchOp       = 0x00A6,
      ExcludeTry           = 0x00A7,
      IncludeTry           = 0x00A8,
      Import               = 0x00A9,
      DictionaryOp         = 0x00AA,
      ProjectInfoOp        = 0x00AB,
      ObjOp                = 0x00AC,
      AttrDictionaryOp     = 0x00AD,

      VariableInfo         = 0x00B0,
      Variable             = 0x00B1,
      VariableAddress      = 0x00B2,
      IntVariableAddress   = 0x00B3,
      LongVariableAddress  = 0x00B4,
      RealVariableAddress  = 0x00B5,
      ByteArrayAddress     = 0x00B6,
      ShortArrayAddress    = 0x00B7,
      IntArrayAddress      = 0x00B8,
      UIntVariableAddress  = 0x00B9,

      ArgumentsInfo        = 0x00C0,
      Parameter            = 0x00C1,
      IntParameterAddress  = 0x00C2,
      LongParameterAddress = 0x00C3,
      RealParameterAddress = 0x00C4,
      ParameterAddress     = 0x00C5,
      MethodName           = 0x00C6,
      ShortArrayParameter  = 0x00C7,
      ByteArrayParameter   = 0x00C8,
      IntArrayParameter    = 0x00C9,
      RealArrayParameter   = 0x00CA,

      BinaryArray          = 0x00D0,

      ByRefOpMark          = 0x4001,
      InplaceCall          = 0x4002,

      Value                = 0x8001,
      Reserved             = 0x8002,      // reserved managed
      ReservedN            = 0x8003,      // reserved unmanaged
      VMTIndex             = 0x8004,
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
      LongMode             = 0x8011,
      Source               = 0x8012,
      Length               = 0x8013,
      TempVar              = 0x8014,
      IndexTableMode       = 0x8015,
      OperatorId           = 0x8016,
      StackIndex           = 0x8017,
      StackAddress         = 0x8018,

      Match                = 0x8FFE,
      Idle                 = 0x8FFF,

      //MetaDictionary    = 0x0022,
      //MetaArray         = 0x1023,
   };

   typedef Map<ustr_t, BuildKey, allocUStr, freeUStr> BuildKeyMap;

   // --- BuildTree ---
   class BuildTree : public Tree<BuildKey, BuildKey::None>
   {
      static Tree<BuildKey, BuildKey::None>::Node nextNonIdle(Tree<BuildKey, BuildKey::None>::Node node)
      {
         do {
            node = node.nextNode();
         } while (node == BuildKey::Idle);

         return node;
      }
      static Tree<BuildKey, BuildKey::None>::Node firstNonIdle(Tree<BuildKey, BuildKey::None>::Node node)
      {
         node = node.firstChild();
         while (node == BuildKey::Idle) {
            node = node.nextNode();
         }

         return node;
      }

   public:
      static bool compare(Tree<BuildKey, BuildKey::None>::Node n1, Tree<BuildKey, BuildKey::None>::Node n2, bool onlyChildren)
      {
         if (onlyChildren || (n1.key == n2.key && n1.arg.value == n2.arg.value)) {
            Tree<BuildKey, BuildKey::None>::Node c1 = firstNonIdle(n1);
            Tree<BuildKey, BuildKey::None>::Node c2 = firstNonIdle(n2);
            while (c1.key != BuildKey::None) {
               if (!compare(c1, c2, false))
                  return false;

               c1 = nextNonIdle(c1);
               c2 = nextNonIdle(c2);
            }

            return c2.key == BuildKey::None;
         }

         return false;
      }

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

      static void copyNode(Tree<BuildKey, BuildKey::None>::Writer& writer, Tree<BuildKey, BuildKey::None>::Node node, 
         bool includingNode = false)
      {
         if (includingNode) {
            if (node.arg.strArgPosition != INVALID_POS) {
               writer.newNode(node.key, node.identifier());
            }
            else writer.newNode(node.key, node.arg.reference);
         }

         auto current = node.firstChild();
         while (current != BuildKey::None) {
            copyNode(writer, current, true);

            current = current.nextNode();
         }

         if (includingNode)
            writer.closeNode();
      }

      static void injectChildren(Tree<BuildKey, BuildKey::None>::Writer& writer, Tree<BuildKey, BuildKey::None>::Node node)
      {
         auto current = node.firstChild();
         while (current != BuildKey::None) {
            if (current.arg.strArgPosition != INVALID_POS) {
               writer.inject(current.key, current.identifier());
            }
            else writer.inject(current.key, current.arg.reference);

            copyNode(writer, current);

            writer.closeNode();

            current = current.nextNode();
         }
      }

      static void loadBuildKeyMap(BuildKeyMap& map)
      {
         map.add("breakpoint", BuildKey::Breakpoint);
         map.add("byrefmark", BuildKey::ByRefOpMark);
         map.add("inplacemark", BuildKey::InplaceCall);
         map.add("int_literal", BuildKey::IntLiteral);
         map.add("string_literal", BuildKey::StringLiteral);
         map.add("copying", BuildKey::Copying);
         map.add("local_address", BuildKey::LocalAddress);
         map.add("saving_stack", BuildKey::SavingInStack);
         map.add("intop", BuildKey::IntOp);
         map.add("byteop", BuildKey::ByteOp);
         map.add("create_struct", BuildKey::CreatingStruct);
         map.add("assigning", BuildKey::Assigning);
         map.add("copying_to_acc", BuildKey::CopyingToAcc);
         map.add("copying_to_acc_exact", BuildKey::CopyingToAccExact);
         map.add("copying_to_acc_field", BuildKey::CopyingToAccField);
         map.add("create_class", BuildKey::CreatingClass);
         map.add("local", BuildKey::Local);
         map.add("intcondop", BuildKey::IntCondOp);
         map.add("realcondop", BuildKey::RealCondOp);
         map.add("nilcondop", BuildKey::NilCondOp);
         map.add("branchop", BuildKey::BranchOp);
         map.add("intbranchop", BuildKey::IntBranchOp);
         map.add("conversion_op", BuildKey::ConversionOp);
         map.add("int_real_op", BuildKey::IntRealOp);
         map.add("real_int_op", BuildKey::RealIntOp);
         map.add("direct_call_op", BuildKey::DirectCallOp);
         map.add("semi_direct_call_op", BuildKey::SemiDirectCallOp);
         map.add("addingint", BuildKey::AddingInt);
         map.add("saving_index", BuildKey::SavingIndex);
         map.add("open_frame", BuildKey::OpenFrame);
         map.add("close_frame", BuildKey::CloseFrame);
         map.add("argument", BuildKey::Argument);
         map.add("class_reference", BuildKey::ClassReference);
         map.add("nil_reference", BuildKey::NilReference);
         map.add("open_statement", BuildKey::OpenStatement);
         map.add("end_statement", BuildKey::EndStatement);
         map.add("saving_int", BuildKey::SavingInt);
         map.add("sealed_dispatching", BuildKey::SealedDispatchingOp);
         map.add("local_reference", BuildKey::LocalReference);
         map.add("varg_sop", BuildKey::VArgSOp);
         map.add("unbox_call_message", BuildKey::UnboxAndCallMessage);
         map.add("loading_index", BuildKey::LoadingIndex);
         map.add("free_varstack", BuildKey::FreeVarStack);
         map.add("exit", BuildKey::Exit);
         map.add("parameter", BuildKey::Parameter);
         map.add("call_op", BuildKey::CallOp);
         map.add("terminator", BuildKey::TerminatorReference);
         map.add("free_varstack", BuildKey::FreeVarStack);
         map.add("going_to_eop", BuildKey::goingToEOP);
         map.add("field", BuildKey::Field);
         map.add("nil", BuildKey::NilReference);
         map.add("field_assign", BuildKey::FieldAssigning);
         map.add("assign_local_to_stack", BuildKey::AssignLocalToStack);
         map.add("set_imm_field", BuildKey::SetImmediateField);
         map.add("value", BuildKey::Value);
         map.add("tape", BuildKey::Tape);
         map.add("type", BuildKey::Type);
         map.add("size", BuildKey::Size);
         map.add("method_name", BuildKey::MethodName);
         map.add("arguments_info", BuildKey::ArgumentsInfo);
         map.add("variable_info", BuildKey::VariableInfo);
         map.add("column", BuildKey::Column);
         map.add("row", BuildKey::Row);
         map.add("message", BuildKey::Message);
         map.add("vmt_index", BuildKey::VMTIndex);
         map.add("length", BuildKey::Length);
         map.add("temp_var", BuildKey::TempVar);
         map.add("message", BuildKey::Message);
         map.add("reserved", BuildKey::Reserved);
         map.add("reserved_n", BuildKey::ReservedN);         
         map.add("index_table_mode", BuildKey::IndexTableMode);
         map.add("class", BuildKey::Class);
         map.add("method", BuildKey::Method);
         map.add("symbol", BuildKey::Symbol);
         map.add("param_address", BuildKey::ParameterAddress);
         map.add("dispatch_op", BuildKey::DispatchingOp);
         map.add("redirect_op", BuildKey::RedirectOp);
         map.add("operator_id", BuildKey::OperatorId);
         map.add("load_long_index", BuildKey::LoadingLIndex);
         map.add("save_long_index", BuildKey::SavingLIndex);
         map.add("real_int_xop", BuildKey::RealIntXOp);
         map.add("procedure_ref", BuildKey::ProcedureReference);
         map.add("break_op", BuildKey::BreakOp);
         map.add("bytearray_op", BuildKey::ByteArrayOp);
         map.add("loop_op", BuildKey::LoopOp);
         map.add("virtual_breakpoint", BuildKey::VirtualBreakpoint);
         map.add("eop_breakpoint", BuildKey::EOPBreakpoint);
         map.add("int_sop", BuildKey::IntSOp);
         map.add("const_param", BuildKey::Const);
      }
   };

   // --- BuildTreeWriter ---
   typedef Tree<BuildKey, BuildKey::None>::Writer BuildTreeWriter;
   typedef Tree<BuildKey, BuildKey::None>::Node   BuildNode;

   constexpr int BuildKeyNoArg = INT_MAX;

   enum class BuildPatternType
   {
      None = 0,
      MatchArg,
      Set,
      Match
   };

   struct BuildPatternArg
   {
      int arg1;
      int arg2;
   };

   // --- BuildPattern ---
   struct BuildPattern
   {
      BuildKey          key;
      BuildPatternType  argType;
      int               argValue;

      bool operator ==(BuildKey key) const
      {
         return (this->key == key);
      }

      bool operator !=(BuildKey key) const
      {
         return (this->key != key);
      }

      bool operator ==(BuildPattern pattern)
      {
         return (key == pattern.key && argType == pattern.argType && argValue == pattern.argValue);
      }

      bool operator !=(BuildPattern pattern)
      {
         return !(*this == pattern);
      }

      bool match(BuildNode node, BuildPatternArg& args)
      {
         if (key != node.key)
            return key == BuildKey::Match;

         switch (argType) {
            case BuildPatternType::Set:
               if (argValue == 1) {
                  args.arg1 = node.arg.value;
               }
               else args.arg2 = node.arg.value;
               return true;
            case BuildPatternType::Match:
               return ((argValue == 1) ? args.arg1 : args.arg2) == node.arg.value;
            case BuildPatternType::MatchArg:
               return node.arg.value == argValue;
            default:
               return true;
         }
      }
   };

   typedef MemoryTrieBuilder<BuildPattern>            BuildCodeTrie;
   typedef MemoryTrieNode<BuildPattern>               BuildCodeTrieNode;

   struct BuildPatternContext
   {
      BuildCodeTrieNode node;
      BuildPatternArg   args;

      BuildPatternContext() = default;
      BuildPatternContext(BuildCodeTrieNode node, BuildPatternArg args)
         : node(node), args(args)
      {
      }
      BuildPatternContext(BuildCodeTrieNode node)
         : node(node), args({})
      {
      }
   };

   typedef CachedList<BuildPatternContext, 10>        BuildPatterns;

   // --- BuildTreeTransformer ---
   struct BuildTreeTransformer
   {
      typedef MemoryTrie<BuildPattern>     MemoryBuildCodeTrie;

      MemoryBuildCodeTrie  trie;
      bool                 loaded;

      BuildTreeTransformer()
         : trie({ })
      {
         loaded = false;
      }
   };


}

#ifdef _MSC_VER

#pragma warning( pop )

#endif

#endif
