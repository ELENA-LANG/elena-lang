//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Debugger Adapater
//
//		This file contains the DPA I/O class declarations
// 
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DPA_STREAM_H
#define DPA_STREAM_H

namespace dpa
{
   //using RequestHandler =
   //   std::function<void(const void* request)>;

   // --- ContentReader ---
   class ContentReader
   {

   public:
      bool isOpen();

      void close();

      ContentReader() = default;
      virtual ~ContentReader();
   };
}

#endif