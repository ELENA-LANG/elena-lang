//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Test Declaration Fixture declarations
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TEST_DECLARATION_H
#define TEST_DECLARATION_H

#include "pch.h"
#include "tests_common.h"

namespace elena_lang
{
   // --- DeclarationFixture ---
   class DeclarationFixture : public BaseFixture
   {
   protected:
      ustr_t _src;

      CompilerEnvironment env;

      SyntaxNode sourceNode;

      void SetUp() override;

   public:
      void runTest();
   };

   // --- TemplateArrayFixture ---
   class TemplateArrayFixture : public DeclarationFixture
   {
   protected:
      void SetUp() override;
   };

   // --- NewTemplateArrayFixture ---
   class NewTemplateArrayFixture : public DeclarationFixture
   {
   protected:
      void SetUp() override;
   };
}

#endif
