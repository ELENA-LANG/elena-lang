//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA x86Compiler
//		classes.
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "x86assembler.h"
#include "module.h"

#include <float.h>

using namespace _ELENA_;

const int gcPageSize       = 0x0010;      // !! temporal

#define ARGUMENT1           "__arg1"
#define ARGUMENT2           "__arg2"

void x86Assembler :: loadDefaultConstants()
{
//   constants.add(_EL_MINIMAL_SIZE, gcPageSize * 0x10);
//   constants.add(_GC_PAGE_MASK, ~(gcPageSize - 1));
//   constants.add(_GC_PAGE_LOG, logth(gcPageSize));
}

void x86Assembler :: readParameterList(TokenInfo& token, ProcedureInfo& info, ReferenceNs& refName)
{
   while (true) {
      token.read();

      if (token.terminal.state==dfaIdentifier) {
         info.parameters.add(token.value, info.parameters.Count());
		   token.read();

		   if (!token.check(":"))
             token.raiseErr("Semicolumn expected (%d)\n");

         token.read();
		   if (token.check("out")) {
            refName.append("&out");
            token.read();
		      if (token.check(")"))
               break;
		   }
         refName.append('&');
         refName.append(token.value);

         token.read();
		   if (token.check(")")) {
             break;
		   }
		   else if (!token.check(","))
             token.raiseErr("Comma expected (%d)\n");
	   }
	   else token.raiseErr("Invalid parameter list syntax (%d)\n");
   }
}

int x86Assembler :: readStReg(TokenInfo& token)
{
	token.read("(", "'(' expected (%d)\n");
	int index = token.readInteger(constants);
	token.read(")", "')' expected (%d)\n");

	return index;
}

x86Assembler::Operand x86Assembler :: defineRegister(TokenInfo& token)
{
	if (token.check("eax")) {
		return x86Helper::otEAX;
	}
	else if (token.check("ecx")) {
		return x86Helper::otECX;
	}
	else if (token.check("ebx")) {
		return x86Helper::otEBX;
	}
	else if (token.check("esi")) {
		return x86Helper::otESI;
	}
	else if (token.check("edi")) {
		return x86Helper::otEDI;
	}
	else if (token.check("ebp")) {
		return Operand(x86Helper::otEBP);
	}
	else if (token.check("edx")) {
		return Operand(x86Helper::otEDX);
	}
	else if (token.check("esp")) {
		return Operand(x86Helper::otESP);
	}
	else if (token.check("al")) {
		return Operand(x86Helper::otAL);
	}
	else if (token.check("bl")) {
		return Operand(x86Helper::otBL);
	}
	else if (token.check("cl")) {
		return Operand(x86Helper::otCL);
	}
	else if (token.check("dl")) {
		return Operand(x86Helper::otDL);
	}
	else if (token.check("dh")) {
		return Operand(x86Helper::otDH);
	}
	else if (token.check("ah")) {
		return Operand(x86Helper::otAH);
	}
	else if (token.check("bh")) {
		return Operand(x86Helper::otBH);
	}
	else if (token.check("ax")) {
		return Operand(x86Helper::otAX);
	}
	else if (token.check("bx")) {
		return Operand(x86Helper::otBX);
	}
	else if (token.check("cx")) {
		return Operand(x86Helper::otCX);
	}
	else if (token.check("dx")) {
		return Operand(x86Helper::otDX);
	}
   else return Operand(x86Helper::otUnknown);
}

x86Assembler::Operand x86Assembler :: defineOperand(TokenInfo& token, ProcedureInfo& info, const char* err)
{
	Operand operand = defineRegister(token);
   if (operand.type == x86Helper::otUnknown) {
		if (token.getInteger(operand.offset, constants)) {
			setOffsetSize(operand);
         // !! HOTFIX: 000000080 constant should be considered as int32 rather then int8
         if (getlength(token.value)==8 && operand.type == x86Helper::otDB) {
            operand.type = x86Helper::otDD;
         }
		}
		else if (token.check("data")) {
         token.read(":", err);
         token.read();
         if (token.check("%")) {
            operand.type = x86Helper::otDD;
            operand.reference = token.readInteger(constants) | mskPreloadDataRef;
            operand.offset = 0x0;
         }
         else {
            IdentifierString structRef(token.terminal.line + 1, token.terminal.length-2);

            operand.type = x86Helper::otDD;
            operand.reference = info.binary->mapReference(structRef) | mskNativeDataRef;
         }

      }
		else if (token.check("rdata")) {
         token.read(":", err);
         token.read();
         IdentifierString structRef(token.terminal.line + 1, token.terminal.length-2);

         operand.type = x86Helper::otDD;
         operand.reference = info.binary->mapReference(structRef) | mskNativeRDataRef;
      }
		else if (token.check("stat")) {
         token.read(":", err);
         token.read();
         IdentifierString structRef(token.terminal.line + 1, token.terminal.length-2);

         operand.type = x86Helper::otDD;
         operand.reference = info.binary->mapReference(structRef) | mskStatSymbolRef;
      }
		else if (token.check("const")) {
         token.read(":", err);
         token.read();
         operand.type = x86Helper::otDD;

         IdentifierString constRef(token.terminal.line + 1, token.terminal.length-2);
         operand.reference = info.binary->mapReference(constRef) | mskConstantRef;
         operand.offset = 0;
      }
      else if (token.check("code")) {
         token.read(":", err);
         token.read();

         if (token.check("%")) {
            operand.type = x86Helper::otDD;
            operand.reference = token.readInteger(constants) | mskPreloadCodeRef;
            operand.offset = 0x0;
         }
         else {
            IdentifierString structRef(token.terminal.line + 1, token.terminal.length - 2);

            operand.type = x86Helper::otDD;
            operand.reference = info.binary->mapReference(structRef) | mskNativeCodeRef;
         }
      }
		else if (token.check(ARGUMENT1)) {
         operand.type = x86Helper::otDD;
         operand.reference = -1;
		}
		else if (token.check(ARGUMENT2)) {
         operand.type = x86Helper::otDD;
         operand.reference = -2;
		}
      else if (info.parameters.exist(token.value)) {
         if (info.inlineMode) {
            operand.type = x86Helper::addPrefix(x86Helper::otEBP, x86Helper::otM32disp32);
         }
         else operand.type = x86Helper::addPrefix(x86Helper::otESP, x86Helper::otM32disp32);

         operand.offset = (info.parameters.Count() - info.parameters.get(token.value))*4;
      }
      else {
         token.raiseErr(err);

         return Operand();
      }
   }
   return operand;
}

