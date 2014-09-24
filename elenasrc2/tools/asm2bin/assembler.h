//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract Assembler declarations
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef assemblerH
#define assemblerH

namespace _ELENA_
{

// --- AssemblerException ---

struct AssemblerException
{
	const char* message;
	int         row;

	AssemblerException(const char* message, int row)
	{
		this->message = message;
		this->row = row;
	}
};

// --- TokenInfo ---

struct TokenInfo
{
   SourceReader* reader;
   wchar16_t     value[50];
   LineInfo      terminal;

   bool Eof() const { return terminal.state == dfaEOF; }

   void raiseErr(const char* err)
   {
      if (!emptystr(err))
         throw AssemblerException(err, terminal.row);
   }

	bool getInteger(int& integer, Map<const wchar16_t*, size_t>& constants)
	{
      if (terminal.state==dfaInteger) {
         integer = StringHelper::strToInt(value);
         return true;
      }
		else if (terminal.state==dfaHexInteger) {
			value[getlength(value)-1] = 0;
         integer = StringHelper::strToLong(value, 16);
			return true;
      }
      else if (terminal.state==dfaIdentifier && constants.exist(value)) {
         integer = constants.get(value);
         return true;
      }
      else if (terminal.state==dfaFullIdentifier && constants.exist(value)) {
         integer = constants.get(value);
         return true;
      }
      else return false;
   }

	const wchar16_t* read()
	{
		terminal = reader->read(value, 50);

		return value;
	}

	const wchar16_t* read(const char* word, const char* err)
	{
		read();
		if (!check(word))
			raiseErr(err);

		return value;
	}

	int readInteger(Map<const wchar16_t*, size_t>& constants)
	{
		read();
		int integer;
		if (getInteger(integer, constants)) {
			return integer;
		}
		else raiseErr("Invalid number (%d)\n");
		return 0;
	}

	int readSignedInteger(Map<const wchar16_t*, size_t>& constants)
	{
      bool negative = false;
		read();

      if (check("-")) {
         negative = true;
		   read();
      }

		int integer;
		if (getInteger(integer, constants)) {
         return negative ? -integer : integer;
		}
		else raiseErr("Invalid number (%d)\n");
		return 0;
	}

   bool check(const wchar16_t* word)
	{
      return StringHelper::compare(value, word);
	}

   bool check(const char* word)
	{
      return ConstantIdentifier::compare(value, word);
	}

   TokenInfo(SourceReader* reader)
   {
      this->reader = reader;
   }
};

// --- Assembler ---

class Assembler
{
public:
	virtual void compile(TextReader* reader, const wchar_t* outputPath) = 0;

	virtual ~Assembler() {}
};

} // _ELENA_

#endif // assemblerH
