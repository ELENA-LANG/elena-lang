//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA AMD64Assembler
//		classes.
//                             (C)2005-2021, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "amd64assembler.h"
#include "module.h"

#include <float.h>

using namespace _ELENA_;

//const int gcPageSize       = 0x0010;      // !! temporal
//
//void x86Assembler :: loadDefaultConstants()
//{
////   constants.add(_EL_MINIMAL_SIZE, gcPageSize * 0x10);
////   constants.add(_GC_PAGE_MASK, ~(gcPageSize - 1));
////   constants.add(_GC_PAGE_LOG, logth(gcPageSize));
//}

int AMD64Assembler :: readStReg(TokenInfo& token)
{
   token.read("(", "'(' expected (%d)\n");
   int index = token.readInteger(constants);
   token.read(")", "')' expected (%d)\n");

   return index;
}

AMD64Assembler::Operand AMD64Assembler :: defineRegister(TokenInfo& token)
{
   if (token.check("eax")) {
      return AMD64Helper::otEAX;
   }
   else if (token.check("ecx")) {
      return AMD64Helper::otECX;
   }
   else if (token.check("ebx")) {
      return AMD64Helper::otEBX;
   }
   else if (token.check("esi")) {
      return AMD64Helper::otESI;
   }
   else if (token.check("edi")) {
      return AMD64Helper::otEDI;
   }
   else if (token.check("edx")) {
      return Operand(AMD64Helper::otEDX);
   }
   else if (token.check("al")) {
      return Operand(AMD64Helper::otAL);
   }
   else if (token.check("bl")) {
      return Operand(AMD64Helper::otBL);
   }
   else if (token.check("cl")) {
		return Operand(AMD64Helper::otCL);
   }
   else if (token.check("dl")) {
      return Operand(AMD64Helper::otDL);
   }
   else if (token.check("dh")) {
      return Operand(AMD64Helper::otDH);
   }
	else if (token.check("ah")) {
      return Operand(AMD64Helper::otAH);
	}
   else if (token.check("bh")) {
      return Operand(AMD64Helper::otBH);
   }
   else if (token.check("ax")) {
      return Operand(AMD64Helper::otAX);
   }
   else if (token.check("bx")) {
      return Operand(AMD64Helper::otBX);
   }
   else if (token.check("cx")) {
      return Operand(AMD64Helper::otCX);
   }
   else if (token.check("dx")) {
      return Operand(AMD64Helper::otDX);
   }
   // SSE registers
   else if (token.check("xmm0")) {
      return Operand(AMD64Helper::otXMM0);
   }
   else if (token.check("xmm1")) {
      return Operand(AMD64Helper::otXMM1);
   }
//	else if (token.check("xmm2")) {
//      return Operand(x86Helper::otXMM2);
//	}
//	else if (token.check("xmm3")) {
//		return Operand(x86Helper::otXMM3);
//	}
//	else if (token.check("xmm4")) {
//		return Operand(x86Helper::otXMM4);
//	}
//	else if (token.check("xmm5")) {
//		return Operand(x86Helper::otXMM5);
//	}
//	else if (token.check("xmm6")) {
//		return Operand(x86Helper::otXMM6);
//	}
//	else if (token.check("xmm7")) {
//		return Operand(x86Helper::otXMM7);
//	}
//   else if (token.check("mm0")) {
//      return Operand(x86Helper::otMM0);
//   }
//   else if (token.check("mm1")) {
//      return Operand(x86Helper::otMM1);
//   }
//   else if (token.check("mm2")) {
//      return Operand(x86Helper::otMM2);
//   }
//   else if (token.check("mm3")) {
//      return Operand(x86Helper::otMM3);
//   }
//   else if (token.check("mm4")) {
//      return Operand(x86Helper::otMM4);
//   }
//   else if (token.check("mm5")) {
//      return Operand(x86Helper::otMM5);
//   }
//   else if (token.check("mm6")) {
//      return Operand(x86Helper::otMM6);
//   }
//   else if (token.check("mm7")) {
//      return Operand(x86Helper::otMM7);
//   }
   else if (token.check("rax")) {
      return Operand(AMD64Helper::otRAX);
   }
   else if (token.check("rcx")) {
      return Operand(AMD64Helper::otRCX);
   }
   else if (token.check("rbx")) {
      return Operand(AMD64Helper::otRBX);
   }
   else if (token.check("rdx")) {
      return Operand(AMD64Helper::otRDX);
   }
   else if (token.check("rsp")) {
      return Operand(AMD64Helper::otRSP);
   }
   else if (token.check("rbp")) {
      return Operand(AMD64Helper::otRBP);
   }
   else if (token.check("rsi")) {
      return Operand(AMD64Helper::otRSI);
   }
   else if (token.check("rdi")) {
      return Operand(AMD64Helper::otRDI);
   }
   else if (token.check("r8")) {
      return Operand(AMD64Helper::otRX8);
   }
   else if (token.check("r9")) {
      return Operand(AMD64Helper::otRX9);
   }
   else if (token.check("r10")) {
      return Operand(AMD64Helper::otRX10);
   }
   else if (token.check("r11")) {
      return Operand(AMD64Helper::otRX11);
   }
   else if (token.check("r12")) {
      return Operand(AMD64Helper::otRX12);
   }
   else if (token.check("r13")) {
      return Operand(AMD64Helper::otRX13);
   }
   else if (token.check("r14")) {
      return Operand(AMD64Helper::otRX14);
   }
   else if (token.check("r15")) {
      return Operand(AMD64Helper::otRX15);
   }
   else return Operand(AMD64Helper::otUnknown);
}

AMD64Assembler::Operand AMD64Assembler :: defineOperand(TokenInfo& token, ProcedureInfo& info, const char* err)
{
   Operand operand = defineRegister(token);
   if (operand.type == AMD64Helper::otUnknown) {
      if (token.getInteger(operand.offset, constants)) {
         setOffsetSize(operand);
         // !! HOTFIX: 000000080 constant should be considered as int32 rather then int8
         if (getlength(token.value)==8 && operand.type == AMD64Helper::otDB) {
            operand.type = AMD64Helper::otDD;
         }
      }
      else if (token.check("data")) {
         token.read(":", err);
         token.read();
         if (token.check("%")) {
            operand.type = AMD64Helper::otDD;
            operand.reference = token.readInteger(constants) | mskPreloadDataRef;
            operand.offset = 0x0;
         }
         else {
            IdentifierString structRef(token.terminal.line + 1, token.terminal.length-2);

            operand.type = AMD64Helper::otDD;
            operand.reference = info.binary->mapReference(structRef) | mskNativeDataRef;
         }
      }
      else if (token.check("rdata")) {
         token.read(":", err);
         token.read();
         if (token.check("%")) {
            operand.type = AMD64Helper::otDQ;
            operand.reference = token.readInteger(constants) | mskPreloadDataRef;
            operand.offset = 0x0;
         }
         else {
            IdentifierString structRef(token.terminal.line + 1, token.terminal.length - 2);

            operand.type = AMD64Helper::otDD;
            operand.reference = info.binary->mapReference(structRef) | mskNativeRDataRef;
         }
      }
	   else if (token.check("stat")) {
         token.read(":", err);
         token.read();
         IdentifierString structRef(token.terminal.line + 1, token.terminal.length-2);

         operand.type = AMD64Helper::otDD;
         operand.reference = info.binary->mapReference(structRef) | mskStatSymbolRef;
      }
	   else if (token.check("const")) {
         token.read(":", err);
         token.read();
         operand.type = AMD64Helper::otDD;

         IdentifierString constRef(token.terminal.line + 1, token.terminal.length-2);
         operand.reference = info.binary->mapReference(constRef) | mskConstantRef;
         operand.offset = 0;
      }
      else if (token.check("code")) {
         token.read(":", err);
         token.read();

         if (token.check("%")) {
            operand.type = AMD64Helper::otDD;
            operand.reference = token.readInteger(constants) | mskPreloadCodeRef;
            operand.offset = 0x0;
         }
         else {
            IdentifierString structRef(token.terminal.line + 1, token.terminal.length - 2);

            operand.type = AMD64Helper::otDD;
            operand.reference = info.binary->mapReference(structRef) | mskNativeCodeRef;
         }
      }
      else if (token.check(ARGUMENT1)) {
         operand.type = AMD64Helper::otDD;
         operand.reference = -1;
      }
      else if (token.check(ARGUMENT2)) {
         operand.type = AMD64Helper::otDD;
         operand.reference = -2;
      }
      else if (token.check(ARGUMENT3)) {
         operand.type = AMD64Helper::otDD;
         operand.reference = -3;
      }
      else if (info.parameters.exist(token.value)) {
         if (info.inlineMode) {
            operand.type = AMD64Helper::addPrefix(AMD64Helper::otRBP, AMD64Helper::otM32disp32);
         }
         else operand.type = AMD64Helper::addPrefix(AMD64Helper::otRSP, AMD64Helper::otM32disp32);

         operand.offset = (info.parameters.Count() - info.parameters.get(token.value))*4;
      }
      else {
         token.raiseErr(err);

         return Operand();
      }
   }
   return operand;
}

bool AMD64Assembler :: setOffset(Operand& operand, Operand disp)
{
   if (disp.reference==0) {
      operand.offset += disp.offset;
   }
   else if (operand.reference==0) {
      operand.offset += disp.offset;
      operand.reference = disp.reference;
   }
   else return false;

   if (disp.type==AMD64Helper::otDB) {
      if (test(operand.type, AMD64Helper::otM16)) {
         operand.type = (OperandType)(operand.type | AMD64Helper::otM16disp8);
      }
      else if (test(operand.type, AMD64Helper::otM8)) {
         operand.type = (OperandType)(operand.type | AMD64Helper::otM8disp8);
      }
      else if (test(operand.type, AMD64Helper::otM32)) {
         operand.type = (OperandType)(operand.type | AMD64Helper::otM32disp8);
      }
      else operand.type = (OperandType)(operand.type | AMD64Helper::otM64disp8);

      return true;
   }
   else if (disp.type==AMD64Helper::otDD) {
      if (test(operand.type, AMD64Helper::otM32)) {
         operand.type = (OperandType)(operand.type | AMD64Helper::otM32disp32);
      }
      else operand.type = (OperandType)(operand.type | AMD64Helper::otM64disp32);

      return true;
   }
   return false;
}

AMD64Assembler::Operand AMD64Assembler :: defineDisplacement(TokenInfo& token, ProcedureInfo& info, const char* err)
{
   if (token.check("+")) {
      token.read();

      return defineOperand(token, info, err);
   }
   else if (token.check("-")) {
      token.read();

      Operand disp = defineOperand(token, info, err);
      if (disp.type == AMD64Helper::otDD || disp.type == AMD64Helper::otDB) {
         disp.offset = -disp.offset;

         return disp;
      }
      else token.raiseErr(err);
   }
   else if (token.value[0]=='-' && (token.terminal.state==dfaInteger || token.terminal.state==dfaHexInteger)) {
      Operand disp = defineOperand(token, info, err);
      if (disp.type == AMD64Helper::otDD || disp.type == AMD64Helper::otDB) {
         return disp;
      }
      else token.raiseErr(err);
   }
   else if (token.check("*")) {
      token.read();
      if(token.check("4")) {
         return Operand(AMD64Helper::otFactor4);
      }
      else if(token.check("8")) {
         return Operand(AMD64Helper::otFactor8);
      }
      else if(token.check("2")) {
         return Operand(AMD64Helper::otFactor2);
      }
      else token.raiseErr(err);
   }

   return Operand();
}

