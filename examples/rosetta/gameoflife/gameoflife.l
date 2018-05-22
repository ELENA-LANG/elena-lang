//import system'routines.
//import system'threading.
//import extensions.
import cellular.

//const int DELAY = 50.
//
//symbol testSet = ((0,1,0),
//                  (0,1,0),
//                  (0,1,0)).

class gameOfLifeRuleSet = RuleSet::
{
    proceed(Space s, int x, int y, ref<int> retVal)
    [
        int cell := s getAt(x, y).
        int number := s getLiveCell(x, y, 1). // NOTE : number of living cells around the self includes the cell itself
        
        if ((cell == 0) && (number == 3))
        [ 
            retVal value := 1 
        ];
        if ((cell == 1) && ((number == 4) || (number == 3)))
        [
            retVal value := 1 
        ];
        [
            retVal value := 0
        ]
    ]
}.

public program =
[
    console clear.
    
    var model := Model newRandomset(gameOfLifeRuleSet).

//    model onUpdate listener_func(:sp)<space>[ sp print ].
//
//    until (console isKeyAvailable)
//    [
//        model run.
//        
//        thread sleep:DELAY.
//    ].
    
    console readChar.
].