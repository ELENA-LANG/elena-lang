import extensions;
 
compareStrings = (val1,val2)
{
    if (val1 == val2) { Console.printLine("The strings ",val1," and ",val2," are equal") };
    if (val1 != val2) { Console.printLine("The strings ",val1," and ",val2," are not equal") };
    if (val1  > val2) { Console.printLine("The string ",val1," is lexically after than ",val2) };
    if (val1  < val2) { Console.printLine("The string ",val1," is lexically before than ",val2) };
    if (val1 >= val2) { Console.printLine("The string ",val1," is not lexically before than ",val2) };
    if (val1 <= val2) { Console.printLine("The string ",val1," is not lexically after than ",val2) }
};
 
public program()
{
    var s1 := "this";
    var s2 := "that";
    compareStrings(s1,s2);
 
    Console.readChar()
}