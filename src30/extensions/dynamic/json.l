#import system.
#import system'text.
#import system'dynamic.
#import extensions.

#symbol jsonDispatcher =
{
    eval:output &int:n
    [
        output write:(n literal).
    ]
    
    eval:output &bool:b
    [
        output write:(b literal).
    ]
    
    eval:output &array:array
    [
        output::jsonSerializer jsonSerialize &enumerable:array.
    ]
    
    eval:output &enumerable:list
    [
        output::jsonSerializer jsonSerialize &enumerable:list.
    ]
    
    eval:output &dto:object
    [
        output::jsonSerializer jsonSerialize &dto:object.
    ]
    
    eval:output &literal:s
    [
        output writeLiteral:"""":s:"""".
    ]
    
    ! : output : object
        = output writeLiteral:"""":(object literal):"""".
}.

#class(extension:textbuffer)jsonSerializer
{
    #method jsonSerialize &dto:object
    [
        #var(type:textbuffer)output := self textbuffer.
        
        output write:"{".
                
        #var(type:mssgarray,size:10) properties.
        #var(type:int)len := 10.
        #var(type:int)offs := 0.

        object read &mssg_mask:%get &index:0 &mssgarray:properties &vint:len.
        #loop (len > 0) ?
        [
            #var(type:int)i:=0.
            #loop (i < len)?
            [
                #var(type:mssg) message := properties@i.
                
                output writeLiteral:"""":(message subject name):""":".
                
                output::jsonSerializer jsonSerialize:(object::message eval).
                
                output write:",".
                
                i := i + 1.
            ].
            
            offs := offs + len.
            
            object read &mssg_mask:%get &index:offs &mssgarray:properties &vint:len.
        ].
        
        #var(type:int)outputLen := output length.
        (outputLen > 1)
            ? [ output setLength &int:(outputLen - 1). ].
        
        output write:"}".
        
        ^ self.
    ]    
    
    #method jsonSerialize &enumerable:object
    [
        #var(type:textbuffer)output := self textbuffer.
        
        output write:"[".

        object run:output &eachPair:(:output:object)
        [
            output::jsonSerializer jsonSerialize:object.
            
            output write:",".
        ].
        
        #var(type:int)outputLen := output length.
        (outputLen > 1)
            ? [ output setLength &int:(outputLen - 1). ].
        
        output write:"]".
        
        ^ self.
    ]    
    
    #method jsonSerialize : object
    [
        ($nil != object)
            ? [
                object cast:%eval &to:jsonDispatcher &with:self.
            ]
            ! [ self write:"null". ].
            
        ^ self.            
    ]
}

#class(extension)jsonOp
{
    #method toJson
        = TextBuffer new jsonSerialize:self literal.
}        