AMD64Assembler::Operand AMD64Assembler:: readDispOperand(TokenInfo& token, ProcedureInfo& info, const char* err, OperandType prefix)
{
   Operand operand = defineOperand(token, info, err);
   if (operand.type != AMD64Helper::otUnknown) {
      operand.type = AMD64Helper::addPrefix(operand.type, prefix);

      if (operand.type == AMD64Helper::otDisp64 && !operand.ebpReg) {
         token.read();
         Operand disp = defineDisplacement(token, info, err);
         if (disp.type == AMD64Helper::otDD || disp.type == AMD64Helper::otDB) {
	         if (disp.reference==0) {
		         operand.offset += disp.offset;
	         }
	         else if (operand.reference==0) {
		         operand.offset += disp.offset;
		         operand.reference = disp.reference;
	         }
	         else token.raiseErr(err);

            token.read();
         }
         else if (disp.type != AMD64Helper::otUnknown)
            token.raiseErr(err);

         // if it is [disp]
         return operand;
      }
      else if (prefix == AMD64Helper::otM32 && operand.type == AMD64Helper::otDisp32 && !operand.ebpReg) {
         token.read();
         Operand disp = defineDisplacement(token, info, err);
         if (disp.type == AMD64Helper::otDD || disp.type == AMD64Helper::otDB) {
            if (disp.reference == 0) {
               operand.offset += disp.offset;
            }
            else if (operand.reference == 0) {
               operand.offset += disp.offset;
               operand.reference = disp.reference;
            }
            else token.raiseErr(err);

            token.read();
         }
         else if (disp.type != AMD64Helper::otUnknown)
            token.raiseErr(err);

         // if it is [disp]
         return operand;
      }
      else {
         token.read();

         Operand disp = defineDisplacement(token, info, err);

         if (disp.type == AMD64Helper::otDD || disp.type == AMD64Helper::otDB) {
            // if it is [r + disp]
            if(!setOffset(operand, disp))
               token.raiseErr(err);

            token.read();
         }
         else if (disp.type == AMD64Helper::otFactor2 || disp.type == AMD64Helper::otFactor4 || disp.type == AMD64Helper::otFactor8) {
            token.read();

            Operand disp2 = defineDisplacement(token, info, err);
            if (disp2.type == AMD64Helper::otDD || disp2.type == AMD64Helper::otDB) {
               // if it is [r*factor + disp]
               int sibcode = ((char)AMD64Helper::otDisp32 + ((char)operand.type << 3)) << 24;

               operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | AMD64Helper::otSIB | disp.type | sibcode);
               operand.offset = disp2.offset;
               operand.reference = disp2.reference;
               operand.factorReg = true;

               token.read();
            }
            else if (disp2.type == AMD64Helper::otUnknown) {
               // if it is [r*factor]
               int sibcode = ((char)AMD64Helper::otDisp32 + ((char)operand.type << 3)) << 24;

               operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | AMD64Helper::otSIB | disp.type | sibcode);
               operand.offset = 0;
               operand.reference = 0;
               operand.factorReg = true;
            }
            else token.raiseErr(err);
         }
         else if (disp.type != AMD64Helper::otUnknown) {
            token.read();

            Operand disp2 = defineDisplacement(token, info, err);
            if (disp2.type == AMD64Helper::otDD || disp2.type == AMD64Helper::otDB) {
               // if it is [r + r + disp]
               int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

               if(!setOffset(operand, disp2))
                  token.raiseErr(err);

               operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | AMD64Helper::otSIB | sibcode);

               token.read();
            }
            else if (disp2.type == AMD64Helper::otFactor2 || disp2.type == AMD64Helper::otFactor4 || disp2.type == AMD64Helper::otFactor8) {
               token.read();

               Operand disp3 = defineDisplacement(token, info, err);
               if (disp3.type == AMD64Helper::otDD || disp3.type == AMD64Helper::otDB) {
                  // if it is [r + r*factor + disp]
                  int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

                  if(!setOffset(operand, disp3))
                     token.raiseErr(err);

                  operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | AMD64Helper::otSIB | sibcode | disp2.type);

                  token.read();
               }
               else if (disp3.type == AMD64Helper::otUnknown) {
                  // if it is [r + r*factor]
				      int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

				      operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | AMD64Helper::otSIB | sibcode | disp2.type);
               }
               else token.raiseErr(err);
            }
            else if (disp2.type == AMD64Helper::otUnknown) {
               // if it is [r + r]
				   int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

				   operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | AMD64Helper::otSIB | sibcode);
            }
            else token.raiseErr(err);
         }
         else {
            // if it is register disp [r]
            if (operand.ebpReg && operand.type == AMD64Helper::otDisp64) {
               operand.type = (OperandType)(operand.type | AMD64Helper::otM64disp8);
	            operand.offset = 0;
	         }
            return operand;
         }
      }
   }
   return operand;
}

AMD64Assembler::Operand AMD64Assembler :: readPtrOperand(TokenInfo& token, ProcedureInfo& info, const char* err, OperandType prefix)
{
   token.read();
   if (token.check("ptr"))
      token.read();

   if (token.check("[")) {
      token.read();
      Operand operand = readDispOperand(token, info, "Invalid expression (%d)", prefix);
      if(!token.check("]")) {
         token.raiseErr("']' expected (%d)\n");
      }
      else token.read();

      return operand;
   }
   else {
      Operand operand = defineOperand(token, info, err);
      if (operand.type == AMD64Helper::otDD && operand.reference == 0 || operand.type== AMD64Helper::otDB) {
         if (prefix== AMD64Helper::otM16) {
            operand.type = AMD64Helper::otDW;
         }
         else if (prefix== AMD64Helper::otM8) {
            operand.type = AMD64Helper::otDB;
         }
         else if (prefix == AMD64Helper::otM32) {
            operand.type = AMD64Helper::otDD;
         }
      }
	   else token.raiseErr("'[' expected (%d)\n");

      token.read();

      return operand;
   }
}

AMD64Assembler::Operand AMD64Assembler:: compileOperand(TokenInfo& token, ProcedureInfo& info/*, _Module* binary*/, const char* err)
{
   Operand	    operand;

   token.read();
	if (token.check("[")) {
		token.read();
      operand = readDispOperand(token, info, "Invalid expression (%d)", AMD64Helper::otM64);

	   if(!token.check("]")) {
          token.raiseErr("']' expected (%d)\n");
	   }
	   else token.read();
	}
	else if (token.check("dword")) {
      token.read("ptr", "'ptr' expected (%d)\n");

      operand = readPtrOperand(token, info, err, AMD64Helper::otM32);
	}
   else if (token.check("qword")) {
      token.read("ptr", "'ptr' expected (%d)\n");

      operand = readPtrOperand(token, info, err, AMD64Helper::otM64);
   }
   else if (token.check("word")) {
	   token.read("ptr", "'ptr' expected (%d)\n");

      operand = readPtrOperand(token, info, err, AMD64Helper::otM16);
   }
   else if (token.check("byte")) {
	   token.read("ptr", "'ptr' expected (%d)\n");

	   operand = readPtrOperand(token, info, err, AMD64Helper::otM8);
   }
//   else if (token.check("fs")) {
//      token.read(":", "Column is expected");
//      operand = readPtrOperand(token, info, err, x86Helper::otM32);
//
//      if (operand.prefix != x86Helper::spNone) {
//         token.raiseErr(err);
//      }
//      else operand.prefix = x86Helper::spFS;
//   }
   else {
      operand = defineOperand(token, info, err);

      if (operand.type != AMD64Helper::otUnknown) {
         token.read();

         if (token.check("+")) {
            operand.offset += token.readInteger(constants);

            token.read();
         }
      }
   }

   return operand;
}

void AMD64Assembler :: compileMOV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

//   // write segment prefix
//   if (sour.prefix != x86Helper::spNone) {
//      if (dest.prefix != x86Helper::spNone)
//         token.raiseErr("Invalid command (%d)");
//
//      code->writeByte(sour.prefix);
//   }
//   else if (dest.prefix != x86Helper::spNone) {
//      if (sour.prefix != x86Helper::spNone)
//         token.raiseErr("Invalid command (%d)");
//
//      code->writeByte(dest.prefix);
//   }

   if (test(sour.type, AMD64Helper::otPtr16)) {
	   sour.type = AMD64Helper::overrideOperand16(sour.type);
	   if (test(dest.type, AMD64Helper::otPtr16)) {
		   dest.type = AMD64Helper::overrideOperand16(dest.type);
	   }
	   else if (dest.type== AMD64Helper::otDD) {
		   dest.type = AMD64Helper::otDW;
	   }
	   code->writeByte(0x66);
   }

//	if (sour.type == x86Helper::otEAX && dest.type == x86Helper::otDisp32) {
//		code->writeByte(0xA1);
//
//      if (dest.reference != 0) {
//         code->writeRef(dest.reference, dest.offset);
//      }
//      else code->writeDWord(dest.offset);
//	}
//	else if (sour.type == x86Helper::otDisp32 && dest.type == x86Helper::otEAX) {
//		code->writeByte(0xA3);
//
//      if (sour.reference != 0) {
//         code->writeRef(sour.reference, sour.offset);
//      }
//      else code->writeDWord(sour.offset);
//	}
   /*else */if (test(sour.type, AMD64Helper::otRX64) && test(dest.type, AMD64Helper::otMX64)) {
      code->writeByte(0x4F);
      code->writeByte(0x8B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && test(dest.type, AMD64Helper::otMX64)) {
      code->writeByte(0x4B);
      code->writeByte(0x8B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && (test(dest.type, AMD64Helper::otR64)||test(dest.type, AMD64Helper::otM64))) {
      code->writeByte(0x48);
      code->writeByte(0x8B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otRX64) && (test(dest.type, AMD64Helper::otR64) || test(dest.type, AMD64Helper::otM64))) {
      code->writeByte(0x4C);
      code->writeByte(0x8B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && (test(dest.type, AMD64Helper::otRX64))) {
      code->writeByte(0x49);
      code->writeByte(0x8B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otRX64) && (test(dest.type, AMD64Helper::otRX64))) {
      code->writeByte(0x4F);
      code->writeByte(0x8B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otM64) && test(dest.type, AMD64Helper::otR64)) {
      code->writeByte(0x48);
      code->writeByte(0x89);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32)||test(dest.type, AMD64Helper::otM32))) {
      code->writeByte(0x8B);
      AMD64Helper::writeModRM(code, sour, dest);
	}
   else if (test(sour.type, AMD64Helper::otM32) && test(dest.type, AMD64Helper::otR32)) {
      code->writeByte(0x89);
      AMD64Helper::writeModRM(code, dest, sour);
   }
	else if (test(sour.type, AMD64Helper::otR32) && (dest.type== AMD64Helper::otDD || dest.type== AMD64Helper::otDB)) {
		dest.type = AMD64Helper::otDD;
		code->writeByte(0xB8 + (char)sour.type);
      AMD64Helper::writeImm(code, dest);
	}
   else if (test(sour.type, AMD64Helper::otRX64) && (dest.type == AMD64Helper::otDD || dest.type == AMD64Helper::otDB 
      || dest.type == AMD64Helper::otDQ)) {
      dest.type = AMD64Helper::otDQ;
      code->writeByte(0x49);
      code->writeByte(0xB8 + (char)sour.type);
      AMD64Helper::writeImm(code, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && (dest.type == AMD64Helper::otDQ || dest.type == AMD64Helper::otDB
      || dest.type == AMD64Helper::otDD)) 
   {
      dest.type = AMD64Helper::otDQ;
      code->writeByte(0x48);
      code->writeByte(0xB8 + (char)sour.type);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR32) || test(sour.type, AMD64Helper::otM32))
            && (dest.type== AMD64Helper::otDD || dest.type== AMD64Helper::otDB))
   {
      dest.type = AMD64Helper::otDD;
      code->writeByte(0xC7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64))
      && (dest.type == AMD64Helper::otDD || dest.type == AMD64Helper::otDB))
   {
      dest.type = AMD64Helper::otDD;
      code->writeByte(0x48);
      code->writeByte(0xC7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if (test(sour.type, AMD64Helper::otR8) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xB0 + (char)sour.type);
	   code->writeByte(dest.offset);
   }
   else if (test(sour.type, AMD64Helper::otM8) && dest.type== AMD64Helper::otDB)
   {
	   code->writeByte(0xC6);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8))&& test(dest.type, AMD64Helper::otR8)) {
	   code->writeByte(0x88);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if (test(sour.type, AMD64Helper::otR8) && (test(dest.type, AMD64Helper::otR8)||test(dest.type, AMD64Helper::otM8))) {
	   code->writeByte(0x8A);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileCMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otPtr16)) {
      sour.type = AMD64Helper::overrideOperand16(sour.type);
	   if (test(dest.type, AMD64Helper::otPtr16)) {
		   dest.type = AMD64Helper::overrideOperand16(dest.type);
	   }
	   else if (dest.type== AMD64Helper::otDD) {
		   dest.type = AMD64Helper::otDW;
	   }
	   code->writeByte(0x66);
   }

   if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32) || test(dest.type, AMD64Helper::otM32))) {
	   code->writeByte(0x3B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
	else if ((test(sour.type, AMD64Helper::otR32) || test(sour.type, AMD64Helper::otM32)) && test(dest.type, AMD64Helper::otR32)) {
		code->writeByte(0x39);
		AMD64Helper::writeModRM(code, dest, sour);
	}
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64)) && test(dest.type, AMD64Helper::otR64)) {
      code->writeByte(0x48);
      code->writeByte(0x39);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if (test(sour.type, AMD64Helper::otR64) && (test(dest.type, AMD64Helper::otR64) || test(dest.type, AMD64Helper::otM64))) {
      code->writeByte(0x48);
      code->writeByte(0x3B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && (test(dest.type, AMD64Helper::otRX64) || test(dest.type, AMD64Helper::otMX64))) {
      code->writeByte(0x49);
      code->writeByte(0x3B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64)) && dest.type == AMD64Helper::otDB) {
      code->writeByte(0x48); 
      code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 7), sour);
      code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDB) {
      code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 7), sour);
      code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDD) {
	   code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 7), sour);
      AMD64Helper::writeImm(code, dest);
   }
//	else if (sour.type==x86Helper::otAL && dest.type==x86Helper::otDB) {
//		code->writeByte(0x3C);
//		code->writeByte(dest.offset);
//	}
   else if (test(sour.type, AMD64Helper::otR8) && test(dest.type, AMD64Helper::otM8)) {
	   code->writeByte(0x3A);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR8) && test(dest.type, AMD64Helper::otR8)) {
	   code->writeByte(0x38);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR8) || test(sour.type, AMD64Helper::otM8)) && dest.type == AMD64Helper::otDB) {
	   code->writeByte(0x80);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 7), sour);
	   code->writeByte(dest.offset);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileADD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

