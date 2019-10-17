// Given a list of arbitrarily many strings, show how to:
//   - test if they are all lexically equal
//   - test if every string is lexically less than the one after it (i.e. whether the list is in strict ascending order) 

import system'collections;
import system'routines;
import extensions;

extension helper
{
    isEqual()
        = nil == self.seekEach(self.FirstMember, (n,m => m.equal:n.Inverted ));
        
    isAscending()
    {
        var former := self.enumerator();
        var later := self.enumerator();
        
        later.next();
        
        ^ nil == former.zipBy(later, (prev,next => next <= prev )).seekEach:(b => b)
    }
}

testCases
    = new string[][]::(
         new string[]::("AA","BB","CC"),
         new string[]::("AA","AA","AA"),
         new string[]::("AA","CC","BB"),
         new string[]::("AA","ACB","BB","CC"),
         new string[]::("single_element"));

public program()
{
    testCases.forEach:(list)
        {
            console.printLine(list.asEnumerable()," all equal - ",list.isEqual());
            console.printLine(list.asEnumerable()," ascending - ",list.isAscending())
        };
        
    console.readChar()
}