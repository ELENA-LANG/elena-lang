//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains AARCH64 Assembler implementation
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------------
#include "armassembler.h"
#include "armhelper.h"
#include "asmconst.h"

using namespace elena_lang;

// an alternative DFA tablem where # is separate from the following digits
const char* alt_token_dfa[17] =
{
     ".????????BB??B??????????????????BDFLND??QQDDDDDHEEEEEEEEEEDDDD?D?CCCCCCCCCCCCCCCCCCCCCCCCCCQ?DDC?CCCCCCCCCCCCCCCCCCCCCCCCCCDDDD?",
     "*********BB*********************B***********************************************************************************************",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMAAAAAAAACCCCCCCCCCAAAAAAACCCCCCCCCCCCCCCCCCCCCCCCCCAAAACACCCCCCCCCCCCCCCCCCCCCCCCCCAAAAA",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAADDAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEEEEEEEEEEAAAAAAAKKKKKKAAAAAAAAAAAAAAAAAAAAAAAAAAEEEEEEEJAAAAAAAAAAAAAAAAAAAAAAA",
     "?FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFGFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
     "*IIIIIIIII*II*IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA",
     "????????????????????????????????????????????????KKKKKKKKKK???????KKKKKK?????????????????????????????????J???????????????????????",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACCCCCCCCCCCCCCCCCCCCCCCCCCAAAACACCCCCCCCCCCCCCCCCCCCCCCCCCAAAAA",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMAAAAAAAA???????????A?????MMMMMMMMMMMMMMMMMMMMMMMMMM??????MMMMMMMMMMMMMMMMMMMMMMMMMM?????",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANNNNNNNNNN?A?????NNNNNNNNNNNNNNNNNNNNNNNNNN??????NNNNNNNNNNNNNNNNNNNNNNNNNN?????",
     "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
     "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
     "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
};

// --- Arm64Assembler ---

Arm64Assembler :: Arm64Assembler(int tabSize, UStrReader* reader, ModuleBase* target)
   : AssemblerBase(alt_token_dfa, tabSize, reader, target)
{
   
}

int Arm64Assembler :: readIntArg(ScriptToken& tokenInfo, ref_t& reference, bool skipRead)
{
   if (!skipRead)
      read(tokenInfo);

   int i = 0;
   getIntConstant(tokenInfo, i, reference);
   read(tokenInfo);

   return i;
}

int Arm64Assembler :: readReferenceArg(ScriptToken& tokenInfo, ref_t& reference, ustr_t errorMessage)
{
   read(tokenInfo);

   int i = 0;
   if (getArgReference(tokenInfo, i, reference)) {
      read(tokenInfo);

      return i;
   }
   else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
}

bool Arm64Assembler :: readOperandReference(ScriptToken& tokenInfo, ref_t mask, int& imm, ref_t& reference)
{
   read(tokenInfo);

   if (tokenInfo.compare("%")) {
      read(tokenInfo);

      if (constants.exist(*tokenInfo.token)) {
         reference = constants.get(*tokenInfo.token) | mask;
         imm = 0;

         read(tokenInfo);
      }
      else return false;
   }
   else if (tokenInfo.compare("#")) {
      imm = readIntArg(tokenInfo, reference);
      reference |= mask;
   }
   else return false;

   return true;
}

bool Arm64Assembler :: readOperandExternalReference(ScriptToken& tokenInfo, ref_t mask, int& imm, ref_t& reference)
{
   read(tokenInfo);

   if (tokenInfo.state == dfaQuote) {
      imm = 0;
      reference = _target->mapReference(*tokenInfo.token) | mask;

      read(tokenInfo);
   }
   else if (tokenInfo.compare("#")) {
      imm = readIntArg(tokenInfo, reference);
      reference |= mask;
   }
   else return false;

   return true;
}

//void Arm64Assembler :: readIOperand(ScriptToken& tokenInfo, int& value, ref_t& reference, ustr_t errorMessage)
//{
//   read(tokenInfo);
//
//   if (tokenInfo.compare("code")) {
//      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
//
//      readPtrOperand(tokenInfo, value, reference, mskCodeRef64, errorMessage);
//
//      read(tokenInfo);
//   }
//   else {
//      if (!getIntConstant(tokenInfo, value, reference))
//         throw SyntaxError(errorMessage, tokenInfo.lineInfo);
//
//      read(tokenInfo);
//   }
//}