//   if (dest.prefix == x86Helper::spFS) {
//      code->writeByte(0x64);   
//   }
//
//	if (sour.type==x86Helper::otEAX && dest.type == x86Helper::otDD) {
//		code->writeByte(0x05);
//		x86Helper::writeImm(code, dest);
//	}
	/*else */if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32)
      ||test(dest.type, AMD64Helper::otM32))) 
   {
		code->writeByte(0x03);
      AMD64Helper::writeModRM(code, sour, dest);
	}
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && test(dest.type, AMD64Helper::otR32)) {
      code->writeByte(0x01);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64)) && test(dest.type, AMD64Helper::otR64)) {
      code->writeByte(0x48);
      code->writeByte(0x01);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
	   code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x80);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 0), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if (test(sour.type, AMD64Helper::otR8)&&(test(dest.type, AMD64Helper::otR8)||test(dest.type, AMD64Helper::otM8))) {
	   code->writeByte(0x02);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) && dest.type== AMD64Helper::otDD) {
      code->writeByte(0x48);
      code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
      AMD64Helper::writeImm(code, dest);
	}
   else if ((test(sour.type, AMD64Helper::otR32) || test(sour.type, AMD64Helper::otM32)) && dest.type == AMD64Helper::otDD) {
      code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) && dest.type==AMD64Helper::otDB) {
      code->writeByte(0x48);
      code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR64 + 0), sour);
      code->writeByte(dest.offset);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileXADD(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//
//	checkComma(token);
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//   if (prefix.lockMode) {
//      code->writeByte(0xF0);
//   }
//   else if(prefix.Exists()) {
//      token.raiseErr("Invalid Prefix (%d)");
//   }
//
//	if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
//		code->writeByte(0x0F);
//      code->writeByte(0xC1);
//		x86Helper::writeModRM(code, dest, sour);
//	}
//   else if ((test(sour.type, x86Helper::otR8) || test(sour.type, x86Helper::otM8)) && test(dest.type, x86Helper::otR8)) {
//      code->writeByte(0x0F);
//      code->writeByte(0xC0);
//      x86Helper::writeModRM(code, dest, sour);
//   }
//   else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileXORPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 57 /r
//	// Mnemonic: XORPS xmm1, xmm2/m128
//	// 0f 57 ca -- xorps xmm1, xmm2
//	// 0f 57 08 -- xorps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x57);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileADC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (sour.type== AMD64Helper::otEAX && dest.type == AMD64Helper::otDD) {
	   code->writeByte(0x15);
      AMD64Helper::writeImm(code, dest);
   }
   else if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32)||test(dest.type, AMD64Helper::otM32))) {
	   code->writeByte(0x13);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && test(dest.type, AMD64Helper::otR32)) {
	   code->writeByte(0x11);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 2), sour);
	   code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x80);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 2), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if (test(sour.type, AMD64Helper::otR8)&&(test(dest.type, AMD64Helper::otR8)||test(dest.type, AMD64Helper::otM8))) {
	   code->writeByte(0x12);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDD) {
	   code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 2), sour);
	   code->writeDWord(dest.offset);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler ::compileADDPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//   if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//      code->writeByte(0x0F);
//      code->writeByte(0x58);
//      x86Helper::writeModRM(code, sour, dest);
//   }
//   else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compileADDSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//    // Opcode:   F3 0F 58 /r
//    // Mnemonic: ADDSS xmm1, xmm2/m32
//    // f3 0f 58 ca -- addss xmm1, xmm2
//    // f3 0f 58 08 -- addss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x58);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compileANDPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 54 /r
//	// Mnemonic: ANDPS xmm1, xmm2/m128
//	// 0f 54 ca -- andps xmm1, xmm2
//	// 0f 54 08 -- andps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x54);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compileANDNPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 55 /r
//	// Mnemonic: ANDNPS xmm1, xmm2/m128
//	// 0f 55 ca -- andnps xmm1, xmm2
//	// 0f 55 08 -- andnps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x55);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileAND(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   bool overridden = false;

   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR16) && dest.type== AMD64Helper::otDD) {
	   sour.type = AMD64Helper::overrideOperand16(sour.type);
	   code->writeByte(0x66);
	   overridden = true;
   }

   if (test(sour.type, AMD64Helper::otR32) && test(dest.type, AMD64Helper::otDB)) {
	   code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
	   code->writeByte(dest.offset);
   }
//	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x23);
//		x86Helper::writeModRM(code, sour, dest);
//	}
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) 
      && test(dest.type, AMD64Helper::otR32)) 
   {
	   code->writeByte(0x21);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64))
      && test(dest.type, AMD64Helper::otR64))
   {
      code->writeByte(0x48);
      code->writeByte(0x21);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   //   else if ((test(sour.type, x86Helper::otR8) || test(sour.type, x86Helper::otM8)) && test(dest.type, x86Helper::otR8)) {
//      code->writeByte(0x22);
//      x86Helper::writeModRM(code, dest, sour);
//   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDD) {
	   code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
	   if (overridden) {
		   code->writeWord(dest.offset);
	   }
	   else AMD64Helper::writeImm(code, dest);
   }
//   else if (test(sour.type, x86Helper::otR8) && test(dest.type, x86Helper::otDB)) {
//		code->writeByte(0x80);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 4), sour);
//      x86Helper::writeImm(code, dest);
//	}
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileXOR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   //if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32)) {
	  // code->writeByte(0x33);
	  // x86Helper::writeModRM(code, sour, dest);
   //}
   if (test(sour.type, AMD64Helper::otR64) && test(dest.type, AMD64Helper::otR64)) {
      code->writeByte(0x48);
      code->writeByte(0x33);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   //else if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otDB)) {
	  // code->writeByte(0x83);
	  // x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
	  // code->writeByte(dest.offset);
   //}
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDD) {
	   code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 6), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) 
      && test(dest.type, AMD64Helper::otR64)) 
   {
      code->writeByte(0x48);
	   code->writeByte(0x31);
      AMD64Helper::writeModRM(code, dest, sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR32) || test(sour.type, AMD64Helper::otM32))
      && test(dest.type, AMD64Helper::otR32))
   {
      code->writeByte(0x31);
      AMD64Helper::writeModRM(code, dest, sour);
      AMD64Helper::writeImm(code, dest);
   }
   //else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32) || test(dest.type, x86Helper::otM32))) {
	  // code->writeByte(0x33);
	  // x86Helper::writeModRM(code, sour, dest);
   //}
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileOR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   bool overridden = false;

   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR16) && dest.type== AMD64Helper::otDD) {
	   sour.type = AMD64Helper::overrideOperand16(sour.type);
	   code->writeByte(0x66);
	   overridden = true;
   }

//	if (sour.type== x86Helper::otEAX && dest.type==x86Helper::otDD) {
//		code->writeByte(0x0D);
//		if (overridden) {
//			code->writeWord(dest.offset);
//		}
//		else x86Helper::writeImm(code, dest);
//	}
   /*else */if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && test(dest.type, AMD64Helper::otDB)) {
	   code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 1), sour);
	   code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) && test(dest.type, AMD64Helper::otR8)) {
	   code->writeByte(0x08);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) 
      && dest.type== AMD64Helper::otDD) 
   {
	   code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 1), sour);
      AMD64Helper::writeImm(code, dest, overridden);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x80);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 1), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) 
      && test(dest.type, AMD64Helper::otR32)) 
   {
	   code->writeByte(0x09);
	   AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64))
      && test(dest.type, AMD64Helper::otR64))
   {
      code->writeByte(0x48);
      code->writeByte(0x09);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if (test(sour.type, AMD64Helper::otR16) && test(dest.type, AMD64Helper::otR16)) {
	   code->writeByte(0x66);
	   code->writeByte(0x09);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32) || test(dest.type, AMD64Helper::otM32))) {
	   code->writeByte(0x0B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileORPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 56 /r
//	// Mnemonic: ANDNPS xmm1, xmm2/m128
//	// 0f 56 ca -- orps xmm1, xmm2
//	// 0f 56 08 -- orps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x56);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileTEST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

//	if (sour.type == x86Helper::otEAX && dest.type == x86Helper::otDD) {
//		code->writeByte(0xA9);
//		x86Helper::writeImm(code, dest);
//	}
   /*else */if (test(sour.type, AMD64Helper::otRX64) && test(dest.type, AMD64Helper::otRX64)) {
      code->writeByte(0x4D);
      code->writeByte(0x85);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) && test(dest.type, AMD64Helper::otR64)) {
      code->writeByte(0x48);
      code->writeByte(0x85);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) && dest.type== AMD64Helper::otDB) {
      code->writeByte(0x48);
      code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
      code->writeDWord(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && test(dest.type, AMD64Helper::otR32)) {
	   code->writeByte(0x85);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDD) {
	   code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
	   code->writeDWord(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xF6);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 0), sour);
      AMD64Helper::writeImm(code, dest);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) && test(dest.type, AMD64Helper::otR8)) {
	   code->writeByte(0x84);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR16)||test(sour.type, AMD64Helper::otM16)) && test(dest.type, AMD64Helper::otR16)) {
      code->writeByte(0x66);
	   code->writeByte(0x85);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler::compileUCOMISS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 2E /r
//	// Mnemonic: UCOMISS xmm1, xmm2/m32
//	// 0f 2e ca -- ucomiss xmm1, xmm2
//	// 0f 2e 08 -- ucomiss xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x2E);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileSUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32)||test(dest.type, AMD64Helper::otM32))) {
	   code->writeByte(0x2B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && (test(dest.type, AMD64Helper::otR64) || test(dest.type, AMD64Helper::otM64))) {
      code->writeByte(0x48);
      code->writeByte(0x2B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otRX64) && test(dest.type, AMD64Helper::otR64)) {
      code->writeByte(0x4C);
      code->writeByte(0x2B);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (sour.type==AMD64Helper::otAL && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x2C);
	   code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && test(dest.type, AMD64Helper::otR32)) {
	   code->writeByte(0x29);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64)) && test(dest.type, AMD64Helper::otR64)) {
      code->writeByte(0x48);
      code->writeByte(0x29);
      AMD64Helper::writeModRM(code, dest, sour);
   }
   else if ((test(sour.type, AMD64Helper::otR64) ||test(sour.type, AMD64Helper::otM64)) && dest.type== AMD64Helper::otDB) {
      code->writeByte(0x48);
   	code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR64 + 5), sour);
   	code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) && dest.type==AMD64Helper::otDD) {
      code->writeByte(0x48);
      code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
      if (dest.reference != 0) {
         code->writeRef(dest.reference, dest.offset);
      }
      else code->writeDWord(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR32) ||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
	   code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDD) {
	   code->writeByte(0x81);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
      if (dest.reference != 0) {
         code->writeRef(dest.reference, dest.offset);
      }
	   else code->writeDWord(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x80);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 5), sour);
	   AMD64Helper::writeImm(code, dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileSUBPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 5C /r
//	// Mnemonic: SUBPS xmm1, xmm2/m128
//	// 0f 5c ca -- subps xmm1, xmm2
//	// 0f 5c 08 -- subps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x5C);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileSUBSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 5C /r
//	// Mnemonic: SUBSS xmm1, xmm2/m32
//	// f3 0f 5c ca -- subss xmm1, xmm2
//	// f3 0f 5c 08 -- subss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x5C);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileSQRTPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 51 /r
//	// Mnemonic: SQRTPS xmm1, xmm2/m128
//	// 0f 51 ca -- sqrtps xmm1, xmm2
//	// 0f 51 08 -- sqrtps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x51);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileSQRTSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 51 /r
//	// Mnemonic: SQRTSS xmm1, xmm2/m32
//	// f3 0f 51 ca -- sqrtss xmm1, xmm2
//	// f3 0f 51 08 -- sqrtss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x51);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileSBB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if ((test(sour.type, AMD64Helper::otR32) ||test(sour.type, AMD64Helper::otM32)) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 3), sour);
	   code->writeByte(dest.offset);
   }
   else if ((test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64)) && dest.type == AMD64Helper::otDB) {
      code->writeByte(0x48);
      code->writeByte(0x83);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 3), sour);
      code->writeByte(dest.offset);
   }
   //else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32)||test(dest.type, x86Helper::otM32))) {
	  // code->writeByte(0x1B);
	  // x86Helper::writeModRM(code, sour, dest);
   //}
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileLEA(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otRX64) && test(dest.type, AMD64Helper::otM64)) {
      code->writeByte(0x4C);
      code->writeByte(0x8D);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otRX64) && test(dest.type, AMD64Helper::otMX64)) {
      code->writeByte(0x4F);
      code->writeByte(0x8D);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && test(dest.type, AMD64Helper::otMX64)) {
      code->writeByte(0x4B);
      code->writeByte(0x8D);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && test(dest.type, AMD64Helper::otM64)) {
      code->writeByte(0x48);
	   code->writeByte(0x8D);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR32) && test(dest.type, AMD64Helper::otM32)) {
      code->writeByte(0x8D);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileSHR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xC1);
	   AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
	   code->writeByte(dest.offset);
   }
   else if (test(sour.type, AMD64Helper::otR64) && dest.type == AMD64Helper::otDB) {
      code->writeByte(0x48);
      code->writeByte(0xC1);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
      code->writeByte(dest.offset);
   }
   else if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otCL) {
	   code->writeByte(0xD3);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
   }
   else if (test(sour.type, AMD64Helper::otR64) && dest.type == AMD64Helper::otCL) {
      code->writeByte(0x48);
      code->writeByte(0xD3);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR64 + 5), sour);
   }
   else if (test(sour.type, AMD64Helper::otR8) && dest.type== AMD64Helper::otDB && dest.offset == 1) {
	   code->writeByte(0xD0);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 5), sour);
   }
   //else if (test(sour.type, AMD64Helper::otR8) && dest.type==x86Helper::otDB) {
	  // code->writeByte(0xC0);
	  // x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 5), sour);
	  // code->writeByte(dest.offset);
   //}
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileSAR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//
//	checkComma(token);
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB) {
//		code->writeByte(0xC1);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
//		code->writeByte(dest.offset);
//	}
//	else if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otCL) {
//		code->writeByte(0xD3);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
//	}
//	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB && dest.offset == 1) {
//		code->writeByte(0xD0);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 7), sour);
//	}
//	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB) {
//		code->writeByte(0xC0);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 7), sour);
//		code->writeByte(dest.offset);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileSHL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xC1);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
	   code->writeByte(dest.offset);
   }
   else if (test(sour.type, AMD64Helper::otR64) && dest.type == AMD64Helper::otDB) {
      code->writeByte(0x48);
      code->writeByte(0xC1);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
      code->writeByte(dest.offset);
   }
   else if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otCL) {
	   code->writeByte(0xD3);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
   }
   else if (test(sour.type, AMD64Helper::otR64) && dest.type == AMD64Helper::otCL) {
      code->writeByte(0x48);
      code->writeByte(0xD3);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR64 + 4), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileSHLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//
