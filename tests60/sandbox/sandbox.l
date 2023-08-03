import extensions;
 
public program()
{
   string s := "Hello";
   int l := s.Length;
   extern libc.write(1, s, l);   
}