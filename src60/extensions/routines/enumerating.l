import extensions'text;

public extension textExOp
{
    forEachWord(Func1 f)
        = f.run(cast Enumerator(TokenEnumerator.new(self)));
}
