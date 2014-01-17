////---------------------------------------------------------------------------
////		E L E N A   P r o j e c t:  ELENA VM Script Engine
////
////                                              (C)2011-2012, by Alexei Rakov
////---------------------------------------------------------------------------
//
//#include "elena.h"
//// --------------------------------------------------------------------------
//#include "scriptparser.h"
//
//using namespace _ELENA_;
//using namespace _ELENA_TOOL_;
//
//#define BODY_KEYWORD          "$body"
//
//const int mskUntracable = 0x10000;
//const int tsEof         = 0x02001;
//
//// --- LALRParser::Reader ---
//
//LALRParser::Reader :: Reader(LALRParser* parser, int tabSize, TextReader* source)
//   : _reader(parser->_dfa, tabSize, source)
//{
//   _parser = parser;
//   terminal = 0;
//}
//
//void LALRParser::Reader :: read()
//{
//   info = _reader.read(_buffer, IDENTIFIER_LEN);
//
//   switch (info.state) {
//      case dfaEOF:
//         terminal = tsEof;
//         break;
//      case dfaQuote:
//         terminal = _parser->_table.defineSymbol(ConstantIdentifier(LITERAL_KEYWORD));
//         break;
//      case dfaFullIdentifier:
//         terminal = _parser->_table.defineSymbol(ConstantIdentifier(REFERENCE_KEYWORD));
//         break;
//      case dfaLong:
//      case dfaInteger:
//      case dfaReal:
//         terminal = _parser->_table.defineSymbol(ConstantIdentifier(NUMERIC_KEYWORD));
//         break;
//      default:
//         terminal = _parser->_table.defineSymbol(info.line);
//         if (terminal == 0 && info.state == dfaIdentifier) { 
//            terminal = _parser->_table.defineSymbol(ConstantIdentifier(IDENTIFIER_KEYWORD));
//         }
//         break;
//   }
//}
//
//// --- LALRParser ---
//
//void LALRParser :: parseRuleWithDSA(int symbol, Reader& reader, ParserStack& stack, _CodeGeneator* compiler)
//{
//   DSARule rule = _dsaRules.get(symbol);
//   if (rule.postfixPtr || rule.prefixPtr) {
//      Terminal terminalInfo;
//      reader.copyTo(terminalInfo);
//
//      if (rule.prefixPtr) {
//         WideLiteralTextReader subScript((const wchar16_t*)_body.get(rule.prefixPtr));
//
//         compiler->generate(&subScript, &terminalInfo);
//      }
//
//      parseRule(symbol, reader, stack, compiler);
//
//      if (rule.postfixPtr) {
//         WideLiteralTextReader subScript((const wchar16_t*)_body.get(rule.postfixPtr));
//
//         compiler->generate(&subScript, &terminalInfo);
//      }      
//   }
//   else  parseRule(symbol, reader, stack, compiler);
//}
//
//void LALRParser :: parseRule(int symbol, Reader& reader, ParserStack& stack, _CodeGeneator* compiler)
//{
//   int current = stack.pop();
//   while (current != 0) {
//      if (test(current, ParserTable::mskTerminal)) {
//         if (current != reader.terminal)
//            throw EParseError(reader.info.column, reader.info.row);
//
//         if (current == tsEof) {
//            break;
//         }
//         else reader.read();
//      }
//      else {
//         stack.push(0);
//
//         if (!_table.read(current, reader.terminal, stack))
//            throw EParseError(reader.info.column, reader.info.row);
//
//         if (!test(current, mskUntracable)) {
//            parseRuleWithDSA(current, reader, stack, compiler);
//         }
//         else parseRule(current, reader, stack, compiler);
//      }
//      current = stack.pop();
//   }
//}
//
//void LALRParser :: parse(TextReader* script, _CodeGeneator* compiler)
//{
//   Reader      reader(this, 4, script);
//   ParserStack stack(tsEof);
//
//   stack.push(1);
//   reader.read();
//
//   parseRule(0, reader, stack, compiler);
//}
//
//int LALRParser :: mapSymbol(const wchar16_t* name, size_t mask)
//{
//   RuleIdentifier ruleName(name);
//   if (mask == 0)
//      ruleName.upper();
//
//   int symbol = _table.defineSymbol(ruleName);
//   if (symbol == 0) {
//      do {
//         _lastId++;
//      } while (!emptystr(_table.retrieveSymbol(_lastId | mask)));
//
//      symbol = _lastId | mask;
//      _table.registerSymbol(symbol, ruleName);
//   }
//
//   return symbol;
//}
//
//void LALRParser :: defineGrammarRule(int symbol, SourceReader& reader, wchar16_t* token)
//{
//   LineInfo info = reader.read(token, IDENTIFIER_LEN);
//
//   int rule[30];
//   int length = 0;
//   while (!ConstantIdentifier::compare(token, ";")) {
//      if (info.state == dfaIdentifier) {
//         rule[length] = mapSymbol(token);
//         length++;
//      }
//      else if (info.state == dfaQuote) {
//         QuoteTemplate<TempString> quote(info.line);
//
//         rule[length] = mapSymbol(quote, ParserTable::mskTerminal);
//         length++;
//      }
//      else if (info.state == dfaPrivate) {
//         if (ConstantIdentifier::compare(token, REFERENCE_KEYWORD)) {
//            rule[length] = mapSymbol(token, ParserTable::mskTerminal);
//            length++;
//         }
//         else if (ConstantIdentifier::compare(token, IDENTIFIER_KEYWORD)) {
//            rule[length] = mapSymbol(token, ParserTable::mskTerminal);
//            length++;
//         }
//         else if (ConstantIdentifier::compare(token, LITERAL_KEYWORD)) {
//            rule[length] = mapSymbol(token, ParserTable::mskTerminal);
//            length++;
//         }
//         else if (ConstantIdentifier::compare(token, NUMERIC_KEYWORD)) {
//            rule[length] = mapSymbol(token, ParserTable::mskTerminal);
//            length++;
//         }
//         else if (ConstantIdentifier::compare(token, EPS_KEYWORD)) {
//            rule[length] = ParserTable::nsEps;
//            length++;
//         }
//         else throw EParseError(info.column, info.row);
//      }
//      else throw EParseError(info.column, info.row);
//
//      info = reader.read(token, IDENTIFIER_LEN);
//   }
//   
//   //HOTFIX: the start rules should be terminated by tsEOF
//   if (symbol == 1) {
//      rule[length++] = tsEof;
//   }
//
//   _table.registerRule(symbol, rule, length);
//}
//
//void LALRParser :: defineDSARule(int rule_id, SourceReader& reader, wchar16_t* token)
//{
//   DSARule rule;
//
//   // read: [prefix] $body [postfix] ;
//   LineInfo info = reader.read(token, IDENTIFIER_LEN);
//
//   bool bodyFound = false;
////   bool tokenMode = false;
////   bool bodyNotDefined = true;
//
//   MemoryWriter writer(&_body);
//
//   rule.prefixPtr = writer.Position();
//
//   wchar_t lastState = 0;
//   while (!ConstantIdentifier::compare(token, ";")) {
//      if (ConstantIdentifier::compare(token, BODY_KEYWORD)) {
//         if (!bodyFound) {
//            bodyFound = true;
//
//            if (rule.prefixPtr == writer.Position()) {
//               rule.prefixPtr = 0;
//            }
//            else writer.writeWideChar(0);
//
//            rule.postfixPtr = writer.Position();
//            lastState = 0;
//         }
//         else throw EParseError(info.column, info.row);
//      }
//      else {
//         if (lastState == dfaFullIdentifier || lastState == dfaIdentifier || lastState == dfaInteger || lastState == dfaReal || lastState == dfaLong) {
//            writer.writeWideChar(0x20);
//         }
//         writer.writeWideLiteral(info.line, info.length);
//
//         lastState = info.state;
//      }
//
//      info = reader.read(token, IDENTIFIER_LEN);
//   }
//
//   if (!bodyFound)
//      throw EParseError(info.column, info.row);
//
//   if (rule.postfixPtr == writer.Position()) {
//      rule.postfixPtr = 0;
//   }
//   else writer.writeWideChar(0);
//
//   _dsaRules.add(rule_id, rule);
//}
//
//void LALRParser :: generate(TextReader* script)
//{
//   int last_id = 0x20;
//
//   SourceReader source(4, script);
//   LineInfo     info;
//   wchar_t      token[IDENTIFIER_LEN];
//
//   info = source.read(token, IDENTIFIER_LEN);
//   while (info.state != dfaEOF) {
//      if (info.state != dfaIdentifier)
//         throw EParseError(info.column, info.row);
//
//      int rule_id = mapSymbol(token);
//
//      info = source.read(token, IDENTIFIER_LEN);
//      if (ConstantIdentifier::compare(token, "::=")) {
//         defineGrammarRule(rule_id, source, token);
//      }
//      else if (ConstantIdentifier::compare(token, "=>")) {
//         defineDSARule(rule_id, source, token);
//      }
//      else throw EParseError(info.column, info.row);
//
//      info = source.read(token, IDENTIFIER_LEN);
//   }
//
//   if(_table.generate() != 0)
//      throw EUnrecognizedException();
//}
