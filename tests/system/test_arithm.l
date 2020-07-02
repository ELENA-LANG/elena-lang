////#import system'routines.
////#import system'collections.
////import system'dynamic.
////#import system'calendar.
import extensions;
////#import extensions'dynamic.

//////// --- OpenArgTest ---
//////
//////#symbol OpenArgTester =
//////{
//////    test : anIndex &args:aParams
//////        = aParams @ (anIndex int).
//////
//////    testParamsCopy &args : aParams
//////    [
//////        #var aLength := aParams length.
//////        
//////        #var aSecond := aParams@1.
//////        #var aFirst := $self test:0 &args:aParams.
//////        
//////        $self testBoxing:aParams.
//////    ]
//////    
//////    testBoxing : anArray
//////    [
//////        #var b := anArray.
//////    ]    
//////}.
//////
//////#symbol OpenArgTest =
//////[
//////    OpenArgTester testParamsCopy &args:(1,"a",3).
//////].
//////
//////// --- MultiDispatchTest ---
//////
//////#symbol DispatchTester =
//////{
//////    eval &x:anX &y:anY
//////    [
//////        ^ anX + anY.
//////    ]
//////}.
//////
//////#symbol MultiDispatchTest =
//////[
//////    #var msg := %"x&y".
//////    
//////    DispatchTester::msg eval:1:2.
//////].
//////
//////// --- MultySelectTest ---
//////
//////#symbol MultySelectTest =
//////[
//////    #var n := 3.
//////    
//////    n =>
//////        1 ? [ console writeLine:"=> operator fails". ]
//////        2 ? [ console writeLine:"=> operator fails". ]
//////        3 ? [ console writeLine:"=> operator works!". ].
//////].
//////
//////// --- LocalTest ---
//////
//////#symbol LocalTester =
//////{
//////    do &int:n
//////    [
//////        #var o := n.
//////    ]
//////    
//////    do:n
//////    [
//////        #var o := n.
//////    ]
//////}.
//////
//////#symbol LocalTest =
//////[
//////    #var(int)n := 3.    
//////    n := n + 1.
//////    
//////    LocalTester do &int:n.
//////    
//////    LocalTester do:(n int).
//////    
//////    #var(long)l := 12345678901234566l.
//////    l := l + 1l.
//////    
//////    #var o := n.
//////    #var o2 := l.
//////].
//////
//////// --- OuterFieldTest ---
//////
//////#symbol OuterFieldTest =
//////[
//////    #var(int) i := 0.
//////    
//////    { eval : n [ i := n. ] } eval:3.
//////    
//////    (i == 3)
//////        ? [ console writeLine:"OuterFieldTest works!!!". ]
//////        ! [ console writeLine:"OuterFieldTest fails!!!". ].
//////].
//////
//////#symbol BoxingTester =
//////{
//////    add : aValue
//////    [
//////        ^ aValue.
//////    ]
//////    
//////    #method(stacksafe) add &int:aValue
//////    [
//////        ^ aValue.
//////    ]
//////}.
//////
//////#symbol BoxingTest =
//////[
//////    #var n := 2 + 3 * 4.    // here should be all primitive operations and the result should be boxed
//////    #var n2 := 2 + 3 * n.   // here should be no primitive operations at all.
//////    
//////    BoxingTester add:n.
//////    BoxingTester add:2.
//////    BoxingTester add &int:(n + 3 * 4).
//////    BoxingTester add :(2 + 3).
//////    BoxingTester add :(2 + n).
//////    
//////    #var n3 := BoxingTester add &int:(2 + 3 * 4).
//////    BoxingTester add :(2 + 3 * 4).
//////    
//////].
//////
//////#symbol DictionaryTest =
//////[
//////    #var aList := Dictionary new.
//////    
//////    aList @ 1 := "1a".
//////    aList @ 2 := "2".
//////    
//////    aList @ 1 := "1".
//////    
//////    #var anN1 := aList @ 1.
//////    #var anN2 := aList @ 2.
//////].
//////
//////#symbol ListTest =
//////[
//////    #var aList := List new append:1 append:2 append:4 append:"abc".
//////
//////    #var anArray := aList toArray.
//////
//////    #var n1 := aList safeSeek:2.
//////    #var s := aList safeSeek:"abc".
//////    #var n2 := aList safeSeek:-1.
//////].
//////
//////#symbol EnumTest =
//////[
//////    "abc" run &each: (:x)
//////    [
//////        console write:x.
//////    ].
//////        
//////    console writeLine:".".
//////].
//////
//////#symbol actionTester =
//////{
//////    testGeneric : anAction
//////    [
//////        anAction eval:"Action Test":" passed".
//////    ]
//////    
//////    testAction : anAction
//////    [
//////        anAction eval &line1:"Action Test" &line2:" passed".
//////    ]
//////}.

