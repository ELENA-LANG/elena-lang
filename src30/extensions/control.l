#define system.
#define system'routines.

// --- controlEx ---

#symbol controlEx =
{
    foreach:anEnumerable &top:aMaxValue &do:aFunction
        = aFunction~functionOp run &enumerator:(TopEnumerator new:anEnumerable &max:aMaxValue) | stop.

    foreach:anEnumerable &top:aMaxValue &where:aCondition &do:aFunction
        = aFunction~functionOp run &each:(TopEnumerator new:anEnumerable &max:aMaxValue) &where:aCondition | stop.

    foreach:anEnumerable &where:aCondition &do:aFunction
        = aFunction~functionOp run &each:(anEnumerable enumerator) &where:aCondition | stop.

}.
