import extensions;

public program()
{
   var variable := "Level 0";
   {
      var variable := "Level 1";

      console.printLine(super variable);
      console.printLine(variable);
   }
}
