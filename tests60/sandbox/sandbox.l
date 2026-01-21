import system'calendar;

public extension dateFormatterOpX : Date
{
   string toShortTimeString()
      = cast TimeStruct(self).toShortTime();
}

public Program()
{
/*   string s3 := cast TimeStruct(Now).toShortTime();
   var s2 := cast TimeStruct(Now).toShortTime();*/
   var s := Now.toShortTimeString()
}