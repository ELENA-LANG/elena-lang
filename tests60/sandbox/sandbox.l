import system;

struct BoxingStruct
{
   int x;
   int y;

   constructor()
   {
      x := 1;
      y := 2;
   }

   set(int x, int y)
   {
      this x := x;
      this y := y;
   }

   test(int x, int y)
   {
      console.writeLine(this x == x);
      console.writeLine(this y == y);
   }
}

public program() 
{
   BoxingStruct st;
   BoxingProber.set(st);
   console.write(".");
   st.test(3,4);
   console.write(".");
}
