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

   loadAndTypecast(params object[] args)
   {
      self.load(cast B(params args));
   }
}

public program()
{
   var b := new B();
   var c := new C();
   
   E.loadAndTypecast(b,c);    

//    var e := E.load(b,c);
}
