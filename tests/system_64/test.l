sealed A
{
    x;
    y;
    
    constructor()
    {
        x := 1;
        y := 2;
    }

    add(object o, object o2, object o3)
    {
    }
    
    add(string s, int n, wide w)
    {
        ^ s
    }
    
    add(int n, string s, wide w)
        = y.add(n); 
}

B
{
    x;
    y;
    
    constructor()
    {
        x := 1;
        y := 2;
    }
    
    add(object o, object o2, object o3)
    {
    }
    
    add(string s, int n, wide w)
    {
        ^ s
    }
    
    add(int n, string s, wide w)
        = y.add(n); 
}

public program()
{
//    var o := nil;
    var a := new A();
    var b := new B();

    var s2 := b.add("a", 3, "b"w);
    var r2 := b.add(4, "c", "b"w);
//    b.add(o,2);
        
    var s := a.add("a", 1, "b"w);
    var r := a.add(4, "r", "b"w);
//    a.add(o,2);
    
//    console;
//    console.write("abc"w)
}

startUp = program();
