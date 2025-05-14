public program() { system'Console.writeLine($1) }
$$
public program() { $1 }
$$
public program() { extensions'scripting'Global.setAt($1); }
$$
extensions'scripting'Global.getAt($1)