bool x86Assembler :: setOffset(Operand& operand, Operand disp)
{
	if (disp.reference==0) {
		operand.offset += disp.offset;
	}
	else if (operand.reference==0) {
		operand.offset += disp.offset;
		operand.reference = disp.reference;
	}
	else return false;

	if (disp.type==x86Helper::otDB) {
      if (test(operand.type, x86Helper::otM16)) {
         operand.type = (OperandType)(operand.type | x86Helper::otM16disp8);
      }
      else if (test(operand.type, x86Helper::otM8)) {
         operand.type = (OperandType)(operand.type | x86Helper::otM8disp8);
      }
		else operand.type = (OperandType)(operand.type | x86Helper::otM32disp8);

		return true;
	}
	else if (disp.type==x86Helper::otDD) {
      operand.type = (OperandType)(operand.type | x86Helper::otM32disp32);
		return true;
	}
   return false;
}

x86Assembler::Operand x86Assembler :: defineDisplacement(TokenInfo& token, ProcedureInfo& info, const char* err)
{
   if (token.check("+")) {
      token.read();

      return defineOperand(token, info, err);
   }
   else if (token.check("-")) {
      token.read();

      Operand disp = defineOperand(token, info, err);
      if (disp.type == x86Helper::otDD || disp.type == x86Helper::otDB) {
         disp.offset = -disp.offset;

         return disp;
      }
      else token.raiseErr(err);
   }
   else if (token.value[0]=='-' && (token.terminal.state==dfaInteger || token.terminal.state==dfaHexInteger)) {
      Operand disp = defineOperand(token, info, err);
      if (disp.type == x86Helper::otDD || disp.type == x86Helper::otDB) {
         return disp;
      }
      else token.raiseErr(err);
   }
   else if (token.check("*")) {
      token.read();
      if(token.check("4")) {
         return Operand(x86Helper::otFactor4);
      }
      else if(token.check("8")) {
         return Operand(x86Helper::otFactor8);
      }
      else if(token.check("2")) {
         return Operand(x86Helper::otFactor2);
      }
      else token.raiseErr(err);
   }

   return Operand();
}

x86Assembler::Operand x86Assembler :: readDispOperand(TokenInfo& token, ProcedureInfo& info, const char* err, OperandType prefix)
{
	Operand operand = defineOperand(token, info, err);
   if (operand.type != x86Helper::otUnknown) {
      operand.type = x86Helper::addPrefix(operand.type, prefix);

      if (operand.type == x86Helper::otDisp32 && !operand.ebpReg) {
         token.read();
         Operand disp = defineDisplacement(token, info, err);
         if (disp.type == x86Helper::otDD || disp.type == x86Helper::otDB) {
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
         else if (disp.type != x86Helper::otUnknown)
            token.raiseErr(err);

         // if it is [disp]
         return operand;
      }
      else {
         token.read();

         Operand disp = defineDisplacement(token, info, err);

         if (disp.type == x86Helper::otDD || disp.type == x86Helper::otDB) {
            // if it is [r + disp]
            if(!setOffset(operand, disp))
               token.raiseErr(err);

            token.read();
         }
         else if (disp.type == x86Helper::otFactor2 || disp.type == x86Helper::otFactor4 || disp.type == x86Helper::otFactor8) {
            token.read();

            Operand disp2 = defineDisplacement(token, info, err);
            if (disp2.type == x86Helper::otDD || disp2.type == x86Helper::otDB) {
               // if it is [r*factor + disp]
               int sibcode = ((char)x86Helper::otDisp32 + ((char)operand.type << 3)) << 24;

               operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | x86Helper::otSIB | disp.type | sibcode);
               operand.offset = disp2.offset;
               operand.reference = disp2.reference;
               operand.factorReg = true;

               token.read();
            }
            else if (disp2.type == x86Helper::otUnknown) {
               // if it is [r*factor]
               int sibcode = ((char)x86Helper::otDisp32 + ((char)operand.type << 3)) << 24;

               operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | x86Helper::otSIB | disp.type | sibcode);
               operand.offset = 0;
               operand.reference = 0;
               operand.factorReg = true;
            }
            else token.raiseErr(err);
         }
         else if (disp.type != x86Helper::otUnknown) {
            token.read();

            Operand disp2 = defineDisplacement(token, info, err);
            if (disp2.type == x86Helper::otDD || disp2.type == x86Helper::otDB) {
               // if it is [r + r + disp]
               int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

               if(!setOffset(operand, disp2))
                  token.raiseErr(err);

               operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | x86Helper::otSIB | sibcode);

               token.read();
            }
            else if (disp2.type == x86Helper::otFactor2 || disp2.type == x86Helper::otFactor4 || disp2.type == x86Helper::otFactor8) {
               token.read();

               Operand disp3 = defineDisplacement(token, info, err);
               if (disp3.type == x86Helper::otDD || disp3.type == x86Helper::otDB) {
                  // if it is [r + r*factor + disp]
                  int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

                  if(!setOffset(operand, disp3))
                     token.raiseErr(err);

                  operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | x86Helper::otSIB | sibcode | disp2.type);

                  token.read();
               }
               else if (disp3.type == x86Helper::otUnknown) {
                  // if it is [r + r*factor]
				      int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

				      operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | x86Helper::otSIB | sibcode | disp2.type);
               }
               else token.raiseErr(err);
            }
            else if (disp2.type == x86Helper::otUnknown) {
               // if it is [r + r]
				   int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

				   operand.type = (OperandType)((operand.type & 0xFFFFFFF8) | x86Helper::otSIB | sibcode);
            }
            else token.raiseErr(err);
         }
         else {
            // if it is register disp [r]
            if (operand.ebpReg && operand.type == x86Helper::otDisp32) {
               operand.type = (OperandType)(operand.type | x86Helper::otM32disp8);
	            operand.offset = 0;
	         }
            return operand;
         }
      }
   }
	return operand;
}

x86Assembler::Operand x86Assembler :: readPtrOperand(TokenInfo& token, ProcedureInfo& info, const char* err, OperandType prefix)
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
      if (operand.type == x86Helper::otDD && operand.reference == 0 || operand.type==x86Helper::otDB) {
         if (prefix==x86Helper::otM16) {
            operand.type = x86Helper::otDW;
         }
         else if (prefix==x86Helper::otM8) {
            operand.type = x86Helper::otDB;
         }
      }
	   else token.raiseErr("'[' expected (%d)\n");

      token.read();

      return operand;
	}
}