class MyException : Exception
{
    constructor new(s)
        <= new(s);
}

class MyException2 : Exception
{
    constructor new(s)
        <= new(s);
}

singleton ExecTester
{
//    do : anAction [ anAction eval. ]
//
//    do:x with:y
//    [
//    ]
//            
//    run : anAction
//    [
//        anAction eval x:"Eval Works!!".
//    ]
    
    break()
    {
        MyException.new:"OnError handler works".raise()
    }
    
    break2()
    {
        MyException2.new("OnError2 handler works").raise()
    }
    
    break3()
    {
        MyException2.new("Nested OnError2 handler works").raise()
    }
}

objectTest()
{
    console.write("objectTest..");
    
    var a := 1;
    var b := "abc";
    
    Assert.ifTrue(a.equalReference(a) && a.equalReference(b).Inverted);
    
    console.writeLine("done")
}

// --- BooleanTest ---

booleanTest()
{
    console.write("booleanTest..");
    
    var b1 := true;
    var b2 := false;
    
    Assert.ifTrue(b1 == b1);
    Assert.ifFalse(b1 == b2);
    Assert.ifFalse(b2 == b1);
    Assert.ifTrue(b2 == b2);
    Assert.ifFalse(b2 != b2);
    Assert.ifTrue(b1 != b2);
    
    console.writeLine("done")
}

// --- ControlTest ---

controlTest()
{
    console.write("controlTest..");
    
    var b1 := false;
    if (true)
    {
        b1 := true
    };
    
    var b2 := nil;
    if (false)
    {
        b2 := false;
    }
    else
    {
        b2 := true
    };
    
    Assert.ifTrue(b1);    
    Assert.ifTrue(b2);    
    
    console.writeLine("done")
}

// --- IntArithmeticTest ---

integerTest()
{
    console.write("integerTest..");
        
    int n := 2;
    int m := 3;
    int k := 4;
                    
    Assert.ifTrue(2 + 3 * 4 == 14);
    Assert.ifTrue(n + m * k == 14);
    Assert.ifFalse(n == m);
    Assert.ifTrue(n == n);
    Assert.ifTrue(n < m);
    Assert.ifFalse(n > m);
    Assert.ifTrue(n <= m);
    Assert.ifTrue(n <= n);
    Assert.ifTrue(n >= n);
    Assert.ifFalse(n >= m);

    int i := 1;
    i += 2;
    i -= 2;
    i *= 2;
    i /= 2;
    
    Assert.ifTrue(i == 1);
        
    console.writeLine("done")        
}

// --- LongArithmeticTest ---

longTest()
{
    console.write("longTest..");
        
    long l1 := 100000000000000l;
    long l2 := 5000000000000l;
    
    var l := l1 / l2;
    Assert.ifTrue(l == 20);
    
    long n := 2l;
    long m := 3l;
    long k := 4l;
    Assert.ifTrue(n + m * k == 14l);
    
    n := 2000000l;
    m := 500000l;
    
    Assert.ifTrue(n * m == 1000000000000l);

    n := 12345678l;
    m := 12345679l;
            
    Assert.ifFalse(n == m);            
    Assert.ifTrue(n != m);            
    Assert.ifFalse(n < k);            
    Assert.ifFalse(n > m);            
    Assert.ifFalse(n <= k);            
    Assert.ifFalse(n >= m);            
    Assert.ifTrue(n >= n);            
    Assert.ifTrue(n <= n);            
    Assert.ifTrue(n < m);            
    Assert.ifTrue(m > n);            
    
    long i := 0l;
    i += 3l;
    i -= 2l;
    i *= 2l;
    i /= 2l;
    
    Assert.ifTrue(i == 1l);
        
    console.writeLine("done")        
}

// --- RealArithmeticTest ---

