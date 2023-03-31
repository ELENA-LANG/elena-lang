import system'routines;
import extensions;


public program()
{
   var s:= "1234".toArray();

   var ch := s[0];

   console.printLine(s[0] == ch);
   console.printLine(s[0].equalReference(ch));

//               var duplicate := guess.seekEach:(x => (x == ch)&&(x.equalReference(ch).Inverted));
//               if (nil != duplicate)
//               {
//                  InvalidArgumentException.raise()
//               };

}