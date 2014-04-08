#define system.

#symbol program =
[
    #var anArray := (1, 2, 3).

    control from:0 &till:(anArray length) &do: i
        [ console write:"a[" write:i write:"]=" writeLine:(anArray@i) ].
].