//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains the assembler exception struct
//
//                            (C)2005-2017, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#ifndef assemblerExceptionH
#define assemblerExceptionH

#include <cstdlib>

// --- AssemblerException ---

namespace _ELENA_
{
struct AssemblerException
{
	int row;
	int procedureNumber;
	const char* message;
	ident_t messageArguments;
	ident_t procedureName;

	AssemblerException(const char* message, int row)
	{
		this->message = message;
		this->row = row;
		this->messageArguments = NULL;
		this->procedureName = NULL;
	}

	AssemblerException(const char* message, ident_t textArgument, int procedureNumber)
	{
		this->message = message;
		this->messageArguments = textArgument.clone();
		this->procedureNumber = procedureNumber;
		this->procedureName = NULL;
      this->row = 0;
	}

	AssemblerException(const char* message, ident_t textArgument, ident_t procedureName)
	{
		this->message = message;
		this->messageArguments = textArgument.clone();
		this->procedureName = procedureName.clone();
      this->row = 0;
	}
};
}

#endif // assemblerExceptionH
