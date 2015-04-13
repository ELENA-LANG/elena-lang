#define system.

#symbol Program =
[
   console writeLine:"Hello Linux World!".

   #var args := commandLineArguments.

   console writeLine:(args @ 0).

   console readChar.
].