public abstract class BootstrapFactoryBase
{
   protected abstract IBootstap create();
   
   internal start()
   {
      IBootstap bootstrap := create();
      
      bootstrap.start();
   }
}

public Program()
{
   BootstrapFactoryBase factory := webapi'bootstrap'WebApiFactory;
   
   factory.start();
}