realTest()
{
    console.write("realTest..");
        
    real n := 2.0r;
    real m := 3.0r;
    real k := 4.0r;

    Assert.ifTrue(n + m * k == 14.0r);
    
    Assert.ifFalse(n == m);            
    Assert.ifTrue(n != m);            
    Assert.ifTrue(n < k);            
    Assert.ifFalse(n > m);            
    Assert.ifFalse(n >= k);            
    Assert.ifFalse(n >= m);            
    Assert.ifTrue(n >= n);            
    Assert.ifTrue(n <= n);            
    Assert.ifTrue(n < m);            
    Assert.ifTrue(m > n);            
    
    real i := 1.0r;
    i += 2.0r;
    i -= 2.0r;
    i *= 2.0r;
    i /= 2.0r;
    
    Assert.ifTrue(i == 1.0r);
        
    console.writeLine("done")        
}

////// --- Action Test ---
//////
//////#symbol ActionTest =
//////[
//////    console write:"Generic ".
//////    actionTester testGeneric: (:x:y) [ console write:x writeLine:y. ].
//////    
//////    console write:"Normal ".
//////    actionTester testAction: (&line1:aLine1 &line2:aLine2) [ console write:aLine1 writeLine:aLine2. ].
//////].
//////
//////// --- Role Tests ---
//////
//////#symbol Role =
//////{
//////    literal = "Role overriding works!".
//////}.
//////
//////#symbol RoleTest =
//////[
//////    console writeLine:("Role overriding doesn't work!!"::Role literal).
//////].

// --- ConvertorTest ---

convertorTest()
{
    console.write("convertorTest..");
        
    var n := "23".toInt();
    var sn := n.toString();
    Assert.ifTrue(sn == "23");
    
    Assert.ifTrue("1234567890123456".toLong() == 1234567890123456l);
    Assert.ifTrue("-123".toInt() == -123);
    Assert.ifTrue("-1234567890123456".toLong() == -1234567890123456l);

    Assert.ifTrue("23".toReal() == 23.0r);
    Assert.ifTrue("123.456789".toReal() == 123.456789r);
    Assert.ifTrue("-123.456789".toReal() == -123.456789r);
    
    Assert.ifTrue(-2l.toInt() == -2);
    Assert.ifTrue(-2.toLong() == -2l);
    
    Assert.ifTrue(-2.0r.toInt() == -2);
    Assert.ifTrue(-2.toReal() == -2.0r);
        
    Assert.ifTrue(-2.0r.toLong() == -2l);
    Assert.ifTrue(-2l.toReal() == -2.0r);
    
    console.writeLine("done")        
}

// --- LiteralTest ---

literalTest()
{
    console.write("literalTest..");
    
    Assert.ifTrue("abc" == "abc");
    Assert.ifFalse("ab" == "abc");
    Assert.ifTrue("ba" > "ab");
    Assert.ifTrue("ab" < "abc");
    Assert.ifTrue("abc" + "de" == "abcde");
//
//    #var n := "abcd" indexOf:"bc" &at:0.
//    #var n2 := "abcd" indexOf:"bac" &at:0.
//
//    #var s := "acd" insert:"b" &at:1.
//    #var ls := s length.
//
//    #var s2 := "abbcd" delete:1 &at:1.
//    #var ls2 := s2 length.
//    
//    #var s3 := "abcd" Substring:2 &at:1.
//    #var ls3 := s3 length.
//   
//    "abc" getAt:2.
//    
//    "abc" getAt:(-1) |
//        if &OutOfRangeError: (:e)[ console writeLine:"Negative index is not allowed" ].
//
//    "abc" getAt:4 |
//        if &OutOfRangeError: (:e) [ console writeLine:"Index bigger than length is not allowed" ].
        
    console.writeLine("done")        
}

////// --- ArrayTest ---
//////
//////#symbol ArrayTest =
//////[
//////    #var a1 := Array(1,2,3).
//////    #var n := a1 @ 1.
//////
//////    #var(array) a4 := Array new:3.
//////    a4 setAt:0:2.
//////    a4 setAt:1:1.
//////    a4 setAt:2:3.
//////    
//////    #var a2 := Array new:5 set &every: (&index:x) [ IntNumber new:x ].    
//////    #var n2 := a2 @ 1.
//////    a2 setAt:1:-1.
//////    #var n3 := a2 @ 1.
//////    
//////    a2 setAt:0:2.
//////        
//////    a4 sort: (:aFormer:aNext)[ aFormer < aNext ].
//////
//////    a4 getAt:(-1) |
//////        if &OutOfRangeError: e [ console writeLine:"Negative index is not allowed" ].
//////
//////    a4 getAt:3 |
//////        if &OutOfRangeError: e [ console writeLine:"Index bigger than length is not allowed" ].
//////].
////
//////// --- MethodNotFoundTest ---
//////
//////symbol MethodNotFoundTest =
//////[
//////    var n := 2.
//////    
//////    try(n test)
//////    {
//////        on(MethodNotFoundException e)
//////        [
//////            console printLine(e message," works")
//////        ]
//////    }
//////].
//////
//////// --- LoopTest ---
//////
//////#symbol LoopTest =
//////[
//////    #var i := 5.
//////            
//////    console writeLine:"Loop running...".
//////    #loop (i > 0) ?
//////    [
//////        console writeLine:i.
//////        
//////        i := i - 1.
//////    ].
//////  
//////    #var(vint)j := 0.
//////    control while: [j < 5] &do:
//////    [
//////        console writeLine:j.
//////        
//////        j += 1.
//////    ].
//////    
//////    control do:
//////    [
//////        console writeLine:j.
//////        
//////        j -= 1.
//////    ]
//////    &until:[j == 0].
//////].