//	checkComma(token);
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	checkComma(token);
//
//	Operand third = compileOperand(token, info, "Invalid third operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otDB) {
//      code->writeByte(0x0F);
//		code->writeByte(0xA4);
//      x86Helper::writeImm(code, third);
//	}
//	else if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otCL) {
//      code->writeByte(0x0F);
//		code->writeByte(0xA5);
//		x86Helper::writeModRM(code, dest, sour);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileSHRD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//
//	checkComma(token);
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	checkComma(token);
//
//	Operand third = compileOperand(token, info, "Invalid third operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otDB) {
//      code->writeByte(0x0F);
//		code->writeByte(0xAC);
//      x86Helper::writeImm(code, third);
//	}
//	else if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otCL) {
//      code->writeByte(0x0F);
//		code->writeByte(0xAD);
//		x86Helper::writeModRM(code, dest, sour);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler:: compileROL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR8) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xC0);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 0), sour);
	   code->writeByte(dest.offset);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileROR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR16)) {
	   sour.type = (OperandType)AMD64Helper::overrideOperand16(sour.type);
	   code->writeByte(0x66);
   }

   if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xC1);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 1), sour);
	   code->writeByte(dest.offset);
   }
   else if (test(sour.type, AMD64Helper::otR8) && dest.type== AMD64Helper::otDB) {
	   code->writeByte(0xC0);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 1), sour);
	   code->writeByte(dest.offset);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileRCR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otDB && dest.offset==1) {
	   code->writeByte(0xD1);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 3), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileXCHG(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (sour.type== AMD64Helper::otEAX && test(dest.type, AMD64Helper::otR32)) {
	   code->writeByte(0x90 + (char)dest.type);
   }
   else if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32)||test(dest.type, AMD64Helper::otM32))) {
	   code->writeByte(0x87);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileRCL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otDB && dest.offset==1) {
	   code->writeByte(0xD1);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 2), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileMOVZX(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR8)||test(dest.type, AMD64Helper::otM8))) {
	   code->writeWord(0xB60F);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + (char)sour.type), dest);
   }
   else if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR16)||test(dest.type, AMD64Helper::otM16))) {
	   code->writeWord(0xB70F);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + (char)sour.type), dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileMOVSXD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR64) && (test(dest.type, AMD64Helper::otM32) || test(dest.type, AMD64Helper::otR32))) {
      code->writeByte(0x48);
      code->writeByte(0x63);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + (char)sour.type), dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compilePUSH(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR64)) {
      code->writeByte(0x50 + (char)sour.type);
   }
   else if (test(sour.type, AMD64Helper::otRX64)) {
      code->writeByte(0x41);
      code->writeByte(0x50 + (char)sour.type);
   }
   //	else if (test(sour.type, x86Helper::otM32)) {
//		code->writeByte(0xFF);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
//	}
   else if (test(sour.type, AMD64Helper::otM64)) {
	   code->writeByte(0xFF);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 6), sour);
   }
//	else if (test(sour.type, x86Helper::otM16)) {
//		code->writeByte(0x66);
//		code->writeByte(0xFF);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
//	}
   else if (sour.type==AMD64Helper::otDB) {
      code->writeByte(0x6A);
      code->writeByte(sour.offset);
   }
   else if (sour.type==AMD64Helper::otDD) {
      code->writeByte(0x68);
      AMD64Helper::writeImm(code, sour);
	}
//	else if (sour.type==x86Helper::otDW) {
//      code->writeByte(0x66);
//		code->writeByte(0x68);
//		x86Helper::writeImm(code, sour);
//	}
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compilePOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR64)) {
      code->writeByte(0x58 + (char)sour.type);
   }
   else if (test(sour.type, AMD64Helper::otRX64)) {
      code->writeByte(0x41);
      code->writeByte(0x58 + (char)sour.type);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) {
	   code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
   }
   else if (test(sour.type, AMD64Helper::otR64) || test(sour.type, AMD64Helper::otM64)) {
      code->writeByte(0x48);
      code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR64 + 4), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileMULPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 59 /r
//	// Mnemonic: MULPS xmm1, xmm2/m128
//	// 0f 59 ca -- mulps xmm1, xmm2
//	// 0f 59 08 -- mulps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x59);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileMULSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 59 /r
//	// Mnemonic: MULSS xmm1, xmm2/m32
//	// f3 0f 59 ca -- subps xmm1, xmm2
//	// f3 0f 59 08 -- subps xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x59);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileMAXPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 5F /r
//	// Mnemonic: MAXPS xmm1, xmm2/m128
//	// 0f 5F ca -- maxps xmm1, xmm2
//	// 0f 5F 08 -- maxps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x5F);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileMAXSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 5F /r
//	// Mnemonic: maxss xmm1, xmm2/m32
//	// f3 0f 5F ca -- maxss xmm1, xmm2
//	// f3 0f 5F 08 -- maxss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x5F);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compileMINPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 5D /r
//	// Mnemonic: MINPS xmm1, xmm2/m128
//	// 0f 5F ca -- minps xmm1, xmm2
//	// 0f 5F 08 -- minps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x5D);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compileMINSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 5D /r
//	// Mnemonic: minss xmm1, xmm2/m32
//	// f3 0f 5F ca -- minss xmm1, xmm2
//	// f3 0f 5F 08 -- minss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x5D);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileIMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   if (!token.check(",")) {
      if (test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) {
	      code->writeByte(0xF7);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
      }
      else if (test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) {
         code->writeByte(0x48);
         code->writeByte(0xF7);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
      }
      else token.raiseErr("Invalid command (%d)");
   }
   else {
	   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	   if (test(sour.type, AMD64Helper::otR32) && dest.type== AMD64Helper::otDB) {
		   code->writeByte(0x6B);
         AMD64Helper::writeModRM(code, sour, sour);
		   code->writeByte(dest.offset);
	   }
      else if (test(sour.type, AMD64Helper::otR32) && dest.type == AMD64Helper::otDD) {
         code->writeByte(0x69);
         AMD64Helper::writeModRM(code, sour, sour);
         AMD64Helper::writeImm(code, dest);
      }
	   else if (test(sour.type, AMD64Helper::otR32) && test(dest.type, AMD64Helper::otR32)) {
		   code->writeByte(0x0F);
		   code->writeByte(0xAF);
         AMD64Helper::writeModRM(code, sour, dest);
	   }
	   else token.raiseErr("Invalid command (%d)");
   }
}

void AMD64Assembler :: compileIDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR64)||test(sour.type, AMD64Helper::otM64)) {
      code->writeByte(0x48);
      code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 7), sour);
   }
   else if (test(sour.type, AMD64Helper::otR32) || test(sour.type, AMD64Helper::otM32)) {
      code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 7), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR32)||test(sour.type, AMD64Helper::otM32)) {
	   code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 6), sour);
   }
   else if (test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) {
	   code->writeByte(0xF6);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 6), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileDIVPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 5E /r
//	// Mnemonic: DIVPS xmm1, xmm2/m128
//	// 0f 5E ca -- divps xmm1, xmm2
//	// 0f 5E 08 -- divps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x5E);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileDIVSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 5E	/r
//	// Mnemonic: DIVSS xmm1, xmm2/m32
//	// f3 0f 5e ca -- divss xmm1, xmm2
//	// f3 0f 5e 08 -- divss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x5E);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileDEC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
//	if (test(sour.type, x86Helper::otR32)) {
//		code->writeByte(0x48 + (char)sour.type);
//	}
//	else if (test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) {
//		code->writeByte(0xFE);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 1), sour);
//	}
//	else if (test(sour.type, x86Helper::otM32)) {
//		code->writeByte(0xFF);
//		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 1), sour);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileINC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR32)) {
	   code->writeByte(0x40 + (char)sour.type);
   }
   else if (test(sour.type, AMD64Helper::otR8)||test(sour.type, AMD64Helper::otM8)) {
	   code->writeByte(0xFE);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 0), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler::compileINT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
//   if (test(sour.type, x86Helper::otDB)) {
//      code->writeByte(0xCD);
//      code->writeByte(sour.offset);
//   }
//   else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileNEG(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR32)) {
	   code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 3), sour);
   }
   else if (test(sour.type, AMD64Helper::otR64)) {
      code->writeByte(0x48);
      code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 3), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileNOT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR32)) {
	   code->writeByte(0xF7);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 2), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileRET(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.terminal.state == dfaInteger || token.terminal.state == dfaHexInteger || token.check(ARGUMENT1) || token.check(ARGUMENT2)) {
//      Operand sour = defineOperand(token, info, "Invalid operand (%d)\n");
//      if (sour.type == x86Helper::otDD || sour.type == x86Helper::otDB) {
//         code->writeByte(0xC2);
//         if (sour.type == x86Helper::otDB)
//            sour.type = x86Helper::otDW;
//
//         x86Helper::writeImm(code, sour);
//      }
      /*else */token.raiseErr("Invalid command (%d)");
//
//      token.read();
   }
   else code->writeByte(0xC3);
}

void AMD64Assembler :: compileCQO(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x48);
   code->writeByte(0x99);

   token.read();
}

void AMD64Assembler :: compileCDQ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x99);
   
   token.read();
}
 
void AMD64Assembler :: compileCWDE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x98);

   token.read();
}

void AMD64Assembler :: compileSTC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0xF9);

   token.read();
}

void AMD64Assembler :: compileSAHF(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x9E);

   token.read();
}

void AMD64Assembler :: compileNOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x90);

   token.read();
}

void AMD64Assembler :: compileREP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xF3);

	token.read();
}

//void x86Assembler :: compileREPZ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	code->writeByte(0xF3);
//
//	token.read();
//}
//
//void x86Assembler :: compileRCPPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 53 /r
//	// Mnemonic: RCPPS xmm1, xmm2/m128
//	// 0f 53 ca -- rcpps xmm1, xmm2
//	// 0f 53 08 -- rcpps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x53);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileRCPSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 53 /r
//	// Mnemonic: RCPSS xmm1, xmm2/m32
//	// f3 0f 53 ca -- rcpss xmm1, xmm2
//	// f3 0f 53 08 -- rcpss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x53);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compileRSQRTPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 52 /r
//	// Mnemonic: RSQRTPS xmm1, xmm2/m128
//	// 0f 52 ca -- rsqrtps xmm1, xmm2
//	// 0f 52 08 -- rsqrtps xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x52);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compileRSQRTSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F 52 /r
//	// Mnemonic: RSQRTSS xmm1, xmm2/m32
//	// f3 0f 52 ca -- rsqrtss xmm1, xmm2
//	// f3 0f 52 08 -- rsqrtss xmm1, DWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0x52);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileLODSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0xAD);

   token.read();
}

void AMD64Assembler:: compileLODSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x66);
   code->writeByte(0xAD);

   token.read();
}

void AMD64Assembler :: compileLODSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0xAC);

   token.read();
}

void AMD64Assembler :: compileMOVSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0xA4);

   token.read();
}

void AMD64Assembler::compileMOVSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0xA5);

   token.read();
}

//void x86Assembler :: compileMOVAPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	code->writeByte(0x0F);
//   if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//      code->writeByte(0x28);
//      x86Helper::writeModRM(code, sour, dest);
//   }
//   else if ((test(sour.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32)) && test(dest.type, x86Helper::otX128)) {
//      code->writeByte(0x29);
//      x86Helper::writeModRM(code, sour, dest);
//   }
//   else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileMOVUPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	code->writeByte(0x0F);
//
//   if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//      code->writeByte(0x10);
//      x86Helper::writeModRM(code, sour, dest);
//   }
//   else if (test(sour.type, x86Helper::otX128) && test(dest.type, x86Helper::otX128)) {
//      code->writeByte(0x11);
//      x86Helper::writeModRM(code, sour, dest);
//   }
//   else if (test(sour.type, x86Helper::otM32) && test(dest.type, x86Helper::otX128)) {
//	   code->writeByte(0x11);
//	   x86Helper::writeModRM(code, dest, sour);
//	   if(sour.offset != 0)
//		   code->writeByte(sour.offset);
//   }
//   else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileSTOSQ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x48);
   code->writeByte(0xAB);

	token.read();
}

void AMD64Assembler :: compileSTOSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0xAB);

   token.read();
}

void AMD64Assembler :: compileSTOSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0xAA);

   token.read();
}

void AMD64Assembler :: compileSTOSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x66);
   code->writeByte(0xAB);

   token.read();
}

//void x86Assembler :: compileCMPSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	code->writeByte(0xA6);
//
//	token.read();
//}

void AMD64Assembler :: compilePUSHFD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x9C);

   token.read();
}

void AMD64Assembler :: compilePOPFD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x9D);

   token.read();
}

