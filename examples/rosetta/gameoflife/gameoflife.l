#import system'routines.
#import extensions.
#import cellular.

symbol testSet = (1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0).

class LoadedSet :: BaseRuleSet
{
    intmatrix theSet.

    constructor load &array:list &int:m
    [
        int n := list length / m.
        
        theSet := IntMatrix new &int:n &int:m.
        
        int i := 0.
        int j := 0.
        list run &each: n
        [
            j += 1.
            (j == m)
                ? [ i += 1. j := 0. ].
                
            theSet write &int:i &int:j &int:(n toInt).
        ].
    ]
        
    constructor load &intmatrix:m
    [
        theSet := m.
    ]
    
    validate &space:s &int:x &int:y 
    [
        int cell := theSet getAt &int:x &int:y.
        
        ^ cell != 0.
    ]
}

class gameOfLifeRuleSet = BaseRuleSet
{
    validate &space:s &int:x &int:y 
    [
        int cell := s getAt &int:x &int:y.
        int number := s getLiveCell &int:x &int:y.
        
        ((cell == 1) && (number == 3))
            ? [ ^ true. ].
        
        ((cell == 1) && ((number == 4) || (number == 3)))
            ? [ ^ true. ].            
            
        ^ false.            
    ]
}.

program =
[
    var model := Model new &ruleset:/*randomSet*/(LoadedSet load &array:testSet &int:5) &ruleset:gameOfLifeRuleSet.

    model onUpdate &listener_func:
        (&space:sp) [ sp print ].

    model init.

    while (true)
    [
        model run.
        
        console readChar.
    ].    
].