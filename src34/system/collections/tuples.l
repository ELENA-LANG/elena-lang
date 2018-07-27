public sealed Tuple<type1>
{
    T<type1> prop item1 :: field1.

    getAt(int index)
    [
        index =>
            0 [ ^ field1 ];
            ! [ OutOfRangeException new; raise ]
    ]

    setAt(int index, type1 object)
    [
        index =>
            0 [ field1 := object ];
            ! [ OutOfRangeException new; raise ]
    ]

    indexer = BaseIndexer::
    {
        @theLength := 1.
        
        @theIndex := 0.
        
        get
            = self getAt(theIndex).
            
        set : o
        [
            self setAt(theIndex, T<type1>(o)).
        ]                        
    }.
            
    cast eval(type1 o1)
    [
        field1 := o1.
    ]
}

public sealed Tuple<type1, type2>
{
    T<type1> prop item1 :: field1.
    T<type2> prop item2 :: field2.
    
    cast eval(type1 o1, type2 o2)
    [
        field1 := o1.
        field2 := o2.
    ]

    cast eval(type1 o1)
    [
        field1 := o1.
    ]

    getAt(int index)
    [
        index =>
            0 [ ^ field1 ];
            1 [ ^ field2 ];
            ! [ OutOfRangeException new; raise ]
    ]

    setAt(int index, object object)
    [
        index =>
            0 [ field1 := T<type1>(object) ];
            1 [ field2 := T<type2>(object) ];
            ! [ OutOfRangeException new; raise ]
    ]

    indexer = BaseIndexer::
    {
        @theLength := 2.
        
        @theIndex := 0.
        
        get
            = self getAt(theIndex).
            
        set : o
        [
            self setAt(theIndex, o).
        ]                        
    }.
}

public sealed Tuple<type1, type2, type3>
{
    T<type1> prop item1 :: field1.
    T<type2> prop item2 :: field2.
    T<type3> prop item3 :: field3.
    
    cast eval(type1 o1, type2 o2, type3 o3)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
    ]

    cast eval(type1 o1, type2 o2)
    [
        field1 := o1.
        field2 := o2.
    ]

    cast eval(type1 o1)
    [
        field1 := o1.
    ]

    getAt(int index)
    [
        index =>
            0 [ ^ field1 ];
            1 [ ^ field2 ];
            2 [ ^ field3 ];
            ! [ OutOfRangeException new; raise ]
    ]

    setAt(int index, object object)
    [
        index =>
            0 [ field1 := object ];
            1 [ field2 := object ];
            2 [ field3 := object ];
            ! [ OutOfRangeException new; raise ]
    ]

    indexer = BaseIndexer::
    {
        @theLength := 3.
        
        @theIndex := 0.
        
        get
            = self getAt(theIndex).
            
        set : o
        [
            self setAt(theIndex, o).
        ]                        
    }.
}

public sealed Tuple<type1, type2, type3, type4>
{
    T<type1> prop item1 :: field1.
    T<type2> prop item2 :: field2.
    T<type3> prop item3 :: field3.
    T<type4> prop item4 :: field4.
    
    cast eval(type1 o1, type2 o2, type3 o3, type4 o4)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
        field4 := o4.
    ]

    cast eval(type1 o1, type2 o2, type3 o3)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
    ]

    cast eval(type1 o1, type2 o2)
    [
        field1 := o1.
        field2 := o2.
    ]

    cast eval(type1 o1)
    [
        field1 := o1.
    ]

    getAt(int index)
    [
        index =>
            0 [ ^ field1 ];
            1 [ ^ field2 ];
            2 [ ^ field3 ];
            3 [ ^ field4 ];
            ! [ OutOfRangeException new; raise ]
    ]

    setAt(int index, object object)
    [
        index =>
            0 [ field1 := object ];
            1 [ field2 := object ];
            2 [ field3 := object ];
            3 [ field4 := object ];
            ! [ OutOfRangeException new; raise ]
    ]

    indexer = BaseIndexer::
    {
        @theLength := 4.
        
        @theIndex := 0.
        
        get
            = self getAt(theIndex).
            
        set : o
        [
            self setAt(theIndex, o).
        ]                        
    }.
}

