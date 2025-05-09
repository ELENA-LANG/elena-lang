import system'routines;
import system'math;
import extensions;
 
extension op 
{
    get Median()
    {
        var sorted := self.ascendant();

        var len := sorted.Length;
        if (len == 0)
        { 
            ^ nil 
        }
        else
        {
            var middleIndex := len / 2;
            if (len.mod(2) == 0)
            { 
                ^ (sorted[middleIndex - 1] + sorted[middleIndex]) / 2 
            }
            else
            { 
                ^ sorted[middleIndex] 
            }
        }
    }
}    

public program()
{
    var a1 := new real[]{4.1r, 5.6r, 7.2r, 1.7r, 9.3r, 4.4r, 3.2r};
    var a2 := new real[]{4.1r, 7.2r, 1.7r, 9.3r, 4.4r, 3.2r};
    
    Console.printLine("median of (",a1.asEnumerable(),") is ",a1.Median);
    Console.printLine("median of (",a2.asEnumerable(),") is ",a2.Median);
    
    Console.readChar()
}