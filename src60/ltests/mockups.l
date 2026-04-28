import system'dynamic;
    
public class Mockup
{
    field object;
    
    constructor(object)
    {
       this object := object;
    }
    
    generic cast()
    {
       var type := __received.__getFirstSignatureMember();
    
       var proxy := object.mockInferface(type);
       
       ^ __received(proxy);
   }
}

public class Mockup<T>
{
    field object;
    
    constructor(object)
    {
       this object := object;
    }
    
    T cast()
    {
       var proxy := object.mockInferface(class T);
       
       ^ proxy;
   }
}
