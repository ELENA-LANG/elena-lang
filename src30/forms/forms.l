#define system.
#define system'collections.

// --- BaseControlCollection ---

#class BaseControlCollection
{
    #field(type:list) theList.
    
    #constructor new
    [
        theList := List new.
    ]

    #method enumerator => theList.
            
    #method append : aControl
    [
        theList append:aControl.
    ]
}

