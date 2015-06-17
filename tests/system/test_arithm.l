#define system.
#define system'routines.
#define system'collections.
#define system'dynamic.
#define system'calendar.
#define system'text.
#define system'io.
#define system'math.
#define extensions.
#define extensions'math.
#define extensions'text.


#symbol XTester =
{
    test            
    [
    ]
}.

#symbol XTest =
[
    XTester test.
].

// --- OpenArgTest ---

#symbol OpenArgTester =
{
//    #method testUnboxing : anArg1 : anArg2 : anArg3
//    [
//        #var b := anArg1.
//    ]

    test : anIndex &args:aParams
        = aParams @ (anIndex int).

    testParamsCopy &args: aParams
    [        
        #var n := aParams @ 0.

        $self testBoxing:aParams.
                        
        #var n2 := $self test:0 &args:aParams.
                        
        ^ n.
    ]
            
//    test &params : aParams
//    [
//        #var n := $self testParamsCopy &:aParams.
//        
//        #var aLength := aParams length.
//        
//        #var aSecond := aParams@1.
//        
//        $self testBoxing:aParams.
////        $self testUnboxing::aParams.
//    ]
    
    testBoxing : anArray
    [
        #var b := anArray.
    ]    
}.

#symbol OpenArgTest =
[
    OpenArgTester testParamsCopy &args:1:"a":3.

    #var aRetVal := OpenArgTester test:1 &args:1:"a":3.

//    OpenArgTester test &args:1:"a":3.
].

// --- MultiDispatchTest ---

#symbol DispatchTester =
{
    eval &x:anX &y:anY
    [
    ]
}.

#symbol MultiDispatchTest =
[
    #var msg := %(x&y).
    
    DispatchTester::msg eval:1:2.
].

// --- MultySelectTest ---

#symbol MultySelectTest =
[
    #var n := 3.
    
    n =>
        1 ? [ console writeLine:"=> operator fails". ]
        2 ? [ console writeLine:"=> operator fails". ]
        3 ? [ console writeLine:"=> operator works!". ].
].

// --- LocalTest ---

#symbol LocalTester =
{
    do &int:n
    [
        #var o := n.
    ]
}.

#symbol LocalTest =
[
    #var(type:int)n := 3.    
    n := n + 1.
    
    LocalTester do &int:n.
    
    #var(type:long)l := 12345678901234566l.
    l := l + 1l.
    
    #var o := n.
    #var o2 := l.
].

// --- OuterFieldTest ---

#symbol OuterFieldTest =
[
    #var i := Integer new.
    { eval : n [ i << n. ] } eval:3.
].

#symbol BoxingTester =
{
    add : aValue
    [
        ^ aValue.
    ]
    
    add &int:aValue
    [
        ^ aValue.
    ]
}.

#symbol BoxingTest =
[
    #var n := 2 + 3 * 4.    // here should be all primitive operations and the result should be boxed
    #var n2 := 2 + 3 * n.   // here should be no primitive operations at all.
    
    BoxingTester add:n.
    BoxingTester add:2.
    BoxingTester add &int:(n + 3 * 4).
    BoxingTester add :(2 + 3).
    BoxingTester add :(2 + n).
    
    #var n3 := BoxingTester add &int:(2 + 3 * 4).
    BoxingTester add :(2 + 3 * 4).
    
].

#symbol DictionaryTest =
[
    #var aList := Dictionary new.
    
    aList @ 1 := "1a".
    aList @ 2 := "2".
    
    aList @ 1 := "1".
    
    #var anN1 := aList @ 1.
    #var anN2 := aList @ 2.
].

#symbol ListTest =
[
    #var aList := List new append:1 append:2 append:4.

    #var anArray := convertor toArray:aList.

    #var n1 := control find:2 &in:aList.
    #var n2 := control find:-1 &in:aList.
].

#symbol EnumTest =
[
    control run:"abc" &forEach: (:x)
    [
        console write:x.
    ].
        
    console writeLine:".".
].

