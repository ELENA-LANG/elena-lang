import system'routines.
import system'threading.
import extensions.
import cellular.

const int DELAY = 50.

symbol testSet = ((0,1,0),
                  (0,1,0),
                  (0,1,0)).

class gameOfLifeRuleSet = BaseRuleSet::
{
    proceed space:s int:x int:y vint:retVal
    [
        int cell := s getAt int:x int:y.
        int number := s getLiveCell int:x int:y int:1. // NOTE : number of living cells around the self includes the cell itself
        
        if ((cell == 0) && (number == 3))
        [ 
            retVal int := 1 
        ];
        if ((cell == 1) && ((number == 4) || (number == 3)))
        [
            retVal int := 1 
        ];
        [
            retVal int := 0
        ]
    ]
}.

program =
[
    console clear.
    
    var model := Model newRandomset ruleset:gameOfLifeRuleSet.

    model onUpdate listener_func(:sp)<space>[ sp print ].

    until (console isKeyAvailable)
    [
        model run.
        
        thread sleep:DELAY.
    ].
    
    console readChar.
].