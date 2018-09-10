//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2009-2018, by Alexei Rakov
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
      void*  _section;
      size_t _length;

   public:
      void init(void* section, size_t length)
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

      virtual void* getLong(pos64_t position) const
      {
         if (position < INT_MAX) {
            return get((pos_t)position);
         }
         else return NULL;
      }

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
   ImageSection   _debugSection;
   size_t         _debugOffset;
   void*          _messageSection;
   LibraryManager _loader;
   MessageMap     _verbs;

   bool loadConfig(path_t configPath);

   bool initSubjectSection(ImageSection& subjectSection);

public:
   // frameHeader contains initialized frame fields
   void startSTA(FrameHeader* frameHeader, SystemEnv* env, void* programEntry);
   void startMTA(FrameHeader* frameHeader, SystemEnv* env, void* programEntry);

   void Exit(int exitCode);

   // !! 
   int readCallStack(size_t framePosition, size_t currentAddress, size_t startLevel, int* buffer, size_t maxLength);

   int loadAddressInfo(size_t retPoint, char* lineInfo, size_t length);

   int loadClassName(size_t classAddress, char* buffer, size_t length);
   int loadSubjectName(size_t subjectRef, char* buffer, size_t length);
   int loadMessageName(size_t subjectRef, char* buffer, size_t length);

   void* loadSymbol(ident_t name);
   void* loadSubject(ident_t name);
   void* loadMessage(ident_t name);

   void init(void* debugSection, void* messageTable,  path_t configPath);

   ELENARTMachine(path_t rootPath);

   virtual ~ELENARTMachine()
   {
   }
};

} // _ELENA_

#endif // elenartmachineH

