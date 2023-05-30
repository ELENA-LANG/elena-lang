import system;

public singleton StreamTapeInterpreter
{
   build(byte[] buffer, int len) : external(system'core_routines'__tape_eval);
}