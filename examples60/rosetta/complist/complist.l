import system'collections;
import system'routines;
import extensions;

extension helper
{
    isEqual()
        = nil == self.seekEach(self.FirstMember, (n,m => m != n));
        
    isAscending()
    {
        var former := self.enumerator();
        var later := self.enumerator();
        
        later.next();
        
        ^ nil == former.zipBy(later, (prev,next => next <= prev )).seekEach::(b => b)
    }
}

testCases
    = new string[][]{
         new string[]{"AA","BB","CC"},
         new string[]{"AA","AA","AA"},
         new string[]{"AA","CC","BB"},
         new string[]{"AA","ACB","BB","CC"},
         new string[]{"single_element"}};

public program()
{
    testCases.forEach::(list)
        {
            Console.printLine(list.asEnumerable()," all equal - ",list.isEqual());
            Console.printLine(list.asEnumerable()," ascending - ",list.isAscending())
        };
        
    Console.readChar()
}