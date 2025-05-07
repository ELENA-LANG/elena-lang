import extensions;

import extensions;
import textgen;

/*const string Script = "<={{=>for (var i := 0; i < self.Length; i := i + 1) {
      if (i > 0) {
        if (i == self.Length - 1) {
           <= and =>
        }
        else {
           <=, =>
        }
      };
         
      <={self[i]}=>
   }<=}}=>";*/

public program()
{
   var s := "<={{!!}}=>";
   var r := 1.generateFrom(s);

   Console.writeLine(r);
}