import system'routines;
import extensions;

public program()
{
//   var f := (x => x + 1);
//   var f2 := (x,y => x + y);

//   console.writeLine(f(2))
//   console.writeLine(f2(2,3));

//   var f3 := (int x => x + 2);
//   var f4 := (int x, int y => x + y);
//   var f5 := (int x, int y){ console.writeLine(x + y) };
   var f6 := (x){ ^ x + 1 };
   var f7 := (x,y){ ^ x + y };

   console.writeLine(f6(2));
   console.writeLine(f7(2,4));

//   var r := $(1, "string");
//   var r := $(1, "string");
//   var r := (1, "string");

}