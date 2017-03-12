#import extensions.

// --- Basic types ---

#subject space   = BaseSpace.
#subject ruleset = BaseRuleSet.

// --- Rule Set ---

limited dispatchable(ruleset) class BaseRuleSet
{
    stacksafe bool validate &space:s &int:x &int:y []
}

class randomSet = BaseRuleSet
{
    validate &space:s &int:x &int:y
        = 1 == randomGenerator eval:0:2.
}.
