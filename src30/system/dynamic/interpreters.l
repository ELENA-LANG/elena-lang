#define system.
#define system'routines.
//#define system'math.

// --- Tape ---

#class(group) Tape :: BaseArray
{
    #method => system'internal'handle_tape.
}

// --- Struct ---

#class(group) Struct :: BaseArray
{
    #method => system'internal'handle_selector.
}

// --- DynamicStruct ---

#class DynamicStruct
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
        #var aNotFound := true.
        
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

            theStruct::classROp setField &int:anIndex &object:$subject.
        ].

        // set the content for the appropriate field
        theStruct::classROp setField &int:(anIndex + 1) &object:anObject.
    ]
    
    #method => theStruct.
}