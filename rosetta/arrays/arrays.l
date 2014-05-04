#define system.

#symbol program =
[
    #var anArray := (1, 2, 3).

    control forrange &int:0 &int:(anArray length - 1) &do: (&int:i)
        [ console write:"a[" write:i write:"]=" writeLine:(anArray@i) ].
].