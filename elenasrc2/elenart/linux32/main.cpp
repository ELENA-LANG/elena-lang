//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include "elenart.h"
#include "instance.h"
#include "linux32/elfhelper.h"

#define ROOT_PATH          "/usr/lib/elena"
#define CONFIG_PATH        "/etc/elena/elenart.config"
#define IMAGE_BASE         0x08048000

using namespace _ELENA_;

static Instance* instance = NULL;

void* init()
{
   instance = new Instance(CONFIG_PATH);

   void* debugSection = NULL;
   void* messageSection = NULL;
   Instance::ImageSection section;
   section.init((void*)IMAGE_BASE, 0x1000);

   size_t ptr = 0;
   MemoryReader reader(&section);

   ELFHelper::seekDebugSegment(reader, ptr);
   debugSection = (void*)ptr;

   PEHelper::seekSection(MemoryReader(&section), ".mdata", ptr);
   messageSection = (void*)ptr;

   instance->init(debugSection, messageSection, CONFIG_PATH);

   return instance;
}

int ReadCallStack(size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength)
{
   //return ((Instance*)instance)->readCallStack(framePosition, currentAddress, startLevel, buffer, maxLength);
   return 0;
}

int LoadAddressInfo(void* retPoint, char* lineInfo, size_t length)
{
   if (instance == NULL)
      init();

   return instance->loadAddressInfo((size_t)retPoint, lineInfo, length);
}

int LoadClassName(void* object, char* buffer, size_t length)
{
   if (instance == NULL)
      init();

   return instance->loadClassName((size_t)object, buffer, length);
}

void* Interpreter(void* tape)
{
   // !! terminator code
   return NULL;
}

void* GetVMLastError(void* retVal)
{
   // !! terminator code
   return NULL;
}

int LoadSubjectName(void* subject, char* lineInfo, size_t length)
{
   if (instance == NULL)
      init();

   return instance->loadSubjectName((size_t)subject, lineInfo, length);
}

void* LoadSubject(void* subjectName)
{
   if (instance == NULL)
      init();

   return instance->loadSubject((const char*)subjectName);
}

void* LoadMessage(void* messageName)
{
   return instance->loadMessage((const char*)messageName);
}

int LoadMessageName(void* subject, char* lineInfo, size_t length)
{
   if (instance == NULL)
      init();

   return instance->loadMessageName((size_t)subject, lineInfo, length);
}

void* LoadSymbol(void* referenceName)
{
   if (instance == NULL)
      init();

   return instance->loadSymbol((const char*)referenceName);
}

