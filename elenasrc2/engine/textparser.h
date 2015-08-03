//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Source Reader class declaration.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef textparserH
#define textparserH 1

namespace _ELENA_
{

// --- LineTooLong exception class ---

class LineTooLong : public _Exception
{
public:
   size_t row;

   LineTooLong(size_t row)
   {
      this->row = row;
   }
};

// --- InvalidChar exception class ---

class InvalidChar : public _Exception
{
public:
   int column, row;
   int ch;

   InvalidChar(int column, int row, int ch)
   {
      this->column = column;
      this->row = row;
      this->ch = ch;
   }
};

// --- ELENA Source Reader class ---

struct LineInfo
{
   ident_t line;
   char    state;
   int     length;

   int position;
   int column, row;

   LineInfo()
   {
      line = NULL;
      column = row = state = 0;
   }
   LineInfo(int position, int column, int row)
   {
      this->position = position;
      this->column = column;
      this->row = row;
      this->state = 0;
   }
};

template <char dfaMaxChar, char dfaStart, char dfaWhitespace, int maxLength> class _TextParser
{
protected:
   const char** _dfa;

   TextReader*  _source;
   int          _tabSize;
   ident_c*     _line;
   size_t       _position;
   size_t       _column, _row;

   void nextColumn(size_t& position)
   {
      if (_line[position]=='\t') {
         _column += calcTabShift(_column - 1, _tabSize);
      }
      else _column++;

      position++;
   }

   bool cacheLine()
   {
      if (_source->read(_line, maxLength)) {
         _position = 0;

         _row++;
         _column = 1;
         if (getlength(_line) == maxLength)
            throw LineTooLong(_row);

         return true;
      }
      else return false;
   }

public:
   void step(uident_c ch, char& state, char& terminateState)
   {
      if(ch > dfaMaxChar) ch = dfaMaxChar;

      terminateState = _dfa[state - dfaStart][ch];

      if (terminateState > dfaStart) {
         state = terminateState;

         terminateState = 0;
      }
   }

   char readLineInfo(char startState, LineInfo& info)
   {
      char state = startState;
      char terminateState = 0;
      do {
         if (_line[_position]=='\0') {
            if(cacheLine()) {
               info.position = _position;
               info.column = _column;
               info.row = _row;
            }
         }

         step(_line[_position], state, terminateState);
         if (terminateState) {
            if (terminateState == dfaWhitespace) {
               info = LineInfo(_position, _column, _row);

               state = startState;
               terminateState = 0;
            }
         }
         else nextColumn(_position);
      }
      while (!terminateState);

      info.state = state;
      return terminateState;
   }

   _TextParser(const char** dfa, int tabSize, TextReader* source)
   {
      _tabSize = tabSize;
      _source = source;
      _row = 0;
      _line = StringHelper::allocate(maxLength + 1, DEFAULT_STR);

      _dfa = dfa;

      if (!cacheLine()) {
         _position = 0;
         _line[0] = 0;
      }
   }
   virtual ~_TextParser() { freestr(_line); }
};

} // _ELENA_

#endif // textparserH
