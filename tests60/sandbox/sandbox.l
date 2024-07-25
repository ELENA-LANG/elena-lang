import extensions;

NilArgB;

NilArgA
{
   NilArgB b;

   constructor new(NilArgB? b)
   {
      this b := b
   }
}

NilArgC
{
   NilArgB b;

   constructor new(NilArgB b)
   {
      this b := b
   }
}

singleton NilArgTester<T>
{
   runWeak()
   {
      var b := nil;
      var t := class T;
      var a := t.new(b);
   }
   runStrong()
   {
      var b := nil;
      var a := class T.new(b);      
   }
}

passingNilArg()
{
   NilArgTester<NilArgA>.runWeak();
   console.write(".");
   NilArgTester<NilArgA>.runStrong();
   console.write(".");

   Assert.ifFailed({ NilArgTester<NilArgC>.runWeak() });
   console.write(".");
   Assert.ifFailed({ NilArgTester<NilArgC>.runStrong() });
   console.write(".");
}

public program()
{
   passingNilArg()
}