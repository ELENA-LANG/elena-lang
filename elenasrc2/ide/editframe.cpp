//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     EditFrame class Implementation File
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

//#include "guicommon.h"
//---------------------------------------------------------------------------
#include "editframe.h"
#include "idecommon.h"
#include "sourcedoc.h"

using namespace _GUI_;

// --- EditFrame ---

const wchar16_t* _EditFrame :: getDocumentPath(int index)
{
   if (index == -1)
      index = getCurrentDocumentIndex();

   return _ELENA_::retrieveKey(_mappings.start(), index, DEFAULT_STR);
}

int _EditFrame :: getDocumentIndex(const tchar_t* path)
{
   return _mappings.get(path);
}

void _EditFrame :: selectDocument(const tchar_t* path)
{
   selectDocumentTab(_mappings.get(path));
}

void _EditFrame :: selectDocument(int index)
{
   selectDocumentTab(index);
}

Document* _EditFrame :: getDocument(int index)
{
   if (index == -1) {
      return _currentDoc;
   }
   else return *_documents.get(index);
}

Document* _EditFrame :: openDocument(const tchar_t* path, Text* text, _GUI_::LexicalStyler* styler, int encoding)
{
   _GUI_::Document* doc = new _GUI_::SourceDoc(text, styler, encoding);

   int index = addDocumentTab(_ELENA_::FileName(path), doc);

   _mappings.add(path, index);
   _documents.add(doc);

   return doc;
}

void _EditFrame :: renameDocument(int index, const tchar_t* path)
{
   const tchar_t* oldPath = _ELENA_::retrieveKey(_mappings.start(), index, DEFAULT_STR);

   _mappings.erase(oldPath);
   _mappings.add(path, index);

   renameDocumentTab(index, _ELENA_::FileName(path));
}

void _EditFrame :: closeDocument(int index)
{
   _currentDoc = NULL;

   _documents.cut(_documents.get(index));

   const tchar_t* path = _ELENA_::retrieveKey(_mappings.start(), index, DEFAULT_STR);
   _mappings.erase(path);

   DocMapping::Iterator it = _mappings.start();
   int i = 0;
   while (!it.Eof()) {
      *it = i;
      it++;
      i++;
   }

   eraseDocumentTab(index);
}

bool _EditFrame :: isDocumentIncluded(int index)
{
   Document* doc = getDocument(index);

   return doc ? doc->status.included : false;
}

bool _EditFrame :: isDocumentModified(int index)
{
   Document* doc = getDocument(index);

   return doc ? doc->status.modifiedMode : false;
}

bool _EditFrame :: isDocumentUnnamed(int index)
{
   Document* doc = getDocument(index);

   return doc ? doc->status.unnamed : false;
}

void _EditFrame :: saveDocument(const tchar_t* path, int index)
{
   Document* doc = getDocument(index);
   if (doc) {
      doc->save(path);
   }
}

void _EditFrame :: markDocumentAsIncluded(int index)
{
   Document* doc = getDocument(index);
   if (doc) {
      doc->status.included = true;
   }
}

void _EditFrame :: markDocumentAsExcluded(int index)
{
   Document* doc = getDocument(index);
   if (doc) {
      doc->status.included = false;
   }
}

void _EditFrame :: undo()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->undo();

      refreshDocument();
   }
}

void _EditFrame :: redo()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->redo();

      refreshDocument();
   }
}

void _EditFrame :: selectAll()
{
   if (_currentDoc) {
      _currentDoc->moveFirst(false);
      _currentDoc->moveLast(true);

      refreshDocument();
   }
}

void _EditFrame :: trim()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->trim();

      refreshDocument();
   }
}

void _EditFrame :: duplicateLine()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->duplicateLine();

      refreshDocument();
   }
}

void _EditFrame :: eraseLine()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->eraseLine();

      refreshDocument();
   }
}

void _EditFrame :: commentBlock()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->commentBlock();

      refreshDocument();
   }
}

void _EditFrame :: uncommentBlock()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->uncommentBlock();

      refreshDocument();
   }
}

void _EditFrame :: toUppercase()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->toUppercase();

      refreshDocument();
   }
}

void _EditFrame :: toLowercase()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->toLowercase();

      refreshDocument();
   }
}

void _EditFrame :: swap()
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->swap();

      refreshDocument();
   }
}

bool _EditFrame :: findText(SearchOption& option)
{
   if (_currentDoc) {
      if (_currentDoc->findLine(option.text, option.matchCase, option.wholeWord)) {
         refreshDocument();

         return true;
      }
   }
   return false;
}

bool _EditFrame :: replaceText(SearchOption& option)
{
   if (_currentDoc && !_currentDoc->status.readOnly) {
      _currentDoc->insertLine(option.newText, _ELENA_::getlength(option.newText));

      refreshDocument();

      return true;
   }
   else return false;
}

FrameState _EditFrame :: getState()
{
   int states = editEmpty;

   if (_currentDoc != NULL) {
      states = editHasDocument;

      // check document statesqwe
      if (_currentDoc->hasSelection())
         states |= editHasSelection;
      if (_currentDoc->canUndo())
         states |= editCanUndo;
      if (_currentDoc->canRedo())
         states |= editCanRedo;
      if (_currentDoc->status.modifiedMode)
         states |= editModifiedMode;
      if (_currentDoc->status.isModeChanged())
         states |= editModeChanged;
      if (_currentDoc->status.overwriteMode)
         states |= editOverwriteMode;
   }
   return (FrameState)states;
}

//bool Editor :: isDocumentUnnamed(int index)
//{
//   if (index >= 0 && index < (int)_documents.Count()) {
//      return (*_documents.get(index))->status.unnamed;
//   }
//   else return false;
//}


bool _EditFrame :: isAnyModified()
{
   for (int i = 0 ; i < getDocumentCount() ; i++) {
      if (getDocument(i)->status.modifiedMode)
         return true;
   }
   return false;
}

void _EditFrame :: setReadOnlyMode(bool mode)
{
   for (int i = 0 ; i < getDocumentCount() ; i++) {
      getDocument(i)->status.readOnly = mode;
   }
}

void _EditFrame :: addDocumentMarker(int index, HighlightInfo info, int bandStyle, int style)
{
   Document* doc = getDocument(index);

   doc->addMarker(info, bandStyle, style);

   refreshDocument();
}

void _EditFrame :: removeDocumentMarker(int index, int row, int bandStyle)
{
   Document* doc = getDocument(index);

   doc->removeMarker(row, bandStyle);

   refreshDocument();
}

void _EditFrame :: removeAllDocumentMarker(int bandStyle)
{
   for (int i = 0 ; i < getDocumentCount() ; i++) {
      getDocument(i)->removeMarker(-1, bandStyle);
   }

   if (_currentDoc)
      refreshDocument();
}
