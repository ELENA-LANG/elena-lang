//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ECVIEWER_H
#define ECVIEWER_H

#include "elena.h"
#include "bytecode.h"
#include "libman.h"

namespace elena_lang
{
   // --- PresenterBase ---
   class PresenterBase
   {
   public:
      virtual void setOutputMode(ustr_t arg) = 0;

      virtual void readLine(char* buffer, size_t length) = 0;

      virtual void print(ustr_t message) = 0;
      virtual void print(ustr_t message, ustr_t arg) = 0;
      virtual void printPath(ustr_t message, path_t arg) = 0;

      virtual ~PresenterBase() = default;
   };

   // --- ByteCodeViewer ---
   class ByteCodeViewer
   {
      PresenterBase*   _presenter;
      LibraryProvider* _provider;
      ModuleBase*      _module;
      int              _pageSize;
      bool             _noPaging;
      bool             _pathMode;
      bool             _showBytecodes;

      MemoryBase* findProcedureCode(ustr_t referenceName);
      MemoryBase* findSymbolCode(ustr_t referenceName);
      MemoryBase* findClassVMT(ustr_t referenceName);
      MemoryBase* findClassCode(ustr_t referenceName);

      bool findClassInfo(ustr_t referenceName, ClassInfo& info);
      bool findMethodInfo(ustr_t referenceName, mssg_t message, MethodInfo& info);

      mssg_t resolveMessageByIndex(MemoryBase* vmt, int index);
      mssg_t resolveMessage(ustr_t methodName);

      void printHelp();
      void printModuleManifest();

      void nextRow(int& row, int pageSize);

      void printLine(ustr_t arg1);
      void printLine(ustr_t arg1, ustr_t arg2);
      void printLineAndCount(ustr_t arg1, ustr_t arg2, int& row, int pageSize);

      void listMembers();

      void addRArg(arg_t arg, IdentifierString& commandStr);
      void addSecondRArg(arg_t arg, IdentifierString& commandStr);

      void addSPArg(arg_t arg, IdentifierString& commandStr);
      void addSecondSPArg(arg_t arg, IdentifierString& commandStr);

      void addFPArg(arg_t arg, IdentifierString& commandStr);
      void addSecondFPArg(arg_t arg, IdentifierString& commandStr);

      void addArg(arg_t arg, IdentifierString& commandStr);
      void addSecondArg(arg_t arg, IdentifierString& commandStr);

      void addLabel(arg_t arg, IdentifierString& commandStr, List<pos_t>& labels);

      void addCommandArguments(ByteCommand& command, IdentifierString& commandStr, 
         List<pos_t>& labels, pos_t commandPosition);

      void addMessage(IdentifierString& commandStr, mssg_t message);

      void printCommand(ByteCommand& command, int indent, 
         List<pos_t>& labels, pos_t commandPosition);
      void printByteCodes(MemoryBase* section, pos_t address, int indent, int pageSize);

      void printFlags(ref_t flags, int& row, int pageSize);
      void printFields(ClassInfo& classInfo, int& row, int pageSize);

      void printMethod(ustr_t name, bool fullInfo);
      void printSymbol(ustr_t name);
      void printProcedure(ustr_t name);
      void printClass(ustr_t name, bool fullInfo);

   public:
      bool load(path_t path);
      bool loadByName(ustr_t name);

      void runSession();

      ByteCodeViewer(LibraryProvider* provider, PresenterBase* presenter, int pageSize)
      {
         _presenter = presenter;
         _provider = provider;
         _module = nullptr;
         _pageSize = pageSize;
         _noPaging = false;
         _pathMode = false;
         _showBytecodes = false;
      }
      virtual ~ByteCodeViewer()
      {
         if (_pathMode)
            freeobj(_module);
      }
   };
}

#endif
