namespace io
{
   public class ConsoleWriter
   {
      method write(string s)
      {
         int l := s.Length;
         extern libc.write(1, s, l);
      }
   }
}