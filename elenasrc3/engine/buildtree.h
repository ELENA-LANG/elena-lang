//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Byte code Build Tree classes
//
//                                             (C)2021-2022, by Aleksey Rakov
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

      Root                 = 0x1000,
      Symbol               = 0x1001,
      Class                = 0x1002,
      Method               = 0x1003,
      Tape                 = 0x1004,
      AbstractMethod       = 0x1005,

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
      AccCopying           = 0x001F,
      Argument             = 0x0020,
      BranchOp             = 0x0021,
      DirectResendOp       = 0x0022,
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

      Import               = 0x0038,
      DictionaryOp         = 0x0039,

      ObjArrayOp           = 0x003A,
      ObjOp                = 0x003B,
      AttrDictionaryOp     = 0x003C,
      DeclOp               = 0x003D,
      DeclDictionaryOp     = 0x003E,
      LoopOp               = 0x003F,
      CatchOp              = 0x0040,

      NestedClass          = 0x0080,

      VariableInfo         = 0x0090,

      BinaryArray          = 0x00A0,

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

      //MetaDictionary    = 0x0022,
      //MetaArray         = 0x1023,
   };

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
   };

   // --- BuildTreeWriter ---
   typedef Tree<BuildKey, BuildKey::None>::Writer BuildTreeWriter;
   typedef Tree<BuildKey, BuildKey::None>::Node   BuildNode;

}

#endif
