import extensions;

public program()
{
   var script := "
<={self[0]}=>
for(var i := 1; i < self.Length; i++) {
  <=,{self[i]}=>
}";

   var target := "1
2
3";

  var output := textgen'TextGen.parseAsLineSeparated(target, script);

//  if ($output != nil) {
//    File.saveContent($output, output) 
//  };

  system'Console.writeLine(output);
   
}