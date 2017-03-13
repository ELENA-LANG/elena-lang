#import system'routines.
#import extensions.
#import cellular.

symbol testSet = ((1,0,0,0,0),(0,0,0,0,0),(0,0,0,0,0),(0,0,0,0,0),(0,0,0,0,0)).

class gameOfLifeRuleSet = BaseRuleSet
{
    validate &space:s &int:x &int:y 
    [
        int cell := s getAt &int:x &int:y.
        int number := s getLiveCell &int:x &int:y. // NOTE : number of living cells around the self includes the cell itself
        
        ((cell == 0) && (number == 3))
            ? [ ^ true. ].
            
        ((cell == 1) && ((number == 4) || (number == 3)))
            ? [ ^ true. ].
            
        ^ false.            
    ]
}.

program =
[
    var model := Model newRandomset &ruleset:gameOfLifeRuleSet.

    model onUpdate &listener_func:
        (&space:sp) [ sp print ].

    while (true)
    [
        model run.
        
        console readChar.
    ].    
].