namespace collections
{
    public sealed Tuple<T1>
    {
        constructor()
        {
        }    
        
        constructor(T1 o1)
        {
            Item1 := o1
        }
        
        prop T1 Item1;
    
        at(int index)
        {
            index =>
                0 { ^ Item1 }
                : { OutOfRangeException.raise() }
        }
    
        setAt(int index, object object)
        {
            index =>
                0 { Item1 := object }
                : { OutOfRangeException.raise() }
        }
    
        indexer = new BaseIndexer
        {
            this theLength := 1;
            
            this theIndex := 0;
            
            get Value()
                = self.at(theIndex);
                
            set Value(o)
            {
                self.setAt(theIndex, o)
            }
        };
    }
    
    public sealed Tuple<T1, T2>
    {
        prop T1 Item1;
        prop T2 Item2;
        
        constructor()
        {
        }    
        
        constructor(T1 o1, T2 o2)
        {
            Item1 := o1;
            Item2 := o2
        }
    
        constructor(T1 o1)
        {
            Item1 := o1
        }
    
        at(int index)
        {
            index =>
                0 { ^ Item1 }
                1 { ^ Item2 }
                : { OutOfRangeException.raise() }
        }
    
        setAt(int index, object object)
        {
            index =>
                0 { Item1 := object }
                1 { Item2 := object }
                : { OutOfRangeException.raise() }
        }
    
        indexer = new BaseIndexer
        {
            this theLength := 2;
            
            this theIndex := 0;
            
            get Value()
                = self.at(theIndex);
                
            set Value(o)
            {
                self.setAt(theIndex, o)
            }                        
        };
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
    
        at(int index)
        {
            index =>
                0 { ^ Item1 }
                1 { ^ Item2 }
                2 { ^ Item3 }
                : { OutOfRangeException.raise() }
        }
    
        setAt(int index, object object)
        {
            index =>
                0 { Item1 := object }
                1 { Item2 := object }
                2 { Item3 := object }
                : { OutOfRangeException.raise() }
        }
    
        indexer = new BaseIndexer
        {
            this theLength := 3;
            
            this theIndex := 0;
            
            get Value()
                = self.at(theIndex);
                
            set Value(o)
            {
                self.setAt(theIndex, o)
            }                        
        };
    }
    
    public sealed Tuple<T1, T2, T3, T4>
    {
        prop T1 Item1;
        prop T2 Item2;
        prop T3 Item3;
        prop T4 Item4;
        
        constructor(T1 o1, T2 o2, T3 o3, T4 o4)
        {
            Item1 := o1;
            Item2 := o2;
            Item3 := o3;
            Item4 := o4
        }
    
        at(int index)
        {
            index =>
                0 { ^ Item1 }
                1 { ^ Item2 }
                2 { ^ Item3 }
                3 { ^ Item4 }
                : { OutOfRangeException.raise() }
        }
    
        setAt(int index, object object)
        {
            index =>
                0 { Item1 := object }
                1 { Item2 := object }
                2 { Item3 := object }
                3 { Item4 := object }
                : { OutOfRangeException.raise() }
        }
    
        indexer = new BaseIndexer
        {
            this theLength := 4;
            
            this theIndex := 0;
            
            get Value()
                = self.at(theIndex);
                
            set Value(o)
            {
                self.setAt(theIndex, o)
            }                        
        };
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
    
        at(int index)
        {
            index =>
                0 { ^ Item1 }
                1 { ^ Item2 }
                2 { ^ Item3 }
                3 { ^ Item4 }
                4 { ^ Item5 }
                : { OutOfRangeException.raise() }
        }
    
        setAt(int index, object object)
        {
            index =>
                0 { Item1 := object }
                1 { Item2 := object }
                2 { Item3 := object }
                3 { Item4 := object }
                4 { Item5 := object }
                : { OutOfRangeException.raise() }
        }
    
        indexer = new BaseIndexer
        {
            this theLength := 5;
            
            this theIndex := 0;
            
            get Value()
                = self.at(theIndex);
                
            set Value(o)
            {
                self.setAt(theIndex, o)
            }                        
        };
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

        at(int index)
        {
            index =>
                0 { ^ Item1 }
                1 { ^ Item2 }
                2 { ^ Item3 }
                3 { ^ Item4 }
                4 { ^ Item5 }
                5 { ^ Item6 }
                : { OutOfRangeException.raise() }
        }
    
        setAt(int index, object object)
        {
            index =>
                0 { Item1 := object }
                1 { Item2 := object }
                2 { Item3 := object }
                3 { Item4 := object }
                4 { Item5 := object }
                5 { Item6 := object }
                : { OutOfRangeException.raise() }
        }
    
        indexer = new BaseIndexer
        {
            this theLength := 6;
            
            this theIndex := 0;
            
            get Value()
                = self.at(theIndex);
                
            set Value(o)
            {
                self.setAt(theIndex, o)
            }                        
        };
    }
}