//void x86Assembler :: compilePAVGB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 66 0F E0 /r
//	// Mnemonic: PAVGB xmm1, xmm2/m128
//	// 66 0f e0 ca -- pavgb xmm1, xmm2
//	// 66 0f e0 08 -- pavgb xmm1, XMMWORD PTR[eax]
//
//	// Opcode: 0F E0 /r
//	// Mnemonic : PAVGB mm1, mm2/m64
//	// 0f e0 c8 -- pavgb mm1, mm0
//	// 0f e0 08 -- pavgb mm1, QWORD PTR [eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xE0);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xE0);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compilePAVGW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 66 0F E0 /r
//	// Mnemonic: PAVGW xmm1, xmm2/m128
//	// 66 0f e3 ca -- pavgw xmm1, xmm2
//	// 66 0f e3 08 -- pavgw xmm1, XMMWORD PTR[eax]
//
//	// Opcode: 0F E0 /r
//	// Mnemonic : PAVGW mm1, mm2/m64
//	// 0f e3 c8 -- pavgw mm1, mm0
//	// 0f e3 08 -- pavgw mm1, QWORD PTR [eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xE3);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xE3);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compilePSADBW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 66 0F F6 /r
//	// Mnemonic: psadbw xmm1, xmm2/m128
//	// 66 0f e3 ca -- psadbw xmm1, xmm2
//	// 66 0f e3 08 -- psadbw xmm1, XMMWORD PTR[eax]
//
//	// Opcode: 0F F6 /r
//	// Mnemonic : psadbw mm1, mm2/m64
//	// 0f e3 c8 -- psadbw mm1, mm0
//	// 0f e3 08 -- psadbw mm1, QWORD PTR [eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xE3);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xE3);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePEXTRW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 66 0F C5 /r ib
//	// Mnemonic: pextrw r32, xmm, imm8
//	// 66 0f c5 c1 03 -- pextrw eax,xmm1,3
//
//	// Opcode: 0F C5 /r ib
//	// Mnemonic: pextrw r32, mm, imm8
//	// 0f c5 c6 05 -- pextrw eax,mm6,5
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//	checkComma(token);
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	token.read();
//	Operand imm = defineOperand(token, info, "Invalid constant");
//	token.read();
//
//	if (test(dest.type, x86Helper::otR32) && test(sour.type, x86Helper::otX128)) {
//		if (imm.offset > 7) token.raiseErr("imm value too large in asm instruction");
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xC5);
//		x86Helper::writeModRM(code, dest, sour);
//		x86Helper::writeImm(code, imm);
//	}
//	else if (test(dest.type, x86Helper::otR32) && test(sour.type, x86Helper::otM64)) {
//		if (imm.offset > 3) token.raiseErr("imm value too large in asm instruction");
//		code->writeByte(0x0F);
//		code->writeByte(0xC5);
//		x86Helper::writeModRM(code, dest, sour);
//		x86Helper::writeImm(code, imm);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePINSRW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F C4 /r ib
//	// Mnemonic: pinsrw mm, r32/m16, imm8
//	// 0f c4 c0 03 -- pinsrw mm0, eax, 3
//	// 0f c4 08 03 -- pinsrw mm1, WORD PTR [eax], 3
//	// 0f c4 c8 03 -- pinsrw mm1, eax, 3
//
//	// Opcode: 66 0F C4 /r ib
//	// Mnemonic: pinsrw xmm, r32/m16, imm8
//	// 66 0f c4 c0 07 -- pinsrw xmm0, eax, 7
//	// 66 0f c4 08 07 -- pinsrw xmm1, WORD PTR [eax], 7
//	// 66 0f c4 c8 07 -- pinsrw xmm1, eax, 7
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//	checkComma(token);
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	token.read();
//	Operand imm = defineOperand(token, info, "Invalid constant");
//	token.read();
//
//	if (test(dest.type, x86Helper::otX128) && (test(sour.type, x86Helper::otM32) || test(sour.type, x86Helper::otR16) || test(sour.type, x86Helper::otR32))) {
//		if (imm.offset > 7) token.raiseErr("imm value too large in asm instruction");
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xC4);
//		x86Helper::writeModRM(code, dest, sour);
//		x86Helper::writeImm(code, imm);
//	}
//	else if (test(dest.type, x86Helper::otM64) && (test(sour.type, x86Helper::otM32) || test(sour.type, x86Helper::otR16) || test(sour.type, x86Helper::otR32))) {
//		if (imm.offset > 3) token.raiseErr("imm value too large in asm instruction");
//		code->writeByte(0x0F);
//		code->writeByte(0xC4);
//		x86Helper::writeModRM(code, dest, sour);
//		x86Helper::writeImm(code, imm);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePMAXSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F EE /r
//	// Mnemonic : pmaxsw mm1, mm2/m64
//	// 0f ee c8 -- pmaxsw mm1, mm0
//	// 0f ee 08 -- pmaxsw mm1, QWORD PTR [eax]
//
//	// Opcode: 66 0F EE /r
//	// Mnemonic: pmaxsw xmm1, xmm2/m128
//	// 66 0f ee ca -- pmaxsw xmm1, xmm2
//	// 66 0f ee 08 -- pmaxsw xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xEE);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xEE);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePMAXUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F DE /r
//	// Mnemonic : pmaxub mm1, mm2/m64
//	// 0f de c8 -- pmaxub mm1, mm0
//	// 0f de 08 -- pmaxub mm1, QWORD PTR [eax]
//
//	// Opcode: 66 0F DE /r
//	// Mnemonic: pmaxub xmm1, xmm2/m128
//	// 66 0f de ca -- pmaxub xmm1, xmm2
//	// 66 0f de 08 -- pmaxub xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xDE);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xDE);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePMINSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F EA /r
//	// Mnemonic : pminsw mm1, mm2/m64
//	// 0f ea c8 -- pminsw mm1, mm0
//	// 0f ea 08 -- pminsw mm1, QWORD PTR [eax]
//
//	// Opcode: 66 0F EA /r
//	// Mnemonic: pminsw xmm1, xmm2/m128
//	// 66 0f ea ca -- pminsw xmm1, xmm2
//	// 66 0f ea 08 -- pminsw xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xEA);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xEA);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePMINUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F DA /r
//	// Mnemonic : pminub mm1, mm2/m64
//	// 0f da c8 -- pminub mm1, mm0
//	// 0f da 08 -- pminub mm1, QWORD PTR [eax]
//
//	// Opcode: 66 0F DA /r
//	// Mnemonic: pminub xmm1, xmm2/m128
//	// 66 0f da ca -- pminub xmm1, xmm2
//	// 66 0f da 08 -- pminub xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xDA);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xDA);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePMOVMSKB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F D7 /r
//	// Mnemonic: pmovmskb r32, mm
//	// 0f d7 c0 -- pmovmskb eax, mm0
//
//	// Opcode: 66 0F D7 /r
//	// Mnemonic: pmovmskb r32, xmm
//	// 66 0f d7 c0 -- pmovmskb eax, xmm0
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otX128)) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xD7);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otM64)) {
//		code->writeByte(0x0F);
//		code->writeByte(0xD7);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePMULHUW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F E4 /r
//	// Mnemonic: pmulhuw mm1, mm2/m64
//	// 0f e4 c1 -- pmulhuw mm0, mm1
//	// 0f e4 00 -- pmulhuw mm0, QWORD PTR [eax]
//
//	// Opcode: 66 0F E4 /r
//	// Mnemonic: pmulhuw xmm1, xmm2/m128
//	// 66 0f e4 c1 -- pmulhuw xmm0, xmm1
//	// 66 0f e4 00 -- pmulhuw xmm0, XMMWORD PTR [eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x66);
//		code->writeByte(0x0F);
//		code->writeByte(0xE4);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else if (test(sour.type, x86Helper::otM64) && (test(dest.type, x86Helper::otM64) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0xE4);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler::compilePSHUFW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 70 /r ib
//	// Mnemonic: pshufw mm1, mm2/m64, imm8
//	// 0f 70 c1 32 -- pshufw mm0, mm1, 50
//	// 0f 70 00 32 -- pshufw mm0, QWORD PTR [eax], 50
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//	checkComma(token);
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	token.read();
//	Operand imm = defineOperand(token, info, "Invalid constant");
//	token.read();
//
//	if (test(dest.type, x86Helper::otM64) && (test(sour.type, x86Helper::otM32) || test(sour.type, x86Helper::otM64))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x70);
//		x86Helper::writeModRM(code, dest, sour);
//		x86Helper::writeImm(code, imm);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileJxx(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int prefix, AMD64JumpHelper& helper)
{
   token.read();

   bool shortJump = false;
   if (token.check("short")) {
      shortJump = true;
      token.read();
   }
   else if (token.check("short")) {
      token.raiseErr("Use short prefix instead");
   }

   // if jump forward
   if (!helper.checkDeclaredLabel(token.value)) {
      helper.writeJxxForward(token.value, prefix, shortJump);
   }
   // if jump backward
   else helper.writeJxxBack(token.value, prefix, shortJump);

   token.read();
}

void AMD64Assembler :: compileJMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, AMD64JumpHelper& helper)
{
   Operand operand = compileOperand(token, info, NULL);

   //if (test(operand.type, x86Helper::otR32)||test(operand.type, x86Helper::otM32)) {
   //   code->writeByte(0xFF);
   //   x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), operand);
   //}
   /*else */if (test(operand.type, AMD64Helper::otRX64) || test(operand.type, AMD64Helper::otMX64)) {
      code->writeByte(0x4F);
      code->writeByte(0xFF);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), operand);
   }
   else if (test(operand.type, AMD64Helper::otR64)||test(operand.type, AMD64Helper::otM64)) {
      code->writeByte(0x48);
      code->writeByte(0xFF);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), operand);
   }
   else if (operand.type == AMD64Helper::otDD && operand.reference == -1) {
      code->writeByte(0xE9);
      code->writeRef(operand.reference, 0);
   }
   else {
      bool shortJump = false;
      if (token.check("short")) {
         shortJump = true;
         token.read();
      }
      else if (token.check("near")) {
         token.raiseErr("Use short prefix instead");
      }
      
      // if jump forward
      if (!helper.checkDeclaredLabel(token.value)) {
         helper.writeJmpForward(token.value, shortJump);
      }
      // if jump backward
      else helper.writeJmpBack(token.value, shortJump);
      
      token.read();
   }
}

//void x86Assembler :: compileLOOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, x86JumpHelper& helper)
//{
//	token.read();
//
//   // if jump forward
//	if (!helper.checkDeclaredLabel(token.value)) {
//      helper.writeLoopForward(token.value);
//	}
//   // if jump backward
//	else helper.writeLoopBack(token.value);
//
//	token.read();
//}

void AMD64Assembler :: compileCALL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code/*, x86JumpHelper& helper*/)
{
   token.read();

   Operand operand = defineRegister(token);
   if (operand.type != AMD64Helper::otUnknown) {
      code->writeByte(0xFF);
      code->writeByte(0xD0 + (char)operand.type);
   }
   else if (token.check("extern")) {
      code->writeWord(0x15FF);
      token.read();
      if (token.check(ARGUMENT1)) {
         code->writeRef(-1, 0);
      }
//      else if (token.check(ARGUMENT2)) {
//         code->writeRef(-2, 0);
//      }
//      else if (token.value[0]==':') {
//         token.read();
//         IdentifierString s(token.terminal.line + 1, token.terminal.length-2);
//
//         ReferenceNs function(DLL_NAMESPACE, s);
//
//	      int ref = info.binary->mapReference(function) | mskImportRef;
//
//	      code->writeRef(ref, 0);
//      }
      else if (token.check("'dlls'", 6)) {
         ReferenceNs function(DLL_NAMESPACE, token.value + 6);

	      token.read(".", "dot expected (%d)\n");
	      function.append(".");
	      function.append(token.read());

	      int ref = info.binary->mapReference(function) | mskRelImportRef;

	      code->writeRef(ref, 0);
      }
      else if (token.check("'rt_dlls")) {
         ReferenceNs function(DLL_NAMESPACE, RTDLL_FORWARD);

         token.read(".", "dot expected (%d)\n");
         function.append(".");
         function.append(token.read());

         int ref = info.binary->mapReference(function) | mskRelImportRef;

         code->writeRef(ref, 0);
      }
      else token.raiseErr("Invalid call label (%d)\n");
   }
   else if (token.check("[")) {
	   token.read();
	   Operand operand = readDispOperand(token, info, "Invalid call target (%d)\n", AMD64Helper::otM64);
	   if (!token.check("]"))
		   token.raiseErr("']' expected(%d)\n");

	   if (test(operand.type, AMD64Helper::otM64)) {
		   code->writeByte(0xFF);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 2), operand);
         AMD64Helper::writeImm(code, operand);
	   }
	   else token.raiseErr("Invalid call target (%d)\n");
   }
   else if (token.check("code")) {
      token.read(":", "Column is expected");
      token.read();

      code->writeByte(0xE8);

		int ref = 0;

      if (token.check("%")) {
         ref = token.readInteger(constants) | mskPreloadRelCodeRef;
      }
      else if (token.check(ARGUMENT1)) {
         ref = -1;
      }
      else if (token.terminal.state==dfaQuote) {
         IdentifierString funRef(token.terminal.line + 1, token.terminal.length-2);
         if (funRef.ident().find(NATIVE_MODULE) == 0) {
            ref = info.binary->mapReference(funRef) | mskNativeRelCodeRef;
         }
         else ref = info.binary->mapReference(funRef) | mskSymbolRelRef;
      }

      code->writeRef(ref, 0);
   }
//   // if jump forward
//   else if (!helper.checkDeclaredLabel(token.value)) {
//      helper.writeCallForward(token.value);
//   }
//   // if jump backward
//   else helper.writeCallBack(token.value);

   token.read();
}

void AMD64Assembler :: fixJump(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, AMD64JumpHelper& helper)
{
   if (helper.checkDeclaredLabel(token.value)) {
      token.raiseErr("Label with such a name already exists (%d)\n");
   }
   
   if (!helper.addLabel(token.value))
      token.raiseErr("Invalid near jump to this label (%d)\n");
   
   token.read(":", "Invalid command or label (%d)\n");
}

void AMD64Assembler :: compileFBLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otM64)) {
	   code->writeByte(0xDF);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFCHS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xE0D9);

   token.read();
}

void AMD64Assembler :: compileFILD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();

   if (token.check("dword")) {
	   Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

	   if (test(sour.type, AMD64Helper::otM32)) {
		   code->writeByte(0xDB);
		   AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), sour);
	   }
	   else token.raiseErr("Invalid command (%d)");
   }
   else if (token.check("qword")) {
	   Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   if (test(sour.type, AMD64Helper::otM64)) {
		   code->writeByte(0xDF);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), sour);
	   }
	   else token.raiseErr("Invalid command (%d)");
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFIST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otM64)) {
	   code->writeByte(0xDB);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 2), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler:: compileFISTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();

   if (token.check("dword")) {
	   Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

	   if (test(sour.type, AMD64Helper::otM32)) {
		   code->writeByte(0xDB);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 3), sour);
	   }
	   else token.raiseErr("Invalid command (%d)");
   }
   else if (token.check("qword")) {
	   Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

	   if (test(sour.type, AMD64Helper::otM32)) {
		   code->writeByte(0xDF);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 7), sour);
	   }
	   else token.raiseErr("Invalid command (%d)");
   }
}

