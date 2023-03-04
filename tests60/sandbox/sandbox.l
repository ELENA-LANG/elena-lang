import extensions;
//import extensions'routines;
import system'routines;

public program()
{
  string _literal := "a b";


//            chLen := ch.Length;
//            current := current + chLen;


         int  len := _literal.Length;
         int  current := 0;
         char ch := $0;
         int  chLen := 0;
            
         while (current < len)
         {
            ch := _literal[current];
  
            console.printLine(ch);
              
//            if (_dfa.proceed(ch))
//            {
//               _index.Value := current;
//                    
//               ^ true
//            }
//            else
//            {
//               ifnot (_dfa.isWhitespace())
//                  { _buffer.write(ch) }
//            };
                    
            chLen := ch.Length;
            current := current + chLen;
         };

//    var o := nil;
//    o.call()

//   var o := new Object();
//   o.fail()
}