ARMOperand Arm64Assembler :: defineRegister(ScriptToken& tokenInfo)
{
   if (tokenInfo.compare("x0")) {
      return { ARMOperandType::X0 };
   }
   else if (tokenInfo.compare("x1")) {
      return { ARMOperandType::X1 };
   }
   else if (tokenInfo.compare("x2")) {
      return { ARMOperandType::X2 };
   }
   else if (tokenInfo.compare("x3")) {
      return { ARMOperandType::X3 };
   }
   else if (tokenInfo.compare("x4")) {
      return { ARMOperandType::X4 };
   }
   else if (tokenInfo.compare("x5")) {
      return { ARMOperandType::X5 };
   }
   else if (tokenInfo.compare("x6")) {
      return { ARMOperandType::X6 };
   }
   else if (tokenInfo.compare("x7")) {
      return { ARMOperandType::X7 };
   }
   else if (tokenInfo.compare("x8")) {
      return { ARMOperandType::X8 };
   }
   else if (tokenInfo.compare("x9")) {
      return { ARMOperandType::X9 };
   }
   else if (tokenInfo.compare("x10")) {
      return { ARMOperandType::X10 };
   }
   else if (tokenInfo.compare("x11")) {
      return { ARMOperandType::X11 };
   }
   else if (tokenInfo.compare("x12")) {
      return { ARMOperandType::X12 };
   }
   else if (tokenInfo.compare("x13")) {
      return { ARMOperandType::X13 };
   }
   else if (tokenInfo.compare("x14")) {
      return { ARMOperandType::X14 };
   }
   else if (tokenInfo.compare("x15")) {
      return { ARMOperandType::X15 };
   }
   else if (tokenInfo.compare("x16")) {
      return { ARMOperandType::X16 };
   }
   else if (tokenInfo.compare("x17")) {
      return { ARMOperandType::X17 };
   }
   else if (tokenInfo.compare("x18")) {
      return { ARMOperandType::X18 };
   }
   else if (tokenInfo.compare("x19")) {
      return { ARMOperandType::X19 };
   }
   else if (tokenInfo.compare("x20")) {
      return { ARMOperandType::X20 };
   }
   else if (tokenInfo.compare("x21")) {
      return { ARMOperandType::X21 };
   }
   else if (tokenInfo.compare("x22")) {
      return { ARMOperandType::X22 };
   }
   else if (tokenInfo.compare("x23")) {
      return { ARMOperandType::X23 };
   }
   else if (tokenInfo.compare("x24")) {
      return { ARMOperandType::X24 };
   }
   else if (tokenInfo.compare("x25")) {
      return { ARMOperandType::X25 };
   }
   else if (tokenInfo.compare("x26")) {
      return { ARMOperandType::X26 };
   }
   else if (tokenInfo.compare("x27")) {
      return { ARMOperandType::X27 };
   }
   else if (tokenInfo.compare("x28")) {
      return { ARMOperandType::X28 };
   }
   else if (tokenInfo.compare("x29")) {
      return { ARMOperandType::X29 };
   }
   else if (tokenInfo.compare("x30")) {
      return { ARMOperandType::X30 };
   }
   else if (tokenInfo.compare("w0")) {
      return { ARMOperandType::W0 };
   }
   else if (tokenInfo.compare("w1")) {
      return { ARMOperandType::W1 };
   }
   else if (tokenInfo.compare("w2")) {
      return { ARMOperandType::W2 };
   }
   else if (tokenInfo.compare("w3")) {
      return { ARMOperandType::W3 };
   }
   else if (tokenInfo.compare("w4")) {
      return { ARMOperandType::W4 };
   }
   else if (tokenInfo.compare("w5")) {
      return { ARMOperandType::W5 };
   }
   else if (tokenInfo.compare("w6")) {
      return { ARMOperandType::W6 };
   }
   else if (tokenInfo.compare("w7")) {
      return { ARMOperandType::W7 };
   }
   else if (tokenInfo.compare("w8")) {
      return { ARMOperandType::W8 };
   }
   else if (tokenInfo.compare("w9")) {
      return { ARMOperandType::W9 };
   }
   else if (tokenInfo.compare("w10")) {
      return { ARMOperandType::W10 };
   }
   else if (tokenInfo.compare("w11")) {
      return { ARMOperandType::W11 };
   }
   else if (tokenInfo.compare("w12")) {
      return { ARMOperandType::W12 };
   }
   else if (tokenInfo.compare("w13")) {
      return { ARMOperandType::W13 };
   }
   else if (tokenInfo.compare("w14")) {
      return { ARMOperandType::W14 };
   }
   else if (tokenInfo.compare("w15")) {
      return { ARMOperandType::W15 };
   }
   else if (tokenInfo.compare("w16")) {
      return { ARMOperandType::W16 };
   }
   else if (tokenInfo.compare("w17")) {
      return { ARMOperandType::W17 };
   }
   else if (tokenInfo.compare("w18")) {
      return { ARMOperandType::W18 };
   }
   else if (tokenInfo.compare("w19")) {
      return { ARMOperandType::W19 };
   }
   else if (tokenInfo.compare("w20")) {
      return { ARMOperandType::W20 };
   }
   else if (tokenInfo.compare("w21")) {
      return { ARMOperandType::W21 };
   }
   else if (tokenInfo.compare("w22")) {
      return { ARMOperandType::W22 };
   }
   else if (tokenInfo.compare("w23")) {
      return { ARMOperandType::W23 };
   }
   else if (tokenInfo.compare("w24")) {
      return { ARMOperandType::W24 };
   }
   else if (tokenInfo.compare("w25")) {
      return { ARMOperandType::W25 };
   }
   else if (tokenInfo.compare("w26")) {
      return { ARMOperandType::W26 };
   }
   else if (tokenInfo.compare("w27")) {
      return { ARMOperandType::W27 };
   }
   else if (tokenInfo.compare("w28")) {
      return { ARMOperandType::W28 };
   }
   else if (tokenInfo.compare("w29")) {
      return { ARMOperandType::W29 };
   }
   else if (tokenInfo.compare("w30")) {
      return { ARMOperandType::W30 };
   }
   else if (tokenInfo.compare("sp")) {
      return { ARMOperandType::SP };
   }
   else if (tokenInfo.compare("d0")) {
      return { ARMOperandType::D0 };
   }
   else if (tokenInfo.compare("d1")) {
      return { ARMOperandType::D1 };
   }
   else if (tokenInfo.compare("d2")) {
      return { ARMOperandType::D2 };
   }
   else if (tokenInfo.compare("d3")) {
      return { ARMOperandType::D3 };
   }
   else if (tokenInfo.compare("d4")) {
      return { ARMOperandType::D4 };
   }
   else if (tokenInfo.compare("d5")) {
      return { ARMOperandType::D5 };
   }
   else if (tokenInfo.compare("d6")) {
      return { ARMOperandType::D6 };
   }
   else if (tokenInfo.compare("d7")) {
      return { ARMOperandType::D7 };
   }
   else if (tokenInfo.compare("d8")) {
      return { ARMOperandType::D8 };
   }
   else if (tokenInfo.compare("d9")) {
      return { ARMOperandType::D9 };
   }
   else if (tokenInfo.compare("d10")) {
      return { ARMOperandType::D10 };
   }
   else if (tokenInfo.compare("d11")) {
      return { ARMOperandType::D11 };
   }
   else if (tokenInfo.compare("d12")) {
      return { ARMOperandType::D12 };
   }
   else if (tokenInfo.compare("d13")) {
      return { ARMOperandType::D13 };
   }
   else if (tokenInfo.compare("d14")) {
      return { ARMOperandType::D14 };
   }
   else if (tokenInfo.compare("d15")) {
      return { ARMOperandType::D15 };
   }
   else if (tokenInfo.compare("d16")) {
      return { ARMOperandType::D16 };
   }
   else if (tokenInfo.compare("d17")) {
      return { ARMOperandType::D17 };
   }
   else if (tokenInfo.compare("d18")) {
      return { ARMOperandType::D18 };
   }
   else if (tokenInfo.compare("d19")) {
      return { ARMOperandType::D19 };
   }
   else if (tokenInfo.compare("d20")) {
      return { ARMOperandType::D20 };
   }
   else if (tokenInfo.compare("d21")) {
      return { ARMOperandType::D21 };
   }
   else if (tokenInfo.compare("d22")) {
      return { ARMOperandType::D22 };
   }
   else if (tokenInfo.compare("d23")) {
      return { ARMOperandType::D23 };
   }
   else if (tokenInfo.compare("d24")) {
      return { ARMOperandType::D24 };
   }
   else if (tokenInfo.compare("d25")) {
      return { ARMOperandType::D25 };
   }
   else if (tokenInfo.compare("d26")) {
      return { ARMOperandType::D26 };
   }
   else if (tokenInfo.compare("d27")) {
      return { ARMOperandType::D27 };
   }
   else if (tokenInfo.compare("d28")) {
      return { ARMOperandType::D28 };
   }
   else if (tokenInfo.compare("d29")) {
      return { ARMOperandType::D29 };
   }
   else return { ARMOperandType::None };
}

ARMOperand Arm64Assembler :: readImmArg(ScriptToken& tokenInfo)
{
   ARMOperand operand;

   read(tokenInfo);

   if (tokenInfo.state == dfaQuote) {
      ustr_t val = *tokenInfo.token;
      size_t index = val.find('E');

      int sign = 0;
      if (val[index + 1] == '-') {
         index++;

         sign = 0x80;
      }

      int frac = StrConvertor::toInt(val + index + 1, 10);

      IdentifierString expVal(val, index);
      int exp = StrConvertor::toInt(*expVal, 10) << 4;

      operand.imm = sign | exp | frac;
      operand.type = ARMOperandType::FImm;

      read(tokenInfo);
   }
   else {
      operand.imm = readIntArg(tokenInfo, operand.reference, true);
      operand.type = ARMOperandType::Imm;
   }

   return operand;
}

