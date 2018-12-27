//import system'collections.

// --- Struct ---

public mixin Struct
{
    object[] items;
    
    internal constructor allocate(int n)
        = new object[](n);
    
    constructor(object[] list)
        <= allocate(list.Length)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            items[i] := list[i]
        }
    }

//    constructor new(ArrayList list, int index, int length)
//        <= new(length);
//    [
//        int i := 0.
//        while(i < length)
//        [
//            self setField(i, list getAt(index + i)).
//            
//            i := i + 1
//        ]
//    ]
    
    dispatch() => system'core_routines'__struct;
}

// --- DynamicStruct ---

public class DynamicStruct
{
    Struct theStruct;
    
    constructor()
    {
        theStruct := Struct.allocate(0)
    }
    
//    constructor new : aStruct
//    [
//        theStruct := T<Struct>(aStruct).
//    ]
//
//    cast T<Struct> = theStruct.
//    
//    generic:anObject
//    [
//        // looking for the current subject
//        int  anIndex := 0.
//        int  aLength := 0.
//        bool aNotFound := true.
//        
//        theStruct readFieldCountTo(&aLength).
//        while ((anIndex < aLength) && aNotFound)
//        [
//            var aCurrent := theStruct~classROp getField(anIndex).
//            
//            if(T<Signature>(aCurrent) equal(__received))
//                [ aNotFound := false ];
//                [ anIndex := anIndex + 2 ]
//        ].
//                
//        // if not found extend the structure
//        if(anIndex == aLength)
//        [                                    
//            theStruct := theStruct reallocate(aLength + 2).
//
//            theStruct~classROp setField(anIndex, Signature new(received)).
//        ].
//
//        // set the content for the appropriate field
//        theStruct~classROp setField(anIndex + 1, anObject).
//    ]
//    
//    containsProperty(Signature subj)
//    [
//        int anIndex := 0.
//        int aLength := 0.
//        
//        theStruct readFieldCountTo(&aLength).
//        while (anIndex < aLength)
//        [
//            var aCurrent := theStruct~classROp getField(anIndex).
//            
//            if(subj equal(T<Signature>(aCurrent)))
//                [ ^ true ].
//                
//            anIndex := anIndex + 2.                
//        ].
//        
//        ^ false
//    ]
//    
//    dispatch => theStruct.
}
