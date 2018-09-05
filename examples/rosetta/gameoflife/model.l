//import extensions.
import cellular.

const int maxX = 48.
const int maxY = 28.

sealed class Model
{
    T<Space>   theSpace.
    T<RuleSet> theRuleSet.
    bool    started.

    Func<Space, object> event onUpdate :: theListener.
    
    constructor newRandomset(RuleSet transformSet)
    [
        theSpace := IntMatrixSpace new(maxY, maxX, randomSet).

        theRuleSet := transformSet.
        
        started := false.
    ]
    
    constructor newLoaded(RuleSet initSet, RuleSet transformSet)
    [
        theSpace := IntMatrixSpace new(maxY, maxX, initSet).
                        
        theRuleSet := transformSet.
        
        started := false.
    ]
    
    private onUpdate
    [
        if (nil != theListener)
            [ theListener(theSpace) ].
    ]
    
////    init
////    [
////        $self $onUpdate.
////    ]
    
    run
    [
        if (started)
            [ theSpace update(theRuleSet) ];
            [ started := true. ].        
        
        self onUpdate.
    ]
}