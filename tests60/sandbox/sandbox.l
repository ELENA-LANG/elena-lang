import system'calendar;
import extensions;

public program()
{
         TimeStruct ts;
         SystemTime time;

         pointer ptr/* := extern libc.localtime(time)*/;
            
         extern libc.memcpy(ts, ptr, 56);
}