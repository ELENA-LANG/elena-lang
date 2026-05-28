import system'io;

const struct TokenType : enum<int>(None = 0, Content = 1, OpenTag = 2, CloseTag = 3, Attribute = 4, AttributeValue = 5);

public class XmlReader : IXmlReader
{
   TextReader     input;
   string         currentLine;
   Reference<int> currentOffset;
   Reference<int> lineNumber;
   bool           available;
   bool           attributeMode;
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
      this attributeMode := false
   }
   
   bool isAvailable
      = available;
      
   bool isOpenTag
      = tokenMode == TokenType.OpenTag;
   
   bool isCloseTag
      = tokenMode == TokenType.CloseTag;
   
   bool isContent
      = tokenMode == TokenType.Content;
   
   bool isAttribute
      = tokenMode == TokenType.Attribute;
   
   bool isAttributeValue
      = tokenMode == TokenType.AttributeValue;
   
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
      
      int lineLen := currentLine.Length;
      int offs := *currentOffset;
      if (offs >= lineLen) {
         if (!input.Available) {
            available := false;
            
            :break;
         };
                     
         currentLine := input.readLine();
         lineLen := currentLine.Length;
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
      
      if (tokenMode == TokenType.AttributeValue) {
         tokenMode := TokenType.OpenTag;

         attributeMode := currentLine[offs] != $62
      };

      if (tokenMode == TokenType.Attribute) {
         if(currentLine[offs] != $34)
            InvalidOperationException.raise($"Invalid xml line:{lineNumber}");

         offs += currentLine[offs].Length;
         start := offs;

         while (offs < lineLen && currentLine[offs] != $34)
            offs += currentLine[offs].Length;

         if (offs == lineLen)
            InvalidOperationException.raise($"Invalid xml line:{lineNumber}");

         content := currentLine.Substring(start, offs - start);
         tokenMode := TokenType.AttributeValue;

         offs += currentLine[offs].Length;
      }
      else if (tokenMode == TokenType.OpenTag && currentLine[offs] != $60) {
         if (attributeMode) {
            tokenMode := TokenType.Attribute;

            if (!currentLine[offs].isLetter())
               InvalidOperationException.raise($"Invalid xml line:{lineNumber}");

            while (offs < lineLen && currentLine[offs].isLToken())
               offs += currentLine[offs].Length;

            tagName := currentLine.Substring(start, offs - start);

            if(currentLine[offs] != $61)
               InvalidOperationException.raise($"Invalid xml line:{lineNumber}");

            offs += currentLine[offs].Length;
         }
         else {
            tokenMode := TokenType.Content;
         
            while (currentLine[offs] != $60) {
               offs += currentLine[offs].Length;
            };

            content := currentLine.Substring(start, offs - start);
         }
      }
      else {
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

         while (offs < lineLen && currentLine[offs].isLetter())
            offs += currentLine[offs].Length;

         tagName := currentLine.Substring(start, offs - start);

         if (offs < lineLen && currentLine[offs] == $62) {
            offs += currentLine[offs].Length
         }
         else {
            if(tokenMode == TokenType.OpenTag) {
               attributeMode := true;
            }
            else InvalidOperationException.raise($"Invalid xml line:{lineNumber}");
         }
      };
      
      currentOffset := offs;
   }
}