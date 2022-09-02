// --- base functions ---
public interface Func
{
   abstract function();
}

public interface Func1
{
   abstract function(arg1);
}

public interface Func<T,TResult>
{
   abstract TResult function(T);
}
