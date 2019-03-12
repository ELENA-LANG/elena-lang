import cellular;

public extension presenterOp : Space
{
    print()
    {
        console.setCursorPosition(0, 0);
        
        int columns := self.Columns;
        int rows := self.Rows;
        
        for(int i := 0, i < rows, i += 1)
        {
            for(int j := 0, j < columns, j += 1)
            {
                int cell := self.at(i, j);
                
                console.write((cell == 0).iif(" ","o"));
            };
            
            console.writeLine()
        }
    }
}