void AMD64Assembler :: compileFLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("st")) {
	   int index = readStReg(token);

	   code->writeByte(0xD9);
	   code->writeByte(0xC0 + index);

	   token.read();
   }
   else if (token.check("qword")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   code->writeByte(0xDD);
	   AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFXCH(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("st")) {
	   int index = readStReg(token);

	   token.read();

	   code->writeByte(0xD9);
	   code->writeByte(0xC8 + index);
   }
   else {
	   code->writeByte(0xD9);
	   code->writeByte(0xC9);
   }
}

void AMD64Assembler :: compileFSUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("st")) {
	   int sour = readStReg(token);

	   token.read(",","',' comma expected(%d)\n");

	   if (sour != 0) {
		   token.read("st", "'st' expected (%d)\n");
		   if (readStReg(token)!=0)
			   token.raiseErr("'0' expected (%d)");

		   token.read();

		   code->writeByte(0xDC);
		   code->writeByte(0xE8 + sour);
	   }
	   else {
		   token.read("st", "'st' expected (%d)\n");
		   sour = readStReg(token);
		   token.read();

		   code->writeByte(0xD8);
		   code->writeByte(0xE0 + sour);
	   }
   }
   else if (token.check("qword")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   code->writeByte(0xDC);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFISUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("dword")) {
      Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

      if (test(sour.type, AMD64Helper::otM32)) {
         code->writeByte(0xDA);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 4), sour);
      }
      else token.raiseErr("Invalid command (%d)");
   }
   else token.raiseErr("Invalid command (%d)");
}


void AMD64Assembler :: compileFIMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("dword")) {
      Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

      if (test(sour.type, AMD64Helper::otM32)) {
         code->writeByte(0xDA);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 1), sour);
      }
      else token.raiseErr("Invalid command (%d)");
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFADD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("st")) {
	   int sour = readStReg(token);

	   token.read(",","',' comma expected(%d)\n");

	   if (sour != 0) {
		   token.read("st", "'st' expected (%d)\n");
		   if (readStReg(token)!=0)
			   token.raiseErr("'0' expected (%d)");

		   token.read();

		   code->writeByte(0xDC);
		   code->writeByte(0xC0 + sour);
	   }
	   else {
		   token.read("st", "'st' expected (%d)\n");
		   sour = readStReg(token);
		   token.read();

		   code->writeByte(0xD8);
		   code->writeByte(0xC0 + sour);
	   }
   }
   else if (token.check("qword")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   code->writeByte(0xDC);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 0), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
//	if (token.check("st")) {
//		int sour = readStReg(token);
//
//		token.read(",", "',' comma expected(%d)\n");
//
//		if (sour != 0) {
//			token.read("st", "'st' expected (%d)\n");
//			if (readStReg(token)!=0)
//				token.raiseErr("'0' expected (%d)");
//
//			token.read();
//
//			code->writeByte(0xDC);
//			code->writeByte(0xC8 + sour);
//		}
//		else {
//			token.read("st", "'st' expected (%d)\n");
//			sour = readStReg(token);
//			token.read();
//
//			code->writeByte(0xD8);
//			code->writeByte(0xC8 + sour);
//		}
//	}
   /*else */if (token.check("qword")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   code->writeByte(0xDC);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR64 + 1), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
//	if (token.check("st")) {
//		int sour = readStReg(token);
//
//		token.read(",","',' comma expected(%d)\n");
//
//		if (sour != 0) {
//			token.read("st", "'st' expected (%d)\n");
//			if (readStReg(token)!=0)
//				token.raiseErr("'0' expected (%d)");
//
//			token.read();
//
//			code->writeByte(0xDC);
//			code->writeByte(0xF8 + sour);
//		}
//		else {
//			token.read("st", "'st' expected (%d)\n");
//			sour = readStReg(token);
//			token.read();
//
//			code->writeByte(0xD8);
//			code->writeByte(0xF0 + sour);
//		}
//	}
   /*else */if (token.check("qword")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

	   code->writeByte(0xDC);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 6), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFIDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("dword")) {
      Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

      if (test(sour.type, AMD64Helper::otM32)) {
         code->writeByte(0xDA);
         AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 6), sour);
      }
      else token.raiseErr("Invalid command (%d)");
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFCOMIP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("st")) {
	   token.read(",", "',' expected (%d)\n");
	   token.read("st", "'st' expected (%d)\n");

	   int dest = readStReg(token);

	   token.read();

	   code->writeByte(0xDF);
	   code->writeByte(0xF0 + dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileFCOMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	token.read();
//	if (token.check("st")) {
//		token.read(",", "',' expected (%d)\n");
//		token.read("st", "'st' expected (%d)\n");
//
//		int dest = readStReg(token);
//
//		token.read();
//
//		code->writeByte(0xD8);
//		code->writeByte(0xD8 + dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileFSTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("st")) {
	   int sour = readStReg(token);
	   token.read();

	   code->writeByte(0xDD);
	   code->writeByte(0xD8 + sour);
   }
   else if (token.check("dword")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

	   code->writeByte(0xD9);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 3), operand);
   }
   else if (token.check("qword")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   code->writeByte(0xDD);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR64 + 3), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFBSTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("tbyte")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM32);

	   code->writeByte(0xDF);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 6), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFSTSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("ax")) {
	   token.read();

	   code->writeByte(0x9B);
	   code->writeWord(0xE0DF);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileFNSTSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	token.read();
//	if (token.check("ax")) {
//		token.read();
//
//		code->writeWord(0xE0DF);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileFINIT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x9B);
   code->writeWord(0xE3DB);

   token.read();
}

void AMD64Assembler :: compileFSTCW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("word")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   code->writeByte(0x9B);
	   code->writeByte(0xD9);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 7), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFLDCW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("word")) {
	   Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", AMD64Helper::otM64);

	   code->writeByte(0xD9);
	   AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR32 + 5), operand);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileFLDZ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xEED9);

   token.read();
}

void AMD64Assembler :: compileFLD1(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xE8D9);

   token.read();
}

void AMD64Assembler :: compileFADDP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xC1DE);

   token.read();
}

//void x86Assembler :: compileFSUBP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	code->writeWord(0xE9DE);
//
//	token.read();
//}
//
//void x86Assembler :: compileFDIVP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	code->writeWord(0xF9DE);
//
//	token.read();
//}

void AMD64Assembler :: compileF2XM1(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xF0D9);

   token.read();
}

void AMD64Assembler :: compileFLDL2T(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xE9D9);

   token.read();
}

void AMD64Assembler :: compileFLDLG2(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xECD9);

   token.read();
}

void AMD64Assembler :: compileFRNDINT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xFCD9);

   token.read();
}

void AMD64Assembler :: compileFSCALE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xFDD9);

   token.read();
}

void AMD64Assembler :: compileFXAM(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xE5D9);

   token.read();
}

void AMD64Assembler :: compileFMULP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xC9DE);

   token.read();
}

void AMD64Assembler :: compileFABS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xE1D9);

   token.read();
}

void AMD64Assembler :: compileFSQRT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xFAD9);

   token.read();
}

void AMD64Assembler :: compileFSIN(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xFED9);

   token.read();
}

void AMD64Assembler :: compileFCOS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xFFD9);

   token.read();
}

void AMD64Assembler :: compileFYL2X(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xF1D9);

   token.read();
}

void AMD64Assembler :: compileFTST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xE4D9);

   token.read();
}

void AMD64Assembler :: compileFLDPI(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xEBD9);

   token.read();
}

void AMD64Assembler :: compileFPREM(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xF8D9);

   token.read();
}

void AMD64Assembler :: compileFPATAN(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xF3D9);

   token.read();
}

void AMD64Assembler :: compileFLDL2E(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	   code->writeWord(0xEAD9);

	   token.read();
}

void AMD64Assembler :: compileFLDLN2(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeWord(0xEDD9);

   token.read();
}

void AMD64Assembler :: compileFFREE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   token.read();
   if (token.check("st")) {
	   int sour = readStReg(token);
	   token.read();

	   code->writeByte(0xDD);
	   code->writeByte(0xC0 + sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileCMPXCHG(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//
//	checkComma(token);
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//   if (prefix.lockMode) {
//      code->writeByte(0xF0);
//   }
//   else if(prefix.Exists()) {
//      token.raiseErr("Invalid Prefix (%d)");
//   }
//
//	if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
//      code->writeByte(0x0F);
//		code->writeByte(0xB1);
//		x86Helper::writeModRM(code, dest, sour);
//	}
//   else if ((test(sour.type, x86Helper::otR8) || test(sour.type, x86Helper::otM8)) && test(dest.type, x86Helper::otR8)) {
//      code->writeByte(0x0F);
//      code->writeByte(0xB0);
//      x86Helper::writeModRM(code, dest, sour);
//   }
//   else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler :: compileSETCC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int postfix)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, AMD64Helper::otR8)) {
	   code->writeByte(0x0F);
      code->writeByte(0x90 + postfix);
      AMD64Helper::writeModRM(code, Operand(AMD64Helper::otR8 + 0), sour);
   }
   else token.raiseErr("Invalid command (%d)");
}

void AMD64Assembler :: compileCMOVCC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int postfix)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (test(sour.type, AMD64Helper::otR32) && (test(dest.type, AMD64Helper::otR32) || test(dest.type, AMD64Helper::otM32) )) {
      code->writeByte(0x0F);
      code->writeByte(0x40 + postfix);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else if (test(sour.type, AMD64Helper::otR64) && (test(dest.type, AMD64Helper::otR64) || test(dest.type, AMD64Helper::otM64))) {
      code->writeByte(0x48);
      code->writeByte(0x0F);
      code->writeByte(0x40 + postfix);
      AMD64Helper::writeModRM(code, sour, dest);
   }
   else token.raiseErr("Invalid command (%d)");
}

//void x86Assembler :: compileCMPSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: F3 0F C2 /r ib
//	// Mnemonic: CMPSS xmm1, xmm2/m32, imm8
//	// f3 0f c2 ca 00 -- cmpss xmm1, xmm2, 0 -- this instruction is the same of: CMPEQSS xmm1, xmm2
//	// f3 0f c2 ca 01 -- cmpss xmm1, xmm2, 1 -- this instruction is the same of: CMPLTSS xmm1, xmm2
//	// f3 0f c2 ca 02 -- cmpss xmm1, xmm2, 2 -- this instruction is the same of: CMPLESS xmm1, xmm2
//	// f3 0f c2 ca 03 -- cmpss xmm1, xmm2, 3 -- this instruction is the same of: CMPUNORDSS xmm1, xmm2
//	// f3 0f c2 ca 04 -- cmpss xmm1, xmm2, 4 -- this instruction is the same of: CMPNEQSS xmm1, xmm2
//	// f3 0f c2 ca 05 -- cmpss xmm1, xmm2, 5 -- this instruction is the same of: CMPNLTSS xmm1, xmm2
//	// f3 0f c2 ca 06 -- cmpss xmm1, xmm2, 6 -- this instruction is the same of: CMPNLESS xmm1, xmm2
//	// f3 0f c2 ca 07 -- cmpss xmm1, xmm2, 7 -- this instruction is the same of: CMPORDSS xmm1, xmm2
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//	checkComma(token);
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	token.read();
//	Operand imm = defineOperand(token, info, "Invalid constant");
//	token.read();
//
//	if (test(dest.type, x86Helper::otX128) && (test(sour.type, x86Helper::otX128) || test(sour.type, x86Helper::otM32))) {
//		if (imm.offset < 0 && imm.offset > 7) token.raiseErr("imm value too large in asm instruction");
//		code->writeByte(0xF3);
//		code->writeByte(0x0F);
//		code->writeByte(0xC2);
//		x86Helper::writeModRM(code, dest, sour);
//		x86Helper::writeImm(code, imm);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileCMPPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F C2 /r ib
//	// Mnemonic: CMPPS xmm1, xmm2/m128, imm8
//	// 0f c2 ca 00 -- cmpps xmm1, xmm2, 0 -- this instruction is the same of: CMPEQPS xmm1, xmm2
//	// 0f c2 ca 01 -- cmpps xmm1, xmm2, 1 -- this instruction is the same of: CMPLTPS xmm1, xmm2
//	// 0f c2 ca 02 -- cmpps xmm1, xmm2, 2 -- this instruction is the same of: CMPLEPS xmm1, xmm2
//	// 0f c2 ca 03 -- cmpps xmm1, xmm2, 3 -- this instruction is the same of: CMPUNORDPS xmm1, xmm2
//	// 0f c2 ca 04 -- cmpps xmm1, xmm2, 4 -- this instruction is the same of: CMPNEQPS xmm1, xmm2
//	// 0f c2 ca 05 -- cmpps xmm1, xmm2, 5 -- this instruction is the same of: CMPNLTPS xmm1, xmm2
//	// 0f c2 ca 06 -- cmpps xmm1, xmm2, 6 -- this instruction is the same of: CMPNLEPS xmm1, xmm2
//	// 0f c2 ca 07 -- cmpps xmm1, xmm2, 7 -- this instruction is the same of: CMPORDPS xmm1, xmm2
//
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//	checkComma(token);
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	token.read();
//	Operand imm = defineOperand(token, info, "Invalid constant");
//	token.read();
//
//	if (test(dest.type, x86Helper::otX128) && (test(sour.type, x86Helper::otX128) || test(sour.type, x86Helper::otM32))) {
//		if (imm.offset < 0 && imm.offset > 7) token.raiseErr("imm value too large in asm instruction");
//		code->writeByte(0x0F);
//		code->writeByte(0xC2);
//		x86Helper::writeModRM(code, dest, sour);
//		x86Helper::writeImm(code, imm);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}
//
//void x86Assembler :: compileCOMISS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
//{
//	// Opcode: 0F 2F /r
//	// Mnemonic: COMISS xmm1, xmm2/m32
//	// 0f 2f ca -- comiss xmm1, xmm2
//	// 0f 2f 08 -- comiss xmm1, XMMWORD PTR[eax]
//
//	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");
//	checkComma(token);
//	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
//
//	if (test(sour.type, x86Helper::otX128) && (test(dest.type, x86Helper::otX128) || test(dest.type, x86Helper::otM32))) {
//		code->writeByte(0x0F);
//		code->writeByte(0x2F);
//		x86Helper::writeModRM(code, sour, dest);
//	}
//	else token.raiseErr("Invalid command (%d)");
//}

