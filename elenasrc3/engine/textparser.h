//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Source Reader class declaration.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TEXTPARSER_H
#define TEXTPARSER_H

#include "elena.h"

namespace elena_lang
{
   // --- ELENA DFA Constants ---
   constexpr char dfaMaxChar        = 127;

   constexpr char dfaStart          = 'a';
   constexpr char dfaIdentifier     = 'c';
   constexpr char dfaOperator       = 'd';
   constexpr char dfaInteger        = 'e';
   constexpr char dfaQuote          = 'g';
   constexpr char dfaHexInteger     = 'j';
   constexpr char dfaReference      = 'm';
   constexpr char dfaAltOperator    = 'q';
   constexpr char dfaCharacter      = 's';
   constexpr char dfaQuoteCode      = 'u';
   constexpr char dfaSignStart      = 'w';
   constexpr char dfaWideQuote      = 'x';
   constexpr char dfaGrOperator     = 'y';
   constexpr char dfaCustomNumber   = 'j';

   constexpr char dfaError          = '?';
   constexpr char dfaEOF            = '.';
   constexpr char dfaWhitespace     = '*';
   constexpr char dfaMinusLookahead = '-';  // indicates that if minus is preceeded by the operator it may be part of the digit
   constexpr char dfaDotLookahead   = '$';
   constexpr char dfaBack           = '!';


   // --- LineInfo ---
   struct LineInfo
   {
      pos_t position;
      pos_t column, row;

      void clear()
      {
         position = column = row = 0;
      }

      LineInfo()
      {
         position = 0;
         column = row = 0;
      }
   };

   // --- LineTooLong ---
   class LineTooLong : public ExceptionBase
   {
   public:
      LineInfo lineInfo;

      LineTooLong(LineInfo lineInfo)
      {
         this->lineInfo = lineInfo;
      }
   };

   // --- InvalidElement ---
   template<class T> class InvalidElement : public ExceptionBase
   {
   public:
      LineInfo lineInfo;
      T        ch;

      InvalidElement(LineInfo lineInfo, T ch)
      {
         this->lineInfo = lineInfo;
         this->ch = ch;
      }
   };

   // --- InvalidChar ---
   typedef InvalidElement<char>  InvalidChar;

   class SyntaxError : public ExceptionBase
   {
   public:
      LineInfo lineInfo;
      ustr_t   message;

      SyntaxError(ustr_t message, LineInfo lineInfo)
      {
         this->lineInfo = lineInfo;
         this->message = message;
      }
   };

   class ProcedureError : public ExceptionBase
   {
   public:
      ustr_t           message;
      IdentifierString name;
      IdentifierString arg;

      ProcedureError(ustr_t message, ustr_t name, ustr_t arg)
         : message(message), name(name), arg(arg)
      {
      }
   };

   // --- TextParser ---
   typedef bool(*state_matcher)(char ch);

   inline bool dummy_matcher(char) { return false; }

   /// <summary>
   /// TextParser tamplate
   /// </summary>
   /// <typeparam name="T">char or wchar</typeparam>
   /// <parameter name="finalState">Parser will stop when DFA in this state. The states which values are less than it, is considered as a special final states</typeparam>
   template<class T, int maxLength, char finalState = dfaStart, char maxChar = dfaMaxChar, state_matcher isContinuous = dummy_matcher> class TextParser
   {
   protected:
      T              _line[maxLength];
      LineInfo       _lineInfo;
      pos_t          _startPosition;
      pos_t          _position;
      int            _tabSize;

      TextReader<T>* _reader;

   private:
      const char**   _dfa;

      bool cacheLine()
      {
         _lineInfo.position = _reader->position();
         if (_reader->read(_line, maxLength)) {
            _startPosition = _position = 0;

            _lineInfo.row++;
            _lineInfo.column = 1;
            if (getlength(_line) == maxLength)
               throw LineTooLong(_lineInfo);

            return true;
         }
         else return false;
      }

      bool continueLine()
      {
         if (_reader->read(_line + _position, maxLength - _position)) {
            _lineInfo.row++;
            _lineInfo.column = 1;
            if (getlength(_line) == maxLength)
               throw LineTooLong(_lineInfo);

            return true;
         }
         return false;
      }

   protected:
      void nextColumn()
      {
         if (_line[_position] == '\t') {
            _lineInfo.column += calcTabShift(_lineInfo.column - 1, _tabSize);
         }
         else _lineInfo.column++;

         _position++;
      }

   public:
      void step(T ch, char& state, char& terminateState)
      {
         char c = ch > maxChar ? maxChar : (char)ch;

         terminateState = _dfa[state - finalState][(unsigned char)c];

         if (terminateState > finalState) {
            state = terminateState;

            terminateState = 0;
         }
      }

      char read(char startState, char& previousState, LineInfo& lineInfo)
      {
         lineInfo = _lineInfo;
         _startPosition = _position;

         char terminateState = 0;
         char state = startState;
         do
         {
            if (_line[_position] == 0) {
               if (isContinuous(state)) {
                  continueLine();
               }
               else if (cacheLine()) {
                  lineInfo = _lineInfo;
               }
            }

            step(_line[_position], state, terminateState);

            if (terminateState) {
               if (terminateState == dfaWhitespace) {
                  lineInfo = _lineInfo;
                  _startPosition = _position;

                  state = startState;
                  terminateState = 0;
               }
            }
            else nextColumn();

         } while (!terminateState);

         previousState = state;
         return terminateState;
      }

      TextParser(const char** dfa, int tabSize, TextReader<T>* reader)
      {
         _tabSize = tabSize;
         _reader = reader;

         _dfa = dfa;

         if (!cacheLine()) {
            _lineInfo.clear();
            _startPosition = _position = 0;
            _line[0] = 0;
         }
      }
   };

}

#endif