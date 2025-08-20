struct A
{
   int x : prop(_x);
   
   int y : prop(_y);
}

struct B
{
   UnsafePointer<A> _a;
   
   int b : prop(_b);
   
   A a
      = _a.Value;
}