// --- ExceptionTest ---

exceptionTest()
{
    console.write("exceptionTest..");
    
    bool raised := false;
    ExecTester.break() | on:(e){ raised := true };
    Assert.ifTrue(raised);
    
    bool passed := false;
    try
    {
        ExecTester.break2()
    }
    catch::
    {
        function(MyException e)
        {
            Assert.ifTrue(false)
        }
        function(MyException2 e)
        {
            passed := true;
        }        
    };
    Assert.ifTrue(passed);
    
    bool passed2 := false;
    try
    {
        try
        {
            MyException2.new("Nested OnError2 handler works").raise()
        }
        catch(MyException e)
        {
            console.writeLine:"!!Wrong handler";
            Assert.ifTrue(false)
        }
    }
    catch(MyException2 e)
    {
        passed2 := true;
    };
    Assert.ifTrue(passed2);
    
    console.writeLine("done")
}

//////// --- TextBufferTest ---
//////
//////#symbol TextBufferTest =
//////[
//////    #var aBuffer := system'text'TextBuffer new.
//////    aBuffer write:"abc".
//////
//////    #var anOutput := aBuffer literal.
//////].
//////
//////// --- DateTest ---
//////
//////#symbol DateTest =
//////[
//////    #var aDate := now.
//////    
//////    #var aYear := aDate year.
//////    #var aMonth := aDate month.
//////    #var aDay := aDate day.
//////    #var anHour := aDate hour.
//////    #var aMinute := aDate minute.
//////    #var aSecond := aDate second.
//////    
//////    #var aRetVal := aDate literal.
//////].
//////
////////// --- GetPropertyTest ---
////////
////////#symbol GetPropertyAgent =
////////{
////////    eval : aGetProperty
////////    [
////////        #var n := aGetProperty property.
////////        
////////        2 == aGetProperty property
////////            ? [ console writeLine:"GetProperty works!". ]
////////            ! [ console writeLine:"!! GetProperty does not work !!". ]
////////    ]
////////}.
////////
////////#symbol GetPropertyTest =
////////[
////////    GetPropertyAgent eval:%property[2].
////////].
//////
//////#symbol StructTest =
//////[
//////    #var a := system'dynamic'Struct ( %x, 2, %print, system'dynamic'Tape ( 2, %"tapeOp.var[]", system'console, %"writeLine[1]")). 
//////
//////    #var x := a x.
//////
//////    a print:"Struct method works".
//////].
////
//////GroupMember1 =
//////{
//////    m1 : prm = "group 1 " + prm.
//////}.
//////
//////GroupMember2 =
//////{
//////    m2 : prm : prm2 = "group 2 " + prm.
//////}.
//////
//////GroupTest =
//////[
//////    var g := Group::(GroupMember1, GroupMember2).
//////    
//////    console printLine(g m1("called")).
//////    console printLine(g m2("called",2)).
//////].
////
//////// --- ClassWithGeneric ---
//////
//////#class ClassWithGeneric
//////{
//////    #field theExtension.
//////    
//////    #constructor new : anExtension
//////    [
//////        theExtension := anExtension.
//////    ]
//////    
//////    #method x = 2.
//////    
//////    #method(generic)get
//////    [
//////        ^ $nil.
//////    ]
//////    
//////    #method => theExtension.
//////}
//////
//////// --- GenericMethodTest --
//////
//////#symbol GenericMethodTest =
//////[
//////    #var o := ClassWithGeneric new:2.
//////    
//////    #var x := o x.
//////    #var y := o y.
//////    
//////    #var s := o add:3.
//////].
//////
//////#class BaseClass
//////{
//////    #field theField1.
//////                   
//////    #constructor new [ theField1 := IntNumber new &int:1. ]
//////}
//////
//////#class ChildClass :: BaseClass
//////{
//////    #field theField2.
//////    
//////    #constructor new <= ( %new ) [ theField2 := IntNumber new &int:2. ]
//////}
//////
//////#symbol ConstructorTest =
//////[
//////    #var o := ChildClass new.
//////].
//////
//////#symbol DTOToStructTest =
//////[
//////    #var aDTO := { X = 2. Y = 3. Z = 4. }.    
//////    #var aProperties := MessageArray new &int:10.
//////
//////    #var(vint)aLength := 0.
//////    aDTO readMatchedEntries &mssg:%"get[0]" &int:0 &mssgarray:aProperties &vint:aLength.
//////
//////    #var aCopy := DynamicStruct new.
//////    aProperties top:aLength run &each:(:aSubject)
//////    [
//////        #var prop_value := aDTO::aSubject eval.
//////        #var prop := aSubject subject.
//////        
//////        aCopy::prop set:prop_value.
//////    ].
//////    
//////    #var x := aCopy X.
//////    #var y := aCopy Y.
//////    #var z := aCopy Z.
//////].
//////
//////#class BoxerHelper
//////{
//////    #constructor new []
//////    
//////    #method init &vint:aRetVal
//////    [
//////        aRetVal := 2.
//////    ]
//////    
//////    #method exchange &ref:anObject1 &ref:anObject2
//////    [
//////        #var tmp := anObject1 get.
//////        anObject1 := anObject2 get.
//////        anObject2 := tmp.
//////    ]    
//////}
//////
//////#symbol UnboxTest =
//////[        
//////    #var(int)n := 1.
//////    
//////    #var o := BoxerHelper new.
//////    
//////    o init &vint:n.
//////            
//////    (2 == n)
//////        ? [ console writeLine:"Unboxing parameter works". ].
////////
////////    #var(byte:4)a.
////////
////////    a@0 := 070h.
////////    a@1 := 080h.
////////    a@2 := 0C0h.
////////    a@3 := 0FFh.
////////
////////    #var(int)r.
////////    a read &int:0 &vint:r.
////////    
////////    (0FFC08070h == r)
////////        ? [ console writeLine:"Unboxing object works". ].
//////].
//////
//////#symbol ByRefTest =
//////[
//////    #var a := 2.
//////    #var b := "hello".
//////    
//////    BoxerHelper new exchange &ref:a &ref:b.
//////    
//////    ((a == "hello")and:(b == 2))
//////        ? [ console writeLine:"ByRef routine works". ].
//////].
//////
//////#class Parent
//////{
//////    #method print
//////    [
//////        console writeLine:"Super works".
//////    ]
//////}
//////
//////#class Child :: Parent
//////{
//////    #method print
//////    [
//////        console writeLine:"Super does not work".
//////    ]
//////    
//////    #method test
//////    [
//////        $super print.
//////    ]
//////}
//////
//////#symbol SuperTest =
//////[
//////    #var o := Child new.
//////    
//////    o test.
//////].
//////
////////#subject(class:TestVar)test_var.
////////#class(sealed,struct:embeddable) TestVar
////////{
////////    #field(type:int)theValue.
////////    #field(type:int)theValue2.
////////    
////////    #constructor(stacksafe,embeddable) new &int:aValue
////////    [
////////        theValue := aValue.
////////        theValue2 := 0.
////////    ]
////////}
////////
////////#symbol EmbedConstTest =
////////[
////////    #var(type:test_var)var := TestVar new &int:2.
////////].
//////
//////#symbol jsonTest =
//////[
//////    #var dto := DTO{ X = 2. Y = "Value". }.
//////    
//////    #var(literal)json := dto toJson literal.
//////    
//////    #var ori := json fromJson.
//////].
//////
//////subExpressionTest =
//////[
//////    var a := Array::(2,3).
//////    var b := Array::(2).
//////    var c := (2).
//////    var d := (2,3).
//////    var e := (1+2)*3.
//////].
//
//nestedFieldsTest()
//{
//    console.write("nestedFieldsTest..");
//    
//    var o := new:: 
//    {
//        field3 := 3;
//        
//        innerMost() => new::
//        {
//            field2 := 2;
//            
//            innerMost() => new::
//            {
//                field1 := 1;
//                
//                innerMost() = this self;
//                
//                getField() = field1;
//                
//                getParentField() = field2;
//                
//                getParentParentField() = field3;
//            };
//        };
//    };
//  
//    var n := o.innerMost();
//    Assert.ifEqual(n.getField(), 1);
//    Assert.ifEqual(n.getParentField(), 2);
//    Assert.ifEqual(n.getParentParentField(), 3);
//    
//    console.writeLine("done")
//}
//
//shortCircuitBooleanOperation()
//{
//    console.write("shortCircuitBooleanOperation..");
//    
//    int n := 0;
//    var b := (n != 0) && (1/n != 0);
//        
//    Assert.ifFalse(b);
//    
//    console.writeLine("done")
//}
//
//sealed class VariadicTested
//{
//    bool test1Called := false;
//    bool test2Called := false;
//    
//    test1(params int[] others)
//    {
//        test1Called := true
//    }
//    
//    test2(string s, params int[] others)
//    {
//        test2Called := true
//    }    
//    
//    validate()
//    {
//        Assert.ifTrue(test1Called).ifTrue(test2Called)
//    }
//}
//
//
//variadicTest()
//{
//    console.write("variadicTest..");
//    
//    // run-time
//    var tester := new VariadicTested();
//    tester.test1(1,3,3);
//    tester.test2("hello",3,3);
//    
//    tester.validate();
//    
//    // compile-time
//    auto tester2 := new VariadicTested();
//    tester2.test1(1,3,3);
//    tester2.test2("hello",3,3);
//    
//    tester2.validate();
//    
//    console.writeLine("done")
//}
//
////singleton ProberTester
////{
////    abort
////    [
////        AbortException new; raise
////    ]
////}
////
////proberTest =
////[
////    console write("proberTest..").
////    
////    var passed := false.
////    
////    ProberTester~__prober abort2.
////    ProberTester abort | if(:e)[ passed := true. ].
////    
////    Assert ifTrue(passed).
////    
////    console writeLine("done").
////].