x86Assembler::Operand x86Assembler :: compileOperand(TokenInfo& token, ProcedureInfo& info/*, _Module* binary*/, const char* err)
{
	Operand	    operand;

	token.read();
	if (token.check("[")) {
		token.read();
      operand = readDispOperand(token, info, "Invalid expression (%d)", x86Helper::otM32);

	   if(!token.check("]")) {
          token.raiseErr("']' expected (%d)\n");
	   }
	   else token.read();
	}
	else if (token.check("dword")) {
	   token.read("ptr", "'ptr' expected (%d)\n");

		operand = readPtrOperand(token, info, err, x86Helper::otM32);
	}
	else if (token.check("word")) {
	   token.read("ptr", "'ptr' expected (%d)\n");

		operand = readPtrOperand(token, info, err, x86Helper::otM16);
	}
	else if (token.check("byte")) {
	   token.read("ptr", "'ptr' expected (%d)\n");

		operand = readPtrOperand(token, info, err, x86Helper::otM8);
	}
   else if (token.check("fs")) {
      token.read(":", "Column is expected");
      operand = readPtrOperand(token, info, err, x86Helper::otM32);

      if (operand.prefix != x86Helper::spNone) {
         token.raiseErr(err);
      }
      else operand.prefix = x86Helper::spFS;
   }
	else {
      operand = defineOperand(token, info, err);

      if (operand.type != x86Helper::otUnknown) {
         token.read();

         if (token.check("+")) {
            operand.offset += token.readInteger(constants);

            token.read();
         }
      }
   }

	return operand;
}

