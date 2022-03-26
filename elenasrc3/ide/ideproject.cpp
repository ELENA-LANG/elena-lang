//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Project Model implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ideproject.h"

using namespace elena_lang;

// --- ProjectModel ---

ProjectModel :: ProjectModel(IDEStatus* status)
{
   this->status = status;

   this->autoRecompile = /*true*/false;// !! temporal
   this->projectPath.copy("C:\\Alex\\ELENA\\tests60\\sandbox\\"); // !! temporal
   this->outputPath.copy("C:\\Alex\\ELENA\\tests60\\sandbox\\"); // !! temporal
}

path_t ProjectModel::getOutputPath()
{
   return *outputPath;
}

ustr_t ProjectModel :: getTarget()
{
   return "sandbox.exe"; // !! temporal
}

ustr_t ProjectModel::getPackage()
{
   return "sandbox"; // !! temporal

}

ustr_t ProjectModel::getArguments()
{
   return nullptr; // !! temporal
}

bool ProjectModel :: getDebugMode()
{
   return true; // !! temporal
}
