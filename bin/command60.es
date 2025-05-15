public program() { system'Console.writeLine($1) }
$$
public program() { $1 }
$$
public program() { extensions'scripting'Globals.setAt($1); }
$$
extensions'scripting'Globals.at($1)