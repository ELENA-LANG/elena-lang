import extensions;

B
{
    test(string command)
    {
        console.printLine(command);
    }
}

C;

public program()
{
    var a := nil;
    var b := new B();
    var c := new C();
  
    a?.test("Fired a?.test");
/*    b?.test("Fired b?.test");
    
    a\.test("Fired a\.test");
    b\.test("Fired b\.test");
    c\.test("Fired c\.test");*/

    console.readLine();
}