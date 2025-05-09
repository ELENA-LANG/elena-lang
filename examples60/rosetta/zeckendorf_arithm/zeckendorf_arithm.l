import extensions;

const dig = new string[]{"00","01","10"};
const dig1 = new string[]{"","1","10"};

sealed struct ZeckendorfNumber
{
    int dVal;
    int dLen;
    
    clone()
        = ZeckendorfNumber.newInternal(dVal,dLen);
    
    cast n(string s)
    {
        int i := s.Length - 1;
        int q := 1;
        
        dLen := i / 2;
        dVal := 0;
        
        while (i >= 0)
        {
            dVal += ((IntConvertor.convert(s[i]) - 48) * q);
            q *= 2;
            
            i -= 1
        }
    }
    
    internal readContent(ref int val, ref int len)
    {
        val := dVal;
        len := dLen;
    }
    
    private a(int n)
    {
        int i := n;

        while (true)
        {
            if (dLen < i)
            {
                dLen := i
            };
            
            int v := (dVal $shr (i * 2)) & 3;
            v =>    
                0 : { ^ self }
                1 : { ^ self }
                2 : {
                    if:not ((dVal $shr ((i + 1) * 2)).allMask(1))
                    {
                        ^ self
                    };
                    
                    dVal += (1 $shl (i*2 + 1));
                    
                    ^ self
                }
                3 : {
                    int tmp := 3 $shl (i * 2);
                    tmp := tmp.bxor(-1);
                    dVal := dVal & tmp;
                    
                    self.b((i+1)*2)
                };
            
            i += 1
        }
    }
    
    inc()
    {
        dVal += 1;
        self.a(0)
    }
    
    private b(int pos)
    {
        if (pos == 0) { ^ self.inc() };
        
        if:not((dVal $shr pos).allMask(1))
        {
            dVal += (1 $shl pos);
            self.a(pos / 2);
            if (pos > 1) { self.a((pos / 2) - 1) }
        }
        else
        {
            dVal := dVal & (1 $shl pos).BInverted;
            self.b(pos + 1);
            int arg := pos - ((pos > 1) ? 2 : 1);
            self.b(/*pos - ((pos > 1) ? 2 : 1)*/arg)
        }
    }

    private c(int pos)
    {
        if ((dVal $shr pos).allMask(1))
        {
            int tmp := 1 $shl pos;
            tmp := tmp.bxor(-1);
            
            dVal := dVal & tmp;
            
            ^ self
        };
                                        
        self.c(pos + 1);
        
        if (pos > 0)
        {
            self.b(pos - 1)
        }
        else
        {
            self.inc()
        }            
    }
            
    internal constructor sum(ZeckendorfNumber n, ZeckendorfNumber m)
    {
        int mVal := 0;
        int mLen := 0;
        
        n.readContent(ref int v, ref int l);        
        m.readContent(ref mVal, ref mLen);
        
        dVal := v;
        dLen := l;

        for(int GN := 0; GN < (mLen + 1) * 2; GN += 1)
        {
            if ((mVal $shr GN).allMask(1))
            {
                self.b(GN)
            }
        }
    }
    
    internal constructor difference(ZeckendorfNumber n, ZeckendorfNumber m)
    {
        int mVal := 0;
        int mLen := 0;
        
        n.readContent(ref int v, ref int l);        
        m.readContent(ref mVal, ref mLen);
        
        dVal := v;
        dLen := l;
        
        for(int GN := 0; GN < (mLen + 1) * 2; GN += 1) 
        {
            if ((mVal $shr GN).allMask(1))
            {
                self.c(GN)
            }
        };
        
        while (((dVal $shr (dLen*2)) & 3) == 0 || dLen == 0)
        {
            dLen -= 1
        }
    }
    
    internal constructor product(ZeckendorfNumber n, ZeckendorfNumber m)
    {
        n.readContent(ref int v, ref int l);              
        
        dVal := v;
        dLen := l;
        
        ZeckendorfNumber Na := m;
        ZeckendorfNumber Nb := m;
        ZeckendorfNumber Nr := 0n;
        ZeckendorfNumber Nt := 0n;
        
        for(int i := 0; i < (dLen + 1) * 2; i += 1) 
        {
            if (((dVal $shr i) & 1) > 0)
            {
                Nr += Nb
            };
            Nt := Nb;
            Nb += Na;
            Na := Nt
        };
        
        Nr.readContent(ref v, ref l);
        
        dVal := v;
        dLen := l;
    }
    
    internal constructor newInternal(int v, int l)
    {
        dVal := v;
        dLen := l
    }
    
    string toPrintable()
    {
        if (dVal == 0)
            { ^ "0" };
            
        string s := dig1[(dVal $shr (dLen * 2)) & 3];
        int i := dLen - 1;
        while (i >= 0)
        {
            s := s + dig[(dVal $shr (i * 2)) & 3];
            
            i-=1
        };
        
        ^ s
    }
    
    add(ZeckendorfNumber n)
        = ZeckendorfNumber.sum(self, n);
        
    subtract(ZeckendorfNumber n)
        = ZeckendorfNumber.difference(self, n);
        
    multiply(ZeckendorfNumber n)
        = ZeckendorfNumber.product(self, n);
}

public program()
{
    Console.printLine("Addition:");
    var n := 10n;
    
    n += 10n;
    Console.printLine(n);
    n += 10n;
    Console.printLine(n);
    n += 1001n;
    Console.printLine(n);
    n += 1000n;
    Console.printLine(n);
    n += 10101n;
    Console.printLine(n);
    
    Console.printLine("Subtraction:");
    n := 1000n;
    n -= 101n;
    Console.printLine(n);
    n := 10101010n;
    n -= 1010101n;
    Console.printLine(n);
    
    Console.printLine("Multiplication:");
    n := 1001n;
    n *= 101n;
    Console.printLine(n);
    n := 101010n;
    n += 101n;
    Console.printLine(n)
}
