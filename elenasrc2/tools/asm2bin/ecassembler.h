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
   MessageMap                    verbs;

   struct LabelInfo
   {
      Map<const wchar16_t*, int> labels;
      Map<const wchar16_t*, int> fwdJumps;
   };

   int mapVerb(const wchar16_t* literal);

   void fixJump(const wchar16_t* label, MemoryWriter& writer, LabelInfo& info);

   ref_t compileRArg(TokenInfo& token, _Module* binary);

   void writeCommand(ByteCommand command, MemoryWriter& writer);

   void compileNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileNNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileICommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileRRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileExtCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileCreateCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);

   void compileNJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);
   void compileMccJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);
   void compileJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);

   void compileCommand(TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary);

   void compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned);

public:
	virtual void compile(TextReader* reader, const tchar_t* outputPath);

	ECodesAssembler()
	{
	}
	virtual ~ECodesAssembler() {}
};

} // _ELENA_

#endif // ecassemblerH
