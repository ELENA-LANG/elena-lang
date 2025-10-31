import webapi'bootstrap;
import net'http;

#if (__project["Https"])

import mbedtls'server'registration;

Registration = (x => x.initSimpleServer("HTTPS", HTTPS_PORT));

#else

Registration = (x => x.initSimpleServer());

#endif

public SampleApi1Bootstap : preloaded =
   WebApiFactory
      .registerServer::(Registration);