void x86Assembler :: compileMOV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   // write segment prefix
   if (sour.prefix != x86Helper::spNone) {
      if (dest.prefix != x86Helper::spNone)
         token.raiseErr("Invalid command (%d)");

      code->writeByte(sour.prefix);
   }
   else if (dest.prefix != x86Helper::spNone) {
      if (sour.prefix != x86Helper::spNone)
         token.raiseErr("Invalid command (%d)");

      code->writeByte(dest.prefix);
   }

	if (test(sour.type, x86Helper::otPtr16)) {
		sour.type = x86Helper::overrideOperand16(sour.type);
		if (test(dest.type, x86Helper::otPtr16)) {
			dest.type = x86Helper::overrideOperand16(dest.type);
		}
		else if (dest.type==x86Helper::otDD) {
			dest.type = x86Helper::otDW;
		}
		code->writeByte(0x66);
	}

	if (sour.type == x86Helper::otEAX && dest.type == x86Helper::otDisp32) {
		code->writeByte(0xA1);

      if (dest.reference != 0) {
         code->writeRef(dest.reference, dest.offset);
      }
      else code->writeDWord(dest.offset);
	}
	else if (sour.type == x86Helper::otDisp32 && dest.type == x86Helper::otEAX) {
		code->writeByte(0xA3);

      if (sour.reference != 0) {
         code->writeRef(sour.reference, sour.offset);
      }
      else code->writeDWord(sour.offset);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32)||test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x8B);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if (test(sour.type, x86Helper::otM32) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x89);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if (test(sour.type, x86Helper::otR32) && (dest.type==x86Helper::otDD || dest.type==x86Helper::otDB)) {
		dest.type = x86Helper::otDD;
		code->writeByte(0xB8 + (char)sour.type);
		x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR32) || test(sour.type, x86Helper::otM32))
      && (dest.type==x86Helper::otDD || dest.type==x86Helper::otDB))
   {
		dest.type = x86Helper::otDD;
		code->writeByte(0xC7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), sour);
		x86Helper::writeImm(code, dest);
	}
	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB) {
		code->writeByte(0xB0 + (char)sour.type);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otM8) && dest.type==x86Helper::otDB)
   {
		code->writeByte(0xC6);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), sour);
		x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8))&& test(dest.type, x86Helper::otR8)) {
		code->writeByte(0x88);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if (test(sour.type, x86Helper::otR8) && (test(dest.type, x86Helper::otR8)||test(dest.type, x86Helper::otM8))) {
		code->writeByte(0x8A);
		x86Helper::writeModRM(code, sour, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileCMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otPtr16)) {
      sour.type = x86Helper::overrideOperand16(sour.type);
		if (test(dest.type, x86Helper::otPtr16)) {
			dest.type = x86Helper::overrideOperand16(dest.type);
		}
		else if (dest.type==x86Helper::otDD) {
			dest.type = x86Helper::otDW;
		}
		code->writeByte(0x66);
	}

	if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32) || test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x3B);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if ((test(sour.type, x86Helper::otR32) || test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x39);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
		code->writeByte(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0x81);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
		x86Helper::writeImm(code, dest);
	}
	else if (sour.type==x86Helper::otAL && dest.type==x86Helper::otDB) {
		code->writeByte(0x3C);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otR8) && test(dest.type, x86Helper::otM8)) {
		code->writeByte(0x3A);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if (test(sour.type, x86Helper::otR8) && test(dest.type, x86Helper::otR8)) {
		code->writeByte(0x38);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if ((test(sour.type, x86Helper::otR8) || test(sour.type, x86Helper::otM8)) && dest.type == x86Helper::otDB) {
		code->writeByte(0x80);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 7), sour);
		code->writeByte(dest.offset);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileADD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (dest.prefix == x86Helper::spFS) {
      code->writeByte(0x64);
   }

	if (sour.type==x86Helper::otEAX && dest.type == x86Helper::otDD) {
		code->writeByte(0x05);
		x86Helper::writeImm(code, dest);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32)||test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x03);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x01);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), sour);
		code->writeByte(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x80);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 0), sour);
		x86Helper::writeImm(code, dest);
	}
	else if (test(sour.type, x86Helper::otR8)&&(test(dest.type, x86Helper::otR8)||test(dest.type, x86Helper::otM8))) {
		code->writeByte(0x02);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0x81);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), sour);
      x86Helper::writeImm(code, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileXADD(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (prefix.lockMode) {
      code->writeByte(0xF0);
   }
   else if(prefix.Exists()) {
      token.raiseErr("Invalid Prefix (%d)");
   }

	if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x0F);
      code->writeByte(0xC1);
		x86Helper::writeModRM(code, dest, sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileADC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (sour.type== x86Helper::otEAX && dest.type == x86Helper::otDD) {
		code->writeByte(0x15);
		x86Helper::writeImm(code, dest);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32)||test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x13);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x11);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 2), sour);
		code->writeByte(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x80);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 2), sour);
		x86Helper::writeImm(code, dest);
	}
	else if (test(sour.type, x86Helper::otR8)&&(test(dest.type, x86Helper::otR8)||test(dest.type, x86Helper::otM8))) {
		code->writeByte(0x12);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0x81);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 2), sour);
		code->writeDWord(dest.offset);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileAND(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	bool overridden = false;

	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");
	if (test(sour.type, x86Helper::otR16) && dest.type==x86Helper::otDD) {
		sour.type = x86Helper::overrideOperand16(sour.type);
		code->writeByte(0x66);
		overridden = true;
	}

	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otDB)) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), sour);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32) || test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x23);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x21);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0x81);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), sour);
		if (overridden) {
			code->writeWord(dest.offset);
		}
		else x86Helper::writeImm(code, dest);
	}
   else if (test(sour.type, x86Helper::otR8) && test(dest.type, x86Helper::otDB)) {
		code->writeByte(0x80);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 4), sour);
      x86Helper::writeImm(code, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileXOR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x33);
		x86Helper::writeModRM(code, sour, dest);
	}
	else if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otDB)) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
		code->writeByte(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0x81);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
		x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x31);
		x86Helper::writeModRM(code, dest, sour);
		x86Helper::writeImm(code, dest);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32) || test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x33);
		x86Helper::writeModRM(code, sour, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileOR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	bool overridden = false;

	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR16) && dest.type==x86Helper::otDD) {
		sour.type = x86Helper::overrideOperand16(sour.type);
		code->writeByte(0x66);
		overridden = true;
	}

	if (sour.type== x86Helper::otEAX && dest.type==x86Helper::otDD) {
		code->writeByte(0x0D);
		if (overridden) {
			code->writeWord(dest.offset);
		}
		else x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otDB)) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 1), sour);
		code->writeByte(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) && test(dest.type, x86Helper::otR8)) {
		code->writeByte(0x08);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0x81);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 1), sour);
		x86Helper::writeImm(code, dest, overridden);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x80);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 1), sour);
		x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x09);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if (test(sour.type, x86Helper::otR16) && test(dest.type, x86Helper::otR16)) {
		code->writeByte(0x66);
		code->writeByte(0x09);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32) || test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x0B);
		x86Helper::writeModRM(code, sour, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileTEST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (sour.type == x86Helper::otEAX && dest.type == x86Helper::otDD) {
		code->writeByte(0xA9);
		x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x85);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0xF7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), sour);
		x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDB) {
		code->writeByte(0xF7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), sour);
		code->writeDWord(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) && dest.type==x86Helper::otDB) {
		code->writeByte(0xF6);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 0), sour);
		x86Helper::writeImm(code, dest);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) && test(dest.type, x86Helper::otR8)) {
		code->writeByte(0x84);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR16)||test(sour.type, x86Helper::otM16)) && test(dest.type, x86Helper::otR16)) {
      code->writeByte(0x66);
		code->writeByte(0x85);
		x86Helper::writeModRM(code, dest, sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32)||test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x2B);
		x86Helper::writeModRM(code, sour, dest);
	}
   else if (sour.type==x86Helper::otAL && dest.type==x86Helper::otDB) {
		code->writeByte(0x2C);
		code->writeByte(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x29);
		x86Helper::writeModRM(code, dest, sour);
	}
	else if ((test(sour.type, x86Helper::otR32) ||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 5), sour);
		code->writeByte(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDD) {
		code->writeByte(0x81);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 5), sour);
      if (dest.reference != 0) {
         code->writeRef(dest.reference, dest.offset);
      }
		else code->writeDWord(dest.offset);
	}
	else if ((test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x80);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 5), sour);
		x86Helper::writeImm(code, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSBB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if ((test(sour.type, x86Helper::otR32) ||test(sour.type, x86Helper::otM32)) && dest.type==x86Helper::otDB) {
		code->writeByte(0x83);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 3), sour);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32)||test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x1B);
		x86Helper::writeModRM(code, sour, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileLEA(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otM32)) {
		code->writeByte(0x8D);
		x86Helper::writeModRM(code, sour, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSHR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC1);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 5), sour);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otCL) {
		code->writeByte(0xD3);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 5), sour);
	}
	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB && dest.offset == 1) {
		code->writeByte(0xD0);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 5), sour);
	}
	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC0);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 5), sour);
		code->writeByte(dest.offset);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSAR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC1);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otCL) {
		code->writeByte(0xD3);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
	}
	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB && dest.offset == 1) {
		code->writeByte(0xD0);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 7), sour);
	}
	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC0);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 7), sour);
		code->writeByte(dest.offset);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSHL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC1);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), sour);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otCL) {
		code->writeByte(0xD3);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSHLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	checkComma(token);

	Operand third = compileOperand(token, info, "Invalid third operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otDB) {
      code->writeByte(0x0F);
		code->writeByte(0xA4);
      x86Helper::writeImm(code, third);
	}
	else if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otCL) {
      code->writeByte(0x0F);
		code->writeByte(0xA5);
		x86Helper::writeModRM(code, dest, sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSHRD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	checkComma(token);

	Operand third = compileOperand(token, info, "Invalid third operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otDB) {
      code->writeByte(0x0F);
		code->writeByte(0xAC);
      x86Helper::writeImm(code, third);
	}
	else if (test(sour.type, x86Helper::otR32) && test(dest.type, x86Helper::otR32) && third.type==x86Helper::otCL) {
      code->writeByte(0x0F);
		code->writeByte(0xAD);
		x86Helper::writeModRM(code, dest, sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileROL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC0);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 0), sour);
		code->writeByte(dest.offset);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileROR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR16)) {
		sour.type = (OperandType)x86Helper::overrideOperand16(sour.type);
		code->writeByte(0x66);
	}

	if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC1);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 1), sour);
		code->writeByte(dest.offset);
	}
	else if (test(sour.type, x86Helper::otR8) && dest.type==x86Helper::otDB) {
		code->writeByte(0xC0);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 1), sour);
		code->writeByte(dest.offset);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileRCR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB && dest.offset==1) {
		code->writeByte(0xD1);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 3), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileXCHG(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (sour.type== x86Helper::otEAX && test(dest.type, x86Helper::otR32)) {
		code->writeByte(0x90 + (char)dest.type);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32)||test(dest.type, x86Helper::otM32))) {
		code->writeByte(0x87);
		x86Helper::writeModRM(code, sour, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileRCL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB && dest.offset==1) {
		code->writeByte(0xD1);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 2), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileMOVZX(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR8)||test(dest.type, x86Helper::otM8))) {
		code->writeWord(0xB60F);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + (char)sour.type), dest);
	}
	else if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR16)||test(dest.type, x86Helper::otM16))) {
		code->writeWord(0xB70F);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + (char)sour.type), dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compilePUSH(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)) {
		code->writeByte(0x50 + (char)sour.type);
	}
	else if (test(sour.type, x86Helper::otM32)) {
		code->writeByte(0xFF);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
	}
	else if (test(sour.type, x86Helper::otM16)) {
		code->writeByte(0x66);
		code->writeByte(0xFF);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
	}
	else if (sour.type==x86Helper::otDB) {
		code->writeByte(0x6A);
		code->writeByte(sour.offset);
	}
	else if (sour.type==x86Helper::otDD) {
		code->writeByte(0x68);
		x86Helper::writeImm(code, sour);
	}
	else if (sour.type==x86Helper::otDW) {
      code->writeByte(0x66);
		code->writeByte(0x68);
		x86Helper::writeImm(code, sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compilePOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)) {
		code->writeByte(0x58 + (char)sour.type);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) {
		code->writeByte(0xF7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileIMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   if (!token.check(",")) {
	   if (test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) {
		   code->writeByte(0xF7);
		   x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 5), sour);
	   }
	   else token.raiseErr("Invalid command (%d)");
   }
   else {
	   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	   if (test(sour.type, x86Helper::otR32) && dest.type==x86Helper::otDB) {
		   code->writeByte(0x6B);
		   x86Helper::writeModRM(code, sour, sour);
		   code->writeByte(dest.offset);
	   }
	   else token.raiseErr("Invalid command (%d)");
   }
}

