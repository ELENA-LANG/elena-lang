import extensions;

#if (__project["_Win32"] || __project["_Win64"])

A;

#elif (__project["_Linux32"])

A;

#elif (__project["_Linux32"])

A;

#endif

public program()
{
   var a := new A();
}
