import system'io;
import system'text;
import extensions;
import extensions'routines;

public program()
{
   File.assign("sandbox.l").forEachLine:(line)
   {
      console.printLine(line);
   };
}