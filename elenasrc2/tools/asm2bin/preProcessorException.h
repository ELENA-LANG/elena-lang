//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains the assembler exception struct
//
//                            (C)2005-2017, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#ifndef preprocessorExceptionH
#define preprocessorExceptionH

#include <cstdlib>

// --- PreProcessorException ---

namespace _ELENA_
{
	struct PreProcessorException
	{
		int row;
		const char* message;
		ident_t macroName;

		PreProcessorException(const char* message)
		{
			this->message = message;
			this->row = -1;
			this->macroName = NULL;
		}

		PreProcessorException(const char* message, int row)
		{
			this->message = message;
			this->row = row;
			this->macroName = NULL;
		}

		PreProcessorException(const char* message, ident_t macroName)
		{
			this->message = message;
			this->row = -1;
			this->macroName = macroName;
		}

		PreProcessorException(const char* message, ident_t macroName, int row)
		{
			this->message = message;
			this->row = row;
			this->macroName = macroName.clone();
		}
	};
}

#endif // preprocessorExceptionH
