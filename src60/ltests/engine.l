public singleton Engine
{
   run()
   {
      auto collection := distributed testCaseCollection;

      foreach(var test; in collection)
      {
         Console.write(test.TestName).write(":");
         test();
         Console.writeLine();   
      }
   }
}