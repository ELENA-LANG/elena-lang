import extensions;

A
{
   constructor new() 
   {
      console.writeLine("A");
   }

   sealed static raise()
   {
      weak self.new()
   }
}

B : A
{
   constructor new() 
   {
      console.writeLine("B");
   }
}


public program()
{
   var a := A.raise();
   var b := B.raise();

}
