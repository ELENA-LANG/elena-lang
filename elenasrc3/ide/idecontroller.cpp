//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller implementation File
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecontroller.h"

using namespace elena_lang;

// --- SourceViewController ---

void SourceViewController :: newSource(TextViewModelBase* model, ustr_t caption, bool autoSelect)
{
//   IdentifierString tabName("unnamed");

   newDocument(model, caption);

   if (autoSelect) {
      selectDocument(model, caption);

      model->DocView()->status.unnamed = true;
      model->DocView()->status.modifiedMode = true;
   }
}

bool SourceViewController :: openSource(TextViewModelBase* model, ustr_t caption, path_t sourcePath, 
   FileEncoding encoding, bool autoSelect)
{
   openDocument(model, caption, sourcePath, encoding);

   if (autoSelect)
      selectDocument(model, caption);

   return true;
}

void SourceViewController :: saveSource(TextViewModelBase* model, ustr_t name)
{
   
}

// --- ProjectController ---

void ProjectController :: defineSourceName(path_t path, IdentifierString& retVal)
{
   if (path.empty()) {
      retVal.copy("undefined");
   }
   else {
      IdentifierString fileName(path + path.findLast(PATH_SEPARATOR) + 1);

      retVal.append(*fileName);
   }
}

bool ProjectController :: startDebugger(ProjectModel& model)
{
   ustr_t target = model.getTarget();
   ustr_t arguments = model.getArguments();

   if (!target.empty()) {
      PathString exePath(*model.projectPath, target);

      // provide the whole command line including the executable path and name
      PathString commandLine(exePath);
      commandLine.append(_T(" "));
      commandLine.append(arguments);

      bool debugMode = model.getDebugMode();
      if (debugMode) {
         if (!_debugController.start(exePath.str(), commandLine.str(), debugMode/*, _breakpoints */)) {
            notifyMessage(ERROR_DEBUG_FILE_NOT_FOUND_COMPILE);

            return false;
         }

      }
      else {
         if (!_debugController.start(exePath.str(), commandLine.str(), false/*, _breakpoints */)) {
            notifyMessage(ERROR_RUN_NEED_RECOMPILE);

            return false;
         }
      }

      return true;
   }
   else {
      notifyMessage(ERROR_RUN_NEED_TARGET);

      return false;
   }
}

bool ProjectController :: isOutaged(bool noWarning)
{
   return false; // !! temporal
}

bool ProjectController :: onDebugAction(ProjectModel& model, DebugAction action)
{
   if (testIDEStatus(*model.status, IDEStatus::Busy))
      return false;

   if (!_debugController.isStarted()) {
      bool toRecompile = model.autoRecompile && !testIDEStatus(*model.status, IDEStatus::AutoRecompiling);
      if (isOutaged(toRecompile)) {
         if (toRecompile) {
            if (!doCompileProject(model, action))
               return false;
         }
         return false;
      }
      if (!startDebugger(model))
         return false;
   }
   return true;
}

void ProjectController :: doDebugAction(ProjectModel& model, DebugAction action)
{
   if (!testIDEStatus(*model.status, IDEStatus::Busy)) {
      if (onDebugAction(model, action)) {
         switch (action) {
            case DebugAction::Run:
               _debugController.run();
               break;
            case DebugAction::StepInto:
               _debugController.stepInto();
               break;
            case DebugAction::StepOver:
               _debugController.stepOver();
               break;
            default:
               break;
         }
      }
   }
}

bool ProjectController :: doCompileProject(ProjectModel& model, DebugAction postponedAction)
{
   return true; // !! temporal
}

// --- IDEController ---

void IDEController :: doNewFile(IDEModel* model)
{
   IdentifierString sourceNameStr;
   projectController.defineSourceName(nullptr, sourceNameStr);

   sourceController.newSource(&model->sourceViewModel, *sourceNameStr, true);
}

bool IDEController :: openFile(IDEModel* model, path_t sourceFile)
{
   ustr_t sourceName = model->sourceViewModel.getDocumentNameByPath(sourceFile);
   if (!sourceName.empty()) {
      sourceController.selectDocument(&model->sourceViewModel, sourceName);

      return false;
   }
   else {
      IdentifierString sourceNameStr;
      projectController.defineSourceName(sourceFile, sourceNameStr);

      sourceName = *sourceNameStr;
   }

   return sourceController.openSource(&model->sourceViewModel, sourceName, sourceFile, defaultEncoding, true);
}

void IDEController :: doOpenFile(FileDialogBase& dialog, IDEModel* model)
{
   List<path_t, freepath> files(nullptr);
   if (dialog.openFiles(files)) {
      for (auto it = files.start(); !it.eof(); ++it) {
         if(openFile(model, *it)) {
            
         }
      }
   }
}

void IDEController :: doSaveFile(FileDialogBase& dialog, IDEModel* model)
{
   auto docView = model->sourceViewModel.DocView();
   if (!docView)
      return;

   if (docView->status.unnamed) {
      
   }

   sourceController.saveSource(&model->sourceViewModel, nullptr);
}
