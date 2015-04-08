//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract ECODES Assembler declarations
//
//                                             (C)2013-2015, by Alexei Rakov
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
   Map<ident_t, size_t> constants;
   MessageMap                    verbs;

   struct LabelInfo
   {
      Map<ident_t, int> labels;
      Map<ident_t, int> fwdJumps;
   };

   int mapVerb(ident_t literal);

   void fixJump(ident_t label, MemoryWriter& writer, LabelInfo& info);

   void readMessage(TokenInfo& token, int& verbId, IdentifierString& subject, int& paramCount);

   ref_t compileRArg(TokenInfo& token, _Module* binary);
   ref_t compileRMessageArg(TokenInfo& token, _Module* binary);
   ref_t compileMessageArg(TokenInfo& token, _Module* binary);

   void writeCommand(ByteCommand command, MemoryWriter& writer);

   void compileNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileNNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileICommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileRRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileRMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileExtCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileCreateCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);

   void compileRJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary);
   void compileNJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);
   void compileMccJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);
   void compileJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);

   void compileCommand(TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary);

   void compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned);

public:
	virtual void compile(TextReader* reader, path_t outputPath);

	ECodesAssembler()
	{
	}
	virtual ~ECodesAssembler() {}
};

} // _ELENA_

#endif // ecassemblerH
