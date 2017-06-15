import extensions.
import cellular.

const int maxX = 48.
const int maxY = 28.

#subject listener_func = ListenerFunc.
limited dispatchable(listener_func) class ListenerFunc
{
    action eval &space:space []
}

sealed class Model
{
    space   theSpace.
    ruleset theRuleSet.
    bool    started.
    
    event(listener_func, onUpdate) theListener.
    
    constructor newRandomset &ruleset:transformSet
    [
        theSpace := IntMatrixSpace new &int:maxY &int:maxX &ruleset:randomSet.
                        
        theRuleSet := transformSet.
        
        started := false.
    ]
    
    constructor newLoaded &ruleset:initSet &ruleset:transformSet
    [
        theSpace := IntMatrixSpace new &int:maxY &int:maxX &ruleset:initSet.
                        
        theRuleSet := transformSet.
        
        started := false.
    ]
    
    $onUpdate
    [
        if ($nil != theListener)
            [ theListener eval &space:theSpace ].
    ]
    
    init
    [
        $self $onUpdate.
    ]
    
    run
    [
        if (started)
            [ theSpace update &ruleset:theRuleSet. ];
            [ started := true. ].        
        
        $self $onUpdate.
    ]
}