//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef elenartmachineH
#define elenartmachineH 1

#include "libman.h"
#include "elenamachine.h"

namespace _ELENA_
{

// --- ELENARTMachine ---

class ELENARTMachine
{
public:
   class ImageSection : public _Memory
   {
      void* _section;
      pos_t _length;

   public:
      void init(void* section, pos_t length)
      {
         _section = section;
         _length = length;
      }
      int init(void* section, IdentifierString& package)
      {
         _section = section;
         _length = 4;

         MemoryReader reader(this);
         _length = reader.getDWord();

         reader.getDWord();
         reader.readString(package);

         return reader.Position();
      }

      virtual pos_t Length() const
      {
         return _length;
      }

      virtual void* get(pos_t position) const;

      //virtual void* getLong(pos64_t position) const
      //{
      //   if (position < INT_MAX) {
      //      return get((pos_t)position);
      //   }
      //   else return NULL;
      //}

      virtual bool read(pos_t position, void* s, pos_t length);

      virtual bool readLong(pos64_t position, void* s, pos64_t length)
      {
         if (position < INT_MAX && length < INT_MAX) {
            return read((pos_t)position, s, (pos_t)length);
         }
         else return false;
      }

      virtual bool write(pos_t position, const void* s, pos_t length)
      {
         // write operations are not supported
         return false;
      }

      virtual void insert(pos_t position, const void* s, pos_t length)
      {
         // insert operations are not supported
      }

      virtual bool writeBytes(pos_t position, char value, pos_t length)
      {
         // write operations are not supported
         return false;
      }

      virtual void trim(pos_t position)
      {
         // trim operations are not supported
      }
      virtual void trimLong(pos64_t position)
      {
         // trim operations are not supported
      }

      ImageSection()
      {
         _section = NULL;
         _length = 0;
      }
   };

private:
   Path           _rootPath;   
   Path           _debugFilePath;
   MemoryDump     _debugSection;
   ClassMap       _generated;

   void*          _messageSection;
   void*          _mattributesSection;

   LibraryManager _loader;

   bool loadConfig(path_t configPath);

   bool loadDebugSection();

   ref_t loadDispatcherOverloadlist(ident_t referenceName);

public:
   // frameHeader contains initialized frame fields
   void startSTA(ProgramHeader* frameHeader, SystemEnv* env, void* programEntry);
   void startMTA(ProgramHeader* frameHeader, SystemEnv* env, void* programEntry);
   void startThread(ProgramHeader* frameHeader, SystemEnv* env, void* entryPoint, int index);

   void Exit(int exitCode);
   void ExitThread(SystemEnv* env, int exitCode);

   int readCallStack(size_t framePosition, vaddr_t currentAddress, vaddr_t startLevel, vaddr_t* buffer, pos_t maxLength);

   vaddr_t loadAddressInfo(size_t retPoint, char* lineInfo, size_t length);

   size_t loadClassName(vaddr_t classAddress, char* buffer, size_t length);
   size_t loadSubjectName(ref_t subjectRef, char* buffer, size_t length);
   size_t loadMessageName(mssg_t messageRef, char* buffer, size_t length);

   vaddr_t loadMetaAttribute(ident_t name, int category);
   ref_t loadSubject(ident_t name);
   mssg_t loadMessage(ident_t name);
   
   vaddr_t loadSignatureMember(mssg_t message, int index);
   
   uintptr_t createPermString(SystemEnv* env, ident_t s, uintptr_t classPtr);

   vaddr_t inherit(SystemEnv* env, const char* name, VMTEntry* src, VMTEntry* base, size_t srcLength, size_t baseLength, 
      pos_t* addresses, size_t length, int flags);

   int loadExtensionDispatcher(const char* moduleList, ref_t message, void* output);

   void init(void* messageTable, void* mattributeTable, path_t configPath);

   ELENARTMachine(path_t dllRootPath, path_t execFile);

   virtual ~ELENARTMachine()
   {
   }
};

} // _ELENA_

#endif // elenartmachineH

