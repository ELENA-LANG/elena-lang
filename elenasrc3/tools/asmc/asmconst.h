//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the assembly compiler common constants
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CLICONST
#define CLICONST

namespace elena_lang
{

   #define ASM_REVISION_NUMBER               0x0068

   constexpr auto N_ARGUMENT1                = "__n_1";
   constexpr auto N_ARGUMENT2                = "__n_2";
   constexpr auto N16_ARGUMENT1              = "__n16_1";
   constexpr auto N16_ARGUMENT2              = "__n16_2";
   constexpr auto N12_ARGUMENT1              = "__n12_1";
   constexpr auto N12_ARGUMENT2              = "__n12_2";
   constexpr auto N16HI_ARGUMENT1            = "__n16hi_1";
   constexpr auto WORD_ARGUMENT1             = "__arg16_1";
   constexpr auto WORD_ARGUMENT2             = "__arg16_2";
   constexpr auto IMM9_ARGUMENT1             = "__arg9_1";
   constexpr auto IMM12_ARGUMENT1            = "__arg12_1";
   constexpr auto IMM12_ARGUMENT2            = "__arg12_2";
   constexpr auto DWORD_ARGUMENT1            = "__arg32_1";
   constexpr auto DWORDHI_ARGUMENT1          = "__arg32hi_1";
   constexpr auto DWORDLO_ARGUMENT1          = "__arg32lo_1";
   constexpr auto DWORD_ARGUMENT2            = "__arg32_2";
   constexpr auto PTR32_ARGUMENT1            = "__ptr32_1";
   constexpr auto PTR32_ARGUMENT2            = "__ptr32_2";
   constexpr auto RELPTR32_ARGUMENT1         = "__relptr32_1";
   constexpr auto RELPTR32_ARGUMENT2         = "__relptr32_2";
   constexpr auto DISP32HI_ARGUMENT1         = "__disp32hi_1";
   constexpr auto DISP32HI_ARGUMENT2         = "__disp32hi_2";
   constexpr auto DISP32LO_ARGUMENT1         = "__disp32lo_1";
   constexpr auto DISP32LO_ARGUMENT2         = "__disp32lo_2";
   constexpr auto PTR32HI_ARGUMENT1          = "__ptr32hi_1";
   constexpr auto PTR32LO_ARGUMENT1          = "__ptr32lo_1";
   constexpr auto PTR32HI_ARGUMENT2          = "__ptr32hi_2";
   constexpr auto PTR32LO_ARGUMENT2          = "__ptr32lo_2";
   constexpr auto PTR64_ARGUMENT1            = "__ptr64_1";
   constexpr auto PTR64_ARGUMENT2            = "__ptr64_2";
   constexpr auto RDATA32_ARGUMENT1          = "rdata32";
   constexpr auto RDATA64_ARGUMENT1          = "rdata64";
   constexpr auto QWORD_ARGUMENT2            = "__arg64_2";

   constexpr auto ASM_GREETING               = "ELENA Assembler Compiler %d.%d.%d (C)2011-2022 by Alexei Rakov\n";
   constexpr auto ASM_HELP                   = "asmc-cli [-amd64 | -x86] <file> <output path>\n";

   constexpr auto ASM_COMPILE_X86            = "X86 Assembler : compiling %s\n";
   constexpr auto ASM_COMPILE_X86_64         = "X86-64 Assembler : compiling %s\n";
   constexpr auto ASM_COMPILE_PPC64le        = "PPC64le Assembler : compiling %s\n";
   constexpr auto ASM_COMPILE_ARM64          = "AArch64 Assembler : compiling %s\n";
   constexpr auto BC_COMPILE_32              = "32bit Byte-code Compiler : compiling %s\n";
   constexpr auto BC_COMPILE_64              = "64bit Byte-code Compiler : compiling %s\n";
   constexpr auto ASM_DONE                   = "Successfully compiled\n";

   constexpr auto ASM_X86_MODE               = "x86";
   constexpr auto ASM_AMD64_MODE             = "amd64";
   constexpr auto ASM_PPC64le_MODE           = "ppc64le";
   constexpr auto ASM_ARM64_MODE             = "arm64";
   constexpr auto BC_32_MODE                 = "bc32";
   constexpr auto BC_64_MODE                 = "bc64";

   constexpr auto ASM_SYNTAXERROR            = "(%d,%d): Syntax error\n";
   constexpr auto ASM_INVALID_SOURCE         = "(%d,%d): Invalid source operand\n";
   constexpr auto ASM_INVALID_TARGET         = "(%d,%d): Invalid target operand\n";
   constexpr auto ASM_INVALID_DESTINATION    = "(%d,%d): Invalid destination operand\n";
   constexpr auto ASM_COMMA_EXPECTED         = "(%d,%d): Comma expected\n";
   constexpr auto ASM_INVALID_COMMAND        = "(%d,%d): Invalid command\n";
   constexpr auto ASM_DOUBLECOLON_EXPECTED   = "(%d,%d): ':' expected\n";
   constexpr auto ASM_SEMICOLON_EXPECTED     = "(%d,%d): ';' expected\n";
   constexpr auto ASM_SBRACKET_EXPECTED      = "(%d,%d): '[' expected\n";
   constexpr auto ASM_SBRACKETCLOSE_EXPECTED  = "(%d,%d): ']' expected\n";
   constexpr auto ASM_BRACKET_EXPECTED       = "(%d,%d): '(' expected\n";
   constexpr auto ASM_BRACKETCLOSE_EXPECTED  = "(%d,%d): ')' expected\n";
   constexpr auto ASM_INVALIDNUMBER          = "(%d,%d): Invalid number\n";
   constexpr auto ASM_PTR_EXPECTED           = "(%d,%d): 'ptr' expected\n";
   constexpr auto ASM_INVALID_CALL_LABEL     = "(%d,%d): 'Invalid call label\n";
   constexpr auto ASM_DUPLICATECONST         = "(%d,%d): Constant already exists\n";
   constexpr auto ASM_PROCEDURE_EXIST        = "(%d,%d): Procedure already exists\n";
   constexpr auto ASM_DUPLICATE_ARG          = "(%d,%d): Duplicate argument \n";
   constexpr auto ASM_JUMP_TOO_LONG          = "(%d,%d): Jump too long\n";
   constexpr auto INVALID_CALL_TARGET        = "(%d,%d): Invalid call target\n";
   constexpr auto ASM_LABEL_EXISTS           = "Label with such a name already exists (%d)\n";

   constexpr auto ASM_CANNOTCREATE_OUTPUT    = "Cannot create an output file\n";
   constexpr auto ASM_CANNOTOPEN_INPUT       = "Cannot open %s an input file\n";

}

#endif
