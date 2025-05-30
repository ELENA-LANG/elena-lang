import system'dynamic;
import extensions;
import system'routines;
import system'collections;
 
extension algorithmOp
{
    s_of_n()
    {
        var counter := new Integer();
        var n := self;
 
        ^ new ArrayList().mixInto(new
        {
            eval(i)
            {
                counter.append(1);

                if (weak self.Length < n)
                { 
                    weak self.append(i) 
                }
                else
                { 
                    if(randomGenerator.nextInt(counter) < n)
                        { weak self[randomGenerator.nextInt(n)] := i }
                };

                ^ weak self.Value
            }
        })
    }
}
 
public program()
{
    var bin := Array.allocate(10).populate::(n => new Integer());
    for(int trial := 0; trial < 10000; trial += 1)
    {
        var s_of_n := 3.s_of_n();
 
        for(int n := 0; n < 10; n += 1)
        {
            var sample := s_of_n.eval(n);
 
            if (n == 9)
                { sample.forEach::(i){ bin[i].append(1) } }
        }
    };    
 
    Console.printLine(bin).readChar()
}