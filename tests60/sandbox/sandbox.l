import extensions;

internal const int __SIZEOF_PTHREAD_MUTEX_T = 24;

internal struct pthread_mutex_t
{
   byte __size[__SIZEOF_PTHREAD_MUTEX_T];
}



A
{
  field x;

  test()
  {
     int x := 2; // warn there is a field with the same name

     x := 3;
  }
}

public Program()
{
}