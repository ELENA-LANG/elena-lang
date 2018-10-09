import system'collections.
import system'routines.

public class SingleEnumerable
{
    object value.

    constructor new:value
    [
        @value := value.
    ]

    enumerator = Enumerator::
    {
        bool notpassed := nil != value.
        
        get
        [
            notpassed := false.
            
            ^ value.
        ]
        
        bool next
            = notpassed.
            
        reset
        [
            notpassed := nil != value.
        ]            
        
        enumerable = self.
    }.
}

public class CompoundEnumerator :: Enumerator
{
    Array<Enumerator> enumerators.
    int               index.
    
    generic constructor new(V<object> args)
    [
        index := 0.
        
        int len := args length.
        int i := 0.
        enumerators := V<Enumerator>(len).        
        while (i < len)
        [
            enumerators[i] := args[i] enumerator.
            i += 1
        ]
    ]
    
    reset
    [
        enumerators forEach(:e)[e reset]
    ]
    
    enumerable = self.
    
    get
        = enumerators[index] get.
        
    bool next
    [
        until (enumerators[index] next)
        [
            int len := enumerators length.
            
            index += 1.
            ifnot (index < len)
            [
                ^ false
            ]
        ].
        
        ^ true.
    ]        
}

