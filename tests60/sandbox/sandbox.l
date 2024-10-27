////import extensions;
////import system'text'parsing;
////
////public program()
////{
////   auto regex1 := new RegEx("^(ht|f)tp(s?)\:\/\/[0-9a-zA-Z]((\.|-+)?[0-9a-zA-Z])*(:(0-9)*)?(\/?)([a-zA-Z0-9\-\.\?\,\'\/\\\+&%\$#_]*)?$");
////
////   Assert.ifTrue(regex1.match("http://localhost"));
////   Assert.ifTrue(regex1.match("http://www.domain.com"));
////   Assert.ifTrue(regex1.match("http://www.domain.com/"));
////   Assert.ifTrue(regex1.match("http://xn--masekowski-d0b.pl"));
////   Assert.ifTrue(regex1.match("http://localhost:9000"));
////
////   Assert.ifTrue(regex1.match("http://www.domain.com/home/index.html"));
////
////
//// //  auto regex1 := new RegEx("^[0-9a-zA-Z](\/?)$");
//// //  Assert.ifTrue(regex1.match("a"));
//// //  Assert.ifTrue(regex1.match("a/"));
////}
//
//import extensions;
//import net'http;
//
//public program()
//{
//   Uri uri := new Uri("http://www.google.com/startSide?arg=3");
//
//   Console.printLine("Scheme:",uri.Scheme," Host:",uri.Host," Port:",uri.Port," Path:",uri.Path," Query:",uri.Query);
//}

import extensions;

sealed class MyIntValue
{
    int value;
    
    constructor(string value)
    {
        this value := value.toInt()
    }
    
    constructor(real value)
    {
        this value := value.toInt()
    }
    
    constructor(int value)
    {
        this value := value
    }
    
    int Value = value;
}

class MyClass
{
    add(MyIntValue v)
    {
        Console.printLine("add(MyIntValue(",v.Value,"))")
    }
    
    add(n)
    {
        Console.printLine("unsupported")
    }
}

public program()
{
    var o := new MyClass();
    
    o.add("2" as: MyIntValue);
    o.add(3 as: MyIntValue);
    o.add(4.0 as: MyIntValue);
}