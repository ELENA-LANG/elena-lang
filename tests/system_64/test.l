sealed A
{
    x;
    y;
    
    constructor()
    {
        x := 1;
        y := 2;
    }

    add(object o)
    {
    }
    
    add(string s)
    {
        ^ s
    }
    
    add(int n)
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
    
    add(object o)
    {
    }
    
    add(string s)
    {
        ^ s
    }
    
    add(int n)
        = y.add(n); 
}

public program()
{
    var o := nil;
    var a := new A();
    var b := new B();

    var s2 := b.add("a");
    var r2 := b.add(4);
    b.add(o);
        
    var s := a.add("a");
    var r := a.add(4);
    a.add(o);
    
//    console;
//    console.write("abc"w)
}

startUp = program();
