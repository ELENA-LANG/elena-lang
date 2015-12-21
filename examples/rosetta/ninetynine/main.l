#import system.
#import system'dynamic.
#import system'routines.
#import extensions.
#import extensions'routines.
#import extensions'text.

#class(extension)bottleOp
{
    #method bottleDescription
        = self literal + (self != 1) iif:" bottles":" bottle".

//    #method bottleEnumerator
//    [
//        #var aTarget := Variable new:self.
//        
//        ^ Enumerator
//        {
//            next = aTarget > 0.
//            
//            get = StringWriter new
//                    writeLine:(aTarget bottleDescription):" of beer on the wall"
//                    writeLine:(aTarget bottleDescription):" of beer"
//                    writeLine:"Take one down, pass it around"
//                    writeLine:((aTarget -= 1) bottleDescription):" of beer on the wall".
//        }.
//    ]

    #method bottleEnumerator = Variable new:self extend 
//                    &with:target 
//                        [  
//                            {
//                                enumerator = Enumerator
//                                {
//                                    next = target > 0.
//                                    
//                                        get = StringWriter new
//                                                writeLine:(self bottleDescription):" of beer on the wall"
//                                                writeLine:(self bottleDescription):" of beer"
//                                                writeLine:"Take one down, pass it around"
//                                                writeLine:((self -= 1) bottleDescription):" of beer on the wall".
//                                }.
//                            }
//                        ].
                        
                    &with:target =
                    {
                        enumerator = Enumerator
                        {
                            next = target > 0.
                            
                                get = StringWriter new
                                        writeLine:(self bottleDescription):" of beer on the wall"
                                        writeLine:(self bottleDescription):" of beer"
                                        writeLine:"Take one down, pass it around"
                                        writeLine:((self -= 1) bottleDescription):" of beer on the wall".
                        }.
                    }.
                        
}

#symbol program =
[
    #var bottles := 99.
    
    bottles bottleEnumerator run &each:printingLn.
].