void AMD64Assembler:: compileStructure(TokenInfo& token, _Module* binary, int mask)
{
	token.read();

   int ref = 0;
   if (token.check("%")) {
      ref = token.readInteger(constants);
   }
   else {
      ReferenceNs refName(NATIVE_MODULE, token.value);

	   ref = binary->mapReference(refName) | mask;
   }

   ProcedureInfo info(binary, false);

   if (binary->mapSection(ref, true)!=NULL) {
      throw AssemblerException("Structure / Procedure already exists (%d)\n", token.terminal.row);
   }
   _Memory* code = binary->mapSection(ref, false);
   MemoryWriter writer(code);

   token.read();
   while (!token.check("end")) {
      if (token.check("dd")) {
         token.read();
         Operand operand = defineOperand(token, info, "Invalid constant");
         if (operand.type != AMD64Helper::otUnknown) {
            token.read();

            if (token.check("+")) {
               operand.offset += token.readInteger(constants);

               token.read();
            }
         }

         if (operand.type== AMD64Helper::otDD) {
            AMD64Helper::writeImm(&writer, operand);
         }
         else if (operand.type== AMD64Helper::otDB) {
            operand.type = AMD64Helper::otDD;
            AMD64Helper::writeImm(&writer, operand);
         }
         else token.raiseErr("Invalid operand (%d)");
      }
      else if (token.check("dq")) {
         token.read();
         Operand operand = defineOperand(token, info, "Invalid constant");
         if (operand.type != AMD64Helper::otUnknown) {
            token.read();

            if (token.check("+")) {
               operand.offset += token.readInteger(constants);

               token.read();
            }
         }

         if (operand.type == AMD64Helper::otDQ) {
            AMD64Helper::writeImm(&writer, operand);
         }
         else if (operand.type == AMD64Helper::otDD) {
            if (!operand.reference) {
               operand.type = AMD64Helper::otDQ;
               AMD64Helper::writeImm(&writer, operand);
            }
            else {
               AMD64Helper::writeImm(&writer, operand);
               writer.writeDWord(0);
            }
         }
         else if (operand.type == AMD64Helper::otDB) {
            operand.type = AMD64Helper::otDQ;
            AMD64Helper::writeImm(&writer, operand);
         }
         else token.raiseErr("Invalid operand (%d)");
      }
      else if (token.check("dw")) {
         token.read();
         Operand operand = defineOperand(token, info, "Invalid constant");
         if (operand.type != AMD64Helper::otUnknown)
            token.read();

         if (operand.type== AMD64Helper::otDB) {
            operand.type = AMD64Helper::otDW;
            AMD64Helper::writeImm(&writer, operand);
         }
         else token.raiseErr("Invalid operand (%d)");
      }
      else if (token.check("db")) {
         token.read();
         Operand operand = defineOperand(token, info, "Invalid constant");
         if (operand.type != AMD64Helper::otUnknown)
            token.read();

         if (operand.type== AMD64Helper::otDB) {
            AMD64Helper::writeImm(&writer, operand);
         }
         else token.raiseErr("Invalid operand (%d)");
      }
      else if (token.Eof()) {
         token.raiseErr("Invalid end of the file\n");
      }
      else token.raiseErr("Invalid operand (%d)");
   }
}

bool AMD64Assembler :: compileCommandA(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("add")) {
      compileADD(token, info, &writer);
      return true;
   }
   else if (token.check("and")) {
      compileAND(token, info, &writer);
      return true;
   }
   else if (token.check("adc")) {
      compileADC(token, info, &writer);
      return true;
   }
//   // SSE instructions
//   else if (token.check("addps")) {
//	   compileADDPS(token, info, &writer);
//	   return true;
//   }
//   else if (token.check("addss")) {
//	   compileADDSS(token, info, &writer);
//	   return true;
//   }
//   else if (token.check("andps")) {
//	   compileANDPS(token, info, &writer);
//	   return true;
//   }
//   else if (token.check("andnps")) {
//	   compileANDNPS(token, info, &writer);
//	   return true;
//   }
   else return false;
}
//bool x86Assembler :: compileCommandB(TokenInfo& token)
//{
//   return false;
//}
bool AMD64Assembler :: compileCommandC(/*PrefixInfo& prefix, */TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer/*, x86JumpHelper& helper*/)
{
   if (token.check("cmp")) {
      compileCMP(token, info, &writer);
      return true;
   }
   else if (token.check("call")) {
      compileCALL(token, info, &writer/*, helper*/);
      return true;
   }
   else if (token.check("cqo")) {
      compileCQO(token, info, &writer);
      return true;
   }
   else if (token.check("cdq")) {
      compileCDQ(token, info, &writer);
      return true;
   }
   //else if (token.check("cmpsb")) {
   //   compileCMPSB(token, info, &writer);
   //   return true;
   //}
//   else if (token.check("cmpxchg")) {
//      compileCMPXCHG(prefix, token, info, &writer);
//      prefix.clear();
//      return true;
//   }
   else if (token.check("cmovle")) {
      compileCMOVCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JLE);
      return true;
   }
   else if (token.check("cmovnz")) {
      compileCMOVCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JNZ);
      return true;
   }
   else if (token.check("cmovz")) {
      compileCMOVCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JZ);
      return true;
   }
   else if (token.check("cmovl")) {
      compileCMOVCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JL);
      return true;
   }
   else if (token.check("cmovg")) {
      compileCMOVCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JG);
      return true;
   }
   else if (token.check("cmovge")) {
      compileCMOVCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JGE);
      return true;
   }
   else if (token.check("cmovs")) {
      compileCMOVCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JS);
      return true;
   }
   else if (token.check("cwde")) {
      compileCWDE(token, info, &writer);
      return true;
   }
//	// SSE instructions
//	else if (token.check("cmpss")) {
//		compileCMPSS(token, info, &writer);
//		return true;
//	}
   else return false;
}
bool AMD64Assembler :: compileCommandD(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
//	if (token.check("dec")) {
//		compileDEC(token, info, &writer);
//      return true;
//	}
   /*else */if (token.check("div")) {
	   compileDIV(token, info, &writer);
      return true;
   }
//	// SSE instructions
//	else if (token.check("divps")) {
//		compileDIVPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("divss")) {
//		compileDIVSS(token, info, &writer);
//		return true;
//	}
   else return false;
}
//bool x86Assembler :: compileCommandE(TokenInfo& token)
//{
//   return false;
//}
bool AMD64Assembler :: compileCommandF(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("finit")) {
      compileFINIT(token, info, &writer);
      return true;
   }
   else if (token.check("fldz")) {
	   compileFLDZ(token, info, &writer);
      return true;
   }
   else if (token.check("fld1")) {
	   compileFLD1(token, info, &writer);
      return true;
   }
   else if (token.check("f2xm1")) {
	   compileF2XM1(token, info, &writer);
      return true;
   }
   else if (token.check("fbld")) {
	   compileFBLD(token, info, &writer);
      return true;
   }
   else if (token.check("fchs")) {
	   compileFCHS(token, info, &writer);
      return true;
   }
   else if (token.check("fild")) {
	   compileFILD(token, info, &writer);
      return true;
   }
   else if (token.check("fist")) {
	   compileFIST(token, info, &writer);
      return true;
   }
   else if (token.check("fistp")) {
	   compileFISTP(token, info, &writer);
      return true;
   }
   else if (token.check("fld")) {
	   compileFLD(token, info, &writer);
      return true;
   }
   else if (token.check("fadd")) {
	   compileFADD(token, info, &writer);
      return true;
   }
   else if (token.check("fsub")) {
	   compileFSUB(token, info, &writer);
      return true;
   }
   else if (token.check("fisub")) {
      compileFISUB(token, info, &writer);
      return true;
   }
   else if (token.check("fcomip")) {
	   compileFCOMIP(token, info, &writer);
      return true;
   }
//	else if (token.check("fcomp")) {
//		compileFCOMP(token, info, &writer);
//      return true;
//	}
   else if (token.check("fmulp")) {
	   compileFMULP(token, info, &writer);
      return true;
   }
   else if (token.check("fmul")) {
	   compileFMUL(token, info, &writer);
      return true;
   }
   else if (token.check("fimul")) {
      compileFIMUL(token, info, &writer);
      return true;
   }
   else if (token.check("fdiv")) {
	   compileFDIV(token, info, &writer);
      return true;
   }
   else if (token.check("fidiv")) {
      compileFIDIV(token, info, &writer);
      return true;
   }
   else if (token.check("faddp")) {
	   compileFADDP(token, info, &writer);
      return true;
   }
//	else if (token.check("fsubp")) {
//		compileFSUBP(token, info, &writer);
//      return true;
//	}
//	else if (token.check("fmulp")) {
//		compileFMULP(token, info, &writer);
//      return true;
//	}
//	else if (token.check("fdivp")) {
//		compileFDIVP(token, info, &writer);
//      return true;
//	}
   else if (token.check("fldl2t")) {
	   compileFLDL2T(token, info, &writer);
      return true;
   }
   else if (token.check("fldlg2")) {
	   compileFLDLG2(token, info, &writer);
      return true;
   }
   else if (token.check("frndint")) {
	   compileFRNDINT(token, info, &writer);
      return true;
   }
   else if (token.check("fxch")) {
	   compileFXCH(token, info, &writer);
      return true;
   }
   else if (token.check("fstp")) {
	   compileFSTP(token, info, &writer);
      return true;
   }
   else if (token.check("fbstp")) {
	   compileFBSTP(token, info, &writer);
      return true;
   }
   else if (token.check("fstsw")) {
	   compileFSTSW(token, info, &writer);
      return true;
   }
//	else if (token.check("fnstsw")) {
//		compileFNSTSW(token, info, &writer);
//      return true;
//	}
   else if (token.check("fstcw")) {
	   compileFSTCW(token, info, &writer);
      return true;
   }
   else if (token.check("fldcw")) {
	   compileFLDCW(token, info, &writer);
      return true;
   }
   else if (token.check("fscale")) {
	   compileFSCALE(token, info, &writer);
      return true;
   }
   else if (token.check("fxam")) {
	   compileFXAM(token, info, &writer);
      return true;
   }
   else if (token.check("fabs")) {
	   compileFABS(token, info, &writer);
      return true;
   }
   else if (token.check("fsqrt")) {
	   compileFSQRT(token, info, &writer);
      return true;
   }
   else if (token.check("fcos")) {
	   compileFCOS(token, info, &writer);
      return true;
   }
   else if (token.check("fsin")) {
	   compileFSIN(token, info, &writer);
      return true;
   }
   else if (token.check("fyl2x")) {
	   compileFYL2X(token, info, &writer);
      return true;
   }
   else if (token.check("ftst")) {
	   compileFTST(token, info, &writer);
      return true;
   }
	else if (token.check("fldpi")) {
	   compileFLDPI(token, info, &writer);
      return true;
	}
	else if (token.check("fprem")) {
	   compileFPREM(token, info, &writer);
      return true;
	}
	else if (token.check("fpatan")) {
	   compileFPATAN(token, info, &writer);
      return true;
	}
   else if (token.check("fldl2e")) {
	   compileFLDL2E(token, info, &writer);
      return true;
   }
   else if (token.check("fldln2")) {
	   compileFLDLN2(token, info, &writer);
      return true;
   }
   else if (token.check("ffree")) {
	   compileFFREE(token, info, &writer);
      return true;
   }
   else return false;
}
//bool x86Assembler :: compileCommandG(TokenInfo& token)
//{
//   return false;
//}
//bool x86Assembler :: compileCommandH(TokenInfo& token)
//{
//   return false;
//}
bool AMD64Assembler :: compileCommandI(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("inc")) {
	   compileINC(token, info, &writer);
      return true;
   }
//   else if (token.check("int")) {
//      compileINT(token, info, &writer);
//      return true;
//   }
   else if (token.check("imul")) {
	   compileIMUL(token, info, &writer);
      return true;
   }
   else if (token.check("idiv")) {
	   compileIDIV(token, info, &writer);
      return true;
   }
   else return false;
}

bool AMD64Assembler :: compileCommandJ(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, AMD64JumpHelper& helper)
{
   if (token.check("jb")||token.check("jc")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JB, helper);
   return true;
   }
   else if (token.check("jnb")||token.check("jnc")||token.check("jae")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JAE, helper);
   return true;
   }
   else if (token.check("jz")||token.check("je")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JZ, helper);
   return true;
   }
   else if (token.check("jnz")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JNZ, helper);
   return true;
   }
   else if (token.check("jbe") || token.check("jna")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JBE, helper);
   return true;
   }
   else if (token.check("ja")||token.check("jnbe")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JA, helper);
   return true;
   }
   else if (token.check("js")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JS, helper);
   return true;
   }
   else if (token.check("jns")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JNS, helper);
   return true;
   }
   else if (token.check("jpe") || token.check("jp")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JP, helper);
   return true;
   }
   else if (token.check("jpo") || token.check("jnp")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JPO, helper);
   return true;
   }
   else if (token.check("jl")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JL, helper);
   return true;
   }
   else if (token.check("jge")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JGE, helper);
   return true;
   }
   else if (token.check("jle")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JLE, helper);
   return true;
   }
   else if (token.check("jg")) {
      compileJxx(token, info, &writer, AMD64Helper::JUMP_TYPE_JG, helper);
      return true;
   }
   /*else */if (token.check("jmp")) {
      compileJMP(token, info, &writer, helper);
      return true;
   }
   else return false;
}
//bool x86Assembler :: compileCommandK(TokenInfo& token)
//{
//   return false;
//}

