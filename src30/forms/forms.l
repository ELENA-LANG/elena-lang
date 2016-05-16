#import system.
#import system'collections.

// --- BaseControlCollection ---

#class BaseControlCollection
{
    #field(list) theList.
    
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
