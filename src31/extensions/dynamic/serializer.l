#import system.
#import system'text.
#import system'dynamic.
#import extensions.

// --- textSerializationOp ---

#class(extension) textSerializationOp
{
    #method toTextSerialized
    [
        #var(textbuffer)aResult := TextBuilder new.
        #var anObject := self.

        aResult write:(anObject className).
        #var aCounter := anObject fieldCount.
        (0 < aCounter) ?
        [       
            aResult write:"{".

            0 till:aCounter &doEach:i
            [
                (0 == i) 
                    ! [ aResult write:"," ].
                
                #var aMember := anObject getFieldAt:i.

                aResult write:(aMember toTextSerialized).
            ].

            aResult write:"}".
        ].

        ^ aResult literal.
    ]
}