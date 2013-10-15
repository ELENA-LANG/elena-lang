//#define testcase'*.
//
////#class Test
////{
////    
////    #method $test32
////    [
////        #var aDate := sys'dates'Now.
////        #var anUTCDate := sys'dates'UtcNow.
////        
////        'program'output << anUTCDate~ELongDateConvertor literal << "%".
////        
////        #var aDifference := aDate - anUTCDate.
////        #var aNewNow := sys'dates'UtcNow + aDifference.
////        
////        'program'output << aNewNow~ELongDateConvertor literal << "%".        
////    ]
////    
//////    #method $test31
//////    [
//////        __group(test'Member11, test'Member1, "abc") sb1'eval:nil.
//////        
//////        __group(2, test'Member1, "abc") sb1'eval:nil.
//////        
//////        __union(2, test'Member1, "abc") sb1:nil.
//////    ]
//////    
//////    #method $test30
//////    [
//////        #var anIndexer := (integer::0, widecharvar::33, integer::2)@0.
//////        #var aGroup := sys'dynamics'GroupVariable append &sys'dynamics'group_member:anIndexer.
//////        
//////        aGroup append &sys'dynamics'group_member:__prop(sb1, __append, __wrap(std'dictionary'index, aGroup)).
//////        aGroup append &sys'dynamics'group_member:__prop(sb2, __append, aGroup).
//////        aGroup append &sys'dynamics'group_member:__prop(sb3, __run, __wrap(EWhileNotZero, aGroup)).
//////
//////        // print action
//////        #var anAction1 := __echo(sb1, 1).
//////        #var anAction2 := __wrap(__write, 'program'output).
//////        #var anAction3 := __wrap(__save, __action(__get, 'program'input, nil)).
//////        #var anAction4 := __echo(sb2, 1).
//////        
//////        #var anAction5 := __echo(sb3, __wrap(__eval, __cast(anAction3, anAction2))).
//////        
//////        __cast(anAction1, anAction5) invoke:aGroup.
//////    ]
////
////////    #method $test3
////////    [
////////        #var aBuffer := Buffer::&binary_size:30.
////////        #var aWriter := aBuffer binary_writer.
////////        aWriter write:2 write:3l write:4.5r write:"abc".
////////
////////        #var aFileWriter :=  sys'io'NewBinaryFileWriter::"test.dat".
////////
////////        aFileWriter write:aBuffer.
////////        aFileWriter free.
////////    ]
////////
////////    #method $test4
////////    [
////////        #var aBuffer := Buffer::&binary_size:30.
////////        #var aFileWriter :=  sys'io'BinaryFileReader::"test.dat".
////////
////////        #var aByteArray := aFileWriter read:aBuffer.
//////// 
////////        #var aReader := aBuffer binary_reader.
////////        #var anN := aReader read:Integer.
////////        #var anL := aReader read:LongInteger.
////////        #var anR := aReader read:Real.
////////        #var aS := aReader read &newliteral &length:3.
////////   ]
//////
////////    #method $test10
////////    [
////////        #var aCustomDay := NewDate::&year:2012 &month:6 &day:13.
////////
////////        'program'output << aCustomDay << "%n".
////////
////////        #var aCurrentDate := Now.
////////
////////        aCurrentDate append &day:7.
////////        aCurrentDate append &minute:2.
////////
////////        'program'output << aCurrentDate.
////////    ]
//////    
//////    #method $test11
//////    [
//////        #var aDate := /*Now*/NewDate::{ &year:2010 &month:2 &day:1 }.
//////        'program'output << __wrap(sys'dates'ELongDateConvertor, aDate) << "%n".
//////        
//////        #var aDiff := aDate - NewDate::{ &year:2010 &month:1 &day:30 }.
//////        
//////        #var aNewDate := NewDate::{ &year:2010 &month:1 &day:1 }/* + aDiff*/.
//////        'program'output << __wrap(sys'dates'ELongDateConvertor, aNewDate) << "%n".
//////        
//////        #var aYear := aDiff year.
//////        #var aMonth := aDiff month.
//////        #var aDay := aDiff day.
//////        #var aHour := aDiff hour.
//////        #var aMinute := aDiff minute.
//////        #var aSecond := aDiff second.
//////    ]
////}
//
//
//// --- Program ---
//
//#symbol Program =
//[
//    //TestCase0.
//    //TestCase1.
//    //TestCase2.
//    //TestCase3.
//    //TestCase4.
//    //TestCase5.
//    //TestCase6.
//    //TestCase8.
//    //TestCase7.
//    //TestCase9.    
//    //TestCase10.    
//    //TestCase11.
//    //TestCase12.
//    //TestCase13.
//    //TestCase14.
//    //TestCase15.
//    //TestCase16.
//    //TestCase17.
//    //TestCase18.
//    TestCase19.
//    
//    //TestCaseX.
//].