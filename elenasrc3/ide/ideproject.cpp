//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Project Model implementation File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ideproject.h"

using namespace elena_lang;

// --- ProjectModel ---

ProjectModel :: ProjectModel(IDEStatus* status)
   : lastOpenFiles(nullptr),
   lastOpenProjects(nullptr),
   sources(nullptr), addedSources(nullptr),
   breakpoints({}), projectTypeList(nullptr)
{
   this->status = status;

   this->autoRecompile = /*true*/false;// !! temporal

   this->empty = true;
   this->started = false;
   this->notSaved = false;

#ifdef _M_IX86
   this->paths.libraryRoot.copy("C:\\Alex\\ELENA\\lib60\\");      // !! temporal
#else
   this->paths.libraryRoot.copy("C:\\Alex\\ELENA\\lib60_64\\");      // !! temporal
#endif

   this->paths.librarySourceRoot.copy("C:\\Alex\\ELENA\\src60\\");// !! temporal

   // !!NOTE : make sure the path separator should tail the path
   if (this->paths.librarySourceRoot[this->paths.librarySourceRoot.length() - 1] != PATH_SEPARATOR)
      this->paths.librarySourceRoot.append(PATH_SEPARATOR);
}

path_t ProjectModel :: getOutputPath()
{
   return *outputPath;
}

ustr_t ProjectModel :: getTarget()
{
   return *target;
}

ustr_t ProjectModel::getPackage()
{
   return *package;
}

ustr_t ProjectModel :: getArguments()
{
   return nullptr; // !! temporal
}

bool ProjectModel :: getDebugMode()
{
   return true; // !! temporal
}
