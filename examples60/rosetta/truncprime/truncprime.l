import extensions;

const MAXN = 1000000;

extension mathOp
{
    isPrime()
    {
        int n := cast int(self);
        
        if (n < 2) { ^ false };        
        if (n < 4) { ^ true };        
        if (n.mod(2) == 0) { ^ false };
        if (n < 9) { ^ true }; 
        if (n.mod(3) == 0) { ^ false };
        
        int r := n.sqrt();
        int f := 5;
        while (f <= r)
        {
            if ((n.mod(f) == 0) || (n.mod(f + 2) == 0))
                { ^ false };
                
            f := f + 6            
        };
        
        ^ true
    }
    
    isRightTruncatable()
    {
        int n := self;
        
        while (n != 0)
        {
            if:not (n.isPrime())
                { ^ false };
            
            n := n / 10
        };
        
        ^ true
    }

    isLeftTruncatable()
    {
        int n := self;
        int tens := 1;
        
        while (tens < n)
            { tens := tens * 10 };
            
        while (n != 0)
        {
            if:not (n.isPrime())
                { ^ false };

            tens := tens / 10;
            n := n - (n / tens * tens)
        };
        
        ^ true
    }
}

public program()
{
    var n := MAXN;
    var max_lt := 0;
    var max_rt := 0;

    while (max_lt == 0 || max_rt == 0)
    {
        if(n.toString().indexOf("0") == -1)
        {
            if ((max_lt == 0) && (n.isLeftTruncatable()))
                {
                    max_lt := n
                };
                
            if ((max_rt == 0) && (n.isRightTruncatable()))
                {
                    max_rt := n
                }
        };
                        
        n := n - 1
    };

    Console.printLine("Largest truncable left is ",max_lt);   
    Console.printLine("Largest truncable right is ",max_rt);
    
    Console.readChar()
}
