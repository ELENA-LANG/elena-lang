#define system.
#define system'routines.

// --- controlEx ---

#symbol controlEx =
{
    do:anAction = anAction eval.
    
    do:anAction &until:anExpr
        = anAction~EAction run &until:anExpr | stop.
        
    while:anExpr
        = $self while:anExpr &do:[].

    while:anExpr &do:anAction
        = anAction~EAction run &if:anExpr | stop.

    foreach:anEnumerable &do:aFunction
        = aFunction~EFunction run &each:(anEnumerable Enumerator) | stop.

    for:anEnumerator &do:aFunction
        = aFunction~EFunction run &each:anEnumerator | stop.

    foreach:anEnumerable &until:aFunction
    [
        #var anEnumerator := anEnumerable Enumerator.
        
        ^ control while:[anEnumerator next] &do:
        [
            aFunction eval:(anEnumerator get) ?
                [ #break anEnumerator get. ].
        ].
    ]
    
    foreach:anEnumerable &top:aMaxValue &do:aFunction
        = aFunction~EFunction run &each:(TopEnumerator new:anEnumerable &max:aMaxValue) | stop.

    foreach:anEnumerable &top:aMaxValue &where:aCondition &do:aFunction
        = aFunction~EFunction run &each:(TopEnumerator new:anEnumerable &max:aMaxValue) &where:aCondition | stop.

    foreach:anEnumerable &where:aCondition &do:aFunction
        = aFunction~EFunction run &each:(anEnumerable Enumerator) &where:aCondition | stop.

    for:anIterator &from:aStart &to:anEndValue &by:aStep &do:aFunction
        = aFunction~EFunction run &for:anIterator &from:aStart &to:anEndValue &by:aStep | stop.

    from:aStart &to:anEndValue &do:aFunction
        = aFunction~EFunction run &for:(system'Integer new) &from:aStart &to:anEndValue &by:1 | stop.
            
    from:aStart &till:anEndValue &do:aFunction
        = aFunction~EFunction run &for:(system'Integer new) &from:aStart &till:anEndValue &by:1 | stop.
            
    for:anIterator &from:aStart &backTo:anEndValue &by:aStep &do:aFunction
        = aFunction~EFunction reverse &for:anIterator &from:anEndValue &to:aStart &by:aStep | stop.
            
    from:aStart &backTo:anEndValue &do:aFunction
        = aFunction~EFunction reverse &for:(system'Integer new) &from:anEndValue &to:aStart &by:1 | stop.
}.
