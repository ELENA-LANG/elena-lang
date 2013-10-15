//#define std'dictionary'*.
//#define std'basic'*.
//#define std'patterns'*.
//#define std'routines'*.
//#define std'collections'*.
//#define std'memory'*.
//
//#define sys'dates'*.
//
//#define std'routines'strings'*.
//
//// --- TestCaseX --
//
//#symbol TestCaseX =
//[
//    #var a := 2 literal | __widestr | __int32.
//    #var b := "abc" __widestr.
//].
//
//// --- TestCase20 ---
//
//#symbol TestCase19 =
//[
//    #var aValue := WideCharVar::80.
//    #var theObject := { get = aValue. write = aValue << $param. }.
//
//    #var theOutMethod := __wrap(invoker::%get, __wraps(sys'vm'routines'dummy, nil, %get, theObject, %write, 'program'output)).
//    #var theInMethod := __wrap(invoker::%set, __wraps(sys'vm'routines'dummy, nil, %get, 'program'input, %write, theObject)).
//
//    #var aDynamicObject := __group(theInMethod, theOutMethod).
//            
//    aDynamicObject set. 
//    aDynamicObject get.
//].
//
//// --- TestCase16 --
//
//#symbol TestCase16 =
//[
//    #var n := 2l * 3l.
//    #var n1 := -2l * 3l.
//    #var n2 := 2l * -3l.
//    #var n3 := -2l * -3l.
//    #var n4 := 2000000l * 4000000l.
//    #var n5 := -2000000l * 4000000l.
//    #var n6 := 2000000l * -4000000l.
//    #var n7 := -2000000l * -4000000l.
//    
//    #var d := 6l / 3l.
//    #var d1 := -6l / 3l.
//    #var d2 := 6l / (-3l).
//    #var d3 := -6l / (-3l).
//].
//
//// --- TestCase15 ---
//
//#symbol TestCase15_Get =
//[
//    ^ 3.
//].
//
//#symbol TestCase15 =
//[
//    #var aDynamicMethod := __wrap(invoker::%std'dictionary'length'get, #symbol TestCase15_Get).
//    
//    #var aRetVal := aDynamicMethod std'dictionary'length'get.
//].
//
//// --- TestCase14 ---
//
//#symbol TestCase14 =
//[
//    #var aS := ext'serialization'TextSerialized::(2,3,4).    
//    
//    #var aS2 := ext'serialization'TextSerialized::%std'dictionary'length'back.
//].
//
//// --- TestCase10 ---
//
//#symbol TestCase10 =
//[
//    // Hello world sample
//    
//    #var aStep1 := __wraps(sys'vm'routines'dummy, "hello", %write, 'program'output).   
//    #var aStep2 := __wraps(%get, 'program'input).
//    
//    #var aProgram := __cast(aStep1, aStep2).
//    aProgram::nil.
//].
//
//// --- TestCase13 ---
//
//#symbol TestCase13 =
//[
//    // succession test
//    #var aSucc := __succ("abc", %get, %length).
//    #var aLength := aSucc eval.
//    
//    // roles test: a~length~__int32
//    #var aRoles := __roles(%length, %__int32).
//    #var aLength2 := "abc"~aRoles get.
//    
//    // nested wraps test
//    #var aLength3 := "abc"~%length~%__int32 get.
//].
//
//// --- TestCase12 ---
//
//#symbol TestCase12 =
//[
//    #var p1 := "abba"~eliteralop first_occ'find:"b".
//    #var p2 := "abba"~eliteralop last_occ'find:"a".
//
//    #var s2 := "abba"~eliteralop strings'first_occ'replace &from:"b" &to:"B".
//    #var s3 := "abba"~eliteralop strings'all_occ'replace  &from:"b" &to:"B".
//
//    #var s4 := "abba"~eliteralop read &from:1 &to:2 &:esubstring.
//    #var s41 := "abba"~eliteralop read &from:1 &length:2 &:esubstring.
//
//    #var s5 := "aa"~eliteralop insert &literal:"bb" &index:1.
//    #var s6 := "abbba"~eliteralop delete &index:2 &length:1.
//    
//    "abba"~eliteralop strings'starting_with'if:"a".
//    "abba"~eliteralop strings'ending_with'if:"a".
//].
//
//// --- TestCase11 : $next ---
//
//#class TestCase11_Member0
//{
//    #generic
//    [
//        ^ $next $message:$param.
//    ]
//}
//
//#class TestCase11_Member1
//{
//    #method proceed
//    [
//        #var aNext := $next.
//        
//        $next proceed.
//    ]
//}
//
//#class TestCase11_Member2
//{
//    #method proceed
//    [
//    ]
//}
//
//#symbol TestCase11 =
//[
//    #var aGroup := __group(TestCase11_Member0, TestCase11_Member1, TestCase11_Member2).
//    
//    aGroup proceed.
//    
//    #var a := integer.
//    #var b := longinteger.
//    #var aCast := __cast(a, b) += 1.
//].
//
//// --- TestCase5
//
//#class TestCase5Helper
//{
//    #field theValue.
//    
//     #initializer : anObject
//     [
//         #if theValue := anObject
//         [ 'program'output << "Object is assigned%n". ]
//         | [
//             'program'output << "Argument list cannot be assigned%n".
//         ].
//     ] 
//}
//
//#symbol TestCase5 =
//[
//    #var aCase1 := TestCase5Helper::2.
//    #var aCase2 := TestCase5Helper &&numeric:2.
//].
//
//// --- TestCase0 ---
//
//#symbol Dummy = { if : aParam [ aParam is. ] is [] }.
//#symbol Dummy2 = { isnot [] }.
//
//#symbol TestCase0 =
//[
//    // simple send operation
//    Dummy is.
//    
//    // simple alternative operation
//    Dummy isnot | is.
//    
//    // simple variable assignment
//    #var a := Dummy is.
//    
//    // simple variable assignment with alternative
//    #var b := Dummy2 is | isnot.
//    
//    #if a is
//    [
//        #if b is
//            [ control fail. ]
//            | [ b isnot. ].
//    ] | [
//        control fail.
//    ].
//    
//    #if a 
//        add [ control fail. ]
//        | subtract [ control fail. ]
//        | [ a is. ].
//        
//].
//
//// --- TestCase9 ---
//
//#symbol TestCase9 =
//[
//    #var a := (1,7,4,-5).
//    a~esort run: aPair = (aPair former < aPair later).
//
//    #var b := ("b","ab","a","aa").
//    b~esort run: aPair = (aPair former < aPair later).
//
//    #var c := list += 3 += -4 += 1.
//    c~esort run: aPair = (aPair former < aPair later).
//
//    #var d := list += "b" += "av" += "a".
//    d~esort run: aPair = (aPair former < aPair later).
//
//    #var e := dictionary append &dictionary_key:2 &content:"b" append &dictionary_key:1 &content:"a" append &dictionary_key:3 &content:"c".
//    e~esort run: aPair = (aPair former dictionary_key < aPair later dictionary_key).
//    
//    #var f := summing &&var:list &prop:ext'patterns'ecloneprop start:scan::"bab".
//    f~esort run: aPair = (aPair former < aPair later).
//].
//
//// --- TestCase8 ---
//
//#symbol TestCase8 =
//[
//    #var aList := std'collections'List += 1 += 2 += 3 += 4.
//    
//    #var aSecond := aList~earray @ 1.
//    
//    Scan &&enumerable:aList &from:1 &length:2 &:EListSubRange run: #symbol ext'patterns'PrintingLn.
//].
//
//// --- TestCase7 ---
//
//#symbol TestCase7 =
//[
//    #var N := (1, 2, 3, 4).
//    #var M := (N, N).
//    
//    ext'routines'CombinatorWithRepetition::M ~foreach run: aList =
//    [
//        'program'output << aList << "%n".
//    ].
//    
//    'program'output << "%n".
//    
//    ext'routines'Combinator::M ~foreach run: aList =
//    [
//        'program'output << aList << "%n".
//    ].
//].
//
//// --- TestCase6 --- 
//
//#symbol TestCase6 =
//[
//    #var aProp := %length.
//    #var aLength := "abc"~aProp get.
//].
//
//// --- TestCase1 ---
//
//#symbol MaxCount = 5.
//#symbol LoopLength = 500. 
//
//#symbol TestCase1 =
//[
//    'program'output << "using stack allocated objects:%n".
//    
//    #var aCounter := Integer << 0.
//    #var aTotal := LongInteger << 0.
//    #var aSum := LongInteger << 0.
//    
//    #while (aCounter < MaxCount)?
//    [
//        #var Start := sys'dates'Now.
//        #var var := integer::0.
//    
//        loop &&from:var &to:LoopLength run: i =
//        [
//            loop &&from:var &to:LoopLength run: j =
//            [
//                loop &&from:var &to:LoopLength run: k =
//                [
//                    aSum += (i * j * k).
//                ].
//            ].
//        ].
//    
//        #var End := sys'dates'Now.
//    
//        aTotal += (End - Start) sys'dates'milliseconds.
//    
//        'program'output << (End - Start) sys'dates'milliseconds << "%n".
//    
//        aCounter += 1.
//    ].
//    
//    'program'output << "average time " << (aTotal / MaxCount) << "%n".
//].
//
//// --- TestCase2 ---
//
//#symbol TestCase2 =
//[
//    'program'output << "using dynamic allocated objects:%n".
//    
//    #var aCounter := Integer << 0.
//    #var aTotal := LongInteger << 0.
//    #var aSum := LongInteger << 0.
//    
//    #while (aCounter < MaxCount)?
//    [
//        #var Start := sys'dates'Now.
//        #var var := integer::0.
//    
//        loop :: { from=var. to=LoopLength. } run: 
//        {
//            eval : i
//            [
//                loop :: { from=var. to=LoopLength. } run:
//                {
//                    eval : j
//                    [
//                        loop :: { from=var. to=LoopLength. } run: 
//                        {
//                            eval : k
//                            [
//                                aSum += (i * j * k).
//                            ]
//                        }.
//                    ]
//                }.
//            ]
//        }.
//    
//        #var End := sys'dates'Now.
//    
//        aTotal += (End - Start) sys'dates'milliseconds.
//    
//        'program'output << (End - Start) sys'dates'milliseconds << "%n".
//    
//        aCounter += 1.
//    ].
//    
//    'program'output << "average time " << (aTotal / MaxCount) << "%n".
//].
//
//// --- TestCase3 ---
//
//#symbol FailedSymbol = control fail.
//
//#static FailedStatic = FailedSymbol.
//
//#symbol TestCase3 =
//[
//    'program'output << "test case3: (failed symbol / static symbol)".
//    
//    #var dummy := FailedStatic
//    | [
//        'program'output << "successful%n".
//    ].
//].
//
//#symbol TestCase4 =
//[
//    #var aSecond := Now sys'dates'second.
//].
//
//#symbol TestCase17 =
//[
//        #var aBuffer := Buffer &&size:30.
//
//        #var aLen := aBuffer length.
//        #var aSize := aBuffer size.
//
//        #var aWriter := aBuffer writer.
//        aWriter write:2 write:3l write:4.5r write:"abc".
//
//        aLen := aBuffer length.
//        aSize := aBuffer size.
//
//        #var aTemp := aBuffer reader read:NewByteArray::aLen.
//        aLen := aTemp length.
//
//        #var aNewBuffer := Buffer &&size:30.
//        aNewBuffer writer write:aTemp.
//
//        aLen := aNewBuffer length.
//
//        #var aReader := aNewBuffer reader.
//        #var anN := aReader read:Integer.
//        #var anL := aReader read:LongInteger.
//        #var anR := aReader read:Real.
//        #var anS := aReader read:NewWideLiteral::3.
//].
//
//#symbol TestCase18 =
//[
//    #var upper := "hello"~eliteralop upper_case.
//    #var lower := "HELLO"~eliteralop lower_case.
//    
//    #if "s"~eliteralop lower_case'is
//    [
//        'program'output << "works".
//    ].
//    
//    #if "S"~eliteralop lower_case'is
//    [
//        'program'output << "doesn't work".
//    ].
//    
//    #if "S"~eliteralop upper_case'is
//    [
//        'program'output << "works".
//    ].
//].