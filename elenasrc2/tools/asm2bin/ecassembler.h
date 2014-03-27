//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract ECODES Assembler declarations
//
//                                             (C)2013-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef ecassemblerH
#define ecassemblerH

#include "elena.h"
#include "source.h"
#include "assembler.h"
#include "bytecode.h"

namespace _ELENA_
{

// --- ECodesAssembler ---

class ECodesAssembler : public Assembler
{
	Map<const wchar16_t*, size_t> constants;

   struct LabelInfo
   {
      Map<const wchar16_t*, int> labels;
      Map<const wchar16_t*, int> fwdJumps;
   };

   void fixJump(const wchar16_t* label, MemoryWriter& writer, LabelInfo& info);

   void writeCommand(ByteCommand command, MemoryWriter& writer);

   virtual void compileNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   virtual void compileNNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   virtual void compileICommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   virtual void compileRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);

   virtual void compileNJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);
   virtual void compileMccJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);
   virtual void compileJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);

   virtual void compileCommand(TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary);

   virtual void compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned);

public:
	virtual void compile(TextReader* reader, const tchar_t* outputPath);

	ECodesAssembler()
	{
	}
	virtual ~ECodesAssembler() {}
};

} // _ELENA_

#endif // ecassemblerH
