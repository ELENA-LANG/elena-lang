struct DeclA
{
   int x : prop(_x);
   
   int y : prop(_y);
}

struct DeclB
{
   UnsafePointer<DeclA> _a;
   
   int b : prop(_b);
   
   A a
      = _a.Value;
}
