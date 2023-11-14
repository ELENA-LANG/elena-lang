import extensions;

A
{
   string _caption;

   string Caption
   {
      set(string s)
      {
         _caption := s;
      }
      
      set(wide s)
      {
         _caption := string cast(s);
      }
   }
}

public program()
{
   auto a := new A();

   a.Caption := "Hello";
   a.Caption := "Hello"w;
}
