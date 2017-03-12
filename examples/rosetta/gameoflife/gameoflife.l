#import cellular.

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
    var model := Model new &ruleset:randomSet &ruleset:gameOfLifeRuleSet.

    model onUpdate &listener_func:
        (&space:sp) [ sp print ].

    while (true)
    [
        model run.
        
        console readChar.
    ].    
].