void x86Assembler :: compileIDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) {
		code->writeByte(0xF7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) {
		code->writeByte(0xF7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), sour);
	}
	else if (test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) {
		code->writeByte(0xF6);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 6), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileDEC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)) {
		code->writeByte(0x48 + (char)sour.type);
	}
	else if (test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) {
		code->writeByte(0xFE);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 1), sour);
	}
	else if (test(sour.type, x86Helper::otM32)) {
		code->writeByte(0xFF);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 1), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileINC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)) {
		code->writeByte(0x40 + (char)sour.type);
	}
	else if (test(sour.type, x86Helper::otR8)||test(sour.type, x86Helper::otM8)) {
		code->writeByte(0xFE);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 0), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler::compileINT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
   if (test(sour.type, x86Helper::otDB)) {
      code->writeByte(0xCD);
      code->writeByte(sour.offset);
   }
   else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileNEG(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)) {
		code->writeByte(0xF7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 3), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileNOT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR32)) {
		code->writeByte(0xF7);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 2), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileRET(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
   if (token.terminal.state == dfaInteger || token.terminal.state == dfaHexInteger || token.check(ARGUMENT1) || token.check(ARGUMENT2)) {
      Operand sour = defineOperand(token, info, "Invalid operand (%d)\n");
      if (sour.type == x86Helper::otDD || sour.type == x86Helper::otDB) {
         code->writeByte(0xC2);
         if (sour.type == x86Helper::otDB)
            sour.type = x86Helper::otDW;

         x86Helper::writeImm(code, sour);
      }
      else token.raiseErr("Invalid command (%d)");

      token.read();
   }
   else code->writeByte(0xC3);
}

void x86Assembler :: compileCDQ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0x99);

	token.read();
}

void x86Assembler :: compileSTC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xF9);

	token.read();
}

void x86Assembler :: compileSAHF(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0x9E);

	token.read();
}

void x86Assembler :: compileNOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0x90);

	token.read();
}

void x86Assembler :: compileREP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xF3);

	token.read();
}

void x86Assembler :: compileREPZ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xF3);

	token.read();
}

void x86Assembler :: compileLODSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xAD);

	token.read();
}

void x86Assembler :: compileLODSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   	code->writeByte(0x66);
	code->writeByte(0xAD);

	token.read();
}

void x86Assembler :: compileLODSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xAC);

	token.read();
}

void x86Assembler :: compileMOVSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xA4);

	token.read();
}

void x86Assembler :: compileSTOSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xAB);

	token.read();
}

void x86Assembler :: compileSTOSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xAA);

	token.read();
}

void x86Assembler :: compileSTOSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0x66);
	code->writeByte(0xAB);

	token.read();
}

void x86Assembler :: compileCMPSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0xA6);

	token.read();
}

void x86Assembler :: compilePUSHFD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0x9C);

	token.read();
}

void x86Assembler :: compilePOPFD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeByte(0x9D);

	token.read();
}

void x86Assembler :: compileJxx(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int prefix, x86JumpHelper& helper)
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

void x86Assembler :: compileJMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, x86JumpHelper& helper)
{
   Operand operand = compileOperand(token, info, NULL);

	if (test(operand.type, x86Helper::otR32)||test(operand.type, x86Helper::otM32)) {
      code->writeByte(0xFF);
      x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), operand);
	}
   else if (operand.type == x86Helper::otDD && operand.reference == -1) {
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

void x86Assembler :: compileLOOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, x86JumpHelper& helper)
{
	token.read();

   // if jump forward
	if (!helper.checkDeclaredLabel(token.value)) {
      helper.writeLoopForward(token.value);
	}
   // if jump backward
	else helper.writeLoopBack(token.value);

	token.read();
}

