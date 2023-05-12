import extensions;

public class DynamicSingleton
{
    // declare a private constructor to prevent the class 
    // from direct use
    private constructor() {}
    
    callMe()
    {
        console.printLine("I'm a singleton")
    }
    
    // declaring a static field
    static DynamicSingleton instance;
    
    // declaring a static property to access the field
    static DynamicSingleton Instance
    {
        get()
        {
            if (instance == nil)
            {
                instance := new DynamicSingleton();
            };
            
            ^ instance
        }
    }
}

public program()
{
    DynamicSingleton.Instance.callMe();
}