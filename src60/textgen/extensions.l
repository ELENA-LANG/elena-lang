import extensions'scripting;

public extension textGenOp
{
   string generateFor(string script)
   {
      string full_script := "function() { " + script + " }";
      
      var function := LScript.eval(full_script);
      
      ^ function();
   }
}