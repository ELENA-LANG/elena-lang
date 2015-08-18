#import system.
#import system'routines.

// --- CastOver ---

#class(group) CastOver :: BaseArray
{
    #constructor new &args:aMembers
        <= (new &int:(aMembers length int))
    [
        #var(type:int)i := 0.
        #var(type:int)l := aMembers length.
        
        #loop (i < l)?
        [
            $self setField &int:i &object:(aMembers@i).
            
            i := i + 1.
        ].
    ]
    
    #method => system'internal'handle_castover.
}

// --- Tape ---

#class(group) Tape :: BaseArray
{
    #method => system'internal'handle_tape.
}

// --- tapeControl ---

#symbol tapeControl =
{
    getAt :anIndex &args:Params
    [
        #var(type:int)aLength := Params length.

        ^ Params@(aLength - anIndex int).
    ]
}.

// --- Struct ---

#class(group) Struct :: BaseArray
{
    #method => system'internal'handle_selector.
}

// --- Dynamic ---

#class Dynamic
{
    #field theStruct.
    
    #constructor new
    [
        theStruct := Struct new &int:0.
    ]
    
    #constructor new : aStruct
    [
        theStruct := aStruct.
    ]

    #method value = theStruct.
            
    #method(generic)set : anObject
    [
        // looking for the current subject
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var(type:bool) aNotFound := true.
        
        $classControl readLength:theStruct &vint:aLength.
        #loop ((anIndex < aLength)and:aNotFound) ?
        [
            #var aCurrent := theStruct::classROp getField &int:anIndex.
            
            ($subject::subjectROp $equalSubject:aCurrent)
                ? [ aNotFound := false. ]
                ! [ anIndex := anIndex + 2. ].
        ].
                
        // if not found extend the structure
        (anIndex == aLength) ?
        [                                    
            theStruct := theStruct::arrayOp reallocate &int:(aLength + 2).

            theStruct::classROp setField &int:anIndex &object:(Signature new &subject:$subject).
        ].

        // set the content for the appropriate field
        theStruct::classROp setField &int:(anIndex + 1) &object:anObject.
    ]
    
    #method(generic) if
    [
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        
        $classControl readLength:theStruct &vint:aLength.
        #loop (anIndex < aLength) ?
        [
            #var aCurrent := theStruct::classROp getField &int:anIndex.
            
            ($subject::subjectROp $equalSubject:aCurrent)
                ? [ ^ true. ].
                
            anIndex := anIndex + 2.                
        ].
        
        ^ false.
    ]
    
    #method => theStruct.
}

// --- obsolete ---

#symbol DynamicStruct = Dynamic.