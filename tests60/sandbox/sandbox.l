//import system'dynamic'expressions;
//import extensions'scripting;
//
////symbol used : preloaded = Expression;
//
//tester
//    = 2.program;
//
//public program()
//{
//   var o := Expression;
//
//   lscript.interpretLine("system'console.writeLine(""Hello World"")");
//}

import system'text;

interface X
{
   int calc(int arg)
      = arg + 2;
}

singleton Y : X
{
//   int calc(int arg)
//      = arg + 3;
}

public program()
{
   int ret := Y.calc(3);
}

//public program()
//{
//      byte tmp[1024];
//      int tmpIndex := 0;
//
//         auto s := "abcd";
//         int sLen := s.Length;
//
//         int tmpLen := UTF8Encoder.toByteArray(s, 0, ref sLen, tmp, tmpIndex, 1024);
//}