//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the Windows Presenter declaration
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PRESENTER_H
#define PRESENTER_H

#include "elena.h"

namespace elena_lang
{
   class LinuxConsolePresenter : public PresenterBase
   {
   public:
      void readLine(char* buffer, size_t length) override;

      void print(ustr_t msg, ustr_t arg) override;
      void print(ustr_t msg, ustr_t arg1, ustr_t arg2) override;
      void print(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3) override;
      void print(ustr_t msg, int arg1, int arg2, int arg3) override;
      void printPath(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4) override;
      void print(ustr_t msg, int arg1) override;
      void print(ustr_t msg, int arg1, int arg2) override;
      void printPath(ustr_t msg, path_t arg) override;
      void print(ustr_t msg) override;
      void print(ustr_t msg, ustr_t path, int col, int row, ustr_t s) override;

      void printLine(ustr_t msg, ustr_t arg) override;
      void printLine(ustr_t msg, ustr_t arg1, ustr_t arg2) override;
      void printLine(ustr_t msg, ustr_t arg1, ustr_t arg2, ustr_t arg3) override;
      void printLine(ustr_t msg, int arg1, int arg2, int arg3) override;
      void printPathLine(ustr_t msg, path_t arg1, int arg2, int arg3, ustr_t arg4) override;
      void printLine(ustr_t msg, int arg1) override;
      void printLine(ustr_t msg, int arg1, int arg2) override;
      void printPathLine(ustr_t msg, path_t arg) override;
      void printLine(ustr_t msg) override;
      void printLine(ustr_t msg, ustr_t path, int col, int row, ustr_t s) override;

      void showProgress() override;
      void stopProgress() override;
   };
}

#endif