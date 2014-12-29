#define system.

#symbol program =
[
    #var anArray := (1, 2, 3).

    control run &int:0 &int:(anArray length - 1) &forEvery: (&index:i)
        [ console write:"a[" write:i write:"]=" writeLine:(anArray@i) ].
].