//#symbol BreakTest =
//[
//    #var a := 2.
//    #var n := control do:
//    [
//       (a == 1) ? [ #break "a". ].
//
//       (a == 2) ? [ #break "b". ].
//
//       ^ nil.
//    ].
//].

#symbol actionTester =
{
    testGeneric : anAction
    [
        anAction eval:"Action Test":" passed".
    ]
    
    testAction : anAction
    [
        anAction eval &line1:"Action Test" &line2:" passed".
    ]
}.

#class MyException :: Exception
{
    #constructor new : aMessage
        <= %new &message:aMessage &error_type:%MyError.
}

#class MyException2 :: Exception
{
    #constructor new : aMessage
        <= %new &message:aMessage &error_type:%MyError2.
}

#symbol ExecTester =
{
    do : anAction [ anAction eval. ]

    do:x &with:y
    [
    ]
            
    run : anAction
    [
        anAction eval &x:"Eval Works!!".
    ]
    
    break
    [
        #throw MyException new:"OnError handler works".
    ]
    
    break2
    [
        #throw MyException2 new:"OnError2 handler works".
    ]
    
    break3
    [
        #throw MyException2 new:"Nested OnError2 handler works".
    ]
}.

#symbol ObjectTest =
[
    #var a := 1.
    #var b := "abc".
    
    ( a equal &reference:a and:[ a equal &reference:b not ])
        ? [ console writeLine:"Reference comparison works" ]
        ! [ console writeLine:"!! ObjectTest failed" ].
].

// --- BooleanTest ---

#symbol BooleanTest =
[
    #var b1 := true.
    #var b2 := false.
    
    console write:"(true == true) is " writeLine:(b1 == b1).
    console write:"(true == false) is " writeLine:(b1 == b2).
    console write:"(false == true) is " writeLine:(b2 == b1).
    console write:"(false == false) is " writeLine:(b2 == b2).
    console write:"(false != false) is " writeLine:(b2 != b2).
    console write:"(false != true) is " writeLine:(b2 != b1).
].

// --- ControlTest ---

#symbol ControlTest =
[
    true ?
    [
        console writeLine:"? operator works!".
    ].
    
    false ?
    [
        console writeLine:"!! ControlTest failed".
    ]
    ! [
        console writeLine:"?-! operator works!".
    ].
].

// --- IntArithmeticTest ---

#symbol IntegerTest =
[
    console write:"(2 + 3 * 4) is " writeLine:(2 + 3 * 4).
    console write:"(2 == 3) is " writeLine:(2 == 3).
    console write:"(2 == 2) is " writeLine:(2 == 2).
    console write:"(2 < 3) is " writeLine:(2 < 3).
    console write:"(2 > 3) is " writeLine:(2 > 3).
    console write:"(2 <= 3) is " writeLine:(2 <= 3).
    console write:"(2 >= 2) is " writeLine:(2 >= 2).
    console write:"(2 >= 3) is " writeLine:(2 >= 3).

    #var n := Integer new write:1.
    n += 2.
    n -= 2.
    n *= 2.
    n /= 2.
    
    (n == 1) ?
        [ console writeLine:"Integer operators are working". ].
].

// --- LongArithmeticTest ---

#symbol LongTest =
[    
    #var l := 100000000000000l / 5000000000000l.
    
    console write:"(2 + 3 * 4) is " writeLine:(2l + 3l * 4l).
    console write:"(2000000*500000) is " writeLine:(2000000l * 500000l).
    console write:"(12345678 == 12345679) is " writeLine:(12345678l == 12345679l).
    console write:"(12345679 != 12345678) is " writeLine:(12345678l != 12345679l).
    console write:"(12345678l < 3) is " writeLine:(12345678l < 3l).
    console write:"(12345678l > 12345679l) is " writeLine:(12345678l > 12345679l).
    console write:"(12345678l <= 3) is " writeLine:(12345678l <= 3l).
    console write:"(12345678l >= 12345679l) is " writeLine:(12345678l >= 12345679l).
    
    #var n := Long new write:1l.
    n += 2l.
    n -= 2l.
    n *= 2l.
    n /= 2l.
    
    (n == 1l) ?
        [ console writeLine:"Long Integer operators are working". ].
].

