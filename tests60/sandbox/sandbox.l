import extensions;

singleton Helper
{
   int? getValue()
   {
      ^ nil
   }
}   

public program()
{
   int x := Helper.getValue() ?? 0
}
