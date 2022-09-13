import system;

struct BoxingStruct
{
   int x;
//   int y;

   constructor()
   {
      x := 1;
//      y := 2;
   }

   set(int x/*, int y*/)
   {
      this x := x;
//      this y := y;
   }

   test(int x/*, int y*/)
   {
      console.writeLine(this x == x);
//      console.writeLine(this y == y);
   }
}

singleton BoxingProber
{
   set(BoxingStruct bs)
   {
      bs.set(3/*,4*/)
   }
}

public program() 
{
   var st2 := new BoxingStruct();
   BoxingProber.set(st2);
   st2.test(3/*,4*/);

//   BoxingStruct st;
//   BoxingProber.set(st);
//   console.write(".");
//   st.test(3,4);
//   console.write(".");
}
