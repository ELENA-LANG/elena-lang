//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE Messages
//
//                                            (C)2021-2024, by Aleksex Rakov
//---------------------------------------------------------------------------

#ifndef MESSAGES_H
#define MESSAGES_H

namespace elena_lang
{
   constexpr auto APP_NAME                      = _T("ELENA IDE 6.0");

   constexpr auto OPEN_FILE_CAPTION             = _T("Open File");
   constexpr auto OPEN_PROJECT_CAPTION          = _T("Open Project");
   constexpr auto SAVEAS_PROJECT_CAPTION        = _T("Save File As");

   constexpr auto QUESTION_SAVE_FILECHANGES     = _T("Save changes to ");
   constexpr auto QUESTION_CLOSE_UNSAVED        = _T("Close the tab anyway");
   constexpr auto QUESTION_NOSOURCE_CONTINUE    = _T("The source code is not found. Do you want to continue?");
   constexpr auto QUESTION_SAVEPROJECT_CHANGES  = _T("Save changes to the project?");

   constexpr auto INFO_RUN_OUT_OF_DATE          = _T("The project modules are out of date\nPlease recompile the project");
   constexpr auto INFO_RUN_UNSAVED_PROJECT      = _T("The project files are not saved\nPlease save them and recompile the project");

   constexpr auto NOT_FOUND_TEXT                = _T("Search string not found");
   constexpr auto REPLACE_TEXT                  = _T("Replace this occurence?");

   // --- Hints ---
   constexpr auto HINT_NEW_FILE                 = _T("New File");
   constexpr auto HINT_OPEN_FILE                = _T("Open File");
   constexpr auto HINT_SAVE_FILE                = _T("Save File");
   constexpr auto HINT_SAVEALL                  = _T("Save All");
   constexpr auto HINT_CLOSE_FILE               = _T("Close File");
   constexpr auto HINT_CLOSE_PROJECT            = _T("Close Project");
   constexpr auto HINT_CUT                      = _T("Cut");
   constexpr auto HINT_COPY                     = _T("Copy");
   constexpr auto HINT_PASTE                    = _T("Paste");
   constexpr auto HINT_UNDO                     = _T("Undo");
   constexpr auto HINT_REDO                     = _T("Redo");
   constexpr auto HINT_RUN                      = _T("Run");
   constexpr auto HINT_STOP                     = _T("Stop");
   constexpr auto HINT_STEPINTO                 = _T("Step Into");
   constexpr auto HINT_STEPOVER                 = _T("Step Over");
   constexpr auto HINT_GOTOSOURCE               = _T("Go to Source");

   // --- About dialog ---
   constexpr auto MIT_LICENSE                   = _T("The MIT License (MIT)\r\n\r\nCopyright (c) 2006-2024: Aleksey Rakov and other contributors:\r\n\r\nhttps://github.com/ELENA-LANG/elena-lang/blob/master/doc/contributors\r\n\r\nPermission is hereby granted, free of charge, to any person obtaining a copy\r\nof this software and associated documentation files (the \"Software\"), to deal\r\nin the Software without restriction, including without limitation the rights\r\nto use, copy, modify, merge, publish, distribute, sublicense, and/or sell\r\ncopies of the Software, and to permit persons to whom the Software is\r\nfurnished to do so, subject to the following conditions:\r\n\r\nThe above copyright notice and this permission notice shall be included in all\r\ncopies or substantial portions of the Software.\r\n\r\nTHE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\r\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\r\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\r\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\r\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\r\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\r\nSOFTWARE.");
   constexpr auto ELENA_HOMEPAGE                = _T("https://elena-lang.github.io/");

}

#endif
