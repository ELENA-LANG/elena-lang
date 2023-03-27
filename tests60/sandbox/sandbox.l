import system'calendar;

public program()
{
//   var d := Date.new(2023, 1, 1);

         TimeStruct st;
         SystemTime value := 0l;
         extern KERNEL32.SystemTimeToFileTime(st, ref value);
}