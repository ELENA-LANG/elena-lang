//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains abstract Assembler declarations
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef assemblerH
#define assemblerH

namespace _ELENA_
{

#define ARGUMENT1           "__arg1"
#define ARGUMENT2           "__arg2"

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
   char          value[50];
   LineInfo      terminal;

   bool Eof() const { return terminal.state == dfaEOF; }

   void raiseErr(const char* err)
   {
      if (!emptystr(err))
         throw AssemblerException(err, terminal.row);
   }

	bool getInteger(int& integer, Map<ident_t, size_t>& constants)
	{
      if (terminal.state==dfaInteger) {
         integer = ident_t(value).toInt();
         return true;
      }
		else if (terminal.state==dfaHexInteger) {
			value[getlength(value)-1] = 0;
         integer = ident_t(value).toULong(16);
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

   bool getInteger(int& integer, Map<ident_t, ref64_t>& constants)
   {
      if (terminal.state == dfaInteger) {
         integer = ident_t(value).toInt();
         return true;
      }
      else if (terminal.state == dfaHexInteger) {
         value[getlength(value) - 1] = 0;
         integer = ident_t(value).toULong(16);
         return true;
      }
      else if (terminal.state == dfaIdentifier && constants.exist(value)) {
         integer = constants.get(value);
         return true;
      }
      else if (terminal.state == dfaFullIdentifier && constants.exist(value)) {
         integer = constants.get(value);
         return true;
      }
      else return false;
   }

   bool getLongInteger(ref64_t& integer, Map<ident_t, ref64_t>& constants)
   {
      if (terminal.state == dfaInteger) {
         integer = ident_t(value).toInt();
         return true;
      }
      if (terminal.state == dfaLong) {
         integer = ident_t(value).toULong(10);
         return true;
      }
      else if (terminal.state == dfaHexInteger) {
         value[getlength(value) - 1] = 0;
         integer = ident_t(value).toULongLong(16);
         return true;
      }
      else if (terminal.state == dfaIdentifier && constants.exist(value)) {
         integer = constants.get(value);
         return true;
      }
      else if (terminal.state == dfaFullIdentifier && constants.exist(value)) {
         integer = constants.get(value);
         return true;
      }
      else return false;
   }

	ident_t read()
	{
		terminal = reader->read(value, 50);
      if (terminal.state == dfaExplicitConst && value[getlength(value) - 1] == 'h')
         terminal.state = dfaHexInteger;

		return value;
	}

	ident_t read(const char* word, const char* err)
	{
		read();
		if (!check(word))
			raiseErr(err);

		return value;
	}

	int readInteger(Map<ident_t, size_t>& constants)
	{
		read();
		int integer;
		if (getInteger(integer, constants)) {
			return integer;
		}
		else raiseErr("Invalid number (%d)\n");
		return 0;
	}
   int readInteger(Map<ident_t, ref64_t>& constants)
   {
      read();
      int integer;
      if (getInteger(integer, constants)) {
         return integer;
      }
      else raiseErr("Invalid number (%d)\n");
      return 0;
   }

	int readSignedInteger(Map<ident_t, size_t>& constants)
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

   int readLongInteger(Map<ident_t, ref64_t>& constants)
   {
      read();
      ref64_t integer;
      if (getLongInteger(integer, constants)) {
         return integer;
      }
      else raiseErr("Invalid number (%d)\n");
      return 0;
   }

   bool check(ident_t word)
	{
      return word.compare(value);
	}
   bool check(ident_t word, size_t length)
   {
      return word.compare(value, length);
   }

   TokenInfo(SourceReader* reader)
   {
      this->reader = reader;
   }
};

// --- Assembler ---

class Assembler
{
protected:
   struct ProcedureInfo
   {
      _Module* binary;

      ref_t reference;
      Map<ident_t, int> parameters;
      bool inlineMode;

      ProcedureInfo(_Module* binary, bool inlineMode)
      {
         this->binary = binary;
         this->reference = 0;
         this->inlineMode = inlineMode;
      }
   };

   void readParameterList(TokenInfo& token, ProcedureInfo& info, ReferenceNs& refName)
   {
      while (true) {
         token.read();

         if (token.terminal.state == dfaIdentifier) {
            info.parameters.add(token.value, info.parameters.Count());
            token.read();

            if (!token.check(":"))
               token.raiseErr("Semicolumn expected (%d)\n");

            token.read();
            if (token.check("out")) {
               refName.append("&out");
               token.read();
               if (token.check(")"))
                  break;
            }
            refName.append('&');
            refName.append(token.value);

            token.read();
            if (token.check(")")) {
               break;
            }
            else if (!token.check(","))
               token.raiseErr("Comma expected (%d)\n");
         }
         else token.raiseErr("Invalid parameter list syntax (%d)\n");
      }
   }

   void checkComma(TokenInfo& token)
   {
      if (!token.check(","))
         throw AssemblerException("',' exprected(%d)\n", token.terminal.row);
   }

public:
	virtual void compile(TextReader* reader, path_t outputPath) = 0;

	virtual ~Assembler() {}
};

} // _ELENA_

#endif // assemblerH
