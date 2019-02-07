// based on msdn random sample

import extensions;

// --- program ---

const int rows = 4;
const int cols = 6;

const int runCount = 1000000;
const int distGroupCount = 20;

public program()
{
    auto intGroupSize := (1.0r + IntNumber.MaxValue) / distGroupCount;

    var intCounts := new int[](distGroupCount);
    var realCounts := new int[](distGroupCount);

    console.printLine:"Random reals:";
    for(int i := 0, i < rows, i += 1)
    {
        for(int j := 0, j <= cols, j += 1)
        {
            console.printPaddingRight(12, randomGenerator.nextReal());
        };
        
        console.writeLine()
    };

    console.writeLine();
    console.writeLine:"Random integers:";
    for(int i := 0, i < rows, i += 1)
    {
        for(int j := 0, j <= cols, j += 1)
        {
            console.printPaddingRight(12, randomGenerator.nextInt());
        };
        
        console.writeLine()
    };

    console.writeLine();
    console.writeLine:"Distribution";

    for(int i := 0, i < runCount, i += 1)
    {
        int index := (randomGenerator.nextInt() / intGroupSize).toInt();
        if(index < distGroupCount)
            { intCounts[index] := intCounts[index] + 1 };
        
        index := (randomGenerator.nextReal() * distGroupCount).toInt();
        if(index < distGroupCount)
            { realCounts[index] := realCounts[index] + 1 }
    };    

    for(int i := 0, i < distGroupCount, i += 1)
    {
        console.printPaddingLeft(12, (i * intGroupSize).toInt(),"-");
        console.printPaddingLeft(12, ((i + 1) * intGroupSize - 1).toInt());
        console.printPaddingLeft(12, intCounts[i]);

        console.printPaddingLeft(14, i.toReal() / distGroupCount,"-");
        console.printPaddingLeft(12, (i + 1).toReal() / distGroupCount);
        console.printPaddingLeft(12, realCounts[i]);
                
        console.writeLine()
    }
}
