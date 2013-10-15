#define system.

#symbol program =
[
    #var anArray := (1, 2, 3).

    control for:(anArray Enumerator) &do: &&:it
        [ console write:"a[" write:(it Index) write:"]=" writeLine:(it get) ].
].