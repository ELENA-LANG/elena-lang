#import extensions.

#subject zeckendorf = ZeckendorfNumber.

const dig = ("00","01","10").
const dig1 = ("","1","10").

sealed dispatchable(zeckendorf) struct ZeckendorfNumber :: BaseNumber
{
    int dVal.
    int dLen.
    
    stacksafe implicit n &literal:s
    [
        int i := s length - 1.
        int q := 1.
        
        dLen := i / 2.
        dVal := 0.
        
        while (i >= 0)
        [
            dVal += (intConvertor convert &char:(s@i) - 48) * q.
            q *= 2. 
            
            i -= 1.
        ].
    ]
    
    stacksafe $readContent &vint:val &vint:len
    [
        val := dVal.
        len := dLen.
    ]
    
    sealed $a &int:n
    [
        int i := n.

        while (true)
        [
            if (dLen < i)
            [
                dLen := i.
            ].
            
            ((dVal shift:(i * 2) and:3)) =>
                0 [ ^ $self. ];
                1 [ ^ $self. ];
                2 [
                    ifnot (dVal shift:(i + 1 * 2) allMask:1)
                    [
                        ^ $self.
                    ].
                    
                    dVal += 1 shift:(-1 - i*2).
                    ^ $self.
                ];
                3 [
                    dVal := dVal and:(3 shift:(-1*i*2) inverted).
                    
                    $self $b &int:((i+1)*2).
                ].
            
            i += 1.
        ].
    ]
    
    $b &int:pos
    [
        if (pos == 0)
        [
            dVal += 1.
            $self $a &int:0.
            
            ^ $self.
        ].
        
        ifnot (dVal shift:pos allMask:1)
        [
            dVal := dVal + 1 shift:pos.
            
            $self $a &int:(pos / 2).
            if (pos > 1) [ $self $a &int:(pos / 2 - 1). ];
            [
                dVal := dVal and:(1 shift:(-1 * pos) inverted).
                
                $self $b &int:(pos + 1).
                $self $b &int:(pos - (pos > 1) iif:2:1).
            ].            
        ].
    ]
    
    constructor $sum &zeckendorf:n &zeckendorf:m
    [
        int mVal := 0.
        int mLen := 0.
        
        n $readContent &vint:dVal &vint:dLen.
        m $readContent &vint:mVal &vint:mLen.
        
        0 till:(mLen + 1 * 2) &doEach:GN
        [
            if (mVal shift:GN allMask:1)
            [
                $self $b &int:GN.
            ].
        ].
    ]
    
    literal
    [
        if (dVal == 0)
            [ ^ "0". ].
            
        literal s := dig1@(dVal shift:(dLen * 2) and:3).
        int i := dLen - 1.
        while (i >= 0)
        [
            s := s + dig@(dVal shift:(dLen * 2) and:3)
            
            i-=1.
        ].
        
        ^ s.
    ]
    
    add &zeckendorf:n
        = ZeckendorfNumber $sum &zeckendorf:$self &zeckendorf:n.
}

program =
[
    var n := 10n.
    
    console writeLine:(n + 10n).
].