void x86Assembler :: compileCALL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, x86JumpHelper& helper)
{
	token.read();

   Operand operand = defineRegister(token);
   if (operand.type != x86Helper::otUnknown) {
      code->writeByte(0xFF);
      code->writeByte(0xD0 + (char)operand.type);
   }
   else if (token.check("extern")) {
      code->writeWord(0x15FF);
      token.read();
      if (token.check(ARGUMENT1)) {
         code->writeRef(-1, 0);
      }
      else if (token.check(ARGUMENT2)) {
         code->writeRef(-2, 0);
      }
      else if (token.value[0]==':') {
         token.read();
         IdentifierString s(token.terminal.line + 1, token.terminal.length-2);

         ReferenceNs function(DLL_NAMESPACE, s);

	      int ref = info.binary->mapReference(function) | mskImportRef;

	      code->writeRef(ref, 0);
      }
      else if (StringHelper::compare(token.value, "'dlls'", 6)) {
         ReferenceNs function(DLL_NAMESPACE, token.value + 6);

	      token.read(".", "dot expected (%d)\n");
	      function.append(".");
	      function.append(token.read());

	      int ref = info.binary->mapReference(function) | mskImportRef;

	      code->writeRef(ref, 0);
      }
      else token.raiseErr("Invalid call label (%d)\n");
   }
	else if (token.check("[")) {
		token.read();
		Operand operand = readDispOperand(token, info, "Invalid call target (%d)\n", x86Helper::otM32);
		if (!token.check("]"))
			token.raiseErr("']' expected(%d)\n");

		if (test(operand.type, x86Helper::otM32)) {
			code->writeByte(0xFF);
			x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 2), operand);
			x86Helper::writeImm(code, operand);
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
         if (StringHelper::find(funRef, NATIVE_MODULE) == 0) {
            ref = info.binary->mapReference(funRef) | mskNativeRelCodeRef;
         }
         else ref = info.binary->mapReference(funRef) | mskSymbolRelRef;
      }

      code->writeRef(ref, 0);
   }
   // if jump forward
   else if (!helper.checkDeclaredLabel(token.value)) {
      helper.writeCallForward(token.value);
   }
   // if jump backward
   else helper.writeCallBack(token.value);

   token.read();
}

void x86Assembler :: fixJump(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, x86JumpHelper& helper)
{
	if (helper.checkDeclaredLabel(token.value)) {
		token.raiseErr("Label with such a name already exists (%d)\n");
	}

	if (!helper.addLabel(token.value))
      token.raiseErr("Invalid near jump to this label (%d)\n");

	token.read(":", "Invalid command or label (%d)\n");
}

void x86Assembler :: compileFBLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otM32)) {
		code->writeByte(0xDF);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFCHS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xE0D9);

	token.read();
}

void x86Assembler :: compileFILD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();

	if (token.check("dword")) {
		Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		if (test(sour.type, x86Helper::otM32)) {
			code->writeByte(0xDB);
			x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), sour);
		}
		else token.raiseErr("Invalid command (%d)");
	}
   else if (token.check("qword")) {
		Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		if (test(sour.type, x86Helper::otM32)) {
			code->writeByte(0xDF);
			x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 5), sour);
		}
		else token.raiseErr("Invalid command (%d)");
   }
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFIST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otM32)) {
		code->writeByte(0xDB);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 2), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFISTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();

	if (token.check("dword")) {
		Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		if (test(sour.type, x86Helper::otM32)) {
			code->writeByte(0xDB);
			x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 3), sour);
		}
		else token.raiseErr("Invalid command (%d)");
	}
   else if (token.check("qword")) {
		Operand sour = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		if (test(sour.type, x86Helper::otM32)) {
			code->writeByte(0xDF);
			x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), sour);
		}
		else token.raiseErr("Invalid command (%d)");
   }
}

void x86Assembler :: compileFLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("st")) {
		int index = readStReg(token);

		code->writeByte(0xD9);
		code->writeByte(0xC0 + index);

		token.read();
	}
	else if (token.check("qword")) {
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xDD);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFXCH(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
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

void x86Assembler :: compileFSUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
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
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xDC);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 4), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFADD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
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
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xDC);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 0), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("st")) {
		int sour = readStReg(token);

		token.read(",", "',' comma expected(%d)\n");

		if (sour != 0) {
			token.read("st", "'st' expected (%d)\n");
			if (readStReg(token)!=0)
				token.raiseErr("'0' expected (%d)");

			token.read();

			code->writeByte(0xDC);
			code->writeByte(0xC8 + sour);
		}
		else {
			token.read("st", "'st' expected (%d)\n");
			sour = readStReg(token);
			token.read();

			code->writeByte(0xD8);
			code->writeByte(0xC8 + sour);
		}
	}
	else if (token.check("qword")) {
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xDC);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 1), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
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
			code->writeByte(0xF8 + sour);
		}
		else {
			token.read("st", "'st' expected (%d)\n");
			sour = readStReg(token);
			token.read();

			code->writeByte(0xD8);
			code->writeByte(0xF0 + sour);
		}
	}
	else if (token.check("qword")) {
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xDC);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFCOMIP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
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

void x86Assembler :: compileFCOMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("st")) {
		token.read(",", "',' expected (%d)\n");
		token.read("st", "'st' expected (%d)\n");

		int dest = readStReg(token);

		token.read();

		code->writeByte(0xD8);
		code->writeByte(0xD8 + dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFSTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("st")) {
		int sour = readStReg(token);
		token.read();

		code->writeByte(0xDD);
		code->writeByte(0xD8 + sour);
	}
	else if (token.check("qword")) {
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xDD);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 3), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFBSTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("tbyte")) {
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xDF);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 6), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFSTSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("ax")) {
		token.read();

		code->writeByte(0x9B);
		code->writeWord(0xE0DF);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFNSTSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("ax")) {
		token.read();

		code->writeWord(0xE0DF);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFINIT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   code->writeByte(0x9B);
   code->writeWord(0xE3DB);

   token.read();
}

void x86Assembler :: compileFSTCW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("word")) {
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0x9B);
		code->writeByte(0xD9);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 7), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFLDCW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	token.read();
	if (token.check("word")) {
		Operand operand = readPtrOperand(token, info, "Invalid operand (%d)\n", x86Helper::otM32);

		code->writeByte(0xD9);
		x86Helper::writeModRM(code, Operand(x86Helper::otR32 + 5), operand);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileFLDZ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xEED9);

	token.read();
}

void x86Assembler :: compileFLD1(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xE8D9);

	token.read();
}

void x86Assembler :: compileFADDP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xC1DE);

	token.read();
}

void x86Assembler :: compileFSUBP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xE9DE);

	token.read();
}

void x86Assembler :: compileFDIVP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xF9DE);

	token.read();
}

void x86Assembler :: compileF2XM1(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xF0D9);

	token.read();
}

void x86Assembler :: compileFLDL2T(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xE9D9);

	token.read();
}

void x86Assembler :: compileFLDLG2(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xECD9);

	token.read();
}

void x86Assembler :: compileFRNDINT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xFCD9);

	token.read();
}

void x86Assembler :: compileFSCALE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xFDD9);

	token.read();
}

void x86Assembler :: compileFXAM(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xE5D9);

	token.read();
}

