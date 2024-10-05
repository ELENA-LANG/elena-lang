import system'io;

public program()
{
   var reader := File.assign("sandbox.l").textreader();
   while (reader.Available) {
      var line := reader.readLine();

      Console.writeLine(line);
   };
   

/*    File.assign(Program_arguments[1]).forEachLine::(line)
    {
        adapter.write(line);
    };*/
}