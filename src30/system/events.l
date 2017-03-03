//
//#template(sealed) eventhandler : function
//{
//    #field(arrayof:function)theFunctions.
//    
//    #constructor &function:func
//    [
//        theFunctions := function:1.
//        theFunctions@0 := func.
//    ]
//
//    #method append &function:func
//    [
//        #var(int)len := theFunctions length.
//        
//        #var(arrayof:function)functions := function:(len + 1).
//        
//        #var(int)i := 0.
//        #loop (i < len)?
//        [
//            functions@i := theFunctions@i.
//            
//            i += 1.
//        ].
//        
//        functions@len := func.
//        
//        theFunctions := functions.
//    ]
//    
//    #method append : func
//        <= append &function:func.
//    
//    #method => system'core_routines'__event.
//}
//
//#template event : function : prop
//{
//    #method prop &function:func
//    [
//        ($nil == target)
//            ? [ target := func. ]
//            ! [ target append &function:func. ].
//    ]
//}
