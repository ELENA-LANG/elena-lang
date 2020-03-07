//import extensions.
import cellular;

const int maxX = 48;
const int maxY = 28;

sealed class Model
{
    Space   theSpace;
    RuleSet theRuleSet;
    bool    started;

    event Func<Space, object> OnUpdate;
    
    constructor newRandomset(RuleSet transformSet)
    {
        theSpace := IntMatrixSpace.allocate(maxY, maxX, randomSet);

        theRuleSet := transformSet;
        
        started := false
    }
    
    constructor newLoaded(RuleSet initSet, RuleSet transformSet)
    {
        theSpace := IntMatrixSpace.allocate(maxY, maxX, initSet);
                        
        theRuleSet := transformSet;
        
        started := false
    }
    
    private onUpdate()
    {
        OnUpdate.?(theSpace)
    }
    
//    init
//    [
//        $self $onUpdate.
//    ]
    
    run()
    {
        if (started)
        { 
            theSpace.update(theRuleSet) 
        }
        else
        {
            started := true
        };
        
        self.onUpdate()
    }
}