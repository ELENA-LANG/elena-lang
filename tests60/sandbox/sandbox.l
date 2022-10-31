import extensions;

// --- stringTests ---
stringTests()
{
    console.write("stringTests");
    string s := "abc";

    Assert.ifTrue(s == "abc");
    console.write(".");
    string s2 := "de";
    var s3 := s + s2;

    Assert.ifTrue(s3 == "abcde");
    console.writeLine(".");
}


//A;

public program()
{
   stringTests();

//   var a := new A();
//   a.fail()  
}
