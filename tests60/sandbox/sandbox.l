A
{
   val;

   constructor(val)
   {
      this val := val;
   }

   dispatch()
      => val;
}



public program()
{
   var a := new A(1);
   
   var b := a + 2;

   console.writeLine(b);
}
