import extensions;

public program()
{
  // reading 
  var number := console.write:"Enter the number you want to convert: ".loadLineTo(new Integer());

  // converting
  var binaryStr := number.toString(2);

  // printing
  console
    .printLine("A binary presentation of the number : ",binaryStr.padLeft($48, (binaryStr.Length / 4 + 1) * 4))
    .printLine("Press any key to continue");

   // waiting for any key
   console.readChar()
}