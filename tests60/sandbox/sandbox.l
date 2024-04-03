public const struct Struct
{
   embeddable __int _value[4];
}

public sealed XReference
{
   embeddable Struct value;
}

public singleton Tester
{
   get Struct Value()
   {
      ^ new Struct();
   }
}

public program()
{
   Struct r := *Tester
}