ARMOperand Arm64Assembler :: readOperand(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   ARMOperand operand;

   read(tokenInfo);
   if (tokenInfo.compare("#")) {
      operand = readImmArg(tokenInfo);
   }
   else {
      operand = defineRegister(tokenInfo);
      if (operand.type == ARMOperandType::None) {
         if (tokenInfo.compare("code")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
         
            if (readOperandReference(tokenInfo, mskCodeRef64, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("data")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskDataRef64, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("rdata")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskRDataRef64, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("rdata_ptr32lo")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskRDataRef32Lo, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("rdata_ptr32hi")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskRDataRef32Hi, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("mdata_ptr32lo")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskMDataRef32Lo, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("mdata_ptr32hi")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskMDataRef32Hi, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("data_ptr32lo")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskDataRef32Lo, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("data_ptr32hi")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskDataRef32Hi, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("code_ptr32lo")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskCodeRef32Lo, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("code_ptr32hi")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskCodeRef32Hi, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("stat_ptr32lo")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskStatDataRef32Lo, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("stat_ptr32hi")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandReference(tokenInfo, mskStatDataRef32Hi, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("import_ptr32lo")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandExternalReference(tokenInfo, mskImportRef32Lo, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (tokenInfo.compare("import_ptr32hi")) {
            read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

            if (readOperandExternalReference(tokenInfo, mskImportRef32Hi, operand.imm, operand.reference)) {
               operand.type = ARMOperandType::Imm;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (getIntConstant(tokenInfo, operand.imm, operand.reference)) {
            operand.type = ARMOperandType::Imm;

            read(tokenInfo);
         }
      }
      else read(tokenInfo);

      if (tokenInfo.compare("+")) {
         if (operand.type != ARMOperandType::Imm)
            throw SyntaxError(errorMessage, tokenInfo.lineInfo);

         read(tokenInfo);

         if (constants.exist(*tokenInfo.token)) {
            operand.imm += constants.get(*tokenInfo.token);
         }
         else throw SyntaxError(errorMessage, tokenInfo.lineInfo);;

         read(tokenInfo);
      }

   }

   return operand;
}

ARMOperand Arm64Assembler :: readPtrOperand(ScriptToken& tokenInfo, ustr_t error)
{
   read(tokenInfo);
   if (!tokenInfo.compare("["))
      throw SyntaxError(ASM_SBRACKET_EXPECTED, tokenInfo.lineInfo);

   ARMOperand ptrOperand = readOperand(tokenInfo, ASM_INVALID_SOURCE);
   if (!ptrOperand.isXR())
      throw SyntaxError(error, tokenInfo.lineInfo);

   bool withImm = false;
   if (tokenInfo.compare(",")) {
      read(tokenInfo);
      if (tokenInfo.compare("#")) {
         ptrOperand.imm = readIntArg(tokenInfo, ptrOperand.reference);
         withImm = true;
      }
      else {
         if (getArgReference(tokenInfo, ptrOperand.imm, ptrOperand.reference)) {
            read(tokenInfo);
            withImm = true;
         }
         else throw SyntaxError(error, tokenInfo.lineInfo);
      }
   }

   if (!tokenInfo.compare("]"))
      throw SyntaxError(ASM_SBRACKETCLOSE_EXPECTED, tokenInfo.lineInfo);

   read(tokenInfo);
   if (tokenInfo.compare("!")) {
      if (withImm) {
         ptrOperand.type = ptrOperand.type | ARMOperandType::Preindex;
      }
      else throw SyntaxError(error, tokenInfo.lineInfo);

      read(tokenInfo);
   }
   else if (!withImm) {
      ptrOperand.type = ptrOperand.type | ARMOperandType::Postindex;
   }
   else ptrOperand.type = ptrOperand.type | ARMOperandType::Unsigned;

   return ptrOperand;
}

ARMOperand Arm64Assembler :: readShiftOperand(ScriptToken& tokenInfo, ustr_t error, int& shift)
{
   ARMOperand operand = { ARMOperandType::Imm };

   ref_t dummy = 0;
   read(tokenInfo);
   if (tokenInfo.compare("lsl")) {
      shift = 0;
   }
   else if (tokenInfo.compare("lsr")) {
      shift = 1;
   }
   else if (tokenInfo.compare("asr")) {
      shift = 2;
   }
   else throw SyntaxError(error, tokenInfo.lineInfo);

   read(tokenInfo, "#", error);
   operand.imm = readIntArg(tokenInfo, dummy);

   return operand;
}

int Arm64Assembler :: readLSL(ScriptToken& tokenInfo, ustr_t error)
{
   ref_t dummy = 0;
   read(tokenInfo, "lsl", error);
   read(tokenInfo, "#", error);
   int lsl = readIntArg(tokenInfo, dummy);

   switch (lsl) {
      case 0:
         return 0;
      case 16:
         return 1;
      case 32:
         return 2;
      case 48:
         return 3;
      default:
         throw SyntaxError(error, tokenInfo.lineInfo);
   }
}

JumpType Arm64Assembler :: readCond(ScriptToken& tokenInfo)
{
   read(tokenInfo);
   if (tokenInfo.compare("eq")) {
      read(tokenInfo);

      return  JumpType::EQ;
   }
   else if (tokenInfo.compare("lt")) {
      read(tokenInfo);

      return  JumpType::LT;
   }
   else if (tokenInfo.compare("gt")) {
      read(tokenInfo);

      return  JumpType::GT;
   }
   else if (tokenInfo.compare("cc")) {
      read(tokenInfo);

      return  JumpType::CC;
   }
   else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);
}

JumpType Arm64Assembler :: invertCond(JumpType type)
{
   int val = (int)type;
   val = val & 0xE;

   if (((int)type & 1) == 0)
      val |= 1;

   return (JumpType)val;
}

void Arm64Assembler :: writeReference(ScriptToken& tokenInfo, ref_t reference, MemoryWriter& writer, ustr_t errorMessage)
{
   switch (reference) {
      case ARG12_1:
      case ARG12_2:
      case NARG12_1:
      case NARG12_2:
      case NARG16_1:
      case NARG16_2:
      case INV_ARG12_1:
      case INV_ARG12_2:
      case ARG16_1:
      case ARG16_2:
      case ARG9_1:
      case ARG32HI_1:
      case ARG32LO_1:
      case NARG16HI_1:
      case NARG16LO_1:
      case NARG16HI_2:
      case INV_NARG16_2:
         writer.Memory()->addReference(reference, writer.position() - 4);
         break;
      case PTR32HI_1:
      case PTR32LO_1:
      case PTR32HI_2:
      case PTR32LO_2:
         writer.Memory()->addReference(reference, writer.position() - 4);
         break;
      default:
         switch (reference & mskAnyRef) {
            case mskRDataRef32Hi:
            case mskRDataRef32Lo:
            case mskRDataDisp32Hi:
            case mskRDataDisp32Lo:
            case mskDataDisp32Hi:
            case mskDataDisp32Lo:
            case mskDataRef32Hi:
            case mskDataRef32Lo:
            case mskCodeRef32Hi:
            case mskCodeRef32Lo:
            case mskCodeDisp32Hi:
            case mskCodeDisp32Lo:
            case mskMDataRef32Hi:
            case mskMDataRef32Lo:
            case mskStatDataRef32Hi:
            case mskStatDataRef32Lo:
            case mskImportRef32Hi:
            case mskImportRef32Lo:
               writer.Memory()->addReference(reference, writer.position() - 4);
               break;
            default:
               throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
   }
}

void Arm64Assembler :: declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   if (labelScope.checkDeclaredLabel(*tokenInfo.token))
      throw SyntaxError(ASM_LABEL_EXISTS, tokenInfo.lineInfo);

   if (!labelScope.declareLabel(*tokenInfo.token, writer))
      throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
}

bool Arm64Assembler :: compileADDShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 0, 0, 0xB, shift, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileANDShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 0, 0, 0xA, shift, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileANDSShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 1, 1, 0xA, shift, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileADDImm(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand imm, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && imm.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeImm12SOpcode(1, 0, 0, 0x22, 0, imm.imm, rn.type, rd.type));

      if (imm.reference)
         writeReference(tokenInfo, imm.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileANDImm(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand ry, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && ry.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeLogicalImm13Opcode(1, 0, 0x24, ry.imm, rn.type, rd.type));

      if (ry.reference)
         writeReference(tokenInfo, ry.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileADRP(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand imm, MemoryWriter& writer)
{
   if (rt.isXR() && imm.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeROpcode(1,0x10,imm.imm, rt.type));

      if (imm.reference)
         writeReference(tokenInfo, imm.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileADR(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand imm, MemoryWriter& writer)
{
   if (rt.isXR() && imm.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeROpcode(0, 0x10, imm.imm, rt.type));

      if (imm.reference)
         writeReference(tokenInfo, imm.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileANDSImm(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand ry, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && ry.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeLogicalImm13Opcode(1, 3, 0x24, ry.imm, rn.type, rd.type));

      if (rn.reference)
         writeReference(tokenInfo, rn.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileB(int imm, MemoryWriter& writer)
{
   ARMLabelHelper::writeB(imm, writer);

   return true;
}

bool Arm64Assembler :: compileBxx(int imm, int cond, MemoryWriter& writer)
{
   writer.writeDWord(ARMHelper::makeBxxOpcode(0x2A, 0, imm >> 2, 0, cond));

   return true;
}

bool Arm64Assembler :: compileBLR(ARMOperand r, MemoryWriter& writer)
{
   if (r.isXR()) {
      writer.writeDWord(ARMHelper::makeBOpcode(0x6B, 0, 0, 1, 0x1F, 0, 0, 0, r.type, ARMOperandType::X0));
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileBR(ARMOperand r, MemoryWriter& writer)
{
   if (r.isXR()) {
      writer.writeDWord(ARMHelper::makeBOpcode(0x6B, 0, 0, 0, 0x1F, 0, 0, 0, r.type, ARMOperandType::X0));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileCSEL(ARMOperand rd, ARMOperand rn, ARMOperand rm, JumpType cond, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeCondOpcode(1, 0, 0xD4, rm.type, (int)cond, 0,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileCSINC(ARMOperand rd, ARMOperand rn, ARMOperand rm, JumpType cond, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeCondOpcode(1, 0, 0xD4, rm.type, (int)cond, 1, 
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileEORShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 1, 0, 0xA, shift, 0, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFABS(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode3Opcode(0, 0, 0x1E, 1, 1, 0, 1, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileFADD(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm, 
   MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR() && rm.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, rm.type, 1, 2, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFCMP(ARMOperand rn, ARMOperand rm, MemoryWriter& writer)
{
   if (rn.isDR() && rm.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, rm.type, 0, 8, rn.type,
         0));
   }
   else if (rn.isDR() && rm.type == ARMOperandType::Imm && rm.imm == 0) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, ARMOperandType::None, 0,
         8, rn.type, 1));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFCVTAS(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRModeOpcode(1, 0, 0x1E, 1, 1, 0, 4,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler ::compileFCVTZS(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRModeOpcode(1, 0, 0x1E, 1, 1, 3, 0, 
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFDIV(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR() && rm.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, rm.type, 0, 6, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFMOV(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.type == ARMOperandType::FImm) {
      writer.writeDWord(ARMHelper::makeFTypeImm8Opcode(0, 0x1E, 1, 1, rn.imm, 3, 0, rd.type));
   }
   else if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, 0, 0x10, rn.type, rd.type));
   }
   else if (rd.isDR() && rn.isXR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcodeX(0, 0x1E, 1, 1, 0, 6, 0, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileFNEG(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, 2, 0x10, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFMUL(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR() && rm.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, rm.type, 0, 2, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFRINTZ(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode2Opcode(0, 0, 0x1E, 1, 1, 1, 3, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFRINTN(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode2Opcode(0, 0, 0x1E, 1, 1, 1, 0, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileFRINTM(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode2Opcode(0, 0, 0x1E, 1, 1, 1, 2, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFRINTX(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode2Opcode(0, 0, 0x1E, 1, 1, 1, 6, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFRINT32Z(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode3Opcode(0, 0, 0x1E, 1, 1, 4, 0, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFRINT64Z(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode3Opcode(0, 0, 0x1E, 1, 1, 4, 2, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFSQRT(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR()) {
      writer.writeDWord(ARMHelper::makeRMode3Opcode(0, 0, 0x1E, 1, 1, 0, 3, 0x10,
         rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFSUB(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR() && rm.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1E, 1, 1, rm.type, 1, 6, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFMADD(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm, ARMOperand ra,
   MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR() && rm.isDR() && ra.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1F, 1, 0, rm.type, 0, ra.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFMSUB(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm, ARMOperand ra,
   MemoryWriter& writer)
{
   if (rd.isDR() && rn.isDR() && rm.isDR() && ra.isDR()) {
      writer.writeDWord(ARMHelper::makeFTypeOpcode(0, 0x1F, 1, 0, rm.type, 1, ra.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileLDP(ARMOperand rt1, ARMOperand rt2, ARMOperand n1, ARMOperand n2, MemoryWriter& writer)
{
   if (rt1.isXR() && rt2.isXR() && n1.isPostindex() && n2.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeOpcode(2, 5, 0, 1, 1, n2.imm >> 3, rt2.type, n1.type, rt1.type));
   }
   else if (rt1.isXR() && rt2.isXR() && n1.isPreindex() && n2.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeOpcode(2, 5, 0, 3, 1, n2.imm >> 3, rt2.type, n1.type, rt1.type));
   }
   else if (rt1.isDR() && rt2.isDR() && n1.isPostindex() && n2.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeOpcode(1, 5, 1, 1, 1, n2.imm >> 3, rt2.type, n1.type, rt1.type));
   }
   else if (rt1.isDR() && rt2.isDR() && n1.isPreindex() && n2.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeOpcode(1, 5, 1, 3, 1, n2.imm >> 3, rt2.type, n1.type, rt1.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileLDR(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isXR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(3, 7, 0, 0, 1, 0, ptr.imm, 3, ptr.type, rt.type));
   }
   else if (rt.isXR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(3, 7, 0, 0, 1, 0, ptr.imm, 1, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isXR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(3, 7, 0, 1, 1, ptr.imm >> 3, ptr.type, rt.type));
   }
   else if (rt.isWR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(2, 7, 0, 1, 1, ptr.imm >> 3, ptr.type, rt.type));
   }
   else if (rt.isWR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(2, 7, 0, 0, 1, 0, ptr.imm, 3, ptr.type, rt.type));
   }
   else if (rt.isWR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(2, 7, 0, 0, 1, 0, ptr.imm, 1, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isDR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(3, 7, 1, 0, 1, 0, ptr.imm, 1, ptr.type, rt.type));
   }
   else if (rt.isDR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(3, 7, 1, 0, 1, 0, ptr.imm, 3, ptr.type, rt.type));
   }
   else if (rt.isDR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(3, 7, 1, 1, 1, ptr.imm >> 3, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileLDRSB(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isXR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(0, 7, 0, 0, 2, 0, ptr.imm, 3, ptr.type, rt.type));
   }
   else if (rt.isXR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(0, 7, 0, 0, 2, 0, ptr.imm, 1, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isXR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(0, 7, 0, 1, 2, ptr.imm >> 3, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileLDRB(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isWR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(0, 7, 0, 0, 1, 0, ptr.imm, 3, ptr.type, rt.type));
   }
   else if (rt.isWR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(0, 7, 0, 0, 1, 0, ptr.imm, 1, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isWR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(0, 7, 0, 1, 1, ptr.imm, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileLDRSW(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isXR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(2, 7, 0, 0, 2, 0, ptr.imm, 3, ptr.type, rt.type));
   }
   else if (rt.isXR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(2, 7, 0, 0, 2, 0, ptr.imm, 1, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isXR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(2, 7, 0, 1, 2, ptr.imm >> 2, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileLDRSH(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isXR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(1, 7, 0, 0, 2, 0, ptr.imm, 3, ptr.type, rt.type));
   }
   else if (rt.isXR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(1, 7, 0, 0, 2, 0, ptr.imm, 1, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isXR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(1, 7, 0, 1, 2, ptr.imm >> 3, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileLSLV(ARMOperand rd, ARMOperand rn, ARMOperand rm, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeOpcode2(1, 0, 0, 0xD6, rm.type, 2, 0, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileLSRV(ARMOperand rd, ARMOperand rn, ARMOperand rm, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeOpcode2(1, 0, 0, 0xD6, rm.type, 2, 1, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileMOVZ(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rn, MemoryWriter& writer)
{
   if (rt.isXR() && rn.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeImm16Opcode(1, 2, 0x25, 0, rn.imm, rt.type));

      if (rn.reference)
         writeReference(tokenInfo, rn.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileMOVN(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rn, MemoryWriter& writer)
{
   if (rt.isXR() && rn.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeImm16Opcode(1, 0, 0x25, 0, rn.imm, rt.type));

      if (rn.reference)
         writeReference(tokenInfo, rn.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileMOVK(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rn, int lsl, MemoryWriter& writer)
{
   if (rt.isXR() && rn.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeImm16Opcode(1, 3, 0x25, lsl, rn.imm, rt.type));

      if (rn.reference)
         writeReference(tokenInfo, rn.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileMADD(ARMOperand rd, ARMOperand rn, ARMOperand rm, ARMOperand ra, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR() && ra.isXR()) {
      writer.writeDWord(ARMHelper::makeOpcode(1, 0, 0x1B, 0, rm.type, 0, ra.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileORRImm(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand ry, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && ry.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeLogicalImm13Opcode(1, 2, 0x9, ry.imm, rn.type, rd.type));

      if (rn.reference)
         writeReference(tokenInfo, rn.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileORRShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 0, 1, 0xA, shift, 0, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileORNShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 0, 1, 0xA, shift, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileRET(ARMOperand r, MemoryWriter& writer)
{
   if (r.isXR()) {
      writer.writeDWord(ARMHelper::makeBOpcode(0x6B, 0, 0, 0x2, 0x1F, 0, 0, 0, r.type, ARMOperandType::X0));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSBFM(ARMOperand rd, ARMOperand rn, int immr, int imms, MemoryWriter& writer)
{
   if (rn.isXR() && rd.isXR()) {
      writer.writeDWord(ARMHelper::makeOpcodeImmRS(1, 0, 0x26, 1, immr, imms, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSCVTF(ARMOperand rd, ARMOperand rn, MemoryWriter& writer)
{
   if (rd.isDR() && rn.isXR()) {
      writer.writeDWord(ARMHelper::makeRModeOpcode(1, 0, 0x1E, 1, 1, 0, 2, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSDIV(ARMOperand rd, ARMOperand rn, ARMOperand rm, MemoryWriter& writer)
{
   if (rm.isXR() && rn.isXR() && rd.isXR()) {
      writer.writeDWord(ARMHelper::makeOpcode(1, 0, 0, 0xD6, rm.type, 1, 1, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileUDIV(ARMOperand rd, ARMOperand rn, ARMOperand rm, MemoryWriter& writer)
{
   if (rm.isXR() && rn.isXR() && rd.isXR()) {
      writer.writeDWord(ARMHelper::makeOpcode(1, 0, 0, 0xD6, rm.type, 1, 0, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSTP(ARMOperand t1, ARMOperand t2, ARMOperand ptr, MemoryWriter& writer)
{
   if (t1.isXR() && t2.isXR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeOpcode(2, 5, 0, 3, 0, ptr.imm >> 3, t2.type, ptr.type, t1.type));
   }
   else if (t1.isXR() && t2.isXR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeOpcode(2, 5, 0, 1, 0, ptr.imm >> 3, t2.type, ptr.type, t1.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSTRB(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isWR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(0, 7, 0, 0, 0, 0, ptr.imm, 3, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isWR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(0, 7, 0, 0, 0, 0, ptr.imm, 1, ptr.type, rt.type));
   }
   else if (rt.isWR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(0, 7, 0, 1, 0, ptr.imm >> 3, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSTRH(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isWR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(1, 7, 0, 0, 0, 0, ptr.imm, 3, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isWR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(1, 7, 0, 0, 0, 0, ptr.imm, 1, ptr.type, rt.type));
   }
   else if (rt.isWR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(1, 7, 0, 1, 0, ptr.imm >> 3, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSTR(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer)
{
   if (rt.isXR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(3, 7, 0, 0, 0, 0, ptr.imm, 3, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isWR() && ptr.isPreindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(2, 7, 0, 0, 0, 0, ptr.imm, 3, ptr.type, rt.type));

      if (ptr.reference)
         writeReference(tokenInfo, ptr.reference, writer, ASM_INVALID_SOURCE);
   }
   else if (rt.isWR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(2, 7, 0, 1, 0, ptr.imm >> 2, ptr.type, rt.type));
   }
   else if (rt.isXR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(3, 7, 0, 0, 0, 0, ptr.imm, 1, ptr.type, rt.type));
   }
   else if (rt.isXR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(3, 7, 0, 1, 0, ptr.imm >> 3, ptr.type, rt.type));
   }
   else if (rt.isWR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(2, 7, 0, 0, 0, 0, ptr.imm, 1, ptr.type, rt.type));
   }
   else if (rt.isDR() && ptr.isPostindex()) {
      writer.writeDWord(ARMHelper::makeImm9Opcode(3, 7, 1, 0, 0, 0, ptr.imm, 1, ptr.type, rt.type));
   }
   else if (rt.isDR() && ptr.isUnsigned()) {
      writer.writeDWord(ARMHelper::makeImm12Opcode(3, 7, 1, 1, 0, ptr.imm >> 3, ptr.type, rt.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSUBImm(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand imm, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && imm.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeImm12SOpcode(1, 1, 0, 0x22, 0, imm.imm, rn.type, rd.type));

      if (imm.reference)
         writeReference(tokenInfo, imm.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSUBShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm,
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 1, 0, 0xB, shift, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSUBS(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rt, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rt.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeImm12SOpcode(1, 1, 1, 0x22, 0, rt.imm, rn.type, rd.type));

      if (rt.reference)
         writeReference(tokenInfo, rt.reference, writer, ASM_INVALID_SOURCE);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileSUBSShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rm, 
   int shift, int amount, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && rm.isXR()) {
      writer.writeDWord(ARMHelper::makeImm6ShiftOpcode(1, 1, 1, 0xB, shift, amount, rm.type, rn.type, rd.type));
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileUBFM(ARMOperand rd, ARMOperand rn, ARMOperand immr, ARMOperand imms, MemoryWriter& writer)
{
   if (rd.isXR() && rn.isXR() && immr.type == ARMOperandType::Imm && imms.type == ARMOperandType::Imm) {
      writer.writeDWord(ARMHelper::makeImmRImmSOpcode(1, 2, 0x26,
         immr.imm, imms.imm, rn.type, rd.type));
   }
   else return false;

   return true;
}

void Arm64Assembler::compileADD(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = false;
   if (tokenInfo.compare(",")) {
      int shift = 0;
      ARMOperand imm = readShiftOperand(tokenInfo, ASM_INVALID_TARGET, shift);
      if (imm.type == ARMOperandType::Imm) {
         isValid = compileADDShifted(tokenInfo, rd, rn, rn2, shift, imm.imm, writer);
      }
   }
   else if (rn2.type == ARMOperandType::Imm) {
      isValid = compileADDImm(tokenInfo, rd, rn, rn2, writer);
   }
   else {
      isValid = compileADDShifted(tokenInfo, rd, rn, rn2, 0, 0, writer);
   }

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileAND(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = false;
   if (rn2.type == ARMOperandType::Imm) {
      isValid = compileANDImm(tokenInfo, rd, rn, rn2, writer);
   }
   else {
      isValid = compileANDShifted(tokenInfo, rd, rn, rn2, 0, 0, writer);
   }

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler::compileADRP(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand imm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   if (!compileADRP(tokenInfo, rd, imm, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler::compileADR(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand imm = { ARMOperandType::Imm };

   read(tokenInfo);
   if (!tokenInfo.compare(":")) {
      // if jump forward
      if (!labelScope.checkDeclaredLabel(*tokenInfo.token)) {
         int label = labelScope.labelNames.count() + 1;
         labelScope.helper->writeLabelAddress(label, writer, mskRelRef32);
         labelScope.labelNames.add(*tokenInfo.token, label);

         compileADR(tokenInfo, rd, imm, writer);
      }
      // if jump backward
      else {
         int offset = labelScope.resolveLabel(*tokenInfo.token, writer);
         if (abs(offset) > 0xFFFF)
            throw SyntaxError(ASM_JUMP_TOO_LONG, tokenInfo.lineInfo);

         imm.imm = offset;

         compileADR(tokenInfo, rd, imm, writer);
      }
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   read(tokenInfo);
}

void Arm64Assembler::compileBLR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileBLR(rt, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler::compileBR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileBR(rt, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileB(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   read(tokenInfo);
   if (!tokenInfo.compare(":")) {
      // if jump forward
      if (!labelScope.checkDeclaredLabel(*tokenInfo.token)) {
         labelScope.registerJump(*tokenInfo.token, writer);

         compileB(0, writer);
      }
      // if jump backward
      else {
         int offset = labelScope.resolveLabel(*tokenInfo.token, writer);
         if (abs(offset) > 0x3FFFF)
            throw SyntaxError(ASM_JUMP_TOO_LONG, tokenInfo.lineInfo);

         compileB(offset, writer);
      }
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   read(tokenInfo);
}

void Arm64Assembler :: compileBxx(ScriptToken& tokenInfo, JumpType cond, MemoryWriter& writer, LabelScope& labelScope)
{
   read(tokenInfo);
   if (!tokenInfo.compare(":")) {
      // if jump forward
      if (!labelScope.checkDeclaredLabel(*tokenInfo.token)) {
         labelScope.registerJump(*tokenInfo.token, writer);

         compileBxx(0, (int)cond, writer);
      }
      // if jump backward
      else {
         int offset = labelScope.resolveLabel(*tokenInfo.token, writer);
         if (abs(offset) > 0x1FFFF)
            throw SyntaxError(ASM_JUMP_TOO_LONG, tokenInfo.lineInfo);

         compileBxx(offset, (int)cond, writer);
      }
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   read(tokenInfo);
}

void Arm64Assembler :: compileCSET(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   JumpType type = readCond(tokenInfo);

   if (!compileCSINC(rn, ARMOperand(ARMOperandType::XZR), ARMOperand(ARMOperandType::XZR), 
      invertCond(type), writer))
   {
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
}

void Arm64Assembler :: compileCSINC(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   JumpType cond = readCond(tokenInfo);

   if (!compileCSINC(rd, rn, rn2, cond, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileCSEL(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   JumpType cond = readCond(tokenInfo);

   if (!compileCSEL(rd, rn, rn2, cond, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileCMP(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if ((rn.isXR() && rt.type == ARMOperandType::Imm)) {
      ARMOperand rd(ARMOperandType::XZR, 0);

      valid = compileSUBS(tokenInfo, rd, rn, rt, writer);
   }
   else if (rn.isXR() && rt.isXR()) {
      ARMOperand rd(ARMOperandType::XZR, 0);

      valid = compileSUBSShifted(tokenInfo, rd, rn, rt, 0, 0, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileEOR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = false;
   isValid = compileEORShifted(tokenInfo, rd, rn, rn2, 0, 0, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFABS(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFABS(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFADD(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileFADD(tokenInfo, rd, rn, rm, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFCMP(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFCMP(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFCVTAS(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.isDR()) {
      valid = compileFCVTAS(rd, rn, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFCVTZS(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.isDR()) {
      valid = compileFCVTZS(rd, rn, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFDIV(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileFDIV(tokenInfo, rd, rn, rm, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFMOV(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFMOV(tokenInfo, rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFMUL(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileFMUL(tokenInfo, rd, rn, rm, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFNEG(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isDR() && rn.isDR()) {
      valid = compileFNEG(tokenInfo, rd, rn, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFRINTZ(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFRINTZ(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFRINTN(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFRINTN(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler::compileFRINTM(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFRINTM(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFRINTX(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFRINTX(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFRINT32Z(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFRINT32Z(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFRINT64Z(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFRINT64Z(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFSQRT(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = compileFSQRT(rd, rn, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFSUB(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileFSUB(tokenInfo, rd, rn, rm, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFMADD(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand ra = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileFMADD(tokenInfo, rd, rn, rm, ra, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileFMSUB(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand ra = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileFMSUB(tokenInfo, rd, rn, rm, ra, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileLDP(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt1 = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rt2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand n1 = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);
   if (n1.isPostindex()) {
      ARMOperand n2 = {};
      if (tokenInfo.compare(",")) {
         n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
      }
      else n2.type = ARMOperandType::Imm;

      if (!compileLDP(rt1, rt2, n1, n2, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (n1.isPreindex()) {
      ARMOperand n2(ARMOperandType::Imm, n1.imm);

      if (!compileLDP(rt1, rt2, n1, n2, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
}

void Arm64Assembler :: compileLDR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }            
      }
      else ptr.imm = 0;

      if (!compileLDR(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileLDR(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileLDRSB(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileLDRSB(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileLDRSB(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileLDRB(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileLDRB(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileLDRB(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileLDRSW(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileLDRSW(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileLDRSW(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileLDRSH(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileLDRSH(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileLDRSH(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileLSL(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.isXR() && rm.type == ARMOperandType::Imm) {
      valid = compileUBFM(rd, rn, 
         ARMOperand(ARMOperandType::Imm, (-rm.imm) % 64),
         ARMOperand(ARMOperandType::Imm, 63 - rm.imm), writer);
   }
   else if (rd.isXR() && rn.isXR() && rm.isXR()) {
      valid = compileLSLV(rd, rn, rm, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileLSR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand ry = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.isXR() && ry.type == ARMOperandType::Imm) {
      valid = compileUBFM(rd, rn, ry, ARMOperand(ARMOperandType::Imm, 63), writer);
   }
   else if (rd.isXR() && rn.isXR() && ry.isXR()) {
      valid = compileLSRV(rd, rn, ry, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileMOV(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if ((rd.type == ARMOperandType::SP && rn.isXR()) || (rd.isXR() && rn.type == ARMOperandType::SP)) {
      valid = compileADDImm(tokenInfo, rd, rn, ARMOperand(ARMOperandType::Imm, 0), writer);
   }
   else if (rd.isXR() && rn.isXR()) {
      valid = compileADDImm(tokenInfo, rd, rn, ARMOperand(ARMOperandType::Imm, 0), writer);
   }
   else if ((rd.isXR() && rn.type == ARMOperandType::Imm)) {
      if (rn.imm < 0) {
         // NOTE : 
         rn.imm = ~rn.imm;
         valid = compileMOVN(tokenInfo, rd, rn, writer);
      }
      else valid = compileMOVZ(tokenInfo, rd, rn, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileMVN(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.isXR()) {
      valid = compileORNShifted(tokenInfo, rd, { ARMOperandType::XZR }, rn, 0, 0, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileMOVN(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.type == ARMOperandType::Imm) {
      valid = compileMOVN(tokenInfo, rd, rn, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileMOVZ(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.type == ARMOperandType::Imm) {
      valid = compileMOVZ(tokenInfo, rd, rn, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileMOVK(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   int lsl = 0;
   if (tokenInfo.compare(",")) {
      lsl = readLSL(tokenInfo, ASM_INVALID_SOURCE);
   }

   bool valid = false;
   if (rd.isXR() && rn.type == ARMOperandType::Imm) {
      valid = compileMOVK(tokenInfo, rd, rn, lsl, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileMUL(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileMADD(rd, rn, rn2, { ARMOperandType::XZR }, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileNEG(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   bool isValid = false;
   if (rd.isXR() && rm.isXR()) {
      isValid = compileSUBShifted(tokenInfo, rd, { ARMOperandType::XZR }, rm, 0, 0, writer);
   }

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler::compileNOP(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeDWord(0xD503201F);

   read(tokenInfo);
}

void Arm64Assembler :: compileORR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = false;
   if (rn2.type == ARMOperandType::Imm) {
      isValid = compileORRImm(tokenInfo, rd, rn, rn2, writer);
   }
   else {
      isValid = compileORRShifted(tokenInfo, rd, rn, rn2, 0, 0, writer);
   }

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSCVTF(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   if (!compileSCVTF(rd, rn, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

}

void Arm64Assembler :: compileSDIV(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileSDIV(rd, rn, rm, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileUDIV(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand rm = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = compileUDIV(rd, rn, rm, writer);

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSTP(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand t1 = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand t2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileSTP(t1, t2, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileSTP(t1, t2, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSTR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileSTR(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileSTR(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSTRB(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileSTRB(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileSTRB(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSTRH(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rt = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand ptr = readPtrOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (ptr.isPostindex()) {
      if (tokenInfo.compare(",")) {
         ARMOperand n2 = readOperand(tokenInfo, ASM_INVALID_SOURCE);
         if (n2.type == ARMOperandType::Imm) {
            ptr.imm = n2.imm;
            ptr.reference = n2.reference;
         }
      }
      else ptr.imm = 0;

      if (!compileSTRH(tokenInfo, rt, ptr, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   }
   else if (!compileSTRH(tokenInfo, rt, ptr, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileRET(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand r = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   if(!compileRET(r, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSUB(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn2 = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool isValid = false;
   /*if (tokenInfo.compare(",")) {

   }
   else*/ if (rn2.type == ARMOperandType::Imm) {
      isValid = compileSUBImm(tokenInfo, rd, rn, rn2, writer);
   }
   else {
      isValid = compileSUBShifted(tokenInfo, rd, rn, rn2, 0, 0, writer);
   }

   if (!isValid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSXTH(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.isXR()) {
      valid = compileSBFM(rd, rn, 0, 15, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileSXTW(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if (rd.isXR() && rn.isWR()) {
      rn.type = (ARMOperandType)((int)ARMOperandType::X0 | ((int)rn.type & 0x1F));

      valid = compileSBFM(rd, rn, 0, 31, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileTST(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ARMOperand rd = readOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   ARMOperand rn = readOperand(tokenInfo, ASM_INVALID_TARGET);

   bool valid = false;
   if ((rd.isXR() && rn.type == ARMOperandType::Imm)) {
      valid = compileANDSImm(tokenInfo, ARMOperand(ARMOperandType::XZR, 0), rd, rn, writer);
   }
   else {
      valid = compileANDSShifted(tokenInfo, ARMOperand(ARMOperandType::XZR, 0), rd, rn, 0, 0, writer);
   }

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

bool Arm64Assembler :: compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   if (tokenInfo.compare("add")) {
      compileADD(tokenInfo, writer);
   }
   else if (tokenInfo.compare("and")) {
      compileAND(tokenInfo, writer);
   }
   else if (tokenInfo.compare("adr")) {
      compileADR(tokenInfo, writer, labelScope);
   }
   else if (tokenInfo.compare("adrp")) {
      compileADRP(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileBOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   if (tokenInfo.compare("blr")) {
      compileBLR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("br")) {
      compileBR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("beq")) {
      compileBxx(tokenInfo, JumpType::EQ, writer, labelScope);
   }
   else if (tokenInfo.compare("bge")) {
      compileBxx(tokenInfo, JumpType::GE, writer, labelScope);
   }
   else if (tokenInfo.compare("bgt")) {
      compileBxx(tokenInfo, JumpType::GT, writer, labelScope);
   }
   else if (tokenInfo.compare("ble")) {
      compileBxx(tokenInfo, JumpType::LE, writer, labelScope);
   }
   else if (tokenInfo.compare("blt")) {
      compileBxx(tokenInfo, JumpType::LT, writer, labelScope);
   }
   else if (tokenInfo.compare("bne")) {
      compileBxx(tokenInfo, JumpType::NE, writer, labelScope);
   }
   else if (tokenInfo.compare("b")) {
      compileB(tokenInfo, writer, labelScope);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   if (tokenInfo.compare("cset")) {
      compileCSET(tokenInfo, writer);
   }
   else if (tokenInfo.compare("csinc")) {
      compileCSINC(tokenInfo, writer);
   }
   else if (tokenInfo.compare("csel")) {
      compileCSEL(tokenInfo, writer);
   }
   else if (tokenInfo.compare("cmp")) {
      compileCMP(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileDOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool Arm64Assembler :: compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("eor")) {
      compileEOR(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileFOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("fabs")) {
      compileFABS(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fadd")) {
      compileFADD(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fcmp")) {
      compileFCMP(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fcvtas")) {
      compileFCVTAS(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fcvtzs")) {
      compileFCVTZS(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fdiv")) {
      compileFDIV(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fmadd")) {
      compileFMADD(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fmov")) {
      compileFMOV(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fmsub")) {
      compileFMSUB(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fmul")) {
      compileFMUL(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fneg")) {
      compileFNEG(tokenInfo, writer);
   }
   else if (tokenInfo.compare("frintn")) {
      compileFRINTN(tokenInfo, writer);
   }
   else if (tokenInfo.compare("frintm")) {
      compileFRINTM(tokenInfo, writer);
   }
   else if (tokenInfo.compare("frintz")) {
      compileFRINTZ(tokenInfo, writer);
   }
   else if (tokenInfo.compare("frintx")) {
      compileFRINTX(tokenInfo, writer);
   }
   else if (tokenInfo.compare("frint32z")) {
      compileFRINT32Z(tokenInfo, writer);
   }
   else if (tokenInfo.compare("frint64z")) {
      compileFRINT64Z(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fsqrt")) {
      compileFSQRT(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fsub")) {
      compileFSUB(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileIOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool Arm64Assembler :: compileJOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& scope)
{
   return false;
}

bool Arm64Assembler :: compileLOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   if (tokenInfo.compare("ldp")) {
      compileLDP(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ldr")) {
      compileLDR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ldrsw")) {
      compileLDRSW(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ldrsb")) {
      compileLDRSB(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ldrb")) {
      compileLDRB(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ldrsh")) {
      compileLDRSH(tokenInfo, writer);
   }
   else if (tokenInfo.compare("lsr")) {
      compileLSR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("lsl")) {
      compileLSL(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("mov")) {
      compileMOV(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movz")) {
      compileMOVZ(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movk")) {
      compileMOVK(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movn")) {
      compileMOVN(tokenInfo, writer);
   }
   else if (tokenInfo.compare("mul")) {
      compileMUL(tokenInfo, writer);
   }
   else if (tokenInfo.compare("mvn")) {
      compileMVN(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileNOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("neg")) {
      compileNEG(tokenInfo, writer);
   }
   else if (tokenInfo.compare("nop")) {
      compileNOP(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileOOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("orr")) {
      compileORR(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler::compilePOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool Arm64Assembler::compileROpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("ret")) {
      compileRET(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler::compileSOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("scvtf")) {
      compileSCVTF(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sdiv")) {
      compileSDIV(tokenInfo, writer);
   }
   else if (tokenInfo.compare("stp")) {
      compileSTP(tokenInfo, writer);
   }
   else if (tokenInfo.compare("str")) {
      compileSTR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("strb")) {
      compileSTRB(tokenInfo, writer);
   }
   else if (tokenInfo.compare("strh")) {
      compileSTRH(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sub")) {
      compileSUB(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sxth")) {
      compileSXTH(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sxtw")) {
      compileSXTW(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileTOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("tst")) {
      compileTST(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileUOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("udiv")) {
      compileUDIV(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool Arm64Assembler :: compileXOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo&)
{
   return false;
}

void Arm64Assembler::compileDBField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ref_t  reference = 0;
   int d = readIntArg(tokenInfo, reference);
   if (reference)
      throw SyntaxError(ASM_INVALID_SOURCE, tokenInfo.lineInfo);

   writer.writeByte(d);
}

void Arm64Assembler::compileDWField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ref_t  reference = 0;
   int d = readIntArg(tokenInfo, reference);
   if (reference)
      throw SyntaxError(ASM_INVALID_SOURCE, tokenInfo.lineInfo);

   writer.writeWord(d);
}

void Arm64Assembler::compileDDField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ref_t  reference = 0;
   int d = readIntArg(tokenInfo, reference);
   if (reference)
      throw SyntaxError(ASM_INVALID_SOURCE, tokenInfo.lineInfo);

   writer.writeDWord(d);
}

void Arm64Assembler::compileDQField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   ref_t  reference = 0;
   int d = 0;

   auto operand = readOperand(tokenInfo, ASM_INVALID_SOURCE);
   if (operand.type == ARMOperandType::Imm) {
      d = operand.imm;
      reference = operand.reference;
   }
   else throw SyntaxError(ASM_INVALID_SOURCE, tokenInfo.lineInfo);

   if (reference) {
      writer.writeQReference(reference, d);
   }
   else writer.writeQWord(d);
}

void Arm64Assembler :: compileDoubleField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   if (tokenInfo.state == dfaQuote) {
      double val = StrConvertor::toDouble(*tokenInfo.token);

      writer.writeDouble(val);

      read(tokenInfo);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void Arm64Assembler :: compileProcedure(ScriptToken& tokenInfo)
{
   ARMLabelHelper helper;

   AssemblerBase::compileProcedure(tokenInfo, &helper);
}
