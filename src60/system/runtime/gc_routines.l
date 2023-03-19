import system;

// --- GCManager ---

public singleton GCManager
{
   collectMinor() : external(system'core_routines'__gc_minor);

   collectFull() : external(system'core_routines'__gc_full);
}