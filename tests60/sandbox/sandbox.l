import system'dynamic;
import extensions;

I;

singleton A : I
{
   whoAmI()
      = "singleton A";
}



public Program()
{   
   DependencyInjector::registerSingleton<I>(A);
   
}
