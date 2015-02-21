#define system.
#define system'routines.
#define system'collections.
#define extensions.

// Averages/Mode

#symbol mode = (:anArray)
[
    #var aCountMap := Dictionary new &default:0.
    control run:anArray &forEach: anItem
    [
        aCountMap set &key:anItem &object:(aCountMap getAt &key:anItem + 1).
    ].

    aCountMap~indexable sort:(:p:n)
        [ p > n ].

    #var aMax := aCountMap first.

    ^ convertor toArray:(
        control run:(
                    aCountMap~enumerable 
                        filter &each:kv [ aMax safeEqual:kv ] 
                        select &each:kv [ kv key ]
                    ) 
                    &forEach:(Summing new:(List new))).
].

#symbol program =
[
    #var anArray1 := (1, 1, 2, 4, 4).
    #var anArray2 := (1, 3, 6, 6, 6, 6, 7, 7, 12, 12, 17).
    #var anArray3 := (1, "blue", 2, 7.5r, 5, "green", "red", 5, 2, "blue", "white").
    
    #var aMode1 := mode:anArray1.
    #var aMode2 := mode:anArray2.
    #var aMode3 := mode:anArray3.
    
    consoleEx writeLine:"mode of (":anArray1:") is (":aMode1:")".
    consoleEx writeLine:"mode of (":anArray2:") is (":aMode2:")".
    consoleEx writeLine:"mode of (":anArray3:") is (":aMode3:")".
].
