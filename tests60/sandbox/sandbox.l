import system;

A
{
    const string __a := $name __decl;

    string test() = __a;

    static field;
}

public program() 
{
   var a := new A();
   console.writeLine(a.test())
}
