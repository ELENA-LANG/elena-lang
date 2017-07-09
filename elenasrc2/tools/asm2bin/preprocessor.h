//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Assembler pre-processor
//
//                            (C)2005-2017, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#ifndef preprocessorH
#define preprocessorH

#include "elena.h"
#include "source.h"
#include "assembler.h"

namespace _ELENA_
{
class MacroDefinition
{
public:
	struct MacroCommand
	{
		ident_t _command;
		size_t _line;
	};

private:
	ident_t _macroName;
	Map<ident_t, size_t> _arguments;
	List<MacroCommand*> _commands;

public:
	void setName(ident_t macroName)
	{
		_macroName = macroName;
	}

	ident_t getName()
	{
		return _macroName;
	}

	void addArgument(ident_t argument)
	{
		_arguments.add(argument, _arguments.Count() + 1);
	}

	size_t getArgument(ident_t argument)
	{
		return _arguments.get(argument);
	}

	void insertCommand(ident_t command, int row)
	{
		MacroCommand* mc = new MacroCommand();
		mc->_command = command.clone();
		mc->_line = row;

		_commands.add(mc);
	}

	List<MacroCommand*> getMacroCommands()
	{
		return _commands;
	}
};

class PreProcessor
{
   typedef String<char, 0x1000>  BufferString;

private:
	int _outputRowCounter;
	ident_t _outPutFileName;
	Path _sourcePath;
	Path _outputPath;
	TextFileReader _reader;
	FILE* _output;

	List<MacroDefinition*> _macros; // Can't use MAP :(  --- Map<ident_t, MacroDefinition> _macros;

public:
   bool isOpened() { return _reader.isOpened(); }

	PreProcessor(ident_t sourceFile);
	~PreProcessor();
	void preProcess();

	ident_t getTempFileName()
	{
		if (_outPutFileName == NULL)
			_outPutFileName = ident_t(tmpnam(NULL));

		return _outPutFileName;
	}

private:
	void initOutputFile();
	void closeOutputFile();
	void preProcessDefMacro(TokenInfo token);
	void preProcessImport();
	void convertMacroCall(TokenInfo token);
	void writeToken(ident_t line, int row);
	void preProcessString(TokenInfo token, BufferString& string);

	MacroDefinition* searchForMacroDefinition(ident_t macroName, int currentLine);
};
}

#endif // preprocessorH