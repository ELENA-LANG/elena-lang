public singleton Engine
{
   run()
   {
      auto collection := distributed testCaseCollection;

      foreach(var test; in collection)
      {
         console.write(test.TestName).write(":");
         test();
         console.writeLine();   
      }
   }
}