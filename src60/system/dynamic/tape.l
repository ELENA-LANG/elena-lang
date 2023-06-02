import system;

// --- DynamicTape ---

public mixin DynamicTape
{
   __string TapeFunction[] theList;
}

// --- StreamTapeInterpreter ---

public singleton StreamTapeInterpreter
{
   build(byte[] buffer, int len) : external(system'core_routines'__tape_eval);
}