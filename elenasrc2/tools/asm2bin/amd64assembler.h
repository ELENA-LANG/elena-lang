//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains I64Assembler declarations
//
//                             (C)2005-2021, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#ifndef amd64assemblerH
#define amd64assemblerH

#include "elena.h"
#include "source.h"
#include "assembler.h"
#include "amd64helper.h"
#include "amd64jumphelper.h"

namespace _ELENA_
{

// --- AMD64Assembler ---

class AMD64Assembler : public Assembler
{
   typedef AMD64Helper::Operand       Operand;
   typedef AMD64Helper::OperandType   OperandType;

protected:
	Map<ident_t, ref64_t> constants;

 //  struct PrefixInfo
 //  {
 //     bool lockMode;

 //     bool Exists() { return lockMode; }

 //     void clear()
 //     {
 //        lockMode = false;
 //     }

 //     PrefixInfo()
 //     {
 //        clear();
 //     }
 //  };

   void setOffsetSize(Operand& operand)
   {
      if (abs(operand.offset) <= 0x80 && (size_t)operand.offset != 0x80000000) {
         operand.type = AMD64Helper::otDB;
      }
      else operand.type = AMD64Helper::otDD;
	}

 //  void loadDefaultConstants();

   int readStReg(TokenInfo& token);
   bool setOffset(Operand& operand, Operand disp);

   Operand defineRegister(TokenInfo& token);
   Operand defineOperand(TokenInfo& token, ProcedureInfo& info, const char* err);
   Operand defineDisplacement(TokenInfo& token, ProcedureInfo& info, const char* err);

   Operand readDispOperand(TokenInfo& token, ProcedureInfo& info, const char* err, OperandType prefix);
   Operand readPtrOperand(TokenInfo& token, ProcedureInfo& info, const char* err, OperandType prefix);

   Operand compileOperand(TokenInfo& token, ProcedureInfo& info, const char* err);

   void compileMOV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileCMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileADD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileXADD(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileXORPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
   void compileADC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileADDPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileADDSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileANDPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileANDNPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE

   void compileAND(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileXOR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileOR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileORPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
   void compileLEA(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileSUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileSUBPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileSUBSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileSQRTPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileSQRTSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
   void compileSBB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileTEST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileUCOMISS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
   void compileSHR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileSAR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileSHL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileSHLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileSHRD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileROL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileROR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileRCR(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileRCL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileXCHG(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);

	void compileMOVZX(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);

	void compileRET(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileNOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileCQO(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileCDQ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileCWDE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileLODSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileLODSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileLODSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileSTOSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileSTOSQ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileSTOSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileMOVSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileMOVSD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileMOVSXD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileMOVAPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileMOVUPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	void compileSTOSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileCMPSB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileCMPXCHG(PrefixInfo& prefix, TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileSTC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileSAHF(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compilePUSHFD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compilePOPFD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compilePAVGB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePAVGW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePSADBW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePEXTRW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePINSRW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePMAXSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePMAXUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePMINSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePMINUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePMOVMSKB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePMULHUW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compilePSHUFW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE

	void compileREP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileREPZ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileRCPPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileRCPSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileRSQRTPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileRSQRTSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE

   void compilePUSH(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compilePOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);

	//void compileDEC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileINC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileINT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileNEG(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileNOT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileMULPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileMULSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileMAXPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileMAXSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileMINPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileMINSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE

	void compileIMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileIDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileDIVPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
	//void compileDIVSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE

	void compileCALL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code/*, x86JumpHelper& helper*/);
	//void compileLOOP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, x86JumpHelper& helper);

   void compileJxx(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int prefix, AMD64JumpHelper& helper);
	void compileJMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, AMD64JumpHelper& helper);

	void fixJump(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, AMD64JumpHelper& helper);

   void compileFINIT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFLDZ(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFLDL2T(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFLDLG2(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFMULP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFRNDINT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileF2XM1(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFLD1(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFADDP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileFSUBP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	//void compileFDIVP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFSCALE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFXAM(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFABS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFSQRT(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFSIN(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFCOS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFYL2X(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFTST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);

	void compileFBLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFCHS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFILD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFIST(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFISTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFADD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFISUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFIMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFSUB(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFMUL(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFIDIV(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFXCH(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFSTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFBSTP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFSTSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileFNSTSW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFSTCW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFLDCW(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
	void compileFCOMIP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
 //  void compileFCOMP(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFLDPI(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFPREM(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFPATAN(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFLDL2E(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFLDLN2(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileFFREE(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);
   void compileSETCC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int postfix);
   void compileCMOVCC(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code, int postfix);
 //  void compileCMPPS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileCMPSS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE
 //  void compileCOMISS(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code); // SSE

   bool compileCommandA(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
 //  bool compileCommandB(TokenInfo& token);
   bool compileCommandC(/*PrefixInfo& prefix, */TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer/*, x86JumpHelper& helper*/);
   bool compileCommandD(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
 //  bool compileCommandE(TokenInfo& token);
   bool compileCommandF(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
 //  bool compileCommandG(TokenInfo& token);
 //  bool compileCommandH(TokenInfo& token);
   bool compileCommandI(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
   bool compileCommandJ(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, AMD64JumpHelper& helper);
 //  bool compileCommandK(TokenInfo& token);
   bool compileCommandL(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer/*, x86JumpHelper& helper*/);
   bool compileCommandM(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
   bool compileCommandN(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
   bool compileCommandO(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
   bool compileCommandP(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
 //  bool compileCommandQ(TokenInfo& token);
   bool compileCommandR(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
   bool compileCommandS(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
   bool compileCommandT(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
 //  bool compileCommandU(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
 //  bool compileCommandV(TokenInfo& token);
 //  bool compileCommandW(TokenInfo& token);
   bool compileCommandX(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);
 //  bool compileCommandY(TokenInfo& token);
 //  bool compileCommandZ(TokenInfo& token);
   bool compileCommand(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer, 
		AMD64JumpHelper& helper);

   virtual void compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned);
   virtual void compileStructure(TokenInfo& token, _Module* binary, int mask);

public:
	virtual void compile(TextReader* reader, path_t outputPath);

   AMD64Assembler()
	{
	   //loadDefaultConstants();
	}
	virtual ~AMD64Assembler() {}
};

} // _ELENA_

#endif // amd64assemblerH
