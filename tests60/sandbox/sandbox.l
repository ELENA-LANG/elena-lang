import extensions;

public class A
{
   constructor new()
   {
      console.printLine("Calling a constructor");
   }
}

public program()
{
    var node := new object();
    while (node.next()) {
        string token := *node;
        if (token == "(") {
            node.append(buildTree(reader));
        }
        else if (token == ")") {
            ^ node;
        }
        else if (token == ";") {
           node := Node.new(node);
        }
        else {
            node.Text := token;
        }
    };
}