A;
B;

public class XReference<T> : B, info<"descr">
{
   test() : info<"test"> {}
}

public program()
{
  XReference<A> ref;
}
