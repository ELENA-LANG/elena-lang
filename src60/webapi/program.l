public Program()
{
   IBootstap bootstrap := webapi'bootstrap'Bootstrap;
   
   bootstrap.Server.start();
}