import system'routines;

// --- RandomGenerator ---

sealed struct RandomGenerator
{
    long theSeed;
    
    constructor()
    {
        extern elenart.core_rnd_init(ref theSeed);
    }

    eval(int maxValue, ref int retVal)
    {
        extern elenart.core_rnd_next(theSeed, maxValue, ref retVal)
    }

    eval(ref int retVal)
    {
        extern elenart.core_rnd_nextint(theSeed, ref retVal)
    }

    eval(minValue, maxValue)
        = minValue + self.eval(maxValue - minValue);

    int eval(int maxValue)
    {
        int nextValue := 0;
        self.eval(maxValue, ref nextValue);
        
        ^ new IntNumber(nextValue)
    }
    
    int nextInt()
    {
        int nextValue := 0;
        self.eval(ref nextValue);
        
        ^ new IntNumber(nextValue)
    }

    int nextInt(val)
        = self.eval(cast int(val));
        
    real nextReal()
    {
        int next := 0;
        self.eval(1000000, ref next);
        
        real retVal := realConvertor.convert(next);
        
        retVal := retVal / 1000000.0r;
        
        ^ new RealNumber(retVal)
    }
}

// --- randomGenerator ---

public static randomGenerator = new RandomGenerator();

// -- ERandomizer --

public extension randomOp
{
    randomize(count)
    {
        var list := self.clone();
        
        int n := cast int(count);
        int i := 0;
        
        int max := self.Length;
        
        while (i < n)
        {
            var j := randomGenerator.eval(max);
            
            list.exchange(i, j);
            
            i := i + 1
        };
        
        ^ list
    }
}
