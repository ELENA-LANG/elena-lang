#new __string testCaseCollection;

public inline testCase()
{
   #share super __string testCaseCollection;

   #let testCaseCollection += 
   ::{
      string TestName = $name __decl;

      function()
         => __decl;
   };
}
