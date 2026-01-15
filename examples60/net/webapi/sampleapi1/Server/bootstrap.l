import webapi;
import webapi'bootstrap;
import net'http;

#if (__project["Https"])

import mbedtls'server'registration;

Action<IBootstap> Registration = (x => x.initSimpleServer("https", /*HTTPS_PORT*/4433));

#else

Action<IBootstap> Registration = (x => x.initSimpleServer());

#endif

public SampleApi1Bootstap : preloaded =
   WebApiFactory
      .registerServer(Registration);
