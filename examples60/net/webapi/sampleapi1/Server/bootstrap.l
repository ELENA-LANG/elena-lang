#if (__project["Https"])

import mbedtls'server'registration;

#endif

import webapi'bootstrap;

public SampleApi1Bootstap : preloaded =
   WebApiFactory
      .registerServer::(x => x.initSimpleServer());
