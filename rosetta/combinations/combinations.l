#define std'dictionary'*.
#define std'basic'*.
#define std'patterns'*.
#define std'collections'*.
#define std'routines'*.
#define ext'patterns'*.
#define ext'routines'*.

#define factory'* = std'basic'factory'*.

//#subject __previous.

#symbol M = 3.
#symbol N = 5. 


// --- Numbers ---

#symbol Numbers : aN
    = NewArray &&pattern:(#join(integer::-1) { clone = integer::(self append:1). }) &count:aN &:factory'epatterninit.

// --- Program ---

#symbol Program =
[
    #var aNumbers := Numbers::N.
    #var aList := List.
    
    Repeater::M run: = ( aList += aNumbers ).
    
    (Combinator::aList)~foreach run: aList =
    [
        'program'output << aList << "%n".
    ].
].