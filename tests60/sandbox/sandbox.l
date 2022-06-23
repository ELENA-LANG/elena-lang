import system;

public sealed ref<T>
{
   embeddable T field;

   T get()
      = field;

   set(T v)
   {
      field := v
   }
}

singleton Tester
{
   run()
   {
      ref<int> r := 1;

//       byte b := 55;

//       byte tmp[10];
//       tmp[0] := b;
//       tmp[1] := 57;
//       tmp[2] := 0;


//      for (int n := 0, n < 5, n := n + 1) {
//         console.write("*");
//      }
   }
}

public symbol program = Tester.run();
