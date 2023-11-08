import extensions;
import system'routines;
import system'dynamic'expressions;

public program()
{
      var s := "abc";

      var failed := false;
      s.at(-1) \\ 
         on:(e)
         {
            failed := true;
         };
            
      Assert.ifTrue(failed);
}