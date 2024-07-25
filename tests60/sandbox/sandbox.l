import system'io;
import system'collections;
import system'routines;
import extensions;

public program()
{
   var dirPath := console.write("Enter a path with *T.xml files:").readLine();

   var files := Directory.assign(dirPath).getFiles("*T.xml");
   var tokens := "";
   for (var i := 0; i < files.Length; i+=1) {
      var file := files[i];

      tokens := loadToken(file);
   }
}

loadToken(var filePath)
{
   console.writeLine(filePath);
   var lines := readAllLines(filePath);

   lines.forEach::(s => console.writeLine(s));
}

//readAllLines(var filePath)
//{
//   var text := File.assign(filePath).readContent();
//   var lines := new List<string>();
//   while (text != "") {
//      var endIndex := text.indexOf($10);
//      if (endIndex == -1) {
//         lines.append(text);
//         text := "";
//      }
//      else {
//         var subStr := text.Substring(0, endIndex);
//         text := text.delete(0, endIndex + 1);
//         lines.append(subStr);
//      }
//   };
//
//   ^ lines
//}

readAllLines(var filePath)
{
   ^ File.assign(filePath).readContent().splitBy($10);
}