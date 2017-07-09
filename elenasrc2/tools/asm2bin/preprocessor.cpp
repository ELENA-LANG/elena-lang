//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Assembler pre-processor
//
//                            (C)2005-2017, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#include "preprocessor.h"
// ----------------------------------------
#include "preProcessorException.h"

namespace _ELENA_
{
	PreProcessor::PreProcessor(ident_t sourceFile)
		: _outputRowCounter(-1),
		  _sourcePath(sourceFile.c_str()),
	 	  _reader(_sourcePath.c_str(), _ELENA_::feUTF8, true)
	{
	}

	PreProcessor::~PreProcessor()
	{
		if (_output)
			fclose(_output);

		// Delete the generate pre-processed file...
		remove(_outPutFileName);
	}

	void PreProcessor::preProcess()
	{
		SourceReader reader(4, &_reader);
		TokenInfo token(&reader);

		initOutputFile();

		do
		{
			token.read();
         if (token.terminal.state == dfaQuote) {
            BufferString temp;
            preProcessString(token, temp);

            writeToken(temp.c_str(), token.terminal.row);
         }
         else if (token.check("defmacro")) {
				preProcessDefMacro(token);
			}
			else if (token.check("import")) {

			}
			else if (token.check("#")) { // Calling a macro definition inside procedure or some other part of assembly code...
				convertMacroCall(token);
			}
         else writeToken(token.value, token.terminal.row);

		} while (!token.Eof());

		closeOutputFile();
	}

	void PreProcessor::initOutputFile()
	{
		_output = fopen(getTempFileName(), "w");
		if (!_output)
			throw PreProcessorException("Fail to create temporary file"); // Just a test
	}

	void PreProcessor::closeOutputFile()
	{
		if (_output)
		{
			fclose(_output);
			_output = NULL;
		}
	}

	void PreProcessor::preProcessDefMacro(TokenInfo token)
	{
		MacroDefinition* mc = new MacroDefinition();

		token.read();
		if (!token.check("#"))
			throw PreProcessorException("Expected '#' in macro name definition in line %d\n", token.terminal.row);

		ident_t macroName = token.read().clone();
		mc->setName(macroName);

		token.read();
		if (!token.check("("))
			throw PreProcessorException("Constant \"(\" not found in macro %s\n", macroName, token.terminal.row);

		// parse all the arguments
		while (!token.check(")") && !token.Eof())
		{
			ident_t argName = token.read().clone();
			token.read();
			if (!token.check(",") && !token.check(")"))
				throw PreProcessorException("Constant ',' or ')' not found in macro ' %s ' parameters declaration\n", macroName, token.terminal.row);

			mc->addArgument(argName);
		}

		// Parse all macro
		token.read();
      BufferString temp;
		while (!token.check("endmacro") && !token.Eof())
		{
         if (token.terminal.state == dfaQuote) {
            temp.clear();
            preProcessString(token, temp);

            mc->insertCommand(temp.c_str(), token.terminal.row);
         }
         else mc->insertCommand(token.value, token.terminal.row);
			
			token.read();
		}

		// check if last token is really a "endmacro"
		if(!token.check("endmacro"))
			throw PreProcessorException("endmacro not found in macro ' %s '\n", macroName);

		_macros.add(mc);
	}

	void PreProcessor::preProcessImport()
	{
	}

	void PreProcessor::convertMacroCall(TokenInfo token)
	{
		ident_t macroName = token.read();
		MacroDefinition* mc = searchForMacroDefinition(macroName, token.terminal.row);

		token.read();
		if (!token.check("("))
			throw PreProcessorException("Expected '%s' on macro calling, in line %d\n", "(", token.terminal.row);

		// parse all parameters
		Map<size_t, char*> parameters(NULL, freestr); // NOTE : we have to proper release the cloned strings
      BufferString parameter;

		token.read();
		while(true)
		{
         if (token.terminal.state == dfaQuote) {
            preProcessString(token, parameter);
         }
			else if (token.check(",") || token.check(")")) {
				parameters.add(parameters.Count() + 1, ident_t(parameter.c_str()).clone());
				parameter.clear();

				if (token.check(")") || token.Eof())
					break;
			}
			else parameter.append(token.value);

			token.read();
		}

		// convert the macro call
		List<MacroDefinition::MacroCommand*> commands = mc->getMacroCommands();
		for (size_t i = 0; i < commands.Count(); i++)
		{
			MacroDefinition::MacroCommand* cmd = commands.get(i)._item()->item;
			size_t arq = mc->getArgument(cmd->_command);
			if(arq > 0)
				writeToken(parameters.get(arq), cmd->_line);
			else
				writeToken(cmd->_command, cmd->_line);
		}
	}

	void PreProcessor::writeToken(ident_t token, int row)
	{
		if(_outputRowCounter == -1)
			_outputRowCounter = row;
		else if (_outputRowCounter != row)
		{
			fprintf(_output, " #:%d\n", _outputRowCounter); // Write to the file the original source line
			_outputRowCounter = row;
		}

		fprintf(_output, "%s ", token.c_str());
	}

	void PreProcessor :: preProcessString(TokenInfo token, BufferString& string)
	{
      QuoteTemplate<BufferString> quote(token.terminal.line);

      string.append('"');      
      string.append(quote.ident());
      string.append('"');
	}

	MacroDefinition* PreProcessor::searchForMacroDefinition(ident_t macroName, int currentLine)
	{
		// o(n) operation for this... It's not good :(
		for (size_t i = 0; i < _macros.Count(); i++)
		{
			auto itm = _macros.get(i)._item()->item;
			if (itm->getName().compare(macroName))
				return itm;
		}
		throw PreProcessorException("Calling a not defined macro ( %s ), in line %d\n", macroName, currentLine);
	}
}