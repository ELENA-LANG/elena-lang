#import extensions.
#import cellular.

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
    
    event(listener_func, onUpdate) theListener.
    
    constructor new &ruleset:initSet &ruleset:transformSet
    [
        theSpace := IntMatrixSpace new &int:maxY &int:maxX &ruleset:initSet.
                        
        theRuleSet := transformSet.
    ]
    
    $onUpdate
    [
        ($nil != theListener)
            ? [ theListener eval &space:theSpace ].
    ]
    
    run
    [
        theSpace update &ruleset:theRuleSet.
        
        $self $onUpdate.
    ]
}