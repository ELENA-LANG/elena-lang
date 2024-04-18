class Environment
{
   field  test;
   string testName;

   constructor(test, string testName)
   {
      this test := test;
      this testName := testName;
   }

   string TestName
      = testName;

   function()
      => test;
}

