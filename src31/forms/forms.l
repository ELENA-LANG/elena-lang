import system'collections.

// --- BaseControlCollection ---

class BaseControlCollection
{
    list theList.
    
    constructor new
    [
        theList := List new.
    ]

    enumerator => theList.
            
    append : aControl
    [
        theList append:aControl
    ]
}
