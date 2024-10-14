import extensions;

test(Action<object> arg)
{
   arg("2");
}

public program()
{
   test((::Action<object> x){ console.printLine("arg ", x, " is passed") });
}