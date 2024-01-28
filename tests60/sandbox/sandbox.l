import extensions;

public program()
{
    until(string line := console.write(">").readLine(); line == "")
    {
        proceed(r, line.split());        
    }
}
