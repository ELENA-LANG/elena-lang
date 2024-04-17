import extensions;
import extensions'scripting;
import system'dynamic'expressions;

symbol used : preloaded = new object[]{Expression, DynamicSingleton,DynamicExpressionSymbol,SymbolCollection,LazySymbolExpression, mssgconst f};

public program()
{
    console.writeLine("Test:");

    lscript.interpretPath("sample.ls");
}