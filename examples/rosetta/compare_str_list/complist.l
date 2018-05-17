// Given a list of arbitrarily many strings, show how to:
//   - test if they are all lexically equal
//   - test if every string is lexically less than the one after it (i.e. whether the list is in strict ascending order) 

import system'collections.
import system'routines.
import extensions.

extension helper
{
    isEqual
        = nil == self seekFor(self firstMember) eachPair(:n:m)( m equal:n; inverted ).
        
    isAscending
    [
        var former := self enumerator.
        var later := self enumerator.
        
        later next.
        
        ^ nil == former zip:later by(:prev:next)( next <= prev ); seekEach(:b)( b )
    ]
}

public program =
[
    (("AA","BB","CC"),("AA","AA","AA"),("AA","CC","BB"),("AA","ACB","BB","CC"),List new; append:"single_element")
        forEach(:list)
        [
            console printLine(list," all equal - ",list isEqual).
            console printLine(list," ascending - ",list isAscending)
        ].
        
    console readChar.            
].