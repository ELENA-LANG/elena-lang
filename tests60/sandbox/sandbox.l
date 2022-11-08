A
{
   val;

   constructor(val)
   {
      this val := val;
   }

   get Value()
      = val;
}



public program()
{
   var a := new A(1);
   
   console.writeLine(*a);
}
