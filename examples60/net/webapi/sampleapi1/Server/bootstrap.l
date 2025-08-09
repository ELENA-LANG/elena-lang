import webapi'bootstrap;

public SampleApi1Bootstap : preloaded =
   WebApiFactory
      .registerServer(x => x.initDummy());
