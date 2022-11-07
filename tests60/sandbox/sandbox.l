A
{
   val;

   string toPrintable()
      => val;

   constructor(string val)
   {
      this val := val;
   }
}



public program()
{
   var a := new A("test");
   console.writeLine(a)
}
