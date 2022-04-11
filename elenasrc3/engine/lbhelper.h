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

      Map<pos_t, pos_t>    labels;
      Map<ref_t, JumpInfo> jumps;

      bool checkLabel(pos_t label) override
      {
         return labels.exist(label);
      }

      bool setLabel(pos_t label, MemoryWriter& writer) override
      {
         labels.add(label, writer.position());

         return fixLabel(label, writer);
      }

      LabelHelper()
         : labels(0), jumps({})
      {
         
      }
   };
}

#endif