import extensions;

public field xprop<prop_name, T, field_name>
{
   T field_name;

   get T prop_name()
   {
      ^ field_name
   }

   set prop_name(T value)
   {
      field_name := value
   }
}

Point
{
   int X : xprop(_x);
}

public program()
{
}
