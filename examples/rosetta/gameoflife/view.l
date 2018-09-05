import cellular.

public extension<Space> presenterOp
{
    print
    [
        console setCursorPosition(0, 0).
        
        int columns := self columns.
        int rows := self rows.
        
        int i := 0.
        int j := 0.
        while (i < rows)
        [
            j := 0.
            
            while (j < columns)
            [
                int cell := self getAt(i, j).
                
                console write((cell == 0)iif(" ","o")).
                
                j := j + 1
            ].
            
            i := i + 1.
            console writeLine
        ].
    ]
}