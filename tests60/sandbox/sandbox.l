import extensions;

singleton X
{

   continueWith(Action<string> action)
   {
      action("hello");
   }

   do()
   {
      continueWith((string x){ console.writeLine(x) });
   }}


public program()
{
   X.do()
}
