import extensions;

singleton Tester
{
   run()
   {
      int i := 0;
      while (true) {
         if (console.KeyAvailable)
            $break;

         i := i + 1;
      }
   }
}

public program()
{
   Tester.run();
}
