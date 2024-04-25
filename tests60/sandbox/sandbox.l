B;

C
{
    B cast()
        = new B();
}

singleton E
{
    load(params B[] o)
    {
        for(int i := 0; i < o.Length; i += 1)
        {
            console.writeLine(o[i])
        }
    }
}

public program()
{
   var b := new B();
   var c := new C();
   var e := E;
    
   e.load(b,c);
//   E.load(b,c);
}