public singleton Engine
{
   run()
   {
      auto collection := distributed testCaseCollection;

      foreach(var test; in collection)
      {
         console.write(test.TestName);
         test();
         console.writeLine();   
      }
   }
}