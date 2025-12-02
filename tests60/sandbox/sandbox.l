import system'collections;
import extensions;

/*
public struct Array
{
   embeddable __string int[] array;

   test()
   {
      array[0] := 2
   }
}
*/

/*public const Array
{
   embeddable __string object[] array;

   test()
   {
      array[0] := 2
   }
}*/

//A;

struct XY
{
   int X : prop(_x);
   int Y : prop(_y);

   constructor(int val)
   {
      _x := val;
      _y := 0;
   }
}

public program()
{
   const XY[] constA := new const XY[] { 1, 2, 3 };

//   const object[] constA := new const object[] { 1, 2, 3 };
//   const object[] constA := new [] { 1, 2, 3 };
//   const object[] constA := new object[] { 1, 2, 3 };

//   const string[] constA;

//   const string[] constA := ReadOnlyArray<string>.new("a","b","c");

//   string[] a := constA;

//   const string[] a := new []{ ".", ".." };
//  string[] dirNames := new []{ ".", ".." };
}
