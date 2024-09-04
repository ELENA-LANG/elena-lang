import extensions;
import system'collections;

public sealed class QList<T>
{
   List<T> _list := new List<T>();

   constructor() {}

   push(T o)
   {
      _list.append(o);
   }
}

public program()
{
   QList<int> l := new QList<int>();

   l.push(1);
   l.push(2);
}
