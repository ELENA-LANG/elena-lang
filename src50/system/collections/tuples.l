namespace collections
{
    public sealed Tuple<T1>
    {
    //    constructor
    //    [
    //    ]    
        
        constructor(T1 o1)
        {
            Item1 := o1
        }
        
        prop T1 Item1;
    
    //    getAt(int index)
    //    [
    //        index =>
    //            0 [ ^ field1 ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    setAt(int index, type1 object)
    //    [
    //        index =>
    //            0 [ field1 := object ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    indexer = BaseIndexer::
    //    {
    //        @theLength := 1.
    //        
    //        @theIndex := 0.
    //        
    //        get
    //            = self getAt(theIndex).
    //            
    //        set : o
    //        [
    //            self setAt(theIndex, T<type1>(o)).
    //        ]                        
    //    }.
    //            
    //    cast eval(type1 o1)
    //    [
    //        field1 := o1.
    //    ]
    }
    
    public sealed Tuple<T1, T2>
    {
        prop T1 Item1;
        prop T2 Item2;
        
    //    constructor
    //    [
    //    ]    
        
        constructor(T1 o1, T2 o2)
        {
            Item1 := o1;
            Item2 := o2
        }
    
        constructor(T1 o1)
        {
            Item1 := o1
        }
    
    //    getAt(int index)
    //    [
    //        index =>
    //            0 [ ^ field1 ];
    //            1 [ ^ field2 ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    setAt(int index, object object)
    //    [
    //        index =>
    //            0 [ field1 := T<type1>(object) ];
    //            1 [ field2 := T<type2>(object) ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    indexer = BaseIndexer::
    //    {
    //        @theLength := 2.
    //        
    //        @theIndex := 0.
    //        
    //        get
    //            = self getAt(theIndex).
    //            
    //        set : o
    //        [
    //            self setAt(theIndex, o).
    //        ]                        
    //    }.
    }
    
    public sealed Tuple<T1, T2, T3>
    {
        prop T1 Item1;
        prop T2 Item2;
        prop T3 Item3;
        
        constructor(T1 o1, T2 o2, T3 o3)
        {
            Item1 := o1;
            Item2 := o2;
            Item3 := o3
        }
    
    //    constructor(type1 o1, type2 o2)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //    ]
    //
    //    constructor(type1 o1)
    //    [
    //        field1 := o1.
    //    ]
    //
    //    getAt(int index)
    //    [
    //        index =>
    //            0 [ ^ field1 ];
    //            1 [ ^ field2 ];
    //            2 [ ^ field3 ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    setAt(int index, object object)
    //    [
    //        index =>
    //            0 [ field1 := object ];
    //            1 [ field2 := object ];
    //            2 [ field3 := object ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    indexer = BaseIndexer::
    //    {
    //        @theLength := 3.
    //        
    //        @theIndex := 0.
    //        
    //        get
    //            = self getAt(theIndex).
    //            
    //        set : o
    //        [
    //            self setAt(theIndex, o).
    //        ]                        
    //    }.
    }
    
    public sealed Tuple<T1, T2, T3, T4>
    {
        prop T1 Item1;
        prop T2 Item2;
        prop T4 Item3;
        prop T4 Item4;
        
        constructor(T1 o1, T2 o2, T3 o3, T4 o4)
        {
            Item1 := o1;
            Item2 := o2;
            Item3 := o3;
            Item4 := o4
        }
    
    //    constructor(type1 o1, type2 o2, type3 o3)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //        field3 := o3.
    //    ]
    //
    //    constructor(type1 o1, type2 o2)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //    ]
    //
    //    constructor(type1 o1)
    //    [
    //        field1 := o1.
    //    ]
    //
    //    getAt(int index)
    //    [
    //        index =>
    //            0 [ ^ field1 ];
    //            1 [ ^ field2 ];
    //            2 [ ^ field3 ];
    //            3 [ ^ field4 ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    setAt(int index, object object)
    //    [
    //        index =>
    //            0 [ field1 := object ];
    //            1 [ field2 := object ];
    //            2 [ field3 := object ];
    //            3 [ field4 := object ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    indexer = BaseIndexer::
    //    {
    //        @theLength := 4.
    //        
    //        @theIndex := 0.
    //        
    //        get
    //            = self getAt(theIndex).
    //            
    //        set : o
    //        [
    //            self setAt(theIndex, o).
    //        ]                        
    //    }.
    }
    
    public sealed Tuple<T1, T2, T3, T4, T5>
    {
        prop T1 Item1;
        prop T2 Item2;
        prop T3 Item3;
        prop T4 Item4;
        prop T5 Item5;
        
        constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5)
        {
            Item1 := o1;
            Item2 := o2;
            Item3 := o3;
            Item4 := o4;
            Item5 := o5
        }
    
    //    constructor(type1 o1, type2 o2, type3 o3, type4 o4)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //        field3 := o3.
    //        field4 := o4.
    //    ]
    //
    //    constructor(type1 o1, type2 o2, type3 o3)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //        field3 := o3.
    //    ]
    //
    //    constructor(type1 o1, type2 o2)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //    ]
    //
    //    constructor(type1 o1)
    //    [
    //        field1 := o1.
    //    ]
    //
    //    getAt(int index)
    //    [
    //        index =>
    //            0 [ ^ field1 ];
    //            1 [ ^ field2 ];
    //            2 [ ^ field3 ];
    //            3 [ ^ field4 ];
    //            4 [ ^ field5 ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    setAt(int index, object object)
    //    [
    //        index =>
    //            0 [ field1 := object ];
    //            1 [ field2 := object ];
    //            2 [ field3 := object ];
    //            3 [ field4 := object ];
    //            4 [ field5 := object ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    indexer = BaseIndexer::
    //    {
    //        @theLength := 5.
    //        
    //        @theIndex := 0.
    //        
    //        get
    //            = self getAt(theIndex).
    //            
    //        set : o
    //        [
    //            self setAt(theIndex, o).
    //        ]                        
    //    }.
    }
    
    public sealed Tuple<T1, T2, T3, T4, T5, T6>
    {
        prop T1 Item1;
        prop T2 Item2;
        prop T3 Item3;
        prop T4 Item4;
        prop T5 Item5;
        prop T6 Item6;
        
        constructor(T1 o1, T2 o2, T3 o3, T4 o4, T5 o5, T6 o6)
        {
            Item1 := o1;
            Item2 := o2;
            Item3 := o3;
            Item4 := o4;
            Item5 := o5;
            Item6 := o6
        }
    
    //    constructor(type1 o1, type2 o2, type3 o3, type4 o4, type5 o5)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //        field3 := o3.
    //        field4 := o4.
    //        field5 := o5.
    //    ]
    //
    //    constructor(type1 o1, type2 o2, type3 o3, type4 o4)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //        field3 := o3.
    //        field4 := o4.
    //    ]
    //
    //    constructor(type1 o1, type2 o2, type3 o3)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //        field3 := o3.
    //    ]
    //
    //    constructor(type1 o1, type2 o2)
    //    [
    //        field1 := o1.
    //        field2 := o2.
    //    ]
    //
    //    constructor(type1 o1)
    //    [
    //        field1 := o1.
    //    ]
    //
    //    getAt(int index)
    //    [
    //        index =>
    //            0 [ ^ field1 ];
    //            1 [ ^ field2 ];
    //            2 [ ^ field3 ];
    //            3 [ ^ field4 ];
    //            4 [ ^ field5 ];
    //            5 [ ^ field6 ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    setAt(int index, object object)
    //    [
    //        index =>
    //            0 [ field1 := object ];
    //            1 [ field2 := object ];
    //            2 [ field3 := object ];
    //            3 [ field4 := object ];
    //            4 [ field5 := object ];
    //            5 [ field6 := object ];
    //            ! [ OutOfRangeException new; raise ]
    //    ]
    //
    //    indexer = BaseIndexer::
    //    {
    //        @theLength := 6.
    //        
    //        @theIndex := 0.
    //        
    //        get
    //            = self getAt(theIndex).
    //            
    //        set : o
    //        [
    //            self setAt(theIndex, o).
    //        ]                        
    //    }.
    }

}