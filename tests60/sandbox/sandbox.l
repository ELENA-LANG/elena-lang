/*
A	    U+0041	41 
ö	    U+00f6	c3 b6 
Ж	    U+0416	d0 96 
€	    U+20ac	e2 82 ac 
𝄞	    U+1d11e	f0 9d 84 9e 
*/ 

import system'routines;
import extensions;
 
extension op : String
{
    string printAsString()
    {
       console.print(self," ")
    }
 
    string printAsUTF8Array()
    {
        self.toByteArray().forEach::(b){ console.print(b.toString(16)," ") }
    }
 
    string printAsUTF32()
    {
        self.toArray().forEach::(c){ console.print("U+",c.toInt().toString(16)," ") }
    }
}
 
public program()
{
    "A".printAsString().printAsUTF8Array().printAsUTF32();
    console.printLine();
 
    "ö".printAsString().printAsUTF8Array().printAsUTF32();
    console.printLine();
 
    "Ж".printAsString().printAsUTF8Array().printAsUTF32();
    console.printLine();
 
    "€".printAsString().printAsUTF8Array().printAsUTF32();
    console.printLine();
 
    "𝄞".printAsString().printAsUTF8Array().printAsUTF32();
    console.printLine();
}
