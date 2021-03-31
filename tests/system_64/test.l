import extensions;
import system'calendar;

public program()
{
    SystemTime st;
    extern KERNEL32.GetSystemTimeAsFileTime(st);
    
    //TimeStruct tm := st;
    var tm := new TimeStruct(st);
    TimeStruct tm2 := tm;
    
   // var s := tm.toShortDate();        
//    var t := tm.toShortTime();                                
                        
    var t2 := tm2.toShortTime();                                
    
    //var d := now.toPrintable();
    
    
    
    
//    var st := now.SystemTime;
//    
//    long t := 01D72621BD8593AFl;
//    
//    short buffer[127];
//        
//    int length := extern KERNEL32.GetDateFormatW(
//                400h,0,/*st*/t,0,buffer,255);
//    
//    var n := length;
//    
////    var d := now;
////    
////    var y := d.Year;
////    var m := d.Month;
////    var dt := d.Day;
////    
////    var s := d.toPrintable();
}