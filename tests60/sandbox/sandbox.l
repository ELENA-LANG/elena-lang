import system'io;
 
public Program()
{
    var text := File.assign("input.txt").readContent();
 
    File.assign("output.txt").saveContent(text);
}
