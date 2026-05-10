import extensions;

sealed class X
{
   int value;
   
   int Value
      = value;

   constructor()
   {
      value := 2
   }
}

public Program()
{
   X x := new X();

   var r := *x
}
