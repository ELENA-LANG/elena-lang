public singleton Engine
{
   run()
   {
      auto collection := distributed testCaseCollection;

      foreach(var test; in collection)
      {
         test()
      }
   }
}