import extensions'scripting;

public extension textGenOp
{
   string generateFor(string script)
   {
      string full_script := "program() { " + script + " }";
      
      var function := LScript.eval(full_script).program;
      
      ^ function();
   }
}