public const struct ThreadPriority : enum<int>(Lowest = 0, BelowNormal = 1, Normal = 2, AboveNormal = 3, Highest = 4);

public const struct TaskStatus : enum<int>(Created = 0, Scheduled = 1, Running = 2,
   RanToCompletion = 3, Canceled = 4, Faulted = 5);
