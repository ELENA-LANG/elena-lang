import system'io;

const struct TokenType : enum<int>(None = 0, Content = 1, OpenTag = 2, CloseTag = 3);

public class XmlReader : IXmlReader
{
   TextReader     input;
   string         currentLine;
   Reference<int> currentOffset;
   Reference<int> lineNumber;
   bool           available;
   TokenType      tokenMode; 
   string         tagName; 
   string         content; 
   
   constructor open(TextReader input)
   {
      this input := input;
      this currentOffset := 0;
      this lineNumber := 0;
      this available := true;
      this currentLine := String.MinValue;
      this tokenMode := TokenType.None;
   }
   
   bool isAvailable
      = available;
      
   bool isOpenTag
      = tokenMode == TokenType.OpenTag;
   
   bool isCloseTag
      = tokenMode == TokenType.CloseTag;
   
   bool isContent
      = tokenMode == TokenType.Content;
   
   string Name
      = tagName;
      
   get string Content()
   {
      if (tokenMode != TokenType.Content) {
         ^ String.MinValue
      };
        
      ^ content;
   }
   
   read()
   {
      if:not(available)
         :break;
      
      int offs := *currentOffset;
      if (offs >= currentLine.Length) {
         if (!input.Available) {
            available := false;
            
            :break;
         };
                     
         currentLine := input.readLine();
         lineNumber.append(1);
         currentOffset.Value := 0;
         offs := 0;
         
         if (currentLine.isEmpty()) {
            read();
            :break;
         }
      };
      
      int start := offs;
      while (currentLine[offs].isWhitespace())
         offs += currentLine[offs].Length;
      
      if (tokenMode == TokenType.OpenTag && currentLine[offs] != $60) {
         tokenMode := TokenType.Content;
         
         while (currentLine[offs] != $60) {
            offs += currentLine[offs].Length;
         };

         content := currentLine.Substring(start, offs - start);
      }
      else {
         while (currentLine[offs].isWhitespace())
            offs += currentLine[offs].Length;
         
         if (currentLine[offs] != $60)
            InvalidOperationException.raise($"Invalid xml line:{lineNumber}");

         offs += currentLine[offs].Length;
         start := offs;

         if (currentLine[offs] == $47) {
            tokenMode := TokenType.CloseTag;
            
            offs += currentLine[offs].Length;
            start := offs;
         }
         else {
            if (tokenMode == TokenType.Content)
               InvalidOperationException.raise($"Invalid xml line:{lineNumber}");
               
            tokenMode := TokenType.OpenTag;               
         };

         while (currentLine[offs] != $62) {
            if (currentLine[offs].isWhitespace())
               InvalidOperationException.raise($"Invalid xml line:{lineNumber}");
               
            offs += currentLine[offs].Length;
         };
         
         tagName := currentLine.Substring(start, offs - start);
         
         offs++;
      };
      
      currentOffset := offs;
   }
}