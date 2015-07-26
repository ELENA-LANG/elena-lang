#define system.

#symbol Program =
[
		  /*#var(type:byte)aRetVal.
        #var(type:int) ch := system'external'libc getchar.

        byteConvertor convert &int:ch &vbyte:aRetVal.
        console write:aRetVal. 

        ch := system'external'libc getchar.
        byteConvertor convert &int:ch &vbyte:aRetVal.
        console write:aRetVal.*/

   #var l := console readLine.

   console write:l.
].