/// a program to find the mode value of a collection. 

#import system'routines.
#import system'collections.
#import extensions.

// Averages/Mode

extension op
{
    mode
    [
        var aCountMap := Dictionary new &default:0.
        self run &each: anItem
        [
            aCountMap@anItem := aCountMap@anItem + 1.
        ].
    
        aCountMap := aCountMap array_list sort:(:p:n) [ p > n ].
    
        var aMax := aCountMap firstMember.
    
        ^ aCountMap
            filter &each:kv [ aMax safeEqual:kv ]
            select &each:kv [ kv key ]
            toArray.
    ]
}

program =
[
    var anArray1 := (1, 1, 2, 4, 4).
    var anArray2 := (1, 3, 6, 6, 6, 6, 7, 7, 12, 12, 17).
    var anArray3 := (1, "blue", 2, 7.5r, 5, "green", "red", 5, 2, "blue", "white").
    
    console 
        writeLine:"mode of (":anArray1:") is (":(anArray1 mode):")"
        writeLine:"mode of (":anArray2:") is (":(anArray2 mode):")"
        writeLine:"mode of (":anArray3:") is (":(anArray3 mode):")"
        readChar.
].
