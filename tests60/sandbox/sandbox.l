import extensions;

singleton Helper
{
   int? get() = 2;
}

public program()
{
   var n := Helper.get()
}
