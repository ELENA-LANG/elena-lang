// based on msdn random sample

#import system.
#import extensions.

// --- program ---

#symbol(const,type:int)rows = 4.
#symbol(const,type:int)cols = 6.

#symbol(const,type:int)runCount = 1000000.
#symbol(const,type:int)distGroupCount = 10.

#symbol program =
[
//        const int distGroupCount = 10;
//        const double intGroupSize = 
//            ( (double)int.MaxValue + 1.0 ) / (double)distGroupCount;

    #var intGroupSize := (1.0r + IntNumber max) / distGroupCount.

    #var intCounts := IntArray new:distGroupCount.
    #var realCounts := IntArray new:distGroupCount.

    console writeLine:"Random reals:".
    0 till:rows &doEach: i
    [
        0 to:cols &doEach: j
        [
            console write:(randomGenerator nextReal) &paddingRight:12.
        ].
        
        console writeLine.
    ].

    console writeLine.
    console writeLine:"Random integers:".
    0 till:rows &doEach: i
    [
        0 to:cols &doEach: j
        [
            console write:(randomGenerator nextInt) &paddingRight:12.
        ].
        
        console writeLine.
    ].

    console writeLine.
    console writeLine:"Distribution".

//
//        Console.WriteLine( 
//            "\nTo demonstrate the proportional distribution, " +
//            "{0:N0} random \nintegers and doubles are grouped " +
//            "into {1} equal value ranges. This \n" +
//            "is the count of values in each range:\n",
//            runCount, distGroupCount );
//        Console.WriteLine( 
//            "{0,21}{1,10}{2,20}{3,10}", "Integer Range",
//            "Count", "Double Range", "Count" );
//        Console.WriteLine( 
//            "{0,21}{1,10}{2,20}{3,10}", "-------------",
//            "-----", "------------", "-----" );
//
//        // Generate random integers and doubles, and then count 
//        // them by group.
    0 till:runCount &doEach: i
    [
        #var index := (randomGenerator nextInt / intGroupSize) toInt.
        (index < 10)?
            [ intCounts@index := intCounts@index + 1. ].
        
        index := (randomGenerator nextReal * distGroupCount) toInt.
        (index < 10)?
            [ realCounts@index := realCounts@index + 1. ].
    ].    
//        for( int i = 0; i < runCount; i++ )
//        {
//            intCounts[ (int)( (double)randObj.Next( ) / 
//                intGroupSize ) ]++;
//            realCounts[ (int)( randObj.NextDouble( ) * 
//                (double)distGroupCount ) ]++;
//        }
//
    0 till:distGroupCount &doEach: i
    [
        console write:((i * intGroupSize) toInt literal):"-" &paddingLeft:12 &with:#32.
        console write:(((i + 1) * intGroupSize - 1) toInt literal) &paddingLeft:12 &with:#32.        
        console write:(intCounts@i literal) &paddingLeft:12 &with:#32.

        console write:((i real / distGroupCount) literal):"-" &paddingLeft:14 &with:#32.
        console write:(((i + 1) real / distGroupCount) literal) &paddingLeft:12 &with:#32.        
        console write:(realCounts@i literal) &paddingLeft:12 &with:#32.
                
        console writeLine.
    ].
].
