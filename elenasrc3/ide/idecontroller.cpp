//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Controller implementation File
//                                             (C)2005-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecontroller.h"
#include "eng/messages.h"

using namespace elena_lang;

// --- SourceViewController ---

void SourceViewController :: newSource(TextViewModelBase* model, ustr_t caption, bool autoSelect)
{
//   IdentifierString tabName("unnamed");

   newDocument(model, caption, model->empty ? NOTIFY_CURRENTVIEW_SHOW : 0);

   if (autoSelect) {
      selectDocument(model, caption);

      model->DocView()->status.unnamed = true;
      model->DocView()->status.modifiedMode = true;
   }
}

bool SourceViewController :: openSource(TextViewModelBase* model, ustr_t caption, path_t sourcePath, 
   FileEncoding encoding, bool autoSelect)
{
   if (openDocument(model, caption, sourcePath, encoding,
      model->empty ? NOTIFY_CURRENTVIEW_SHOW : 0))
   {
      if (autoSelect)
         selectDocument(model, caption);

      return true;
   }
   else return false;
}

void SourceViewController :: renameSource(TextViewModelBase* model, ustr_t oldName, ustr_t newName, path_t newSourcePath)
{
   model->renameDocumentView(oldName, newName, newSourcePath);
}

void SourceViewController :: closeSource(TextViewModelBase* model, ustr_t name, bool autoSelect)
{
   int index = model->getDocumentIndex(name);
   if (index != -1) {
      int count = model->getDocumentCount();

      closeDocument(model, name, count == 1 ? NOTIFY_CURRENTVIEW_HIDE : 0);

      if (autoSelect && !model->empty) {
         if (index == count) {
            selectDocument(model, model->getDocumentName(count - 1));
         }
         else selectDocument(model, model->getDocumentName(index));
      }
   }
}

void SourceViewController :: saveSource(TextViewModelBase* model, ustr_t name)
{
   auto docInfo = model->getDocument(name);
   path_t path = model->getDocumentPath(name);

   if (docInfo)
      docInfo->save(path);
}

// --- ProjectController ---

bool ProjectController::isIncluded(ProjectModel& model, ustr_t ns)
{
   return NamespaceString::isIncluded(model.getPackage(), ns);
}

void ProjectController :: defineFullPath(ProjectModel& model, ustr_t ns, path_t path, 
   PathString& fullPath)
{
   if (isIncluded(model, ns)) {
      fullPath.copy(*model.projectPath);
      fullPath.combine(path);
   }
   else {
      fullPath.copy(*model.paths.librarySourceRoot);
      fullPath.combine(ns);
      fullPath.combine(path);
   }
}

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

bool ProjectController :: onDebugAction(ProjectModel& model, path_t singleProjectPath, DebugAction action)
{
   if (testIDEStatus(model.getStatus(), IDEStatus::Busy))
      return false;

   if (!_debugController.isStarted()) {
      bool toRecompile = model.autoRecompile && !testIDEStatus(model.getStatus(), IDEStatus::AutoRecompiling);
      if (isOutaged(toRecompile)) {
         if (toRecompile) {
            if (!doCompileProject(model, singleProjectPath, action))
               return false;
         }
         return false;
      }
      if (!startDebugger(model))
         return false;
   }
   return true;
}