void x86Assembler :: compileFMULP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xC9DE);

	token.read();
}

void x86Assembler :: compileFABS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xE1D9);

	token.read();
}

void x86Assembler :: compileFSQRT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xFAD9);

	token.read();
}

void x86Assembler :: compileFSIN(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xFED9);

	token.read();
}

void x86Assembler :: compileFCOS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xFFD9);

	token.read();
}

void x86Assembler :: compileFYL2X(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xF1D9);

	token.read();
}

void x86Assembler :: compileFTST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xE4D9);

	token.read();
}

void x86Assembler :: compileFLDPI(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xEBD9);

	token.read();
}

void x86Assembler :: compileFPREM(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xF8D9);

	token.read();
}

void x86Assembler :: compileFPATAN(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xF3D9);

	token.read();
}

void x86Assembler :: compileFLDL2E(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xEAD9);

	token.read();
}

void x86Assembler :: compileFLDLN2(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	code->writeWord(0xEDD9);

	token.read();
}

void x86Assembler :: compileFFREE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
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

void x86Assembler :: compileCMPXCHG(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   if (prefix.lockMode) {
      code->writeByte(0xF0);
   }
   else if(prefix.Exists()) {
      token.raiseErr("Invalid Prefix (%d)");
   }

	if ((test(sour.type, x86Helper::otR32)||test(sour.type, x86Helper::otM32)) && test(dest.type, x86Helper::otR32)) {
      code->writeByte(0x0F);
		code->writeByte(0xB1);
		x86Helper::writeModRM(code, dest, sour);
	}
   else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileSETCC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int postfix)
{
	Operand sour = compileOperand(token, info, "Invalid operand (%d)\n");
	if (test(sour.type, x86Helper::otR8)) {
		code->writeByte(0x0F);
      code->writeByte(0x90 + postfix);
		x86Helper::writeModRM(code, Operand(x86Helper::otR8 + 0), sour);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileCMOVCC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int postfix)
{
	Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

	checkComma(token);

	Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

	if (test(sour.type, x86Helper::otR32) && (test(dest.type, x86Helper::otR32) || test(dest.type, x86Helper::otM32) )) {
      code->writeByte(0x0F);
      code->writeByte(0x40 + postfix);
		x86Helper::writeModRM(code, sour, dest);
	}
	else token.raiseErr("Invalid command (%d)");
}

void x86Assembler :: compileStructure(TokenInfo& token, _Module* binary, int mask)
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
         if (operand.type==x86Helper::otDD) {
            x86Helper::writeImm(&writer, operand);
         }
         else if (operand.type==x86Helper::otDB) {
            operand.type = x86Helper::otDD;
            x86Helper::writeImm(&writer, operand);
         }
         else token.raiseErr("Invalid operand (%d)");
      }
      else if (token.check("dw")) {
         token.read();
         Operand operand = defineOperand(token, info, "Invalid constant");
         if (operand.type==x86Helper::otDB) {
            operand.type = x86Helper::otDW;
            x86Helper::writeImm(&writer, operand);
         }
         else token.raiseErr("Invalid operand (%d)");
      }
      else if (token.check("db")) {
         token.read();
         Operand operand = defineOperand(token, info, "Invalid constant");
         if (operand.type==x86Helper::otDB) {
            x86Helper::writeImm(&writer, operand);
         }
         else token.raiseErr("Invalid operand (%d)");
      }
      else if (token.Eof()) {
         token.raiseErr("Invalid end of the file\n");
      }
      else token.read();
   }
}

