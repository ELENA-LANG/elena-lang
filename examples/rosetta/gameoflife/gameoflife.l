////import system'routines.
import system'threading.
////import extensions.
import cellular.

const int DELAY = 50.

symbol testSet = ((0,1,0),
                  (0,1,0),
                  (0,1,0)).

singleton gameOfLifeRuleSet :: RuleSet
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
}

playGame(Model model)
[
    console clear.
    
    model onUpdate := (:sp)<Space>[ sp print ].

    until (console isKeyAvailable)
    [
        model run.
        
        threadControl sleep:DELAY
    ].

    console readChar
]

public test
[
    playGame(Model newLoaded(LoadedSet load(testSet), gameOfLifeRuleSet)).
]

public program
[
    playGame(Model newRandomset(gameOfLifeRuleSet)).
]