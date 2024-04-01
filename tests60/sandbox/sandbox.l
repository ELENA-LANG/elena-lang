public const struct Struct
{
   embeddable __int _value[4];
}

public singleton Tester
{
   Struct getValue()
   {
      ^ new Struct();
   }
}

public program()
{
   Struct r := Tester.getValue()
}