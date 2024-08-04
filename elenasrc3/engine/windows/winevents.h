//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 event template class
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN32EVENT_H
#define WIN32EVENT_H

#include <windows.h>

namespace elena_lang
{
   // --- EventManager ---
   template<class T, int EventCount> class EventManager
   {
      HANDLE _events[EventCount];

   public:
      void init(T startEvent)
      {
         for (T i = 0; i < EventCount; i++)
            _events[i] = CreateEvent(nullptr, TRUE, i == startEvent ? TRUE : FALSE, nullptr);;
      }

      void setEvent(T event)
      {
         SetEvent(_events[event]);
      }

      void resetEvent(T event)
      {
         ResetEvent(_events[event]);
      }
      
      int  waitForAnyEvent()
      {
         return WaitForMultipleObjects(EventCount, _events, FALSE, INFINITE);
      }

      bool waitForEvent(T event, int timeout = INFINITE)
      {
         return (WaitForSingleObject(_events[event], timeout) == WAIT_OBJECT_0);
      }

      void close()
      {
         for (T i = 0; i < EventCount; i++) {
            if (_events[i]) {
               CloseHandle(_events[i]);
               _events[i] = nullptr;
            }
         }
      }

      EventManager()
      {
         for (T i = 0; i < EventCount; i++)
            _events[i] = nullptr;
      }
      virtual ~EventManager()
      {
         close();
      }
   };
}

#endif