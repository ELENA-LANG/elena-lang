//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Debugger Adapater
//
//		This file contains the DPA queue declarations
// 
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DPA_QUEUE_H
#define DPA_QUEUE_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <optional>

namespace dpa
{
   // --- ContentReader ---
   template <typename T>
   class ThreadQueue
   {
      bool _closed = false;
      std::queue<T> _queue;
      std::condition_variable _cv;
      std::mutex _mutex;

   public:
      void close();

      void put(const T& in);
      std::optional<T> take();
   };

   template <typename T>
   void ThreadQueue<T> :: close()
   {
      std::unique_lock<std::mutex> lock(_mutex);
      _closed = true;
      _cv.notify_all();
   }

   template <typename T>
   void ThreadQueue<T> :: put(const T& in)
   {
      std::unique_lock<std::mutex> lock(_mutex);
      auto notify = _queue.size() == 0 && !_closed;
      _queue.push(in);
      if (notify)
         _cv.notify_all();
   }

   template <typename T>
   std::optional<T> ThreadQueue<T> :: take()
   {
      std::unique_lock<std::mutex> lock(_mutex);
      _cv.wait(lock, [&] { return _queue.size() > 0 || _closed; });
      if (_queue.size() == 0) {
         return std::optional<T>();
      }
      auto out = std::move(_queue.front());
      _queue.pop();
      return std::optional<T>(std::move(out));
   }
}

#endif