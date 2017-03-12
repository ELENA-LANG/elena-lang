#import cellular.
#import system'text.

extension(space) presenterOp
{
    print
    [
        console setCursorPosition &int:0 &int:0.
        
        int columns := self columns.
        int rows := self rows.
        
        int i := 0.
        int j := 0.
        while (i < rows)
        [
            j := 0.
            
            while (j < columns)
            [
                int cell := self getAt &int:i &int:j.
                
                console write:((cell == 0)iif:" ":"o").
                
                j += 1.
            ].
            
            i += 1.
            console writeLine.
        ].
    ]
}