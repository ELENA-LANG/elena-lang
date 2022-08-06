namespace io
{
   public class ConsoleWriter
   {
      method write(string s)
      {
          int h := extern KERNEL32.GetStdHandle(0FFFFFFF5h);
          int l := s.Length;
          extern KERNEL32.WriteConsoleA(h,s,l,0,0);
      }
   }
}