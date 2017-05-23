#import system'routines.
#import system'collections.
#import system'text.
#import extensions.

extension op
{
    fibonacci
    [
        if (self < 2)
        [ 
            ^ self.
        ];
        [
            ^ (self - 1) fibonacci + (self - 2) fibonacci.
        ].
    ]
    
    zeckendorf
    [
        listof(int) fibonacciNumbers  := 10.
        
        int num := self.
        int fibPosition := 2.
        int currentFibonaciNum := fibPosition fibonacci.
        
        while (currentFibonaciNum <= num)
        [
            fibonacciNumbers += currentFibonaciNum.
            
            fibPosition += 1.
            currentFibonaciNum := fibPosition fibonacci.
        ].
        
        textbuffer output := TextBuilder new.
        int temp := num.
        
        fibonacciNumbers reverse run &each:item
        [
            if (item <= temp)
            [
                output << "1".
                temp := temp - item.
            ];
            [
                output << "0".
            ].
        ].
        
        ^ output literal.
    ]
}
                
program =
[
    1 to:20 &doEach:i
    [
        var zeckendorfRepresentation := i zeckendorf.
        
        console writeFormatted:"{0} : {1}":i:zeckendorfRepresentation writeLine.
    ].
    
    console readChar.
].
