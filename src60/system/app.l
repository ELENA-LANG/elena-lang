import extensions;

class MyClass
{
    add(n)
    {
        console.printLine("add(",n,")")
    }
}

public program()
{
    var o := new MyClass();
    
    o.add("string");
    o.add(2);
    o.add(3.1)
}