public sealed Tuple<type1, type2, type3, type4, type5>
{
    T<type1> prop item1 :: field1.
    T<type2> prop item2 :: field2.
    T<type3> prop item3 :: field3.
    T<type4> prop item4 :: field4.
    T<type5> prop item5 :: field5.
    
    cast eval(type1 o1, type2 o2, type3 o3, type4 o4, type5 o5)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
        field4 := o4.
        field5 := o5.
    ]

    cast eval(type1 o1, type2 o2, type3 o3, type4 o4)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
        field4 := o4.
    ]

    cast eval(type1 o1, type2 o2, type3 o3)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
    ]

    cast eval(type1 o1, type2 o2)
    [
        field1 := o1.
        field2 := o2.
    ]

    cast eval(type1 o1)
    [
        field1 := o1.
    ]

    getAt(int index)
    [
        index =>
            0 [ ^ field1 ];
            1 [ ^ field2 ];
            2 [ ^ field3 ];
            3 [ ^ field4 ];
            4 [ ^ field5 ];
            ! [ OutOfRangeException new; raise ]
    ]

    setAt(int index, object object)
    [
        index =>
            0 [ field1 := object ];
            1 [ field2 := object ];
            2 [ field3 := object ];
            3 [ field4 := object ];
            4 [ field5 := object ];
            ! [ OutOfRangeException new; raise ]
    ]

    indexer = BaseIndexer::
    {
        @theLength := 5.
        
        @theIndex := 0.
        
        get
            = self getAt(theIndex).
            
        set : o
        [
            self setAt(theIndex, o).
        ]                        
    }.
}

public sealed Tuple<type1, type2, type3, type4, type5, type6>
{
    T<type1> prop item1 :: field1.
    T<type2> prop item2 :: field2.
    T<type3> prop item3 :: field3.
    T<type4> prop item4 :: field4.
    T<type5> prop item5 :: field5.
    T<type6> prop item6 :: field6.
    
    cast eval(type1 o1, type2 o2, type3 o3, type4 o4, type5 o5, type6 o6)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
        field4 := o4.
        field5 := o5.
        field6 := o6.
    ]

    cast eval(type1 o1, type2 o2, type3 o3, type4 o4, type5 o5)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
        field4 := o4.
        field5 := o5.
    ]

    cast eval(type1 o1, type2 o2, type3 o3, type4 o4)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
        field4 := o4.
    ]

    cast eval(type1 o1, type2 o2, type3 o3)
    [
        field1 := o1.
        field2 := o2.
        field3 := o3.
    ]

    cast eval(type1 o1, type2 o2)
    [
        field1 := o1.
        field2 := o2.
    ]

    cast eval(type1 o1)
    [
        field1 := o1.
    ]

    getAt(int index)
    [
        index =>
            0 [ ^ field1 ];
            1 [ ^ field2 ];
            2 [ ^ field3 ];
            3 [ ^ field4 ];
            4 [ ^ field5 ];
            5 [ ^ field6 ];
            ! [ OutOfRangeException new; raise ]
    ]

    setAt(int index, object object)
    [
        index =>
            0 [ field1 := object ];
            1 [ field2 := object ];
            2 [ field3 := object ];
            3 [ field4 := object ];
            4 [ field5 := object ];
            5 [ field6 := object ];
            ! [ OutOfRangeException new; raise ]
    ]

    indexer = BaseIndexer::
    {
        @theLength := 6.
        
        @theIndex := 0.
        
        get
            = self getAt(theIndex).
            
        set : o
        [
            self setAt(theIndex, o).
        ]                        
    }.
}