class NestedTester
{
    int param1;
    int param2;
    
    constructor()
    {
        param1 := 1;
        param2 := 2;
    } 
    
    validate()
    {
        var c := { ^ param2 };        
        
        Assert.ifTrue(c() == param2);
    }
}

nestedTest()
{
    console.write("nestedTest..");
    
    auto tester := new NestedTester();
        
    tester.validate();
    
    console.writeLine("done")
}

class NestetNestedTester
{
    int param1;
    int param2;
    
    constructor()
    {
        param1 := 1;
        param2 := 2;
    } 
    
    validate()
    {
        var c := { ^ { ^ param2 } };        
        var cc := c();
        
        Assert.ifTrue(cc() == param2);
    }
}


nestetNestedTest()
{
    console.write("nestetNestedTest..");
    
    auto tester := new NestetNestedTester();
        
    tester.validate();
    
    console.writeLine("done")
}

//duplicateStructBoxing()
//{
//    console.write("duplicateStructBoxing..");
//    
//    int mode := -1;
//    var value := false;
//    value ? { mode := 0 } : { mode := 1 }  ;
//    
//    Assert.ifTrue(mode == 1);
//    
//    console.writeLine("done")
//}

duplicateBoxing()
{
    console.write("duplicateBoxing..");

    var t := 0;
    int t2 := 0;
    console.then(
    {
        Assert.ifTrue(t == 0);
        Assert.ifTrue(t2 == 0);
        
        t := 1;
        t2 += 1;
    }).then(
    {
        Assert.ifTrue(t == 1);
        Assert.ifTrue(t2 == 1);
        
        t := 2;
        t2 += 1;
    });
    
    Assert.ifTrue(t == 2);
    Assert.ifTrue(t2 == 2);

    var mode := -1;
    var value := false;
    value ? { mode := 0 } : { mode := 1 }  ;
    
    Assert.ifTrue(mode == 1);
    
    value := true;
    value ? { mode := 0 } : { mode := 1 }  ;
    
    Assert.ifTrue(mode == 0);

    console.writeLine("done")
}

public stackSafeExtensions()
{
    console.write("stackSafeExtensions..");
    
    int i := 5;
    var m := 5;
   
    int j := 2;
    var n := 2;
                         
    int r1 := i.mod(j);
    int r2 := i.mod(n);
    int r3 := m.mod(j);
    int r4 := m.mod(n);
    
    Assert.ifTrue(r1 == 1);
    Assert.ifTrue(r2 == 1);
    Assert.ifTrue(r3 == 1);
    Assert.ifTrue(r4 == 1);
    
    console.writeLine("done")
}