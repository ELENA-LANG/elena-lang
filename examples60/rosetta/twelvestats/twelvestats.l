import system'routines;
import extensions;
import extensions'text;

extension op
{
    printSolution(bits)
        = self.zipBy(bits,
            (s,b => s.iif("T","F") + (s.xor(b)).iif("* ","  "))).summarize(new StringWriter());

    toBit()
        = self.iif(1,0);
}

puzzle = new Func1[]
{
    (bits => bits.Length == 12),
    
    (bits => bits.last(6).selectBy::(x => x.toBit()).summarize() == 3 ),
    
    (bits => bits.zipBy(new Range(1, 12), 
                        (x,i => (i.toInt().isEven()).and(x).toBit())).summarize() == 2 ),
    
    (bits => bits[4].iif(bits[5] && bits[6],true) ),
    
    (bits => ((bits[1] || bits[2]) || bits[3]).Inverted ),
    
    (bits => bits.zipBy(new Range(1, 12), 
                        (x,i => (i.toInt().isOdd()).and(x).toBit() )).summarize() == 4 ),
    
    (bits => bits[1].xor(bits[2]) ),
    
    (bits => bits[6].iif(bits[5] && bits[4],true) ),
    
    (bits => bits.top(6).selectBy::(x => x.toBit() ).summarize() == 3 ),
    
    (bits => bits[10] && bits[11] ),
    
    (bits => (bits[6].toBit() + bits[7].toBit() + bits[8].toBit())==1 ),
    
    (bits => bits.top(11).selectBy::(x => x.toBit()).summarize() == 4 )
};

public program()
{
    Console.writeLine();
    
    for(int n := 0; n < 2.power(12); n += 1)
    {
        var bits := BitArray32.load(n).top(12).toArray();
        var results := puzzle.selectBy::(r => r(bits)).toArray();
        
        var counts := bits.zipBy(results, (b,r => b.xor(r).toBit() )).summarize();

        counts =>
            0 : { Console.printLine("Total hit :",results.printSolution(bits)) }
            1 : { Console.printLine("Near miss :",results.printSolution(bits)) }
            12: { Console.printLine("Total miss:",results.printSolution(bits)) };
    };
    
    Console.readChar()
}