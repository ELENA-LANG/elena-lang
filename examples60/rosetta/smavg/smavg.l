import system'routines;
import system'collections;
import extensions;

class SMA
{
    object thePeriod;
    object theList;
    
    constructor new(period)
    {
        thePeriod := period;
        theList :=new List();
    }
    
    append(n)
    {
        theList.append(n);

        var count := theList.Length;
        count =>
            0 : { ^0.0r }
            ! : {
                if (count > thePeriod)
                {
                    theList.removeAt(0);
                    
                    count := thePeriod
                };
        
                var sum := theList.summarize(Real.new());
                
                ^ sum / count
            }
    }
}

// --- Program ---

public program()
{
    var SMA3 := SMA.new(3);
    var SMA5 := SMA.new(5);

    for (int i := 1; i <= 5; i += 1) {
        Console.printPaddingRight(30, "sma3 + ", i, " = ", SMA3.append(i));
        Console.printLine("sma5 + ", i, " = ", SMA5.append(i))
    };

    for (int i := 5; i >= 1; i -= 1) {
        Console.printPaddingRight(30, "sma3 + ", i, " = ", SMA3.append(i));
        Console.printLine("sma5 + ", i, " = ", SMA5.append(i))
    };
    
    Console.readChar()
}