// --- RealArithmeticTest ---

#symbol RealTest =
[
    console write:"(2 + 3 * 4) is " writeLine:(2.0r + 3.0r * 4.0r).
    console write:"(10/3) is " writeLine:(10.0r / 3.0r).
    
    #var n := Real new write:1.0r.
    n += 2.0r.
    n -= 2.0r.
    n *= 2.0r.
    n /= 2.0r.
    
    (n == 1.0r) ?
        [ console writeLine:"Real operators are working". ].
].

// --- Action Test ---

#symbol ActionTest =
[
    console write:"Generic ".
    actionTester testGeneric: (:x:y) [ console write:x writeLine:y. ].
    
    console write:"Normal ".
    actionTester testAction: (&line1:aLine1 &line2:aLine2) [ console write:aLine1 writeLine:aLine2. ].
].

// --- Role Tests ---

#symbol Role =
{
    literal = "Role overriding works!".
}.

#symbol RoleTest =
[
    console writeLine:("Role overriding doesn't work!!"::Role literal).
].

// --- ConvertorTest ---

#symbol ConvertorTest =
[
    #var n := convertor toInt:"23".
    #var sn := n literal.
    (sn == "23")
        ! [ console writeLine:"Int conversion does not work" ].
    
    #var l := convertor toLong:"1234567890123456".
    #var nn := convertor toInt:"-123".
    #var ll := convertor toLong:"-1234567890123456".

    #var r := convertor toReal:"23".
    #var r2 := convertor toReal:"123.456789".
    #var r3 := convertor toReal:"-123.456789".
].

// --- LiteralTest ---

#symbol LiteralTest =
[
    #var b1 := "abc" == "abc".
    #var b2 := "ab" == "abc".
    #var b3 := "ba" > "ab".
    #var b4 := "ab" < "abc".
    
    #var l := "abc" + "de".
    #var ll := l length.
    #var c := l@2.

    #var a := l~enumerable array.
//    #var a2 := l~indexable arreay.

    control run:"abc" &forEach: (:ch)[ console write:ch ].

    console writeLine.

    #var n := "abcd" indexOf &index:0 &literal:"bc".
    #var n2 := "abcd" indexOf &index:0 &literal:"bac".

    #var s := "acd" insert &index:1 &literal:"b".
    #var ls := s length.

    #var s2 := "abbcd" delete &index:1 &length:1.
    #var ls2 := s2 length.
    
    #var s3 := "abcd" Substring &index:1 &length:2.
    #var ls3 := s3 length.
   
    "abc" getAt:2.
    
    "abc" getAt:(-1) |
        if &OutOfRangeError: (:e)[ console writeLine:"Negative index is not allowed" ].

    "abc" getAt:4 |
        if &OutOfRangeError: (:e) [ console writeLine:"Index bigger than length is not allowed" ].
].

// --- ArrayTest ---

#symbol ArrayTest =
[
    #var a1 := Array(1,2,3).
    #var n := a1 @ 1.

    #var(type:array) a4 := Array new &length:3.
    a4 setAt:0:2.
    a4 setAt:1:1.
    a4 setAt:2:3.
    
    #var a2 := Array new &length:5 set &every: (&index:x) [ Integer new &int:x ].    
    #var n2 := a2 @ 1.
    a2 setAt:1:-1.
    #var n3 := a2 @ 1.
    
    a2 setAt:0:2.
        
    a4 sort: (:aFormer:aNext)[ aFormer < aNext ].

    a4 getAt:(-1) |
        if &OutOfRangeError: e [ console writeLine:"Negative index is not allowed" ].

    a4 getAt:3 |
        if &OutOfRangeError: e [ console writeLine:"Index bigger than length is not allowed" ].
].

// --- MethodNotFoundTest ---

#symbol MethodNotFoundTest =
[
    #var n := 2.
    
    n test | if &MethodNotFoundError: e [ console write:(e message) writeLine:" works". ].
].

// --- LoopTest ---

