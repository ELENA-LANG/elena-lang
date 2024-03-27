public singleton Tester
{
   int getValue()
   {
      ^ 2;
   }
}

public program()
{
   int r := Tester.getValue()
}