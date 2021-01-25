//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract ECODES Assembler declarations
//
//                                             (C)2013-2020, by Alexei Rakov
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
   ident_t              postfix;
   Map<ident_t, size_t> constants;
   //MessageMap           verbs;

   struct LabelInfo
   {
      Map<ident_t, int> labels;
      Map<ident_t, int> fwdJumps;

	  Map<ident_t, size_t> declaredLabels;

	  bool declareLabel(ident_t label)
	  {
		  if (declaredLabels.get(label) == 0)
		  {
			  declaredLabels.add(label, declaredLabels.Count() + 1);
			  return true;
		  }
		  return false;
	  }

	  bool checkAllUsedLabels(ident_t errorMessage, ident_t procedureName)
	  {
		  auto it = labels.start();
		  while (!it.Eof())
		  {
			  ident_t label = it._item()->key;

			  // Check if label is declared
			  if (declaredLabels.get(label) == NULL)
				  throw AssemblerException(errorMessage, label, procedureName);
			  it++;
		  }
		  return false;
	  }
   };

   //int mapVerb(ident_t literal);

   void fixJump(ident_t label, MemoryWriter& writer, LabelInfo& info);

   bool readMessage(ident_t quote, IdentifierString& subject, ref_t& signRef, int& paramCount, _Module* binary);
   void readMessage(TokenInfo& token, IdentifierString& subject, ref_t& signRef, int& paramCount);
   void compileMessage(TokenInfo& token, IdentifierString& message, ref_t& signRef, _Module* binary);
   void compileMessageName(TokenInfo& token, IdentifierString& message);

   int compileFArg(TokenInfo& token, _Module* binary);
   ref_t compileRArg(TokenInfo& token, _Module* binary);
   ref_t compileRMessageArg(TokenInfo& token, _Module* binary);
   ref_t compileMessageArg(TokenInfo& token, _Module* binary);

   void writeCommand(ByteCommand command, MemoryWriter& writer);

   void compileNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileVCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileNNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileICommand(ByteCode code, TokenInfo& token, MemoryWriter& writer);
   void compileFCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileFNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   //void compileFCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileRRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileRMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileRNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileExtNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);
   void compileCreateCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary);

   void compileRJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary);
   void compileNJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);
   void compileMccJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary);
   void compileJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info);

   void compileCommand(TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary);

   void compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned, int mask);

   void loadDefaultConstants();

public:
	virtual void compile(TextReader* reader, path_t outputPath);

	ECodesAssembler(ident_t postfix)
	{
      this->postfix = postfix;

      loadDefaultConstants();
	}
	virtual ~ECodesAssembler() {}
};

} // _ELENA_

#endif // ecassemblerH
