import system'io;
import system'text;
import extensions;
import extensions'routines;

public program()
{
//   File.assign("sandbox.l").forEachLine:(line)
//   {
//      console.printLine(line);
//   };

   var f := TextStreamReader.new("sandbox.l");
   var l1 := f.readLine();
   console.printLine(l1);
   var l2 := f.readLine();
   console.printLine(l2);
   var l3 := f.readLine();
   console.printLine(l3);
   var l4 := f.readLine();
   console.printLine(l4);

}