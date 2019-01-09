import system'routines;

// --- Arguments ---

public static program_arguments = 
    new extensions'text'TokenEnumerator(commandLine, new extensions'text'TokenStatemachine()).selectBy:(s => s.trim:$34).toArray();
