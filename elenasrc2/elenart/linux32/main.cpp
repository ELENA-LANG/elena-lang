//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include "elenart.h"
#include "instance.h"
#include "linux32/elfhelper.h"

#define ROOT_PATH          "/usr/lib/elena"
#define CONFIG_PATH        "/etc/elena/elc.config"
#define IMAGE_BASE         0x08048000

using namespace _ELENA_;

static Instance* instance = NULL;

void* Init(void* debugSection, const char* package)
{
   if (instance == NULL) {
      instance = new Instance(CONFIG_PATH);
   }

   if (debugSection == NULL) {
      Instance::ImageSection section;
      section.init((void*)IMAGE_BASE, 0x1000);

      size_t ptr = 0;
      MemoryReader reader(&section);

      ELFHelper::seekDebugSegment(reader, ptr);
      debugSection = (void*)ptr;
   }

   instance->init(debugSection, package, CONFIG_PATH);

   return instance;
}

int ReadCallStack(void* instance, size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   return ((Instance*)instance)->readCallStack(framePosition, currentAddress, startLevel, buffer, maxLength);
}

int LoadAddressInfo(void* instance, int retPoint, char* lineInfo, int length)
{
   return ((Instance*)instance)->loadAddressInfo(retPoint, lineInfo, length);
}

int LoadClassName(void* instance, void* object, char* buffer, int length)
{
   return ((Instance*)instance)->loadClassName((size_t)object, buffer, length);
}

void* GetSymbolRef(void* instance, void* referenceName)
{
   return ((Instance*)instance)->loadSymbol((ident_t)referenceName);
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

int LoadSubjectName(void* instance, void* subject, ident_c* lineInfo, int length)
{
   return ((Instance*)instance)->loadSubjectName((size_t)subject, lineInfo, length);
}

void* LoadSubject(void* instance, void* subjectName)
{
   return ((Instance*)instance)->loadSubject((ident_t)subjectName);
}

