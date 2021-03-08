//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA PPC64Assembler
//		classes.
//                             (C)2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "ppc64assembler.h"
#include "module.h"

using namespace _ELENA_;

// -- PPC64Assembler --

PPC64Assembler::Operand PPC64Assembler :: defineRegister(TokenInfo& token)
{
   if (token.value[0] == 'r') {
      int index = ident_t(token.value + 1).toInt();
      if (index == 0 && !token.check("r0")) {
         return Operand(PPC64Helper::otUnknown);
      }
      else if (index >= 0 && index < 32) {
         return Operand((PPC64Helper::OperandType)index);
      }
   }
   else return Operand(PPC64Helper::otUnknown);
}

PPC64Assembler::Operand PPC64Assembler :: defineOperand(TokenInfo& token, ProcedureInfo& info, const char* err)
{
   Operand operand = defineRegister(token);
   if (operand.type == PPC64Helper::otUnknown) {

   }
   return operand;
}

PPC64Assembler::Operand PPC64Assembler :: compileOperand(TokenInfo& token, ProcedureInfo& info/*, _Module* binary*/, const char* err)
{
   Operand	    operand;

   token.read();
   operand = defineOperand(token, info, err);

   if (operand.type != PPC64Helper::otUnknown) {
      token.read();

   }

   return operand;
}

void PPC64Assembler :: compileLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code)
{
   Operand sour = compileOperand(token, info, "Invalid source operand (%d)\n");

   checkComma(token);

   Operand dest = compileOperand(token, info, "Invalid destination operand (%d)\n");

   token.raiseErr("Invalid command (%d)");
}

bool PPC64Assembler :: compileCommandL(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer)
{
   if (token.check("ld")) {
      compileLD(token, info, &writer);
      return true;
   }
   else return false;
}

bool PPC64Assembler::compileCommand(/*PrefixInfo& prefix, */TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer/*,
   AMD64JumpHelper& helper*/)
{
   bool recognized = false;
   //if (token.value[0] == 'a') {
   //   recognized = compileCommandA(token, info, writer);
   //}
   ////   else if (token.value[0]=='b') {
   ////      recognized = compileCommandB(token);
   ////   }
   //else if (token.value[0] == 'c') {
   //   recognized = compileCommandC(/*prefix, */token, info, writer/*, helper*/);
   //}
   //else if (token.value[0] == 'd') {
   //   recognized = compileCommandD(token, info, writer);
   //}
   ////   else if (token.value[0]=='e') {
   ////      recognized = compileCommandE(token);
   ////   }
   //else if (token.value[0] == 'f') {
   //   recognized = compileCommandF(token, info, writer);
   //}
   ////   else if (token.value[0]=='g') {
   ////      recognized = compileCommandG(token);
   ////   }
   ////   else if (token.value[0]=='h') {
   ////      recognized = compileCommandH(token);
   ////   }
   //else if (token.value[0] == 'i') {
   //   recognized = compileCommandI(token, info, writer);
   //}
   //else if (token.value[0] == 'j') {
   //   recognized = compileCommandJ(token, info, writer, helper);
   //}
   ////   else if (token.value[0]=='k') {
   ////      recognized = compileCommandK(token);
   ////   }
   /*else */if (token.value[0] == 'l') {
      recognized = compileCommandL(token, info, writer/*, helper*/);
   }
   //else if (token.value[0] == 'm') {
   //   recognized = compileCommandM(token, info, writer);
   //}
   //else if (token.value[0] == 'n') {
   //   recognized = compileCommandN(token, info, writer);
   //}
   //else if (token.value[0] == 'o') {
   //   recognized = compileCommandO(token, info, writer);
   //}
   //else if (token.value[0] == 'p') {
   //   recognized = compileCommandP(token, info, writer);
   //}
   ////   else if (token.value[0]=='q') {
   ////      recognized = compileCommandQ(token);
   ////   }
   //else if (token.value[0] == 'r') {
   //   recognized = compileCommandR(token, info, writer);
   //}
   //else if (token.value[0] == 's') {
   //   recognized = compileCommandS(token, info, writer);
   //}
   //else if (token.value[0] == 't') {
   //   recognized = compileCommandT(token, info, writer);
   //}
   //   else if (token.value[0]=='u') {
   //      recognized = compileCommandU(token, info, writer);
   //   }
   //   else if (token.value[0]=='v') {
   //      recognized = compileCommandV(token);
   //   }
   //   else if (token.value[0]=='w') {
   //      recognized = compileCommandW(token);
   //   }
   //else if (token.value[0] == 'x') {
   //   recognized = compileCommandX(token, info, writer);
   //}
   ////   else if (token.value[0]=='y') {
   ////      recognized = compileCommandY(token);
   ////   }
   ////   else if (token.value[0]=='z') {
   ////      recognized = compileCommandZ(token);
   ////   }
   //else if (token.value[0] == '#') {
   //   token.read();
   //   token.read();
   //   token.read();
   //   recognized = true;
   //}

   if (!recognized) {
      //if (token.Eof()) {
         token.raiseErr("Invalid end of the file\n");
      //}
      //else {
      //   fixJump(token, info, &writer, helper);
      //   token.read();
      //}
   }
   return recognized;
}

