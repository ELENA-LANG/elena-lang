import system'collections.

class SingleEnumerable
{
    object value.

    constructor new:value
    [
        @value := value.
    ]

    enumerator = Enumerator::
    {
        bool notpassed := $nil != value.
        
        get
        [
            notpassed := false.
            
            ^ value.
        ]
        
        bool next
            = notpassed.
    }.
}

//class CompoundEnumerator :: Enumerator
//{
//    array<Enumerator> enumerators.
//    int               index.
//    
//    generic constructor new(object<> args)
//    [
//        index := 0.
//        
//        int len := args length.
//        int i := 0.
//        enumerators := type<Enumerator><>(len).        
//        while (i < len)
//        [
//            enumerators[i] := args[i].
//            i += 1
//        ]
//    ]
//    
//    get
//        = enumerators[index] get.
//        
//    bool next
//    [
//        until (enumerators[index] next)
//        [
//            int len := enumerators length.
//            
//            index += 1.
//            ifnot (index < len)
//            [
//                ^ false
//            ]
//        ].
//        
//        ^ true.
//    ]        
//}