void ProjectController :: doDebugAction(ProjectModel& model, path_t singleProjectPath, DebugAction action)
{
   if (!testIDEStatus(model.getStatus(), IDEStatus::Busy)) {
      if (onDebugAction(model, singleProjectPath, action)) {
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

bool ProjectController :: compile()
{


   return true;
}

bool ProjectController :: compileSingleFile(ProjectModel& model, path_t singleProjectFile)
{
   PathString appPath(model.paths.appPath);
   appPath.combine(*model.paths.compilerPath);

   PathString cmdLine(*model.paths.compilerPath);
   cmdLine.append(" ");
   cmdLine.append(singleProjectFile);

   PathString curDir;
   curDir.append(singleProjectFile, singleProjectFile.findLast('\\'));

   return _osController->execute(*appPath, *cmdLine, *curDir);
}

bool ProjectController :: doCompileProject(ProjectModel& model, path_t singleProjectFile, DebugAction postponedAction)
{
   if (!singleProjectFile.empty()) {
      return compileSingleFile(model, singleProjectFile);
   }
   else return false;   
}

// --- IDEController ---

void IDEController :: init(IDEModel* model)
{
   model->changeStatus(IDEStatus::Ready);
}

bool IDEController :: selectSource(ProjectModel* model, SourceViewModel* sourceModel,
   ustr_t ns, path_t sourcePath)
{
   PathString fullPath;
   projectController.defineFullPath(*model, ns, sourcePath, fullPath);

   return openFile(sourceModel, *fullPath);
}

void IDEController :: doNewFile(IDEModel* model)
{
   IdentifierString sourceNameStr;
   projectController.defineSourceName(nullptr, sourceNameStr);

   sourceController.newSource(&model->sourceViewModel, *sourceNameStr, true);
}

bool IDEController :: openFile(IDEModel* model, path_t sourceFile)
{
   return openFile(&model->sourceViewModel, sourceFile);
}

bool IDEController :: openFile(SourceViewModel* model, path_t sourceFile)
{
   ustr_t sourceName = model->getDocumentNameByPath(sourceFile);
   if (!sourceName.empty()) {
      sourceController.selectDocument(model, sourceName);

      return true;
   }
   else {
      IdentifierString sourceNameStr;
      projectController.defineSourceName(sourceFile, sourceNameStr);

      sourceName = *sourceNameStr;
   }

   return sourceController.openSource(model, sourceName, sourceFile, 
      defaultEncoding, true);
}

void IDEController :: doOpenFile(DialogBase& dialog, IDEModel* model)
{
   List<path_t, freepath> files(nullptr);
   if (dialog.openFiles(files)) {
      for (auto it = files.start(); !it.eof(); ++it) {
         if(openFile(model, *it)) {
            
         }
      }
   }
}

bool IDEController :: doSaveFile(DialogBase& dialog, IDEModel* model, bool saveAsMode, bool forcedSave)
{
   auto docView = model->sourceViewModel.DocView();
   if (!docView)
      return false;

   if (docView->status.unnamed || saveAsMode) {
      PathString path;
      if (!dialog.saveFile(_T("l"), path))
         return false;

      IdentifierString sourceNameStr;
      projectController.defineSourceName(*path, sourceNameStr);

      sourceController.renameSource(&model->sourceViewModel, nullptr, *sourceNameStr, *path);

      forcedSave = true;
   }

   if (forcedSave)
      sourceController.saveSource(&model->sourceViewModel, nullptr);

   return true;
}

bool IDEController :: doSaveProject(DialogBase& dialog, IDEModel* model, bool forcedMode)
{
   // !! temporal
   if (!doSaveFile(dialog, model, false, forcedMode))
      return false;

   return true;
}

bool IDEController :: doCloseFile(DialogBase& dialog, IDEModel* model)
{
   auto docView = model->sourceViewModel.DocView();
   if (docView) {
      ustr_t current = model->sourceViewModel.getDocumentName(-1);

      if (docView->status.unnamed) {
         doSaveFile(dialog, model, false, true);
      }
      else if (docView->status.modifiedMode) {
         path_t path = model->sourceViewModel.getDocumentPath(current);

         auto result = dialog.question(
            QUESTION_SAVE_FILECHANGES, path);

         if (result == DialogBase::Answer::Cancel) {
            return false;
         }
         else if (result == DialogBase::Answer::Yes) {
            if (!doSaveFile(dialog, model, false, true))
               return false;
         }
      }

      sourceController.closeSource(&model->sourceViewModel, current, true);

      return true;
   }
   return false;
}

bool IDEController :: doExit()
{
   return true;
}

path_t IDEController :: retrieveSingleProjectFile(IDEModel* model)
{
   return model->sourceViewModel.getDocumentPath(
      model->sourceViewModel.getDocumentName(-1));
}

void IDEController :: doDebugAction(IDEModel* model, DebugAction action)
{
   projectController.doDebugAction(model->projectModel, retrieveSingleProjectFile(model), action);
}

void IDEController :: onCompilationStart(IDEModel* model)
{
   model->status = IDEStatus::Busy;

   model->onIDEChange();

   _notifier->notifyMessage(NOTIFY_SHOW_RESULT, model->ideScheme.compilerOutputControl);
}

void IDEController :: onCompilationStop(IDEModel* model)
{
   model->status = IDEStatus::Ready;

   model->onIDEChange();
}

void IDEController :: onCompilationBreak(IDEModel* model)
{
   model->status = IDEStatus::Ready;

   model->onIDEChange();
}

bool IDEController :: doCompileProject(DialogBase& dialog, IDEModel* model)
{
   onCompilationStart(model);

   if (!doSaveProject(dialog, model, false)) {
      onCompilationBreak(model);

      return false;
   }

   if (projectController.doCompileProject(model->projectModel, retrieveSingleProjectFile(model), 
      DebugAction::None)) 
   {
      onCompilationStop(model);

      return true;
   }

   return false;
}
