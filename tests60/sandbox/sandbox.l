import extensions;
 
sealed B
{
   test()
      <= test();
}

public program()
{
   var b := new B();
   b.test()
}
