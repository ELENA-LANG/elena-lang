import system'text;
 
interface I
{
   int foo(int n)
     = 2 + n;
}

public singleton A : I
{
}

public Program()
{
    int n := A.foo(1);

//      byte tmp[1024];
//      int tmpIndex := 0;
//         auto s := "abc";
//         int sLen := /*s.Length*/3;
//
//         int tmpLen := UTF8Encoder.saveToByteArray(s, 0, ref sLen, tmp, tmpIndex, 1024);
}
