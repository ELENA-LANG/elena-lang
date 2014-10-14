#define system.
#define system'routines.
#define system'math.

// --- ETapeControl --

#symbol tapeControl =
{
    read :anIndex ::Params
    [
        #var(type:int)aLength.
        
        Params read &out'length:aLength.
        
        ^ Params@(aLength - anIndex int).
    ]
}.

// --- Tape ---

#class(group,dbg:array) Tape : BaseArray
{
    #method => <handle_tape>.
}

// --- Struct ---

#class Struct : Tape
{
    #method => <handle_selector>.
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
    
    #method(generic)set : anObject
    [
        #var aSubject := $subject.
        
        // looking for the current subject
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var aNotFound := true.
        theStruct~arrayOp read &out'length:aLength.
        #loop ((anIndex < aLength)and:aNotFound) ?
        [
            #var aCurrent := theStruct~arrayOp getAt &index:anIndex.
            
            (aCurrent~intOp equal &object:aSubject)
                ? [ aNotFound := false. ]
                ! [ anIndex := anIndex + 2. ].
        ].
                
        // if not found extend the structure
        (anIndex == aLength) ?
        [                                    
            theStruct := theStruct~arrayOp reallocate &length:(aLength + 2).

            theStruct~arrayOp setAt &index:anIndex &object:aSubject.
        ].

        // set the content for the appropriate field
        theStruct~arrayOp setAt &index:(anIndex + 1) &object:anObject.
    ]
    
    #method => theStruct.
}