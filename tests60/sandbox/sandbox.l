import extensions;
import extensions'scripting;
import system'dynamic'expressions;

symbol used : preloaded = new object[]{Expression, DynamicSingleton,DynamicExpressionSymbol,SymbolCollection,LazySymbolExpression, mssgconst f, Integer};

public program()
{
    console.printLine("Test:");

    lscript.interpretPath("sample1.ls");
}
