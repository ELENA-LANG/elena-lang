//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE Messages
//
//                                            (C)2021-2023, by Aleksex Rakov
//---------------------------------------------------------------------------

#ifndef MESSAGES_H
#define MESSAGES_H

namespace elena_lang
{
   constexpr auto APP_NAME                   = _T("ELENA IDE 6.0");

   constexpr auto OPEN_FILE_CAPTION          = _T("Open File");
   constexpr auto OPEN_PROJECT_CAPTION       = _T("Open Project");
   constexpr auto SAVEAS_PROJECT_CAPTION     = _T("Save File As");

   constexpr auto QUESTION_SAVE_FILECHANGES  = _T("Save changes to ");
   constexpr auto QUESTION_CLOSE_UNSAVED     = _T("Close the tab anyway");

   constexpr auto NOT_FOUND_TEXT             = _T("Search string not found");
   constexpr auto REPLACE_TEXT               = _T("Replace this occurence?");

   // --- Hints ---
   constexpr auto HINT_NEW_FILE              = _T("New File");
   constexpr auto HINT_OPEN_FILE             = _T("Open File");
   constexpr auto HINT_SAVE_FILE             = _T("Save File");
   constexpr auto HINT_SAVEALL               = _T("Save All");
   constexpr auto HINT_CLOSE_FILE            = _T("Close File");
   constexpr auto HINT_CLOSE_PROJECT         = _T("Close Project");
   constexpr auto HINT_CUT                   = _T("Cut");
   constexpr auto HINT_COPY                  = _T("Copy");
   constexpr auto HINT_PASTE                 = _T("Paste");
   constexpr auto HINT_UNDO                  = _T("Undo");
   constexpr auto HINT_REDO                  = _T("Redo");
   constexpr auto HINT_RUN                   = _T("Run");
   constexpr auto HINT_STOP                  = _T("Stop");
   constexpr auto HINT_STEPINTO              = _T("Step Into");
   constexpr auto HINT_STEPOVER              = _T("Step Over");
   constexpr auto HINT_GOTOSOURCE            = _T("Go to Source");

}

#endif
