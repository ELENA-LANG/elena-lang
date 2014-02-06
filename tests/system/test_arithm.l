#define system.
#define system'routines.
#define system'collections.
#define system'calendar.
#define extensions.
#define extensions'math.

#class Tester
{
}
   
#symbol XTest =
[
    #var n := 012345678h.
    
    #var hw := n~intOp HiWord.
    #var lw := n~intOp LoWord.
    #var hb := n~intOp HiByte.
    #var lb := n~intOp LoByte.
].

// --- OpenArgTest ---

#class OpenArgTester
{
//    #method testUnboxing : anArg1 : anArg2 : anArg3
//    [
//        #var b := anArg1.
//    ]

    #method test : anIndex :: aParams
        = aParams @ (anIndex int).

    #method testUnbox :: aParams
    [
        #var n := aParams @ 0.
        
        ^ n.
    ]
            
    #method test :: aParams
    [
        #var n := $self testUnbox &:aParams.
        
        #var aLength := aParams length.
        
        #var aSecond := aParams@1.
        
        $self testBoxing:aParams.
//        $self testUnboxing::aParams.
    ]
    
    #method testBoxing : anArray
    [
        #var b := anArray.
    ]    
}

#symbol OpenArgTest =
[
    #var aRetVal := OpenArgTester test:1 &:1:"a":3.
    
    OpenArgTester test &:1:"a":3.
].

// --- MultiDispatchTest ---

#class DispatchTester
{
    #method eval &x:anX &y:anY
    [
    ]
}

#symbol MultiDispatchTest =
[
    #var msg := %(x&y).
    
    DispatchTester~msg eval:1:2.
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

#class LocalTester
{
    #method do &int:n
    [
        #var o := n.
    ]
}

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

#class BoxingTester
{
    #method add : aValue
    [
    ]
    
    #method add &int:aValue
    [
        ^ aValue.
    ]
}

#symbol BoxingTest =
[
    #var n := 2 + 3 * 4.    // here should be all primitive operations and the result should be boxed
    #var n2 := 2 + 3 * n.   // here should be no primitive operations at all.
    
    BoxingTester add:n.
    BoxingTester add:2.
    BoxingTester add &int:(n + 3 * 4).
    BoxingTester add &int:(2 + 3).
    BoxingTester add :(2 + 3).
    BoxingTester add &int:(2 + n).
    BoxingTester add :(2 + n).
    
    #var n3 := BoxingTester add &int:(2 + 3 * 4).
    BoxingTester add :(2 + 3 * 4).
    
].

#symbol DictionaryTest =
[
    #var aList := Dictionary new.
    
    aList append &key:1 &value:"1a".
    aList append &key:2 &value:"2".
    
    aList set &key:1 &value:"1".
    
    #var anN1 := aList getAt &key:1.
    #var anN2 := aList getAt &key:2.
].

#symbol ListTest =
[
    #var aList := List new append:1 append:2 append:4.

    #var anArray := listControl toArray:aList.

    #var n1 := listControl find:2 &in:aList.
    #var n2 := listControl find:-1 &in:aList.
].

#symbol EnumTest =
[
    control foreach:"abc" &do: (:x)
    [
        console write:x.
    ].
        
    console writeLine:".".
].

#symbol BreakTest =
[
    #var a := 2.
    #var n := control do:
    [
       (a == 1) ? [ #break "a". ].

       (a == 2) ? [ #break "b". ].

       ^ nil.
    ].    
    
    control while:[true] &do:
    [
        true ? [
            #break nil.
        ].
    ].
].

#class A
{
}

#class B
{
}

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

#class MyException : Exception
{
    #method onError : aHandler
    [
        aHandler eval:$self.
    ]
}

#class MyException2 : Exception
{
    #method onError2 : aHandler
    [
        aHandler eval:$self.
    ]
}

#class ExecTester
{
    #method do : anAction [ anAction eval. ]

    #method do:x &with:y
    [
    ]
            
    #method run : anAction
    [
        anAction eval &x:"Eval Works!!".
    ]
    
    #method break
    [
        #throw MyException new:"OnError handler works".
    ]
    
    #method break2
    [
        #throw MyException2 new:"OnError2 handler works".
    ]
}

#symbol ObjectTest =
[
    (A == A)and:[A != B]
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
    
    #var n := Integer new write::1.
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
    
    #var n := Long new write::1l.
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
    
    #var n := Real new write::1.0r.
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
    console writeLine:("Role overriding doesn't work!!"~Role literal).
].

// --- ConvertorTest ---

#symbol ConvertorTest =
[
    #var n := convertor toInt:"23".
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
    #var a2 := l~indexable array.

    control foreach:"abc" &do: (:ch) [ console write:ch ].

    console writeLine.
    
    #var n := "abcd" indexOf &index:0 &literal:"bc".
    #var n2 := "abcd" indexOf &index:0 &literal:"bac".
    
    #var s := "acd" insert &index:1 &literal:"b".
    #var ls := s length.

    #var s2 := "abbcd" delete &index:1 &length:1.
    #var ls2 := s2 length.
    
    #var s3 := "abcd" Substring &index:1 &length:2.
    #var ls3 := s3 length.
   
    "abc"getAt:2.
    
    "abc" getAt:(-1) |
        onOutOfRangeError: (:e)[ console writeLine:"Negative index is not allowed" ].

    "abc" getAt:4 |
        onOutOfRangeError: (:e) [ console writeLine:"Index bigger than length is not allowed" ].
].

// --- ArrayTest ---

#symbol ArrayTest =
[
    #var a1 := Array(1,2,3).
    #var n := a1 @ 1.

    #var a4 := Array new &length:3.
    a4 setAt:0:2.
    a4 setAt:1:1.
    a4 setAt:2:3.
    
    #var a2 := arrayControl new &length:5 &each: x [ Integer new:x ].
    #var n2 := a2 @ 1.
    a2 setAt:1:-1.
    #var n3 := a2 @ 1.
    
    a2 setAt:0:2.
    
    
    a4~indexable sort: (:aFormer:aNext)[ aFormer < aNext ].
    
    a4 @ (-1) |
        onOutOfRangeError: e [ console writeLine:"Negative index is not allowed" ].

    a4 @ 3 |
        onOutOfRangeError: e [ console writeLine:"Index bigger than length is not allowed" ].
].

// --- AltTest ---

#symbol AltTest =
[
    #var n := 3.
    
    #var r :=  n length | literal.
    #var r2 :=   n literal | length.
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
    ExecTester break | ifFailed: [ console writeLine:"Throw works!". ].
    
    ExecTester break | onError: e [ console writeLine:(e Message). ].
    
    ExecTester break2 | onError: e [ console writeLine:(e Message). ] | onError2: e [ console writeLine:(e Message). ].
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
    
    #var aYear := aDate Year.
    #var aMonth := aDate Month.
    #var aDay := aDate Day.
    #var anHour := aDate Hour.
    #var aMinute := aDate Minute.
    #var aSecond := aDate Second.
    
    #var aRetVal := aDate literal.
].

// --- GetPropertyTest ---

#class GetPropertyAgent
{
    #method eval : aGetProperty
    [
        #var n := aGetProperty property.
        
        2 == aGetProperty property
            ? [ console writeLine:"GetProperty works!". ]
            ! [ console writeLine:"!! GetProperty does not work !!". ]
    ]
}

#symbol GetPropertyTest =
[
    GetPropertyAgent eval:%property[2].
].
