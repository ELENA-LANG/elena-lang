//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Debugger Adapater
//
//		This file contains the DPA Session declarations
// 
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DPA_SESSION_H
#define DPA_SESSION_H

#include <functional>
#include <thread>

#include "dpa_stream.h"
#include "dpa_queue.h"

namespace dpa
{
   using Payload = std::function<void()>;
   using PayloadQueue = ThreadQueue<Payload>;

   using ClosedHandler = std::function<void()>;

   //using RequestHandler =
   //   std::function<void(const void* request)>;

   // --- Session ---
   class Session
   {
      ContentReader _reader;
      PayloadQueue  _inbox;

      std::thread _recvThread;
      std::thread _dispatchThread;

      Payload getPayload();

   public:
      //virtual void registerHandler(const TypeInfo* typeinfo,
      //   const RequestHandler& handler);

      void connect();
      void start(const ClosedHandler& onClose = {});

      Session();
      virtual ~Session();
   };
}

#endif