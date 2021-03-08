//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains Power8 64 declarations
//
//                             (C)2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef ppc64assemblerH
#define ppc64assemblerH

#include "elena.h"
#include "source.h"
#include "assembler.h"
#include "ppc64helper.h"

namespace _ELENA_
{

// --- PPC64Assembler ---

class PPC64Assembler : public Assembler
{
	typedef PPC64Helper::Operand       Operand;
	typedef PPC64Helper::OperandType   OperandType;

protected:
	Map<ident_t, ref64_t> constants;

	Operand defineRegister(TokenInfo& token);
	Operand defineOperand(TokenInfo& token, ProcedureInfo& info, const char* err);
	Operand compileOperand(TokenInfo& token, ProcedureInfo& info, const char* err);

	void compileLD(TokenInfo& token, ProcedureInfo& info, MemoryWriter* code);

	bool compileCommandL(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer);

	bool compileCommand(TokenInfo& token, ProcedureInfo& info, MemoryWriter& writer/*,
		AMD64JumpHelper& helper*/);

	virtual void compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned);
	virtual void compileStructure(TokenInfo& token, _Module* binary, int mask);

public:
	virtual void compile(TextReader* reader, path_t outputPath);

	PPC64Assembler()
	{
	}
	virtual ~PPC64Assembler() {}
};

} // _ELENA_

#endif // ppc64assemblerH
