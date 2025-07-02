B;

A
{
//   test(B? b) {}

   B? test() = nil;  
}

C : A
{
//   test(B? b) {}

   B test() = new B();
}

public program()
{
}
