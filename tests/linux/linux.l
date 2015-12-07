#import system.
#import system'calendar. 

#symbol Program =
[
   #var d1 := Date new &year:2015 &month:1 &day:1.
   #var d2 := Date new &year:2015 &month:2 &day:1.

   #var s := d2 - d1.
   #var msec := s milliseconds.

   console writeLine:d1.
   console writeLine:d2.
   console writeLine:msec.
   console writeLine:s.
].