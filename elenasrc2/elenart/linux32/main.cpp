//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"

extern "C"
{
   void* Init(void* debugSection, const char* package)
   {
//      if (instance) {
//         if (debugSection == NULL) {
//            Instance::ImageSection section;
//            section.init((void*)0x400000, 0x1000);
//
//            size_t ptr = 0;
//            PEHelper::seekSection(MemoryReader(&section), ".debug", ptr);
//            debugSection = (void*)ptr;
//         }
//
//         instance->init(debugSection, package);
//
//         return instance;
//      }
      /*else */return 0;
   }

   int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
   {
      return /*((Instance*)instance)->readCallStack(framePosition, currentAddress, startLevel, buffer, maxLength)*/0;
   }

   int LoadAddressInfo(void* instance, int retPoint, char* lineInfo, int length)
   {
      return /*((Instance*)instance)->loadAddressInfo(retPoint, lineInfo, length)*/0;
   }

   int LoadClassName(void* instance, void* object, char* lineInfo, int length)
   {
      // !! terminator code
      return 0;
   }

   void* GetSymbolRef(void* instance, void* referenceName)
   {
      // !! terminator code
      return NULL;
   }

   void* Interpreter(void* instance, void* tape)
   {
      // !! terminator code
      return NULL;
   }

   void* GetRTLastError(void* instance, void* retVal)
   {
      // !! terminator code
      return NULL;
   }
}