#symbol LoopTest =
[
    #var i := 5.
        
    console writeLine:"Loop running...".
    #loop (i > 0) ?
    [
        console writeLine:i.
        
        i := i - 1.
    ].
  
    #var j := Integer new.
    control while: [j < 5] &do:
    [
        console writeLine:j.
        
        j += 1.
    ].
    
    control do:
    [
        console writeLine:j.
        
        j -= 1.
    ]
    &until:[j == 0].
].

// --- ExceptionTest ---

#symbol ExceptionTest =
[
    ExecTester break | if &Error: e [ console writeLine:"Throw works!". ].
    
    ExecTester break | if &Error: e [ console writeLine:(e literal). ].
    
    ExecTester break2 
    | on:
    {
        MyError = e [ console writeLine:"!!Wrong handler". ].
        
        MyError2 = e [ console writeLine:(e literal). ].
    }.
    
    control do:
    [
        ExecTester break3 | if &MyError: e [ console writeLine:"!!Wrong handler". ].
    ]
    | if &MyError2: e [ console writeLine:(e literal). ].
    
].

// --- TextBufferTest ---

#symbol TextBufferTest =
[
    #var aBuffer := system'text'TextBuffer new.
    aBuffer write:"abc".

    #var anOutput := aBuffer literal.
].

// --- DateTest ---

#symbol DateTest =
[
    #var aDate := now.
    
    #var aYear := aDate year.
    #var aMonth := aDate month.
    #var aDay := aDate day.
    #var anHour := aDate hour.
    #var aMinute := aDate minute.
    #var aSecond := aDate second.
    
    #var aRetVal := aDate literal.
].

////// --- GetPropertyTest ---
////
////#symbol GetPropertyAgent =
////{
////    eval : aGetProperty
////    [
////        #var n := aGetProperty property.
////        
////        2 == aGetProperty property
////            ? [ console writeLine:"GetProperty works!". ]
////            ! [ console writeLine:"!! GetProperty does not work !!". ]
////    ]
////}.
////
////#symbol GetPropertyTest =
////[
////    GetPropertyAgent eval:%property[2].
////].

#symbol StructTest =
[
    #var a := system'dynamic'Struct ( %x, 2, %print, system'dynamic'Tape ( 1, extensions'control, %(getAt&args)(1), system'console, %(writeLine)(1) )).    

    #var x := a x.

    a eval&print:"Struct method works".
].

#symbol GroupMember1 =
{
    m1 : prm = "group 1 " + prm.
}.

#symbol GroupMember2 =
{
    m2 : prm : prm2 = "group 2 " + prm.
}.

#symbol GroupTest =
[
    #var g := Group(GroupMember1, GroupMember2).
    
    #var m1 := g m1:"called".
    #var m2 := g m2:"called":2.
].

// --- ClassWithGeneric ---

#class ClassWithGeneric
{
    #field theExtension.
    
    #constructor new : anExtension
    [
        theExtension := anExtension.
    ]
    
    #method x = 2.
    
    #method(generic)get
    [
        ^ nil.
    ]
    
    #method => theExtension.
}

// --- GenericMethodTest --

#symbol GenericMethodTest =
[
    #var o := ClassWithGeneric new:2.
    
    #var x := o x.
    #var y := o y.
    
    #var s := o add:3.
].

#class BaseClass
{
    #field theField1.
                   
    #constructor new [ theField1 := 1. ]
}

#class ChildClass :: BaseClass
{
    #field theField2.
    
    #constructor new <= ( %new ) [ theField2 := 2. ]
}

#symbol ConstructorTest =
[
    #var o := ChildClass new.
].

#symbol DTOToStructTest =
[
    #var aDTO := { X = 2. Y = 3. Z = 4. }.    
    #var(type:enumerable) aProperties := MessageArray new &length:10.
    #var aLength := Integer new.
    
    aDTO read &mssg_mask:%get &index:0 &mssgarray:aProperties &vint:aLength.

    #var aCopy := DynamicStruct new.
    control run:(aProperties top:aLength) &forEach:(:aSubject)
    [
        #var prop_value := aDTO::aSubject eval.
        #var prop := aSubject subject.
        
        aCopy::prop set:prop_value.
    ].
].