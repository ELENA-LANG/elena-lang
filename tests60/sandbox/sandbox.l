import extensions;
import extensions'routines;
import system'routines;

public program()
{
   var a := Array.allocate(3);

   var r := a.populate:(int i => i);

   r.forEach:(x){ console.writeLine(x) };

//            var r := Array.allocate(3);
//            r[0] := "1";
//            r[1] := "2";
//            r[2] := "3";
            
//            ParallelEnumerator.new(r).forEach:(line)
//            {
//               console.writeLine(line);                  

//                console.writeLine()
//            }
}