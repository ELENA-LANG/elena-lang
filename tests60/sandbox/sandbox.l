import system'io;
import system'routines;
import extensions'routines;
   
public program()
{
    var dir := Directory.assign("d:\Alex");
    
    dir.getFiles("*.*").forEach:printingLn;
}