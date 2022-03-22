// Invoking a class symbol

singleton B
{
    test() {}
}

class A
{
   test() {}

   constructor new() {}
}

//public symbol program = A.new().test();
public symbol program = B.test();

