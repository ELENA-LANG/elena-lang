import extensions;

// --- Program --- 

public program()
{
    // reading    
    var number := Console.write("Enter the number you want to convert: ").loadLineTo(Integer.new());
    
    // converting
    var binaryStr := number.toString(2);

    // printing
    Console
        .printLine("A binary presentation of the number : ",binaryStr.padLeft($48, (binaryStr.Length / 4 + 1) * 4))
        .printLine("Press any key to continue");

    // waiting for any key
    Console.readChar()
}
