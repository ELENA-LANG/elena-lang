//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Debugger Adapater
//
//		This file contains the DPA Session implementations
// 
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "dpa_session.h"

using namespace dpa;

Session :: Session()
{

}

Session :: ~Session()
{
   _inbox.close();
   if (_recvThread.joinable()) {
      _recvThread.join();
   }
   if (_dispatchThread.joinable()) {
      _dispatchThread.join();
   }
   _reader.close();
}

void Session :: connect()
{
   _reader = ContentReader();
}

Payload Session :: getPayload()
{
   return {};
}

void Session :: start(const ClosedHandler& onClose)
{
   _recvThread = std::thread([this/*, onClose*/] {
      while (_reader.isOpen()) {
         if (auto payload = getPayload()) {
            _inbox.put(std::move(payload));
         }
      }
      //if (onClose) {
      //   onClose();
      //}
   });

   _dispatchThread = std::thread([this] {
      while (auto payload = _inbox.take()) {
         payload.value()();
      }
   });

}