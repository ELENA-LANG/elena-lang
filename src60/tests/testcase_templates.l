#new __string testCaseCollection;

public inline testCase()
{
   #share super __string testCaseCollection;

   #let testCaseCollection += 
   ::{
      string testName = $name __decl;

      function()
         => __decl;
   };
}
