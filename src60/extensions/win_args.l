import system'routines;

// --- program_arguments ---

public static Program_arguments = 
    new extensions'text'TokenEnumerator(CommandLine.toString(), new extensions'text'TokenStatemachine()).selectBy::(s => s.trim($34)).toArray();

// obosolete
public program_arguments = Program_arguments;
  