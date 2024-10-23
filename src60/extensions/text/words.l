import system'text;

namespace text
{
   // --- Statemachine ---    
   public interface Statemachine
   {
      abstract bool proceed(char ch);
        
      abstract bool isWhitespace();
   }
    
   public class SimpleStatemachine : Statemachine
   {
      bool _spaceMode;
        
      constructor()
      {
         _spaceMode := true
      }
        
      bool proceed(char ch)
      {
         if (ch.isWhitespace())
         {
            if:not (_spaceMode)
               { _spaceMode := true; ^ true }
         }
         else
         {
            _spaceMode := false
         };
                
         ^ false
      }
        
      bool isWhitespace() = _spaceMode;
   }
    
   public class ExpressionStatemachine : Statemachine
   {
      bool _spaceMode;
      bool _quoteMode;
      bool _newToken;
      bool _operatorMode;
        
      constructor()
      {
         _spaceMode := true;
         _quoteMode := false;
         _newToken := true;
         _operatorMode := false
      }
        
      bool proceed(char ch)
      {
         if (_quoteMode)
         {
            if (ch == $22h)
               { _quoteMode := false; _spaceMode := false; _newToken := false; _operatorMode := true };
                    
            ^ false
         };
            
         bool whitespace := ch.isWhitespace();
         bool digit := ch.isDigit();
         bool letter := ch.isLToken();
         if (whitespace)
         {
            if:not(_spaceMode)
               { _spaceMode := true; _newToken := true; ^ true }
         }
         else
         {
            if (ch == $22h)
            {
               if:not(_newToken)
                  { _newToken := true; ^ true };
                    
               _quoteMode := true;
               _spaceMode := false;
                    
               ^ false
            };
                
            if (_newToken)
            { 
               _newToken := false;
               _spaceMode := false; 
               if (ch == $45) {
                  _operatorMode := false;
               }
               else _operatorMode := (letter || digit) ? false : true
            }
            else if (letter || digit)
            {
               if(_operatorMode)
                  { _newToken := true; _operatorMode := false; ^ true }
            }
            else
            { 
               _operatorMode := true;
               _newToken := true; ^ true
            }
         };
            
         ^ false
      }
        
      bool isWhitespace() = _spaceMode;    
   }

   public class TokenEnumerator : Enumerator<string>, interface<Enumerator>
   {
      string         _literal;
      Reference<int> _index;
      TextBuilder    _buffer;
      Statemachine   _dfa;
    
      constructor()
      {
         _literal := String.MinValue;
         _index := 0;
         _buffer := new TextBuilder();
         _dfa := new SimpleStatemachine();
      }    
            
      constructor(string s, Statemachine dfa)
      {
         _literal := s;
         _index := 0;
         _buffer := new TextBuilder();
         _dfa := dfa;
      }    
        
      constructor new(s)
      {
         _literal := cast string(s);
      }
        
      reset()
      {
         _index.Value := 0;
      }
        
      enumerable() = _literal;
        
      bool next()
      {
         _buffer.clear();
    
         int  len := _literal.Length;
         int  current := *_index;
         char ch := $0;
         int  chLen := 0;
            
         while (current < len)
         {
            ch := _literal[current];
                
            if (_dfa.proceed(ch))
            {
               _index.Value := current;
                    
               ^ true
            }
            else
            {
               if:not (_dfa.isWhitespace())
                  { _buffer.write(ch) }
            };
                    
            chLen := ch.Length;
            current := current + chLen
         };
    
         _index.Value := current;

         ^ _buffer.isNonempty()
      }
        
      get string Value() = *_buffer;
   }

   public class TokenStatemachine : Statemachine
   {
      bool _spaceMode;
      bool _quoteMode;
      bool _newToken;
        
      constructor()
      {
         _spaceMode := true;
         _quoteMode := false;
         _newToken := true
      }
        
      bool proceed(char ch)
      {
         if (_quoteMode)
         {
            if (ch == $22h)
               { _quoteMode := false; _spaceMode := false; _newToken := false };
                    
            ^ false
         };
            
         bool whitespace := ch.isWhitespace();
         if (whitespace)
         {
            if:not(_spaceMode)
               { _spaceMode := true; _newToken := true; ^ true }
         }
         else
         {
            if (ch == $22h)
            {
               if:not(_newToken)
                  { _newToken := true; ^ true };
                    
               _quoteMode := true;
               _spaceMode := false;
                    
               ^ false
            };
                                      
            if (_newToken)
            { 
               _newToken := false;
               _spaceMode := false 
            }
         };
            
         ^ false
      }
        
      bool isWhitespace() = _spaceMode;    
   }
}