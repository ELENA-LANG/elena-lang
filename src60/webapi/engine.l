import net'http'server;

public interface IBootstap
{
   get /*internal*/ abstract IHttpServer Server();
   
   abstract initDummy();
}