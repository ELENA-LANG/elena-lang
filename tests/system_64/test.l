sealed A
{
    x;
    y;
    
    constructor()
    {
        x := 1;
        y := 2;
    }

//    add(object o, object o2, object o3)
//    {
//    }
    
    add(params int[] args)
    {
    }
    
    add(params string[] args)
    {
    }
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
    
//    add(object o, object o2, object o3)
//    {
//    }
    
    add(params int[] args)
    {
    }
    
    add(params string[] args)
    {
    }
}

public program()
{
//    var o := nil;
    var a := new A();
    var b := new B();

    var s2 := b.add("a", "b", "c");
    var r2 := b.add(1,2,3);
//    b.add(o,2);
        
    var s := a.add("a", "b", "c");
    var r := a.add(1,2,3);
//    a.add(o,2);
    
//    console;
//    console.write("abc"w)
}

startUp = program();
