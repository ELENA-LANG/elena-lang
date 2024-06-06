import extensions;

class IntWrapper
{ 
    int value;
      
    constructor(int value) 
    {
       this value := value 
    }

    int cast()
       = value;  
}

class MyClass
{
    add(int n)
    {
        console.printLine("add int:(",n,")")
    }
    
    add(string s)
    {
        console.printLine("add literal:(",s,")")
    }
    
    add(o)
    {
        if (o; is int n) {
           ^ self.add(n);
        };

        console.printLine("unsupported")
    }
}

public program()
{
    var o := new MyClass();
    var wrapper := new IntWrapper(4);
    
    o.add("string");
    o.add(2);
    o.add(wrapper);
    o.add(2.3);  // when this line is commented, the code works correctly and ends without a problem
}