void PPC64Assembler :: compileStructure(TokenInfo& token, _Module* binary, int mask)
{
   //token.read();

   //int ref = 0;
   //if (token.check("%")) {
   //   ref = token.readInteger(constants);
   //}
   //else {
   //   ReferenceNs refName(NATIVE_MODULE, token.value);

   //   ref = binary->mapReference(refName) | mask;
   //}

   //ProcedureInfo info(binary, false);

   //if (binary->mapSection(ref, true) != NULL) {
   //   throw AssemblerException("Structure / Procedure already exists (%d)\n", token.terminal.row);
   //}
   //_Memory* code = binary->mapSection(ref, false);
   //MemoryWriter writer(code);

   //token.read();
   //while (!token.check("end")) {
   //   if (token.check("dd")) {
   //      token.read();
   //      Operand operand = defineOperand(token, info, "Invalid constant");
   //      if (operand.type != AMD64Helper::otUnknown) {
   //         token.read();

   //         if (token.check("+")) {
   //            operand.offset += token.readInteger(constants);

   //            token.read();
   //         }
   //      }

   //      if (operand.type == AMD64Helper::otDD) {
   //         AMD64Helper::writeImm(&writer, operand);
   //      }
   //      else if (operand.type == AMD64Helper::otDB) {
   //         operand.type = AMD64Helper::otDD;
   //         AMD64Helper::writeImm(&writer, operand);
   //      }
   //      else token.raiseErr("Invalid operand (%d)");
   //   }
   //   else if (token.check("dq")) {
   //      token.read();
   //      Operand operand = defineOperand(token, info, "Invalid constant");
   //      if (operand.type != AMD64Helper::otUnknown) {
   //         token.read();

   //         if (token.check("+")) {
   //            operand.offset += token.readInteger(constants);

   //            token.read();
   //         }
   //      }

   //      if (operand.type == AMD64Helper::otDQ) {
   //         AMD64Helper::writeImm(&writer, operand);
   //      }
   //      else if (operand.type == AMD64Helper::otDD) {
   //         if (!operand.reference) {
   //            operand.type = AMD64Helper::otDQ;
   //            AMD64Helper::writeImm(&writer, operand);
   //         }
   //         else {
   //            AMD64Helper::writeImm(&writer, operand);
   //            writer.writeDWord(0);
   //         }
   //      }
   //      else if (operand.type == AMD64Helper::otDB) {
   //         operand.type = AMD64Helper::otDQ;
   //         AMD64Helper::writeImm(&writer, operand);
   //      }
   //      else token.raiseErr("Invalid operand (%d)");
   //   }
   //   else if (token.check("dw")) {
   //      token.read();
   //      Operand operand = defineOperand(token, info, "Invalid constant");
   //      if (operand.type != AMD64Helper::otUnknown)
   //         token.read();

   //      if (operand.type == AMD64Helper::otDB) {
   //         operand.type = AMD64Helper::otDW;
   //         AMD64Helper::writeImm(&writer, operand);
   //      }
   //      else token.raiseErr("Invalid operand (%d)");
   //   }
   //   else if (token.check("db")) {
   //      token.read();
   //      Operand operand = defineOperand(token, info, "Invalid constant");
   //      if (operand.type != AMD64Helper::otUnknown)
   //         token.read();

   //      if (operand.type == AMD64Helper::otDB) {
   //         AMD64Helper::writeImm(&writer, operand);
   //      }
   //      else token.raiseErr("Invalid operand (%d)");
   //   }
   //   else if (token.Eof()) {
   //      token.raiseErr("Invalid end of the file\n");
   //   }
   //   else token.raiseErr("Invalid operand (%d)");
   //}
}

void PPC64Assembler::compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned)
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
   if (binary->mapSection(info.reference, true) != NULL) {
      throw AssemblerException("Procedure already exists (%d)\n", token.terminal.row);
   }

   _Memory* code = binary->mapSection(info.reference, false);
   MemoryWriter writer(code);

//   AMD64JumpHelper helper(&writer);

   //PrefixInfo prefix;

   while (!token.check("end")) {
      compileCommand(/*prefix, */token, info, writer/*, helper*/);
   }
   //if (aligned)
   //   writer.align(8, 0x90);
}

void PPC64Assembler:: compile(TextReader* source, path_t outputPath)
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