bool x86Assembler :: compileCommandA(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
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
   else return false;
}
bool x86Assembler :: compileCommandB(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandC(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, x86JumpHelper& helper)
{
	if (token.check("cmp")) {
		compileCMP(token, info, &writer);
      return true;
	}
	else if (token.check("call")) {
		compileCALL(token, info, &writer, helper);
      return true;
	}
	else if (token.check("cdq")) {
		compileCDQ(token, info, &writer);
      return true;
	}
	else if (token.check("cmpsb")) {
		compileCMPSB(token, info, &writer);
      return true;
	}
   else if (token.check("cmpxchg")) {
      compileCMPXCHG(prefix, token, info, &writer);
      prefix.clear();
      return true;
   }
	else if (token.check("cmovle")) {
      compileCMOVCC(token, info, &writer, x86Helper::JUMP_TYPE_JLE);
      return true;
	}
	else if (token.check("cmovnz")) {
      compileCMOVCC(token, info, &writer, x86Helper::JUMP_TYPE_JNZ);
      return true;
	}
	else if (token.check("cmovl")) {
      compileCMOVCC(token, info, &writer, x86Helper::JUMP_TYPE_JL);
      return true;
	}
	else if (token.check("cmovg")) {
      compileCMOVCC(token, info, &writer, x86Helper::JUMP_TYPE_JG);
      return true;
	}
	else if (token.check("cmovge")) {
      compileCMOVCC(token, info, &writer, x86Helper::JUMP_TYPE_JGE);
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandD(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
	if (token.check("dec")) {
		compileDEC(token, info, &writer);
      return true;
	}
	else if (token.check("div")) {
		compileDIV(token, info, &writer);
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandE(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandF(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
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
	else if (token.check("fcomip")) {
		compileFCOMIP(token, info, &writer);
      return true;
	}
	else if (token.check("fcomp")) {
		compileFCOMP(token, info, &writer);
      return true;
	}
	else if (token.check("fmulp")) {
		compileFMULP(token, info, &writer);
      return true;
	}
	else if (token.check("fmul")) {
		compileFMUL(token, info, &writer);
      return true;
	}
	else if (token.check("fdiv")) {
		compileFDIV(token, info, &writer);
      return true;
	}
	else if (token.check("faddp")) {
		compileFADDP(token, info, &writer);
      return true;
	}
	else if (token.check("fsubp")) {
		compileFSUBP(token, info, &writer);
      return true;
	}
	else if (token.check("fmulp")) {
		compileFMULP(token, info, &writer);
      return true;
	}
	else if (token.check("fdivp")) {
		compileFDIVP(token, info, &writer);
      return true;
	}
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
	else if (token.check("fnstsw")) {
		compileFNSTSW(token, info, &writer);
      return true;
	}
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
bool x86Assembler :: compileCommandG(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandH(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandI(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
	if (token.check("inc")) {
		compileINC(token, info, &writer);
      return true;
	}
   else if (token.check("int")) {
      compileINT(token, info, &writer);
      return true;
   }
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
bool x86Assembler :: compileCommandJ(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, x86JumpHelper& helper)
{
	if (token.check("jb")||token.check("jc")) {
      compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JB, helper);
      return true;
	}
	else if (token.check("jnb")||token.check("jnc")||token.check("jae")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JAE, helper);
      return true;
	}
	else if (token.check("jz")||token.check("je")) {
      compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JZ, helper);
      return true;
	}
	else if (token.check("jnz")) {
      compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JNZ, helper);
      return true;
	}
	else if (token.check("jbe") || token.check("jna")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JBE, helper);
      return true;
	}
	else if (token.check("ja")||token.check("jnbe")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JA, helper);
      return true;
	}
	else if (token.check("js")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JS, helper);
      return true;
	}
	else if (token.check("jns")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JNS, helper);
      return true;
	}
	else if (token.check("jpe") || token.check("jp")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JP, helper);
      return true;
	}
	else if (token.check("jpo") || token.check("jnp")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JPO, helper);
      return true;
	}
	else if (token.check("jl")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JL, helper);
      return true;
	}
	else if (token.check("jge")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JGE, helper);
      return true;
	}
	else if (token.check("jle")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JLE, helper);
      return true;
	}
	else if (token.check("jg")) {
		compileJxx(token, info, &writer, x86Helper::JUMP_TYPE_JG, helper);
      return true;
	}
	else if (token.check("jmp")) {
		compileJMP(token, info, &writer, helper);
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandK(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandL(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, x86JumpHelper& helper)
{
	if (token.check("lea")) {
		compileLEA(token, info, &writer);
      return true;
	}
	else if (token.check("loop")) {
		compileLOOP(token, info, &writer, helper);
      return true;
	}
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
   else if (token.check("lock")) {
      prefix.lockMode = true;

      token.read();

      return true;
   }
   else return false;
}
bool x86Assembler :: compileCommandM(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
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
   else return false;
}
bool x86Assembler :: compileCommandN(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
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
bool x86Assembler :: compileCommandO(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
	if (token.check("or")) {
		compileOR(token, info, &writer);
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandP(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
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
   else return false;
}
bool x86Assembler :: compileCommandQ(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandR(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
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
	else if (token.check("repz")) {
		compileREPZ(token, info, &writer);
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandS(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
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
	else if (token.check("sar")) {
		compileSAR(token, info, &writer);
      return true;
	}
	else if (token.check("shl")) {
		compileSHL(token, info, &writer);
      return true;
	}
	else if (token.check("stc")) {
		compileSTC(token, info, &writer);
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
	else if (token.check("shld")) {
		compileSHLD(token, info, &writer);
      return true;
	}
	else if (token.check("shrd")) {
		compileSHRD(token, info, &writer);
      return true;
	}
	else if (token.check("setc")) {
		compileSETCC(token, info, &writer, x86Helper::JUMP_TYPE_JB);
      return true;
	}
	else if (token.check("setg")) {
		compileSETCC(token, info, &writer, x86Helper::JUMP_TYPE_JG);
      return true;
	}
	else if (token.check("setnc")) {
		compileSETCC(token, info, &writer, x86Helper::JUMP_TYPE_JAE);
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandT(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
	if (token.check("test")) {
		compileTEST(token, info, &writer);
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandU(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandV(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandW(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandX(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
	if (token.check("xor")) {
		compileXOR(token, info, &writer);
      prefix.clear();
      return true;
	}
	else if (token.check("xchg")) {
		compileXCHG(token, info, &writer);
      prefix.clear();
      return true;
	}
	else if (token.check("xadd")) {
		compileXADD(prefix, token, info, &writer);
      prefix.clear();
      return true;
	}
   else return false;
}
bool x86Assembler :: compileCommandY(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommandZ(TokenInfo& token)
{
   return false;
}
bool x86Assembler :: compileCommand(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, x86JumpHelper& helper)
{
   bool recognized = false;
   if (token.value[0]=='a') {
      recognized = compileCommandA(token, info, writer);
   }
   else if (token.value[0]=='b') {
      recognized = compileCommandB(token);
   }
   else if (token.value[0]=='c') {
      recognized = compileCommandC(prefix, token, info, writer, helper);
   }
   else if (token.value[0]=='d') {
      recognized = compileCommandD(token, info, writer);
   }
   else if (token.value[0]=='e') {
      recognized = compileCommandE(token);
   }
   else if (token.value[0]=='f') {
      recognized = compileCommandF(token, info, writer);
   }
   else if (token.value[0]=='g') {
      recognized = compileCommandG(token);
   }
   else if (token.value[0]=='h') {
      recognized = compileCommandH(token);
   }
   else if (token.value[0]=='i') {
      recognized = compileCommandI(token, info, writer);
   }
   else if (token.value[0]=='j') {
      recognized = compileCommandJ(token, info, writer, helper);
   }
   else if (token.value[0]=='k') {
      recognized = compileCommandK(token);
   }
   else if (token.value[0]=='l') {
      recognized = compileCommandL(prefix, token, info, writer, helper);
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
   else if (token.value[0]=='q') {
      recognized = compileCommandQ(token);
   }
   else if (token.value[0]=='r') {
      recognized = compileCommandR(token, info, writer);
   }
   else if (token.value[0]=='s') {
      recognized = compileCommandS(token, info, writer);
   }
   else if (token.value[0]=='t') {
      recognized = compileCommandT(token, info, writer);
   }
   else if (token.value[0]=='u') {
      recognized = compileCommandU(token);
   }
   else if (token.value[0]=='v') {
      recognized = compileCommandV(token);
   }
   else if (token.value[0]=='w') {
      recognized = compileCommandW(token);
   }
   else if (token.value[0]=='x') {
      recognized = compileCommandX(prefix, token, info, writer);
   }
   else if (token.value[0]=='y') {
      recognized = compileCommandY(token);
   }
   else if (token.value[0]=='z') {
      recognized = compileCommandZ(token);
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

void x86Assembler :: compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned)
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

	x86JumpHelper helper(&writer);

   PrefixInfo prefix;

	while (!token.check("end")) {
      compileCommand(prefix, token, info, writer, helper);
	}
   if (aligned)
	   writer.align(4, 0x90);
}

void x86Assembler :: compile(TextReader* source, path_t outputPath)
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
		else token.raiseErr("Invalid statement (%d)\n");

	} while (!token.Eof());

   FileWriter writer(outputPath, feRaw, false);
   binary.save(writer);
}
