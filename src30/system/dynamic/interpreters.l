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
        theStruct := Struct new &length:0.
    ]
    
    #constructor new : aStruct
    [
        theStruct := aStruct.
    ]

    #method value = theStruct.
            
    #method(generic)set : anObject
    [
        #var(type:subject) aSubject := $subject.
        
        // looking for the current subject
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var aNotFound := true.
        
        $classControl readLength:theStruct &vint:aLength.
        #loop ((anIndex < aLength)and:aNotFound) ?
        [
            #var aCurrent := theStruct::classOp getField &index:anIndex.
            
            (aCurrent::subjectOp equal &subject:aSubject)
                ? [ aNotFound := false. ]
                ! [ anIndex := anIndex + 2. ].
        ].
                
        // if not found extend the structure
        (anIndex == aLength) ?
        [                                    
            theStruct := theStruct::arrayOp reallocate &length:(aLength + 2).

            theStruct::classOp setField &index:anIndex &object:aSubject.
        ].

        // set the content for the appropriate field
        theStruct::classOp setField &index:(anIndex + 1) &object:anObject.
    ]
    
    #method => theStruct.
}