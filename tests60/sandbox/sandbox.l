import extensions;
 
B
{
   getX()
      <= X;  

   int X : prop;

   constructor()
   {
      X := 1;
   }
}

public program()
{
   var b := new B();
   
   var x := b.getX();
}