#import system.
#import system'collections.
#import system'routines.
#import system'dynamic.

#import extensions.
#import extensions'scripting.

// --- Tape ---

#class BFTape
{
    #field theArray.
    #field thePointer.
    #field theBrackets.

    #constructor new:aLength
    [
        theArray := Array new:aLength set &every: (&int:n) [ Integer new:0 ].
        
        thePointer := Integer new:0.
        
        theBrackets := Stack new.
    ]

    #method append
    [
         (theArray@thePointer) += 1.
    ]
    
    #method reduce
    [
         (theArray@thePointer) -= 1.
    ]
    
    #method next
    [
        thePointer += 1.
    ]

    #method previous
    [
        thePointer -= 1.
    ]
    
    #method push : bookmark
    [
        theBrackets push:bookmark.
    ]
    
    #method pop
        => theBrackets.
    
    #method input
    [
        theArray@thePointer := console readChar toInt.
    ]
    
    #method output
    [
        console write:((theArray@thePointer) toChar).
    ]

    #method check = theArray@thePointer != 0.
}

#symbol program =
[
    //[,.]
    
    #var program := Tape(
        2, %"tapeOp.var[]", 
        
        -2, %"tapeOp.ptr[]", 
        1, %"tapeOp.stack[]", 
        %"push[1]" , 
        
        %"input[0]", 
        %"output[0]", 
        
        0, %"tapeOp.stack[]", 
        %"check[0]", 
        1, %"tapeOp.stack[]", %"pop[0]",
        %"tapeOp.jumpif[1,]").
    
    program eval:(BFTape new:1024).
].

#symbol program2 =
[
//    #var text := "++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.,".
//    #var text2 := ">++++++++[<+++++++++ >-]<+++.>+++++[<+++++ >-]<+..>+++[<+++ >-]<++.>++++++++[<--------- >-]<--------.>++++++++[<+++++++++ >-]<+++++++.-.>++++++++[<--------- >-]<------.>++++++++[<++++++++ >-]<++.>++++[<++++ >-]<.>++++[<---- >-]<-.>++[<+++ >-]<++.+++++.>++[<--- >-]<--.>+++[<++++ >-]<+++.>++++[<---- >-]<--.>++[<+++ >-]<++.--.+++++.>++++++++[<-------- >-]<-------.------.>,".
   
    ('program'arguments length == 1) ?
        [ console write:"Please provide the path to the file to interpret". #throw AbortException new. ].
          
    #var program := scriptEngine 
        load &path:"rules.es"
        eval &path:('program'arguments@1/*"sample2.txt"*/).

    program eval:(BFTape new:1024).
].