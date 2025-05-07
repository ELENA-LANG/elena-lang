import system'text;

singleton ScriptGenerator
{
   private writeOutput(StringBuilder fullScript, string script, int start, int index)
   {
      if (start == index)
         :break;
      
      fullScript.write("output.write(""");   
      
      int cbIndex := script.indexOf(start, "}}");
      if (cbIndex > 0 && cbIndex <= index) {
         fullScript.writeSubstring(script, start, index - cbIndex + 1);
         int next := cbIndex + 2;
         cbIndex := script.indexOf(next, "}}");
         while (cbIndex > 0 && cbIndex <= index) {
            fullScript.writeSubstring(script, next, next - cbIndex + 1);
            next := cbIndex + 2;
            cbIndex := script.indexOf(next, "}}");
         };
         
         fullScript.writeSubstring(script, next, index - next);
      }
      else fullScript.writeSubstring(script, start, index - start);
      
      fullScript.write(""");");      
   }
   
   private writeExpression(StringBuilder fullScript, string script, int start, int index)
   {
      fullScript.write("output.write(");   
      fullScript.writeSubstring(script, start, index - start);
      fullScript.write(");");      
   }
   
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
            
            start := index + 2;
            index := script.indexOf(start, "=>");
            
            if (index >= 0) {
               int injectIndex := script.indexOf(start, "{");
               while (injectIndex > 0 && injectIndex < index) {
                  if (injectIndex > start) {
                     if (script[injectIndex + 1] == "{") {
                        writeOutput(fullScript, script, start, injectIndex + 1);
                        
                        start := injectIndex + 2;
                        injectIndex := script.indexOf(start, "{");
                        :continue;
                     };
                     
                     writeOutput(fullScript, script, start, injectIndex);
                  }
                  else {
                     :break;
                  };                  
                  
                  start := script.indexOf(start, "}");
                  if (start == -1 || start > index)
                     InvalidArgumentException.raise("script");
                     
                  writeExpression(fullScript, script, injectIndex + 1, start);
                  
                  injectIndex := script.indexOf(start, "{");
                  
                  start++
               };
            
               writeOutput(fullScript, script, start, index);
               
               start := index + 2;               
            }
            else InvalidArgumentException.raise("script");
         }
         else {
            fullScript.writeSubstring(script, start, length - start);
            
            start := length;
         }
      };
      
      fullScript.write(" }");
      
      ^ *fullScript
   }
}