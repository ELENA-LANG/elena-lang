import system'text;

singleton ScriptGenerator
{
   string parse(string script)
   {
      int length := script.Length;
      
      auto fullScript := new StringBuilder();
      fullScript.write("program(self, output) { ");
      
      int start := 0;
      
      while (start < length) {
         int index := script.indexOf(start, "<=");
         if (index >= 0) {
            fullScript.writeSubstring(script, start, index - start);
            
            fullScript.write("output.write(""");
            
            start := index + 2;
            index := script.indexOf(start, "=>");
            
            if (index >= 0) {
               fullScript.writeSubstring(script, start, index - start);
               fullScript.write(""");");
               
               start := index + 2;               
            }
            else InvalidArgumentException.raise("script");
         }
         else {
            fullScript.writeSubstring(script, start, length - start);
            
            start := length;
         }
      };
      
      ^ *fullScript
   }
}