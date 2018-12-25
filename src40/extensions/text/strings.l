////import extensions.
//
//// --- StringBookmark ---
//
//public class StringBookmark :: BaseValue
//{
//    object theValue.
//    object theIndex.
//    object theStart.
//    
//    constructor new
//    [
//        theValue := emptyLiteral.
//        theIndex := 0.
//        theStart := 0.
//    ]
//
//    constructor new : aText
//    [
//        theValue := aText.
//        theIndex := 0.
//        theStart := 0.
//    ]
//
//    bool equal(int aValue)
//        => theIndex.
//
//    bool equal(literal aValue)
//        => theValue.
//
//    bool equal(wide aValue)
//        => theValue.
//
//    find : anObject
//        <= find(T<int>(anObject)).
//
//    append : anObject
//        <= append(T<int>(anObject)).
//
//    insert : anObject
//        <= insert(anObject literal).
//
//    delete : anObject
//        <= delete(T<int>(anObject)).
//
//    find(int anIndex)
//    [
//        theIndex := anIndex.
//    ]
//
//    find(literal aValue)
//    [
//        theIndex := theValue indexOf:aValue at:theIndex.
//        
//        ^ (-1 != theIndex)
//    ]
//    
//    find(wide aValue)
//    [
//        theIndex := theValue indexOf:aValue at:theIndex.
//        
//        ^ (-1 != theIndex)
//    ]
//    
//    mark
//    [
//        if(-1 == theIndex)
//            [
//                self clear
//            ];
//            [
//                theIndex := 1 + theIndex.
//                theStart := theIndex
//            ].
//        
//    ]
//    
//    append(literal aValue)
//    [
//        theValue := theValue + aValue.
//    ]
//    
//    append(wide aValue)
//    [
//        theValue := theValue + aValue.
//    ]
//    
//    append(int aValue)
//    [
//        theIndex := theIndex + aValue.
//    ]
//    
//    insert(literal aValue)
//    [
//        theValue := theValue insert:aValue at:theIndex.
//        
//        theIndex := theIndex + aValue length.
//    ]
//    
//    insert(wide aValue)
//    [
//        theValue := theValue insert:aValue at:theIndex.
//        
//        theIndex := theIndex + aValue length.
//    ]
//    
//    delete(int aLength)
//    [
//        theValue := theValue delete:aLength at:theIndex.
//    ]
//
//    delete(literal l)
//        <= delete(l length).
//
//    delete(wide l)
//        <= delete(l length).
//
//    clear
//    [
//        theValue := emptyLiteral.
//        theIndex := 0.
//        theStart := 0.
//    ]
//
//    Marked
//    [
//        if(-1 == theIndex)
//            [ ^theValue Substring:theStart ];
//            [ ^theValue Substring:(theIndex - theStart) at:theStart ]
//    ]
//    
//    T<literal> literal => theValue.
//    
//    cast T<literal> = T<literal>(theValue).
//    
//    cast T<wide> = T<wide>(theValue).
//    
//    cast T<int> = T<int>(theIndex).
//    
//    length => theValue.
//    
//    index = theIndex.
//}

// --- StringWriter ---

public sealed class StringWriter //: BaseVariable
{    
    string theValue;
    string theNewLine;
    
    constructor()
    {
        theValue := emptyString;
        theNewLine := forward newLine;
    }
    
//    constructor new : aText
//    [
//        theValue := aText.
//    ]
//
//    constructor new(literal s, int count)
//    [
//        theValue := emptyLiteral.
//        
//        0 till:count do(:i)[ self append(s) ].
//    ]
    
    write(string s)
    {
        theValue := theValue.add:s
    }
    
    writeLine(string s)
    {
        self.write(s).write(theNewLine)
    }
   
    writeLine()
    {
        self.write(theNewLine)
    }    

//    set value : aText
//    [
//        theValue := aText.
//    ]
//
//    append : aText
//    [
//        theValue := theValue add:aText.
//    ]
//    
//    insert:s at:index
//    [
//        theValue := theValue insert:s at:index.
//    ]
//
//    replace(literal s, literal s2)
//    [
//        theValue := theValue replace(s, s2).
//    ]
//    
//    replace(object s, object s2)
//    [
//        theValue := theValue replace(s literal, s2 literal).
//    ]
//    
//    value = theValue.
//
//    clear
//    [
//        theValue := emptyLiteral.
//    ]

    string Printable = theValue;

    string cast() = theValue;

    string Value = theValue;

//    cast T<wide> = theValue.
//
//    dispatch => theValue.
}
