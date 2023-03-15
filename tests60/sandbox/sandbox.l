import extensions;

//B
//{
//   int field : prop;
//}

//extension BOp : B
//{
//   test(int t)
//   {
//       self.field := t;

//       console.writeLine(self.field);
//   }
//}

A<T>
{
   T field : prop;
}

extension AOp<T> : A<T>
{
   test(T t)
   {
       self.field := t;

       console.writeLine(self.field);
   }
}

public program()
{
   /*auto*/var a := new A<int>();
   a.test(2);
}
