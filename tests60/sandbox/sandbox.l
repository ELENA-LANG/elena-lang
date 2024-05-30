import extensions;



struct X
{
   int x/* : prop*/;
}

singleton Helper
{
   X get()
   {
      ^ nil
   }
}

public program() 
{
//   X x := Helper.get()
}
