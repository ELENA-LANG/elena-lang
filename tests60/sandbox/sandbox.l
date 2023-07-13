import extensions;   

// --- LoopTest ---

LoopTester
{
   int x := 5;

   bool next()
   {
      x -= 1;

      ^ x > 0;
   }

   assert()
   {
      Assert.ifTrue(x == -1)
   }
}

loopTest()
{
    var counter := new LoopTester();
    while(counter.next()) {};
}

public program()
{
   loopTest();
}