bool AMD64Assembler :: compileCommandL(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer/*, x86JumpHelper& helper*/)
{
	if (token.check("lea")) {
		compileLEA(token, info, &writer);
      return true;
	}
//	else if (token.check("loop")) {
//		compileLOOP(token, info, &writer, helper);
//      return true;
//	}
	else if (token.check("lodsd")) {
	   compileLODSD(token, info, &writer);
      return true;
	}
	else if (token.check("lodsw")) {
	   compileLODSW(token, info, &writer);
      return true;
	}
   else if (token.check("lodsb")) {
	   compileLODSB(token, info, &writer);
      return true;
   }
//   else if (token.check("lock")) {
//      prefix.lockMode = true;
//
//      token.read();
//
//      return true;
//   }
   else return false;
}

bool AMD64Assembler :: compileCommandM(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("mov")) {
      compileMOV(token, info, &writer);
      return true;
   }
	else if (token.check("mul")) {
	   compileMUL(token, info, &writer);
      return true;
	}
   else if (token.check("movzx")) {
	   compileMOVZX(token, info, &writer);
      return true;
   }
   else if (token.check("movsb")) {
	   compileMOVSB(token, info, &writer);
      return true;
   }
   else if (token.check("movsd")) {
	   compileMOVSD(token, info, &writer);
      return true;
   }
   else if (token.check("movsxd")) {
      compileMOVSXD(token, info, &writer);
      return true;
   }
   //	// SSE instructions
//	else if (token.check("movaps")) {
//		compileMOVAPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("movups")) {
//		compileMOVUPS(token, info, &writer);
//		return true;
//	}
//
//	else if (token.check("mulps")) {
//		compileMULPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("mulss")) {
//		compileMULSS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("maxps")) {
//		compileMAXPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("maxss")) {
//		compileMAXSS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("minps")) {
//		compileMINPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("minss")) {
//		compileMINSS(token, info, &writer);
//		return true;
//	}
   else return false;
}
bool AMD64Assembler :: compileCommandN(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("neg")) {
	   compileNEG(token, info, &writer);
      return true;
   }
   else if (token.check("not")) {
	   compileNOT(token, info, &writer);
      return true;
   }
   else if (token.check("nop")) {
	   compileNOP(token, info, &writer);
      return true;
   }
   else return false;
}

bool AMD64Assembler :: compileCommandO(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("or")) {
	   compileOR(token, info, &writer);
      return true;
   }
   else return false;
}

bool AMD64Assembler :: compileCommandP(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("push")) {
      compilePUSH(token, info, &writer);
      return true;
   }
   else if (token.check("pop")) {
      compilePOP(token, info, &writer);
      return true;
   }
   else if (token.check("pushfd")) {
	   compilePUSHFD(token, info, &writer);
      return true;
   }
   else if (token.check("popfd")) {
      compilePOPFD(token, info, &writer);
      return true;
   }
//	// SSE instructions
//	else if (token.check("pavgb")) {
//		compilePAVGB(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pavgw")) {
//		compilePAVGW(token, info, &writer);
//		return true;
//	}
//	else if (token.check("psadbw")) {
//		compilePSADBW(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pextrw")) {
//		compilePEXTRW(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pinsrw")) {
//		compilePINSRW(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pmaxsw")) {
//		compilePMAXSW(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pmaxub")) {
//		compilePMAXUB(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pminsw")) {
//		compilePMINSW(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pminub")) {
//		compilePMINUB(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pmovmskb")) {
//		compilePMOVMSKB(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pmulhuw")) {
//		compilePMULHUW(token, info, &writer);
//		return true;
//	}
//	else if (token.check("pshufw")) {
//		compilePSHUFW(token, info, &writer);
//		return true;
//	}
   else return false;
}

//bool x86Assembler :: compileCommandQ(TokenInfo& token)
//{
//   return false;
//}
bool AMD64Assembler :: compileCommandR(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("ret")) {
      compileRET(token, info, &writer);
      return true;
   }
   else if (token.check("rol")) {
	   compileROL(token, info, &writer);
      return true;
   }
   else if (token.check("ror")) {
	   compileROR(token, info, &writer);
      return true;
   }
   else if (token.check("rcr")) {
	   compileRCR(token, info, &writer);
      return true;
   }
   else if (token.check("rcl")) {
	   compileRCL(token, info, &writer);
      return true;
   }
   else if (token.check("rep")) {
	   compileREP(token, info, &writer);
      return true;
   }
//	else if (token.check("repz")) {
//		compileREPZ(token, info, &writer);
//      return true;
//	}
//
//	// SSE instructions
//	else if (token.check("rcpps")) {
//		compileRCPPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("rcpss")) {
//		compileRCPSS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("rsqrtps")) {
//		compileRSQRTPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("rsqrtss")) {
//		compileRSQRTSS(token, info, &writer);
//		return true;
//	}
   else return false;
}

bool AMD64Assembler :: compileCommandS(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("sub")) {
      compileSUB(token, info, &writer);
      return true;
   }
   else if (token.check("sahf")) {
	   compileSAHF(token, info, &writer);
      return true;
   }
   else if (token.check("sbb")) {
	   compileSBB(token, info, &writer);
      return true;
   }
   else if (token.check("shr")) {
	   compileSHR(token, info, &writer);
      return true;
   }
//	else if (token.check("sar")) {
//		compileSAR(token, info, &writer);
//      return true;
//	}
   else if (token.check("shl")) {
	   compileSHL(token, info, &writer);
      return true;
   }
   else if (token.check("stc")) {
	   compileSTC(token, info, &writer);
      return true;
   }
   else if (token.check("stosq") || token.check("stos")) {
      compileSTOSQ(token, info, &writer);
      return true;
   }
   else if (token.check("stosd")) {
	   compileSTOSD(token, info, &writer);
      return true;
   }
   else if (token.check("stosb")) {
	   compileSTOSB(token, info, &writer);
      return true;
   }
   else if (token.check("stosw")) {
	   compileSTOSW(token, info, &writer);
      return true;
   }
//	else if (token.check("shld")) {
//		compileSHLD(token, info, &writer);
//      return true;
//	}
//	else if (token.check("shrd")) {
//		compileSHRD(token, info, &writer);
//      return true;
//	}
   else if (token.check("setz")) {
      compileSETCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JZ);
      return true;
   }
   else if (token.check("sete")) {
      compileSETCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JE);
      return true;
   }
   else if (token.check("setl")) {
      compileSETCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JL);
      return true;
   }
//   else if (token.check("setc")) {
//		compileSETCC(token, info, &writer, x86Helper::JUMP_TYPE_JB);
//      return true;
//	}
//	else if (token.check("setg")) {
//		compileSETCC(token, info, &writer, x86Helper::JUMP_TYPE_JG);
//      return true;
//	}
   else if (token.check("setnc")) {
	   compileSETCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JAE);
      return true;
   }
   else if (token.check("setb")) {
      compileSETCC(token, info, &writer, AMD64Helper::JUMP_TYPE_JB);
      return true;
   }
//	// SSE instructions
//	else if (token.check("subps")) {
//		compileSUBPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("subss")) {
//		compileSUBSS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("sqrtps")) {
//		compileSQRTPS(token, info, &writer);
//		return true;
//	}
//	else if (token.check("sqrtss")) {
//		compileSQRTSS(token, info, &writer);
//		return true;
//	}
   else return false;
}
bool AMD64Assembler :: compileCommandT(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("test")) {
      compileTEST(token, info, &writer);
      return true;
   }
   else return false;
}
//bool x86Assembler :: compileCommandU(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
//{
//	if (token.check("ucomiss")) {
//		compileUCOMISS(token, info, &writer);
//		return true;
//	}
//   return false;
//}
//bool x86Assembler :: compileCommandV(TokenInfo& token)
//{
//   return false;
//}
//bool x86Assembler :: compileCommandW(TokenInfo& token)
//{
//   return false;
//}
bool AMD64Assembler :: compileCommandX(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("xor")) {
      compileXOR(token, info, &writer);
      return true;
   }
   else if (token.check("xchg")) {
	   compileXCHG(token, info, &writer);
      return true;
   }
//	else if (token.check("xadd")) {
//		compileXADD(prefix, token, info, &writer);
//      prefix.clear();
//      return true;
//	}
//
//	// SSE instructions
//	else if (token.check("xorps")) {
//		compileXORPS(token, info, &writer);
//		prefix.clear();
//		return true;
//	}
   else return false;
}

//bool x86Assembler :: compileCommandY(TokenInfo& token)
//{
//   return false;
//}
//bool x86Assembler :: compileCommandZ(TokenInfo& token)
//{
//   return false;
//}

bool AMD64Assembler :: compileCommand(/*PrefixInfo& prefix, */TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, 
   AMD64JumpHelper& helper)
{
   bool recognized = false;
   if (token.value[0]=='a') {
      recognized = compileCommandA(token, info, writer);
   }
//   else if (token.value[0]=='b') {
//      recognized = compileCommandB(token);
//   }
   else if (token.value[0]=='c') {
      recognized = compileCommandC(/*prefix, */token, info, writer/*, helper*/);
   }
   else if (token.value[0]=='d') {
      recognized = compileCommandD(token, info, writer);
   }
//   else if (token.value[0]=='e') {
//      recognized = compileCommandE(token);
//   }
   else if (token.value[0]=='f') {
      recognized = compileCommandF(token, info, writer);
   }
//   else if (token.value[0]=='g') {
//      recognized = compileCommandG(token);
//   }
//   else if (token.value[0]=='h') {
//      recognized = compileCommandH(token);
//   }
   else if (token.value[0]=='i') {
      recognized = compileCommandI(token, info, writer);
   }
   else if (token.value[0]=='j') {
      recognized = compileCommandJ(token, info, writer, helper);
   }
//   else if (token.value[0]=='k') {
//      recognized = compileCommandK(token);
//   }
   else if (token.value[0]=='l') {
      recognized = compileCommandL(token, info, writer/*, helper*/);
   }
   else if (token.value[0]=='m') {
      recognized = compileCommandM(token, info, writer);
   }
   else if (token.value[0]=='n') {
      recognized = compileCommandN(token, info, writer);
   }
   else if (token.value[0]=='o') {
      recognized = compileCommandO(token, info, writer);
   }
   else if (token.value[0]=='p') {
      recognized = compileCommandP(token, info, writer);
   }
//   else if (token.value[0]=='q') {
//      recognized = compileCommandQ(token);
//   }
   else if (token.value[0]=='r') {
      recognized = compileCommandR(token, info, writer);
   }
   else if (token.value[0]=='s') {
      recognized = compileCommandS(token, info, writer);
   }
   else if (token.value[0]=='t') {
      recognized = compileCommandT(token, info, writer);
   }
//   else if (token.value[0]=='u') {
//      recognized = compileCommandU(token, info, writer);
//   }
//   else if (token.value[0]=='v') {
//      recognized = compileCommandV(token);
//   }
//   else if (token.value[0]=='w') {
//      recognized = compileCommandW(token);
//   }
   else if (token.value[0]=='x') {
      recognized = compileCommandX(token, info, writer);
   }
//   else if (token.value[0]=='y') {
//      recognized = compileCommandY(token);
//   }
//   else if (token.value[0]=='z') {
//      recognized = compileCommandZ(token);
//   }
   else if (token.value[0] == '#') {
      token.read();
      token.read();
      token.read();
      recognized = true;
   }

   if (!recognized) {
      if (token.Eof()) {
         token.raiseErr("Invalid end of the file\n");
      }
      else {
         fixJump(token, info, &writer, helper);
         token.read();
      }
   }
   return recognized;
}

void AMD64Assembler :: compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned)
{
   ProcedureInfo info(binary, inlineMode);

   token.read();
   if (token.check("%")) {
      info.reference = token.readInteger(constants);

      token.read();
   }
   else {
      ReferenceNs refName(NATIVE_MODULE, token.value);

      token.read();
	   if (token.check("(")) {
	      readParameterList(token, info, refName);
	      token.read();
      }

      info.reference = binary->mapReference(refName) | mskNativeCodeRef;
   }
   if (binary->mapSection(info.reference, true)!=NULL) {
      throw AssemblerException("Procedure already exists (%d)\n", token.terminal.row);
   }

   _Memory* code = binary->mapSection(info.reference, false);
   MemoryWriter writer(code);

   AMD64JumpHelper helper(&writer);

   //PrefixInfo prefix;

   while (!token.check("end")) {
      compileCommand(/*prefix, */token, info, writer, helper);
   }
   if (aligned)
	   writer.align(8, 0x90);
}

void AMD64Assembler :: compile(TextReader* source, path_t outputPath)
{
   Module       binary("$binary");
   SourceReader reader(4, source);

   TokenInfo    token(&reader);

   token.read();
   do {
		if (token.check("define")) {
         IdentifierString name(token.read());
         size_t value = token.readInteger(constants);

         if (name[0]=='\'')
            constants.erase(name);

			if (!constants.add(name, value, true))
				token.raiseErr("Constant already exists (%d)\n");

         token.read();
		}
		else if (token.check("procedure")) {
			compileProcedure(token, &binary, true, true);

			token.read();
		}
      else if (token.check("inline")) {
         compileProcedure(token, &binary, false, false);

         token.read();
      }
      else if (token.check("structure")) {
         compileStructure(token, &binary, mskNativeDataRef);
         
         token.read();
      }
      else if (token.check("rstructure")) {
         compileStructure(token, &binary, mskNativeRDataRef);

	      token.read();
      }
	  else if (token.value[0] == '#')
	  {
		  token.read();
		  token.read();
		  token.read();
	  }
		else token.raiseErr("Invalid statement (%d)\n");

	} while (!token.Eof());

   FileWriter writer(outputPath, feRaw, false);
   binary.save(writer);
}
