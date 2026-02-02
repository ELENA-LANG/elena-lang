public const struct ThreadPriority : enum<int>(Lowest = 0, BelowNormal = 1, Normal = 2, AboveNormal = 3, Highest = 4);

#if (__project["_Linux32"])

internal const int __SIZEOF_PTHREAD_MUTEX_T = 24;
internal const int __SIZEOF_PTHREAD_COND_T  = 48;

#elif (__project["_Linux64"])

internal const int __SIZEOF_PTHREAD_MUTEX_T = 40;
internal const int __SIZEOF_PTHREAD_COND_T  = 48;

#endif
