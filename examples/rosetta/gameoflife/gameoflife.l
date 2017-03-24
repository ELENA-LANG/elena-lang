#import system'routines.
#import system'threading.
#import extensions.
#import cellular.

const int DELAY = 50.

symbol testSet = ((0,1,0),
                  (0,1,0),
                  (0,1,0)).

class gameOfLifeRuleSet = BaseRuleSet
{
    validate &space:s &int:x &int:y 
    [
        int cell := s getAt &int:x &int:y.
        int number := s getLiveCell &int:x &int:y. // NOTE : number of living cells around the self includes the cell itself
        
        if ((cell == 0) && (number == 3))
            [ ^ true. ].
            
        if ((cell == 1) && ((number == 4) || (number == 3)))
            [ ^ true. ].
            
        ^ false.            
    ]
}.

program =
[
    console clear.
    
    var model := Model newRandomset &ruleset:gameOfLifeRuleSet.

    model onUpdate &listener_func:
        (&space:sp) [ sp print ].

    until (console is &keyAvailable)
    [
        model run.
        
        thread sleep:DELAY.
    ].
    
    console readChar.
].