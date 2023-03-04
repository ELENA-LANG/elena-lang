import extensions;
//import extensions'routines;
//import system'routines;
import system'collections;
import system'routines;

extension op
{
    get Mode()
    {
        var countMap := Dictionary.new(0);
        self.forEach:(item)
        {
            countMap[item] := countMap[item] + 1
        };
    
        countMap := countMap.Values.sort:(p,n => p > n);
    
        var max := countMap.FirstMember;
    
        ^ countMap
            .filterBy:(kv => max.equal(kv.Value))
            .selectBy:(kv => kv.Key)
            .toArray()
    }
}

public program()
{
   var array3 := new object[]{3, 1/*, "blue", 2, 7.5r, 5, "green", "red", 5, 2, "blue", "white"*/};
    
   console 
      .printLine("mode of (",array3.asEnumerable(),") is (",array3.Mode,")")
}


//public program()
//{
//    var o := nil;
//    o.call()

//   var o := new Object();
//   o.fail()
//}