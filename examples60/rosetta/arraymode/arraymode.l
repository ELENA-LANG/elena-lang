/// a program to find the mode value of a collection. 

import system'routines;
import system'collections;
import extensions;

// Averages/Mode

extension op
{
    get Mode()
    {
        var countMap := Dictionary.new(0);
        self.forEach::(item)
        {
            countMap[item] := countMap[item] + 1
        };
    
        countMap := countMap.Values.sort::(p,n => p > n);
    
        var max := countMap.FirstMember;
    
        ^ countMap
            .filterBy::(kv => max.equal(kv.Value))
            .selectBy::(kv => kv.Key)
            .toArray()
    }
}

public program()
{
   var array1 := new int[]{1, 1, 2, 4, 4};
   var array2 := new int[]{1, 3, 6, 6, 6, 6, 7, 7, 12, 12, 17};
   var array3 := new object[]{1, "blue", 2, 7.5r, 5, "green", "red", 5, 2, "blue", "white"};
    
   Console 
      .printLine("mode of (",array1.asEnumerable(),") is (",array1.Mode,")")
      .printLine("mode of (",array2.asEnumerable(),") is (",array2.Mode,")")
      .printLine("mode of (",array3.asEnumerable(),") is (",array3.Mode,")")
      .readChar()
}
