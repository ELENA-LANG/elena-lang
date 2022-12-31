//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA Label helper base class.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LBHELPER_H
#define LBHELPER_H

#include "elena.h"
#include "lbhelper.h"

namespace elena_lang
{
   // --- LabelHelper ---
   struct LabelHelper : LabelHelperBase
   {
      struct JumpInfo
      {
         pos_t position;
         int offset;

         JumpInfo()
         {
            position = -1;
            offset = 0;
         }
         JumpInfo(pos_t position)
         {
            this->position = position;
            this->offset = 0;
         }
         JumpInfo(pos_t position, int offset)
         {
            this->position = position;
            this->offset = offset;
         }
      };

      struct LabelAddressInfo
      {
         pos_t position;
         ref_t mask;
      };

      Map<pos_t, pos_t>             labels;
      Map<ref_t, JumpInfo>          jumps;
      Map<ref_t, LabelAddressInfo>  addresses;

      bool checkLabel(pos_t label) override
      {
         return labels.exist(label);
      }

      void declareJump(pos_t label, MemoryWriter& writer)
      {
         jumps.add(label, { writer.position() });
      }

      bool setLabel(pos_t label, MemoryWriter& writer, ReferenceHelperBase* rh) override
      {
         labels.add(label, writer.position());

         return fixLabel(label, writer, rh);
      }

      void writeLabelAddress(pos_t label, MemoryWriter& writer, ref_t mask) override
      {
         addresses.add(label, { writer.position(), mask});
      }

      LabelHelper()
         : labels(0), jumps({}), addresses({})
      {
         
      }
   };
}

#endif