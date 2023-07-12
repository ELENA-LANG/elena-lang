import extensions;   
import system'dynamic'expressions;

public program()
{
   for (int i := 1, i <= 5, i += 1) {
      if (i == 3) {
         i += 1;

         $continue;
      };         

      console.writeLine(i);   
   }
}
