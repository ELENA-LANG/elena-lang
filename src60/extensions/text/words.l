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
            ifnot (_spaceMode)
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
               ifnot (_dfa.isWhitespace())
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
}