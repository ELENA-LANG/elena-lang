//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Project Model implementation File
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ideproject.h"

using namespace elena_lang;

// --- ProjectModel ---

ProjectModel :: ProjectModel(IDEStatus* status)
   : referencePaths(nullptr),
   lastOpenFiles(nullptr),
   lastOpenProjects(nullptr),
   sources(nullptr), addedSources(nullptr), removeSources(nullptr),
   breakpoints({}), projectTypeList(nullptr), profileList(nullptr)
{
   this->status = status;

   this->withPersistentConsole = false;
   this->autoRecompile = false;

   this->empty = true;
   this->started = false;
   this->notSaved = false;

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
