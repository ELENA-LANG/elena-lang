import extensions;

//const string[] strings = new /*const string*/[] { "a", "b", "c" };

public Program()
{
   const string[] strings := new /*const string*/[] { "a", "b", "c" };

   Console.printLine(strings.asEnumerable())
}
