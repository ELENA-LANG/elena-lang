#define system.
#define system'routines.

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
        #var(type:int)anIndex.
        theStruct~arrayOp indexOf &index:0 &object:aSubject &out'index:anIndex.

        // if not found extend the structure
        (anIndex == -1) ?
        [                                    
            #var(type:int)aLength.
            theStruct~arrayOp read &out'length:aLength.

            theStruct := theStruct~arrayOp reallocate &length:(aLength + 2).

            anIndex := aLength.

            theStruct~arrayOp set &index:anIndex &object:aSubject.
        ].

        // set the content for the appropriate field
        theStruct~arrayOp set &index:(anIndex + 1) &object:anObject.
    ]
    
    #method => theStruct.
}