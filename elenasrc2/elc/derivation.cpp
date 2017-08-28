//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Engine Derivation Tree class implementation
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "derivation.h"
#include "errors.h"
#include "bytecode.h"

using namespace _ELENA_;

inline bool isPrimitiveRef(ref_t reference)
{
   return (int)reference < 0;
}

//void test2(SNode node)
//{
//   SNode current = node.firstChild();
//   while (current != lxNone) {
//      test2(current);
//      current = current.nextNode();
//   }
//}

// --- DerivationWriter ---

void DerivationWriter :: writeNode(Symbol symbol)
{
   switch (symbol)
   {
      case nsToken:
         _writer.newNode(lxAttribute);
         break;
      case nsArrayIdentifier:
         _writer.newNode(lxAttributeValue, -1);
         break;
      case nsExpression:
      case nsRootExpression:
         _writer.newNode(lxExpression);
         break;
      case nsCodeEnd:
         _writer.newNode(lxEOF);
         break;
      case nsMethodParameter:
         _writer.newNode(lxMethodParameter);
         break;
      case nsMessageParameter:
         _writer.newNode(lxMessageParameter);
         break;
      case nsNestedClass:
         _writer.newNode(lxNestedClass);
         break;
      case nsAssigning:
         _writer.newNode(lxAssigning);
         break;
      case nsResendExpression:
         _writer.newNode(lxResendExpression);
         break;
      case nsObject:
      case nsAngleObject:
      case nsRootAngleObject:
         _writer.newNode(lxObject);
         break;
      case nsAngleOperator:
      case nsRootAngleOperator:
         _writer.newNode(lxAngleOperator);
         break;
      case nsBaseClass:
         _writer.newNode(lxBaseParent);
         break;
      case nsL0Operation:
      case nsL3Operation:
      case nsL4Operation:
      case nsL5Operation:
      case nsL6Operation:
      case nsL7Operation:
      case nsRootL6Operation:
         _writer.newNode(lxOperator);
         break;
      case nsArrayOperation:
         _writer.newNode(lxOperator, REFER_MESSAGE_ID);
         break;
      case nsSizeExpression:
         _writer.newNode(lxOperator, -3);
         break;
      case nsXInlineClosure:
         _writer.newNode(lxInlineClosure);
         break;
      case nsMessageOperation:
         _writer.newNode(lxMessage);
         break;
      case nsSubjectArg:
         _writer.newNode(lxMessage, -1);
         break;
      case nsRootMessage:
         _writer.newNode(lxMessage, -2);
         break;
      case nsLazyExpression:
         _writer.newNode(lxLazyExpression);
         break;
      case nsRetStatement:
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask | lxExprMask));
         break;
      case nsMessageReference:
         _writer.newNode(lxMessageReference);
         break;
      case nsSizeValue:
         _writer.newNode(lxSize);
         break;
      case nsSwitching:
         _writer.newNode(lxSwitching);
         break;
      case nsSubCode:
      case nsScope:
      case nsTemplate:
      case nsTokenParam:
//      case nsSubject:
      case nsDispatchExpression:
      case nsExtension:
      case nsCatchMessageOperation:
      case nsAltMessageOperation:
      case nsSwitchOption:
      case nsLastSwitchOption:
//      case nsBiggerSwitchOption:
//      case nsLessSwitchOption:
//      case nsInlineClosure:
         _writer.newNode((LexicalType)(symbol & ~mskAnySymbolMask));
         break;
      case nsAttribute:
         _writer.newNode(lxAttributeDecl);
         break;
      default:
         _writer.newNode((LexicalType)symbol);
         break;
   }   
}

void DerivationWriter :: writeSymbol(Symbol symbol)
{
   if (symbol != nsNone) {
      writeNode(symbol);
   }
   else _writer.closeNode();
}

void DerivationWriter :: writeTerminal(TerminalInfo& terminal)
{
   // HOT FIX : if there are several constants e.g. $10$13, it should be treated like literal terminal
   if (terminal == tsCharacter && terminal.value.findSubStr(1, '$', terminal.length, NOTFOUND_POS) != NOTFOUND_POS) {
      terminal.symbol = tsLiteral;
   }

//   if (terminal.symbol == tsAttribute) {
//      _writer.newNode(lxExplicitAttr);
//   }
   /*else */_writer.newNode((LexicalType)(terminal.symbol & ~mskAnySymbolMask | lxTerminalMask | lxObjectMask));

   if (terminal==tsLiteral || terminal==tsCharacter || terminal==tsWide) {
      // try to use local storage if the quote is not too big
      if (getlength(terminal.value) < 0x100) {
         QuoteTemplate<IdentifierString> quote(terminal.value);
      
         _writer.appendNode(lxTerminal, quote.ident());
      }
      else {
         QuoteTemplate<DynamicString<char> > quote(terminal.value);
      
         _writer.appendNode(lxTerminal, quote.ident());
      }
   }
   else _writer.appendNode(lxTerminal, terminal.value);

   _writer.appendNode(lxCol, terminal.col);
   _writer.appendNode(lxRow, terminal.row);
   _writer.appendNode(lxLength, terminal.length);
   //   _writer->writeDWord(terminal.disp);

   _writer.closeNode();
}

// --- DerivationReader ---

inline void copyIdentifier(SyntaxWriter& writer, SNode ident)
{
   if (emptystr(ident.identifier())) {
      SNode terminal = ident.findChild(lxTerminal);
      if (terminal != lxNone) {
         writer.newNode(ident.type, terminal.identifier());
      }
      else writer.newNode(ident.type);
   }
   else writer.newNode(ident.type, ident.identifier());

   SyntaxTree::copyNode(writer, lxRow, ident);
   SyntaxTree::copyNode(writer, lxCol, ident);
   SyntaxTree::copyNode(writer, lxLength, ident);
   writer.closeNode();
}

inline SNode findLastAttribute(SNode current)
{
   SNode lastAttribute;
   while (current == lxAttribute) {
      lastAttribute = current;
      current = current.nextNode();
   }

   return lastAttribute;
}

inline bool setIdentifier(SNode current)
{
   SNode lastAttribute = findLastAttribute(current);

   if (lastAttribute == lxAttribute) {
      lastAttribute = lxNameAttr;

      current.refresh();

      return true;
   }
   else return false;
}

inline bool isTerminal(LexicalType type)
{
   switch (type)
   {
      case lxIdentifier:
      case lxPrivate:
      case lxInteger:
      case lxHexInteger:
      case lxLong:
      case lxReal:
      case lxLiteral:
      case lxReference:
      case lxCharacter:
      case lxWide:
      case lxExplicitConst:
      case lxMemberIdentifier:
         return true;
      default:
         return false;
   }
}

inline SNode goToNode(SNode current, LexicalType type)
{
   while (current != lxNone && current != type)
      current = current.nextNode();

   return current;
}

inline bool isAttribute(ref_t attr)
{
   return (int)attr < 0;
}

inline int readSizeValue(SNode node, int radix)
{
   ident_t val = node.identifier();
   if (emptystr(val))
      val = node.findChild(lxTerminal).identifier();

   return val.toLong(radix);
}

inline void copyOperator(SyntaxWriter& writer, SNode ident, int operator_id)
{
   if (operator_id != 0) {
      writer.newNode(lxOperator, operator_id);
   }
   else if (emptystr(ident.identifier())) {
      SNode terminal = ident.findChild(lxTerminal);
      if (terminal != lxNone) {
         writer.newNode(lxOperator, terminal.identifier());
      }
      else writer.newNode(ident.type);
   }
   else writer.newNode(lxOperator, ident.identifier());

   SyntaxTree::copyNode(writer, lxRow, ident);
   SyntaxTree::copyNode(writer, lxCol, ident);
   SyntaxTree::copyNode(writer, lxLength, ident);
   writer.closeNode();
}

inline bool isTemplateDeclaration(SNode node)
{
   SNode current = node.findChild(lxOperator);
   if (current == lxOperator) {
      SNode angleOperator = current.findChild(lxObject).findChild(lxAngleOperator);

      return (angleOperator != lxNone && angleOperator.existChild(lxAssigning));
   }
   return false;
}

inline void copyAutogeneratedClass(SyntaxTree& sourceTree, SyntaxTree& destionationTree)
{
   SyntaxWriter writer(destionationTree);

   SyntaxTree::moveNodes(writer, sourceTree, lxClass);
}

inline bool isArrayDeclaration(SNode node)
{
   SNode current = node.findChild(lxAttributeValue);
   if (current == lxAttributeValue && current.argument == -1 && current.nextNode() == lxSize) {
      return true;
   }

   return false;
}

// --- DerivationReader::DerivationScope ---

ref_t DerivationReader::DerivationScope :: mapNewReference(ident_t identifier)
{
   ReferenceNs name(moduleScope->module->Name(), identifier);

   return moduleScope->module->mapReference(name);
}

ref_t DerivationReader::DerivationScope :: mapAttribute(SNode attribute, int& paramIndex)
{   
   SNode terminal = attribute.firstChild(lxTerminalMask);
   if (terminal == lxNone)
      terminal = attribute;

   int index = mapParameter(terminal);
   if (index) {
      paramIndex = index;

      return attribute.existChild(lxAttributeValue) ? V_ATTRTEMPLATE : INVALID_REF;
   }
   else if (attribute.existChild(lxAttributeValue)) {
      return V_ATTRTEMPLATE;
   }
   else return moduleScope->mapAttribute(attribute);
}

ref_t DerivationReader::DerivationScope :: mapTerminal(SNode terminal, bool existing)
{
   return moduleScope->mapTerminal(terminal, existing);
}

bool DerivationReader::DerivationScope :: isTypeAttribute(SNode terminal)
{
   ident_t name = terminal.identifier();
   if (emptystr(name))
      name = terminal.findChild(lxTerminal).identifier();

   if (!parameters.exist(name)) {
      ref_t attr = moduleScope->attributes.get(name);

      return attr != 0 && !::isAttribute(attr);
   }
   else return true;
}

bool DerivationReader::DerivationScope :: isAttribute(SNode terminal)
{
   ident_t name = terminal.identifier();
   if (emptystr(name))
      name = terminal.findChild(lxTerminal).identifier();

   ref_t attr = moduleScope->attributes.get(name);

   return attr != 0 && ::isAttribute(attr);
}

bool DerivationReader::DerivationScope :: isImplicitAttribute(SNode terminal)
{
   ident_t name = terminal.identifier();
   if (emptystr(name))
      name = terminal.findChild(lxTerminal).identifier();

   ref_t attr = moduleScope->attributes.get(name);

   return attr == V_GENERIC || attr == V_CONVERSION;
}

ref_t DerivationReader::DerivationScope :: mapTemplate(SNode terminal, int prefixCounter)
{
   int paramCounter = 0;
   if (terminal == lxBaseParent) {
      paramCounter = SyntaxTree::countChild(terminal, lxAttributeValue);
   }
   else if (terminal == lxObject) {
      paramCounter = SyntaxTree::countChild(terminal, lxExpression);
   }
      
   IdentifierString attrName(terminal.findChild(lxIdentifier).findChild(lxTerminal).identifier());
   if (prefixCounter != 0) {
      attrName.append('#');
   }
   attrName.append('#');
   attrName.appendInt(paramCounter + prefixCounter);
 
   ref_t ref = moduleScope->attributes.get(attrName);
   if (!ref)
      raiseError(errInvalidHint, terminal);

   return ref;
}

ref_t DerivationReader::DerivationScope :: mapClassTemplate(SNode terminal)
{
   int paramCounter = 0;
   if (terminal == lxObject) {
      paramCounter = SyntaxTree::countChild(terminal.nextNode(), lxObject);
   }

   IdentifierString attrName(terminal.findChild(lxIdentifier).findChild(lxTerminal).identifier());
   attrName.append('#');
   attrName.appendInt(paramCounter);

   ref_t ref = moduleScope->attributes.get(attrName);
   if (!ref)
      raiseError(errInvalidHint, terminal);

   return ref;
}

void DerivationReader::DerivationScope :: loadAttributeValues(SNode attributes, bool classMode)
{
   SNode current = attributes;
   // load template parameters
   while (current != lxNone) {
      if (current == lxAttributeValue) {
         SNode terminalNode = current.firstChild(lxObjectMask);
         ref_t attr = mapAttribute(terminalNode);
         if (attr == 0) {
            if (classMode)
               // if it is not a declared type - check if it is a class
               attr = mapTerminal(current.findChild(lxIdentifier, lxPrivate), true);

            if (!attr)
               raiseError(errInvalidHint, current);
         }

         this->attributes.add(this->attributes.Count() + 1, attr);
      }
      else if (current == lxExpression || current == lxObject) {
         SNode item = current == lxObject ? current : current.findChild(lxObject);
         if (item != lxNone && item.nextNode() == lxNone) {
            ref_t attr = 0;
            if (classMode) {
               attr = mapTerminal(item.findChild(lxIdentifier), true);
            }
            else attr = mapAttribute(item);
            if (attr == 0) {
               raiseError(errInvalidHint, current);
            }

            this->attributes.add(this->attributes.Count() + 1, attr);
         }
         else raiseError(errInvalidHint, current);
      }
      else if (current == lxClassRefAttr) {
         ref_t attr = attr = mapTerminal(current, true);
         if (attr == 0) {
            raiseError(errInvalidHint, current);
         }

         this->attributes.add(this->attributes.Count() + 1, attr);
      }
      else if (current == lxNameAttr && type == ttFieldTemplate) {
         this->attributes.add(this->attributes.Count() + 1, INVALID_REF);

         identNode = current;

         break;
      }
      else if (current == lxTemplateAttribute) {
         ref_t subject = parent->attributes.get(current.argument);

         this->attributes.add(this->attributes.Count() + 1, subject);
      }
      //else if (prefixMode && current == lxNameAttr)
      //   break;

      current = current.nextNode();
   }
}

void DerivationReader::DerivationScope :: loadParameters(SNode node)
{
   SNode current = node.firstChild();
   // load template parameters
   while (current != lxNone) {
      if (current == lxBaseParent) {               
         ident_t name = current.firstChild(lxTerminalMask).findChild(lxTerminal).identifier();

         parameters.add(name, parameters.Count() + 1);
      }

      current = current.nextNode();
   }
}

void DerivationReader::DerivationScope :: loadFields(SNode node)
{
   SNode current = node;
   // load template parameters
   while (current != lxNone) {
      if (current == lxBaseParent) {
         SNode ident = current.findChild(lxIdentifier);
         ident_t name = ident.findChild(lxTerminal).identifier();

         fields.add(name, fields.Count() + 1);
      }

      current = current.nextNode();
   }
}

int DerivationReader::DerivationScope :: mapParameter(SNode terminal)
{
   ident_t identifier = terminal.identifier();
   if (emptystr(identifier))
      identifier = terminal.findChild(lxTerminal).identifier();

   int index = parameters.get(identifier);
   if (!index) {
      if (parent != NULL) {
         return parent->mapParameter(terminal);
      }
      else return 0;
   }
   else return index;
}

int DerivationReader::DerivationScope :: mapIdentifier(SNode terminal)
{
   ident_t identifier = terminal.identifier();
   if (emptystr(identifier))
      identifier = terminal.findChild(lxTerminal).identifier();

   if (type == DerivationScope::ttFieldTemplate) {
      return fields.get(identifier);
   }
   else if (type == DerivationScope::ttCodeTemplate) {
      return parameters.get(identifier);
   }
   else return 0;
}

void DerivationReader::DerivationScope :: copySubject(SyntaxWriter& writer, SNode terminal)
{
   int index = mapParameter(terminal);
   if (index) {
      writer.newNode(lxTemplateParam, index);
      copyIdentifier(writer, terminal);
      writer.closeNode();
   }
   else if (type == DerivationScope::ttMethodTemplate) {
      ident_t identifier = terminal.identifier();
      if (emptystr(identifier))
         identifier = terminal.findChild(lxTerminal).identifier();

      index = fields.get(identifier);
      if (index != 0) {
         writer.newNode(lxTemplateMethod, parameters.Count() + index);
         copyIdentifier(writer, terminal);
         writer.closeNode();
      }
      else copyIdentifier(writer, terminal);
   }
   else copyIdentifier(writer, terminal);
}

void DerivationReader::DerivationScope :: copyIdentifier(SyntaxWriter& writer, SNode terminal)
{
   ::copyIdentifier(writer, terminal);
}

bool DerivationReader::DerivationScope :: generateClassName()
{
   ident_t templateName = moduleScope->module->resolveReference(templateRef);

   NamespaceName rootNs(templateName);
   IdentifierString name;
   name.append(templateName);

   SubjectMap::Iterator it = attributes.start();
   while (!it.Eof()) {
      name.append('&');

      ident_t param = moduleScope->module->resolveReference(*it);
      if (NamespaceName::compare(param, rootNs)) {
         name.append(param + getlength(rootNs) + 1);
      }
      else name.append(param);

      it++;
   }
   name.replaceAll('\'', '@', 0);

   reference = moduleScope->mapTemplateClass(name);

   return moduleScope->mapSection(reference | mskVMTRef, true) == NULL;
}

_Memory* DerivationReader::DerivationScope :: loadTemplateTree()
{
   ref_t ref = templateRef;

   _Module* argModule = moduleScope->loadReferenceModule(ref);

   return argModule ? argModule->mapSection(ref | mskSyntaxTreeRef, true) : NULL;
}

// --- DerivationReader ---

DerivationReader ::DerivationReader(SyntaxTree& tree)
{
   _root = tree.readRoot();

   ByteCodeCompiler::loadVerbs(_verbs);
}

bool DerivationReader :: isVerbAttribute(SNode attribute)
{
   if (attribute != lxAttribute)
      return false;

   return _verbs.exist(attribute.findChild(lxIdentifier).findChild(lxTerminal).identifier());
}

void DerivationReader :: copyExpressionTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   if (node.strArgument != -1) {
      writer.newNode(node.type, node.identifier());
   }
   else writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      copyTreeNode(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

void DerivationReader :: copyTreeNode(SyntaxWriter& writer, SNode current, DerivationScope& scope/*, bool methodMode*/)
{
   if (test(current.type, lxTerminalMask | lxObjectMask)) {
      scope.copyIdentifier(writer, current);
   }
   else if (current == lxTemplate) {
      writer.appendNode(lxTemplate, scope.templateRef);
   }
   else if (current == lxTemplateParam) {
      if (scope.type == DerivationScope::ttCodeTemplate && current.argument == 1) {
         // if it is a code template parameter
         DerivationScope* parentScope = scope.parent;

         /*if (scope.exprNode == lxNone) {
            writer.appendNode(lxNil);
         }
         else */generateExpressionTree(writer, scope.exprNode, *parentScope, true);
      }
      else if (scope.type == DerivationScope::ttCodeTemplate && current.argument == 0) {
         // if it is a code template parameter
         DerivationScope* parentScope = scope.parent;

         generateCodeTree(writer, scope.codeNode, *parentScope);
      }
      else if (scope.type == DerivationScope::ttCodeTemplate && current.argument == 3) {
         DerivationScope* parentScope = scope.parent;

         // if it is an else code template parameter
         SNode subParam = current.findSubNode(lxTemplateParam);
         if (subParam == lxTemplateParam && subParam.argument == 0) {
            // HOTFIX : insert if-else code
            generateCodeTree(writer, scope.codeNode, *parentScope);
         }

         generateCodeTree(writer, scope.elseNode, *parentScope);
      }
      else if (scope.type == DerivationScope::ttCodeTemplate && current.argument == 2) {
         // if it is a code template parameter
         DerivationScope* parentScope = scope.parent;

         writer.newBookmark();
         generateObjectTree(writer, scope.nestedNode, *parentScope);
         writer.removeBookmark();
      }
      else if (current.argument == INVALID_REF) {
         ref_t attrRef = scope.moduleScope->attributes.get(current.findChild(lxTemplate).identifier());

         DerivationScope templateScope(&scope, attrRef);
         templateScope.loadAttributeValues(current.firstChild()/*, true*/);

         SyntaxTree buffer;
         SyntaxWriter bufferWriter(buffer);
         generateTemplate(bufferWriter, templateScope, true);
         copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

         writer.newNode(lxReference, scope.moduleScope->module->resolveReference(templateScope.reference));
         copyIdentifier(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
      else {
         // if it is a template parameter
         ref_t attrRef = scope.attributes.get(current.argument);
         if (attrRef == INVALID_REF && (scope.type == DerivationScope::ttFieldTemplate || scope.type == DerivationScope::ttMethodTemplate)) {
            copyIdentifier(writer, scope.identNode.firstChild(lxTerminalMask));
         }
         else {
            ident_t attrName = scope.moduleScope->module->resolveReference(attrRef);
            //if (subjName.find('$') != NOTFOUND_POS) {
            writer.newNode(lxReference, attrName);
            /*}
            else writer.newNode(lxIdentifier, subjName);*/

            SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
            writer.closeNode();
         }
      }
   }
   else if (current == lxTemplateField && current.argument >= 0) {
      ident_t fieldName = retrieveIt(scope.fields.start(), current.argument).key();

      writer.newNode(lxIdentifier, fieldName);

      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
      writer.closeNode();
   }
   else if (current == lxTemplateMethod && current.argument >= 0) {
      ident_t methodName = retrieveIt(scope.fields.start(), current.argument - scope.attributes.Count()).key();

      writer.newNode(lxIdentifier, methodName);

      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
      writer.closeNode();
   }
//   else if (current == lxTemplateType) {
//      ref_t subjRef = scope.subjects.get(current.argument);
//      ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
//      writer.newNode(lxTypeAttr, subjName);
//
//      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//      writer.closeNode();
//   }
//   else if (current == lxTypeAttr || current == lxClassRefAttr) {
//      writer.appendNode(current.type, current.identifier());
//   }
   else if (current == lxTemplateVar && current.argument == INVALID_REF) {
      ref_t attrRef = scope.moduleScope->attributes.get(current.findChild(lxTemplate).identifier());

      DerivationScope templateScope(&scope, attrRef);
      templateScope.loadAttributeValues(current.firstChild()/*, true*/);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);
      generateTemplate(bufferWriter, templateScope, true);
      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      writer.newNode(lxVariable);
      writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
      copyIdentifier(writer, current.findChild(lxIdentifier));
      writer.closeNode();
   }
   else if (current == lxTemplateBoxing) {
      ref_t attrRef = scope.moduleScope->attributes.get(current.findChild(lxTemplate).identifier());

      DerivationScope templateScope(&scope, attrRef);
      templateScope.loadAttributeValues(current.firstChild()/*, true*/);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);
      generateTemplate(bufferWriter, templateScope, true);
      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      writer.newNode(lxBoxing);
      writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
      copyIdentifier(writer, current.findChild(lxIdentifier));
      writer.closeNode();

      writer.newNode(lxExpression);
      generateExpressionTree(writer, current.findChild(lxReturning), scope, 0);
      writer.closeNode();

      writer.closeNode();
   }
   else if (current == lxTemplateAttribute) {
      ref_t classRef = scope.attributes.get(current.argument);
      ident_t subjName = scope.moduleScope->module->resolveReference(classRef);
      writer.newNode(lxClassRefAttr, subjName);

      SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
      writer.closeNode();
   }
   else copyExpressionTree(writer, current, scope);
}

void DerivationReader :: copyFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxIdentifier || current == lxPrivate || current == lxReference) {
         copyIdentifier(writer, current);
      }
      else if (current == lxTemplateParam && current.argument == INVALID_REF) {
         ref_t attrRef = scope.moduleScope->attributes.get(current.findChild(lxTemplate).identifier());

         DerivationScope templateScope(&scope, attrRef);
         templateScope.loadAttributeValues(current.firstChild()/*, true*/);

         SyntaxTree tempBuffer;
         SyntaxWriter bufferWriter(tempBuffer);
         generateTemplate(bufferWriter, templateScope, true);
         copyAutogeneratedClass(tempBuffer, *scope.autogeneratedTree);

         writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
         copyIdentifier(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
//      else if (current == lxTemplateParam) {
//         ref_t subjRef = scope.subjects.get(current.argument);
//         ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
//         if (subjName.find('$') != NOTFOUND_POS) {
//            writer.newNode(lxReference, subjName);
//         }
//         else writer.newNode(lxIdentifier, subjName);
//
//         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//         writer.closeNode();
//      }
      else if (current == lxTemplateField && current.argument >= 0) {
         ident_t fieldName = retrieveIt(scope.fields.start(), current.argument).key();

         writer.newNode(lxIdentifier, fieldName);

         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
      else if (current == lxTemplateAttribute) {
         ref_t classRef = scope.attributes.get(current.argument);
         ident_t subjName = scope.moduleScope->module->resolveReference(classRef);
         writer.newNode(lxClassRefAttr, subjName);

         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
         writer.closeNode();
      }
      else if (current == lxClassRefAttr) {
         writer.appendNode(current.type, current.identifier());
      }
      else if (current == lxAttribute)
         writer.appendNode(current.type, current.argument);

      current = current.nextNode();
   }

   writer.closeNode();

   SyntaxTree::moveNodes(writer, buffer, lxClassMethod, lxClassField);
}

void DerivationReader :: copyFieldInitTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxMemberIdentifier) {
         copyIdentifier(writer, current);
      }
      else copyExpressionTree(writer, current, scope);

      current = current.nextNode();
   }

   writer.closeNode();
}

void DerivationReader :: copyMethodTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SyntaxTree& buffer)
{
   writer.newNode(node.type, node.argument);

   SNode current = node.firstChild();
   while (current != lxNone) {
      copyTreeNode(writer, current, scope/*, true*/);

      current = current.nextNode();
   }

   writer.closeNode();

   SyntaxTree::moveNodes(writer, buffer, lxClassMethod);
}

void DerivationReader :: copyTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributeValues)
{
   scope.loadAttributeValues(attributeValues, true);

   if (generateTemplate(writer, scope, false)) {
      //if (/*variableMode && */scope.reference != 0)
      //   writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(scope.reference));
   }
   else scope.raiseError(errInvalidHint, node);
}

void DerivationReader :: copyTemplateAttributeTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   SNode current = node.firstChild();
   // validare template parameters
   while (current != lxNone) {
      if (current == lxExpression || current == lxObject) {
         SNode objectNode = current == lxExpression ? current.findChild(lxObject) : current;
         int paramIndex = 0;
         ref_t attrRef = scope.mapAttribute(objectNode.findChild(lxPrivate, lxIdentifier), paramIndex);
         if (attrRef == INVALID_REF) {
            writer.appendNode(lxTemplateAttribute, paramIndex);
         }
         else if (isAttribute(attrRef)) {
            writer.newNode(lxAttribute, attrRef);
            copyIdentifier(writer, current.findChild(lxIdentifier));
            writer.closeNode();
         }
         else if (attrRef != 0) {
            writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(attrRef));
            copyIdentifier(writer, current.firstChild(lxTerminalMask));
            writer.closeNode();
         }
         else {
            writer.newNode(lxAttributeValue);
            copyIdentifier(writer, objectNode.findChild(lxPrivate, lxIdentifier));
            writer.closeNode();
         }
      }
      else if (current == lxAttributeValue) {
         int paramIndex = 0;
         ref_t attrRef = scope.mapAttribute(current.findChild(lxPrivate, lxIdentifier), paramIndex);
         if (attrRef == INVALID_REF) {
            writer.appendNode(lxTemplateAttribute, paramIndex);
         }
         else if (attrRef != 0) {
            writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(attrRef));
            copyIdentifier(writer, current.firstChild(lxTerminalMask));
            writer.closeNode();
         }
      }

      current = current.nextNode();
   }
}

bool DerivationReader :: generateTemplate(SyntaxWriter& writer, DerivationScope& scope, bool declaringClass)
{
   _Memory* body = scope.loadTemplateTree();
   if (body == NULL)
      return false;
   
   SyntaxTree templateTree(body);
   SNode root = templateTree.readRoot();

   if (declaringClass) {
      // HOTFIX : exiting if the class was already declared in this module
      if (!scope.generateClassName())
         return true;

      writer.newNode(lxClass, -1);
      writer.appendNode(lxReference, scope.moduleScope->module->resolveReference(scope.reference));
      writer.appendNode(lxAttribute, V_SEALED);
   }

   SyntaxTree buffer;

   SNode current = root.firstChild();
   while (current != lxNone) {
      if (current == lxAttribute) {
//         if (current.argument == INVALID_REF) {
//            TemplateScope templateScope(&scope, scope.moduleScope->module->mapSubject(current.findChild(lxReference).identifier(), false));
//
//            copyTemplateTree(writer, current, templateScope, current.firstChild()/*, false, embeddableMode*/);
//         }
         /*else */if (current.argument == V_TEMPLATE/* && scope.type != TemplateScope::ttAttrTemplate*/) {
            // ignore template attributes
         }
         else if (current.argument == V_FIELD/* && scope.type != TemplateScope::ttAttrTemplate*/) {
            // ignore template attributes
         }
         else if (current.argument == V_METHOD) {
            if (scope.type == DerivationScope::ttFieldTemplate) {
               // HOTFIX : is it is a method template, consider the field name as a message subject
               scope.type = DerivationScope::ttMethodTemplate;

               //ForwardMap::Iterator it = scope.fields.start();
               //while (!it.Eof()) {
               //   scope.subjects.add(scope.subjects.Count() + 1, scope.moduleScope->module->mapSubject(it.key(), false));

               //   it++;
               //}

               //scope.subjects.add()
            }
         }
         else {
            writer.newNode(current.type, current.argument);
            SyntaxTree::copyNode(writer, current);
            writer.closeNode();
         }
      }
      else if (current == lxTemplateParent) {
         // HOTFIX : class based template
         writer.newNode(lxBaseParent, -1);
         SyntaxTree::copyNode(writer, current);
         writer.closeNode();
      }
////      else if (current == lxTemplateType) {
////         ref_t subjRef = scope.subjects.get(current.argument);
////         ident_t subjName = scope.moduleScope->module->resolveSubject(subjRef);
////         writer.newNode(lxTypeAttr, subjName);
////
////         SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
////         writer.closeNode();
////      }
      else if (current == lxClassMethod) {
         copyMethodTree(writer, current, scope, buffer);
      }
      else if (current == lxClassField) {
         if (current.argument == -1 && current.existChild(lxTemplateMethod)) {
            // ignore virtual method declaration
         }
         else copyFieldTree(writer, current, scope, buffer);
      }
      else if (current == lxFieldInit) {
         writer.newNode(lxFieldInit);
         copyIdentifier(writer, current.findChild(lxMemberIdentifier));
         writer.closeNode();

         SyntaxWriter initWriter(*scope.autogeneratedTree);
         copyFieldInitTree(initWriter, current, scope);
      }
      //      else if (current == lxTemplateField && current.argument == INVALID_REF) {
//         ref_t attrRef = scope.moduleScope->module->mapSubject(current.findChild(lxReference).identifier(), false);
//            
//         TemplateScope templateScope(&scope, attrRef);
//         templateScope.loadAttributeValues(current.firstChild()/*, true*/);
//
//         SyntaxWriter bufferWriter(buffer);
//         generateTemplate(bufferWriter, templateScope, true);
//
//         copyAutogeneratedClass(buffer, *scope.autogeneratedTree);
//
//         writer.newNode(lxClassField);
//         writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
//         if (scope.type == TemplateScope::ttFieldTemplate) {
//            SNode field = current.findChild(lxTemplateField);
//            if (field != lxNone) {
//               ident_t fieldName = retrieveIt(scope.fields.start(), field.argument).key();
//
//               writer.newNode(lxIdentifier, fieldName);
//               SyntaxTree::copyNode(writer, current.findChild(lxIdentifier));
//               writer.closeNode();
//            }
//            else copyIdentifier(writer, current.findChild(lxIdentifier));
//         }
//         else copyIdentifier(writer, current.findChild(lxIdentifier));
//         writer.closeNode();
//      }
      current = current.nextNode();
   }

   if (declaringClass) {
      writer.closeNode();
   }

   return true;
}

void DerivationReader :: generateAttributes(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, bool templateMode)
{
   SNode current = attributes;
   while (current == lxAttribute || current == lxAttributeDecl) {
      int paramIndex = 0;
      ref_t attrRef = current.argument;
      if (!attrRef)
         attrRef = scope.mapAttribute(current, paramIndex);

      if (attrRef == V_ATTRTEMPLATE) {
         generateAttributeTemplate(writer, current, scope, templateMode);
      }
      else if (attrRef == INVALID_REF) {
         writer.appendNode(lxTemplateAttribute, paramIndex);
      }
      else if (isAttribute(attrRef)) {
         writer.newNode(lxAttribute, attrRef);
         copyIdentifier(writer, current.findChild(lxIdentifier));
         writer.closeNode();
         
         if (attrRef == V_TEMPLATE && current.existChild(lxBaseParent)) {
            //HOTFIX : check if it is template based on the class
            writer.newNode(lxTemplateParent);
            copyIdentifier(writer, current.findChild(lxBaseParent).firstChild(lxTerminalMask));
            writer.closeNode();
         }
      }
      else if (attrRef != 0) {
         writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(attrRef));
         copyIdentifier(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }
      else scope.raiseError(errInvalidHint, current);

      current = current.nextNode();
   }

   if (current == lxNameAttr && node.existChild(lxSize)) {
      SNode sizeNode = node.findChild(lxSize).findChild(lxInteger, lxHexInteger);
      if (sizeNode != lxNone) {
         writer.appendNode(lxSize, readSizeValue(sizeNode, sizeNode == lxHexInteger ? 16 : 10));
      }
      else scope.raiseError(errInvalidHint, current);
   }

   if (node != lxNone) {
      SNode nameNode = current == lxNameAttr ? current.findChild(lxIdentifier, lxPrivate) : node.findChild(lxIdentifier, lxPrivate);
      if (nameNode != lxNone)
         scope.copySubject(writer, nameNode);
   }
}

void DerivationReader :: generateSwitchTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxSwitchOption:
         case lxBiggerSwitchOption:
         case lxLessSwitchOption:
            if (current.type == lxBiggerSwitchOption) {
               writer.newNode(lxOption, GREATER_MESSAGE_ID);
            }
            else if (current.type == lxLessSwitchOption) {
               writer.newNode(lxOption, LESS_MESSAGE_ID);
            }
            else writer.newNode(lxOption, EQUAL_MESSAGE_ID);
            generateExpressionTree(writer, current, scope, false);
            writer.closeNode();
            break;
         case lxLastSwitchOption:
            writer.newNode(lxElse);
            generateExpressionTree(writer, current, scope, false);
            writer.closeNode();
            break;
         default:
            break;
      }

      current = current.nextNode();
   }
}

void DerivationReader :: generateClosureTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   SNode current = node.firstChild();

   // COMPILER MAGIC : advanced closure syntax
   writer.newBookmark();

   do {
      bool closureMode = false;
      if (current == lxMessage) {
         closureMode = true;

         writer.newNode(lxClosureMessage);
         copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
         writer.closeNode();

         current = current.nextNode();
      }

      if (closureMode) {
         generateObjectTree(writer, current, scope);
      }
      else generateExpressionTree(writer, current, scope, 0);

      current = current.nextNode();
   } while (current.compare(lxMessage, lxMethodParameter));

   if (current.compare(lxCode, lxReturning)) {
      generateObjectTree(writer, current, scope);
   }
   writer.removeBookmark();
}

void DerivationReader:: generateMessageTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxObject:
         case lxMessageParameter:
            generateExpressionTree(writer, current, scope, EXPRESSION_MESSAGE_MODE);
            break;
         case lxExpression:
            generateExpressionTree(writer, current, scope, EXPRESSION_EXPLICIT_MODE | EXPRESSION_MESSAGE_MODE);
            break;
         case lxExtern:
            generateCodeTree(writer, current, scope);
            break;
         case lxCode:
            generateCodeTree(writer, current, scope);
            if (scope.type == DerivationScope::ttCodeTemplate) {
               if (scope.parameters.Count() == 2) {
                  if (scope.codeNode == lxNone) {
                     writer.insert(lxTemplateParam);
                     scope.codeNode = current;
                  }
                  else {
                     writer.insert(lxTemplateParam, 3);
                     scope.codeNode = SNode();
                  }
               }
               else writer.insert(lxTemplateParam);
               writer.closeNode();
            }

            writer.insert(lxExpression);
            writer.closeNode();
            break;
         case lxInlineClosure:
            // COMPILER MAGIC : advanced closure syntax
            generateClosureTree(writer, current, scope);
            break;
         case lxMessage:
            writer.newNode(lxMessage);
            if (!current.existChild(lxAttributeValue)) {
               scope.copySubject(writer, current.firstChild(lxTerminalMask));
            }
            else generateAttributeTemplate(writer, current, scope, scope.reference == INVALID_REF);
            writer.closeNode();
            break;
         case lxIdentifier:
         case lxPrivate:
         case lxReference:
            writer.newNode(lxMessage);
            scope.copySubject(writer, current);
            writer.closeNode();
            break;
      }
      current = current.nextNode();
   }
}

void DerivationReader :: generateObjectTree(SyntaxWriter& writer, SNode current, DerivationScope& scope)
{
   switch (current.type) {
      case lxAssigning:
         writer.appendNode(lxAssign);
         generateExpressionTree(writer, current, scope, 0);
         break;
      case lxSwitching:
         generateSwitchTree(writer, current, scope);
         writer.insert(lxSwitching);
         writer.closeNode();
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxOperator:
         copyOperator(writer, current.firstChild(), current.argument);
         generateExpressionTree(writer, current, scope, 0);
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxCatchOperation:
      case lxAltOperation:
         if (scope.type == DerivationScope::ttCodeTemplate && scope.templateRef == 0) {
            // HOTFIX : for try-catch template
            scope.codeNode = SNode();
         }
         writer.newBookmark();
      case lxMessage:
         if (current.argument == -1 && current.nextNode() == lxMethodParameter) {
            writer.newNode(lxClosureMessage);
            copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
            writer.closeNode();
         }
         else {
            generateMessageTree(writer, current, scope/*, false*/);

            writer.insert(lxExpression);
            writer.closeNode();
         }
         if (current == lxCatchOperation) {
            writer.removeBookmark();
            writer.insert(lxTrying);
            writer.closeNode();
         }
         else if (current == lxAltOperation) {
            writer.removeBookmark();
            writer.insert(lxAlt);
            writer.closeNode();
         }
         break;
      case lxExtension:
         writer.newNode(current.type, current.argument);
         generateExpressionTree(writer, current, scope, 0);
         writer.closeNode();
         break;
      case lxExpression:
         generateExpressionTree(writer, current, scope, 0);
         break;
      case lxMessageReference:
      case lxLazyExpression:
         writer.newNode(lxExpression);
         writer.newNode(current.type);
         if (current == lxLazyExpression) {
            generateExpressionTree(writer, current, scope, 0);
         }
         else if (scope.type == DerivationScope::ttFieldTemplate) {
            scope.copySubject(writer, current.findChild(lxIdentifier, lxPrivate, lxLiteral));
         }
         else copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate, lxLiteral));
         writer.closeNode();
         writer.closeNode();
         break;
      case lxObject:
         generateExpressionTree(writer, current, scope, 0);
         break;
      case lxNestedClass:
         if (scope.type == DerivationScope::ttCodeTemplate) {
            writer.insert(lxTemplateParam, 2);
            writer.closeNode();
         }
         else {
            generateScopeMembers(current, scope);

            generateClassTree(writer, current, scope, SNode(), -1);
         }
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxReturning:
         writer.newNode(lxCode);
      case lxCode:
         generateCodeTree(writer, current, scope);
         if (current == lxReturning) {
            writer.closeNode();
         }
         else if (scope.type == DerivationScope::ttCodeTemplate) {
            if (scope.parameters.Count() == 2) {
               if (scope.codeNode == lxNone) {
                  writer.insert(lxTemplateParam);
                  scope.codeNode = current;
               }
               else {
                  writer.insert(lxTemplateParam, 3);
                  scope.codeNode = SNode();
               }
            }
            else writer.insert(lxTemplateParam);
            writer.closeNode();
         }
         writer.insert(lxExpression);
         writer.closeNode();
         break;
      case lxMethodParameter:
         writer.newNode(lxMethodParameter);
         copyIdentifier(writer, current.findChild(lxIdentifier, lxPrivate));
         writer.closeNode();
         break;
      default:
      {
         if (isTerminal(current.type)) {
            if (scope.type == DerivationScope::ttFieldTemplate) {
               int index = scope.mapIdentifier(current);
               if (index != 0) {
                  writer.newNode(lxTemplateField, index);
                  copyIdentifier(writer, current);
                  writer.closeNode();
               }
               else copyIdentifier(writer, current);
            }
            else if (scope.type == DerivationScope::ttCodeTemplate && scope.mapIdentifier(current)) {
               writer.newNode(lxTemplateParam, 1);
               copyIdentifier(writer, current);
               writer.closeNode();
            }
            else copyIdentifier(writer, current);
         }               
         break;
      }
   }
}

inline bool checkNode(SNode node, LexicalType type, ref_t argument)
{
   return node == type && node.argument == argument;
}

void DerivationReader :: generateNewOperator(SyntaxWriter& writer, SNode terminal, DerivationScope& scope)
{
   if (!test(terminal.type, lxTerminalMask))
      scope.raiseError(errInvalidSyntax, terminal);

   scope.copySubject(writer, terminal);

   writer.appendNode(lxOperator, -1);
   generateExpressionTree(writer, terminal.nextNode().findChild(lxExpression), scope, 0);
   writer.insert(lxExpression);
   writer.closeNode();
}

void DerivationReader :: generateNewTemplate(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode)
{
   IdentifierString attrName;

   SNode expr;
   SNode attr = node.findChild(lxObject).findChild(lxIdentifier, lxPrivate);
   if (attr == lxNone)
      attr = node.findChild(lxIdentifier, lxPrivate);

   SNode current = node.findChild(lxOperator);
   ref_t attrRef = 0;
   ref_t typeRef = 0;
   bool classMode = false;
   if (current.findChild(lxObject).existChild(lxAngleOperator)) {
      classMode = true;

      SNode typeNode = current.findChild(lxObject);

      SNode operatorNode = typeNode.findChild(lxAngleOperator);
      expr = operatorNode.findChild(lxReturning);

      int prefixCounter = SyntaxTree::countChild(current, lxObject);
      if (scope.mapAttribute(attr/*, true*/) == V_TYPETEMPL && prefixCounter == 1) {
         attrRef = V_TYPETEMPL;

         current = typeNode;
      }
      else {
         attrName.copy(attr.findChild(lxTerminal).identifier());
         attrName.append('#');
         attrName.appendInt(prefixCounter);

         attrRef = scope.moduleScope->attributes.get(attrName);
      }
   }

   if (!attrRef)
      scope.raiseError(errInvalidHint, node);

   // if it is an autogenerated class
   if (templateMode) {
      // template in template should be copied "as is" (resolving all references)
      writer.newNode(lxTemplateBoxing, -1);
      writer.appendNode(lxTemplate, attrName.c_str());

      if (classMode) {
         copyTemplateAttributeTree(writer, current, scope);
      }
      else copyTemplateAttributeTree(writer, node, scope);
      copyIdentifier(writer, attr);      

      writer.newNode(lxReturning);
      SyntaxTree::copyNode(writer, expr);
      writer.closeNode();

      writer.closeNode();

      return;
   }
   else if (attrRef == V_TYPETEMPL) {
      attr = current.findChild(lxIdentifier, lxPrivate, lxReference);

      typeRef = scope.mapTerminal(attr, true);
      if (typeRef == 0)
         typeRef = scope.mapTerminal(attr);
   }
   else {
      DerivationScope templateScope(&scope, attrRef);
      if (classMode) {
         templateScope.loadAttributeValues(current.findChild(lxObject), true);
      }
      else templateScope.loadAttributeValues(node.findChild(lxExpression), false);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);
      generateTemplate(bufferWriter, templateScope, true);

      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      typeRef = templateScope.reference;
   }

   writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(typeRef));
   copyIdentifier(writer, attr);
   writer.closeNode();

   writer.newNode(lxExpression);
   generateExpressionTree(writer, expr, scope, 0);
   writer.closeNode();

   writer.insert(lxBoxing);
   writer.closeNode();
}

void DerivationReader :: generateExpressionTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, int mode)
{
   writer.newBookmark();

   SNode current = node.firstChild();
   SNode next = current.nextNode();
   bool identifierMode = current.type == lxIdentifier;
   bool listMode = false;   
   if (checkNode(next, lxOperator, (ref_t)-3)) {
      // check if it is new operator
      generateNewOperator(writer, current, scope);
   }
   else if (next == lxOperator && next.findChild(lxObject).existChild(lxAngleOperator)) {
      // check if it is new template
      generateNewTemplate(writer, node, scope, scope.reference == INVALID_REF);
   }
   else {
      while (current != lxNone) {
         if (current == lxExpression) {
            // HOTFIX : to supper collection of one element
            if ((current.nextNode() == lxExpression || (identifierMode && current.nextNode() == lxNone)) && !test(mode, EXPRESSION_MESSAGE_MODE))
               listMode = true;

            if (identifierMode) {
               generateExpressionTree(writer, current, scope, listMode ? EXPRESSION_EXPLICIT_MODE : 0);
            }
            else if (listMode) {
               generateExpressionTree(writer, current, scope, EXPRESSION_EXPLICIT_MODE);
            }
            else generateObjectTree(writer, current, scope);
         }
         else if (listMode && (current == lxMessage || current == lxOperator)) {
            // HOTFIX : if it is an operation with a collection
            listMode = false;
            writer.insert(lxExpression);
            writer.closeNode();

            generateObjectTree(writer, current, scope);
         }
         else generateObjectTree(writer, current, scope);

         current = current.nextNode();
      }
   }

   if (listMode) {
      writer.insert(lxExpression);
      writer.closeNode();
   }

   if (test(mode, EXPRESSION_EXPLICIT_MODE)) {
      writer.insert(node.type);
      writer.closeNode();
   }

   writer.removeBookmark();
}

void DerivationReader:: generateSymbolTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   writer.newNode(lxSymbol);

   generateAttributes(writer, node, scope, attributes, false);

   generateExpressionTree(writer, node.findChild(lxExpression), scope);

   writer.closeNode();
}

void DerivationReader :: generateVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   // check if the first token is attribute
   SNode current = node.firstChild();
   SNode attr = node.findChild(lxIdentifier, lxPrivate, lxMemberIdentifier);
   SNode nextNode = attr.nextNode();
   ref_t attrRef = (attr == lxIdentifier) ? scope.mapAttribute(attr/*, true*/) : 0;
   if (attrRef != 0 && nextNode != lxAssigning) {
      // HOTFIX : set already recognized attribute value if it is not a template parameter
      if (attrRef != INVALID_REF) {
         attr.setArgument(attrRef);
      }

      while (current != lxAssigning && current != lxAttributeValue) {
         current = lxAttribute;

         current = current.nextNode();
      }

      writer.newNode(lxVariable);

      int size = 0;
      if (current == lxAttributeValue) {
         current = lxNameAttr;
         current = current.nextNode();
         size = -1;
      }
      else setIdentifier(node.firstChild());

      SNode ident = node.findChild(lxNameAttr);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);

      generateAttributes(bufferWriter, SNode(), scope, node.firstChild(), scope.reference == INVALID_REF);
      SyntaxTree::moveNodes(writer, buffer, lxAttribute, lxIdentifier, lxPrivate, lxTemplateParam, lxClassRefAttr, lxTemplateAttribute);

//      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      copyIdentifier(writer, ident.findChild(lxIdentifier, lxPrivate));

      if (size == -1) {
         writer.appendNode(lxAttribute, V_OBJARRAY);
      }

      writer.closeNode();

      writer.newNode(lxExpression);

      copyIdentifier(writer, ident.findChild(lxIdentifier, lxPrivate));
      writer.appendNode(lxAssign);
      generateExpressionTree(writer, current, scope, 0);

      writer.closeNode();
   }
   else generateExpressionTree(writer, node, scope);
}

void DerivationReader :: generateAttributeTemplate(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode)
{
   IdentifierString attrName;

   ref_t attrRef = 0;
   ref_t typeRef = 0;
   int prefixCounter = SyntaxTree::countChild(node, lxAttributeValue);
   if (scope.mapAttribute(node/*, true*/) == V_TYPETEMPL && prefixCounter == 1) {
      attrRef = V_TYPETEMPL;
   }
   else {
      attrName.copy(node.findChild(lxIdentifier, lxPrivate).findChild(lxTerminal).identifier());
      attrName.append('#');
      attrName.appendInt(prefixCounter);

      attrRef = scope.moduleScope->attributes.get(attrName);
   }

   if (templateMode) {
      // template in template should be copied "as is" (resolving all references)
      writer.newNode(lxTemplateParam, -1);
      writer.appendNode(lxTemplate, attrName.c_str());

      copyTemplateAttributeTree(writer, node, scope);

      writer.closeNode();

      return;
   }
   else if (attrRef == V_TYPETEMPL) {
      SNode attr = node.findChild(lxTemplateAttribute);

      typeRef = scope.mapTerminal(attr, true);
      if (typeRef == 0)
         typeRef = scope.mapTerminal(attr);
   }
   else {
      DerivationScope templateScope(&scope, attrRef);
      templateScope.loadAttributeValues(node.findChild(lxAttributeValue), true);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);
      generateTemplate(bufferWriter, templateScope, true);

      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      typeRef = templateScope.reference;
   }
   if (node == lxAttribute) {
      writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(typeRef));
   }
   else writer.newNode(lxReference, scope.moduleScope->module->resolveReference(typeRef));
   copyIdentifier(writer, node);
   writer.closeNode();
}

void DerivationReader :: generateTemplateVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, bool templateMode)
{
   IdentifierString attrName;

   SNode ident;
   SNode expr;
   SNode attr = node.findChild(lxIdentifier, lxPrivate);

   SNode current = node.findChild(lxOperator);
   ref_t attrRef = 0;
   ref_t typeRef = 0;
   int size = 0;
   bool classMode = false;
   if (current.findChild(lxObject).existChild(lxAngleOperator)) {
      classMode = true;

      SNode typeNode = current.findChild(lxObject);

      SNode operatorNode = typeNode.findChild(lxAngleOperator);
      ident = operatorNode.findChild(lxIdentifier, lxPrivate);
      if (ident == lxNone) {
         ident = operatorNode.findChild(lxAttributeValue).findChild(lxIdentifier, lxPrivate);
         size = -1;
      }         

      expr = operatorNode.findChild(lxAssigning, lxSize);

      int prefixCounter = SyntaxTree::countChild(current, lxObject);
      if (scope.mapAttribute(attr/*, true*/) == V_TYPETEMPL && prefixCounter == 1) {
         attrRef = V_TYPETEMPL;

         current = typeNode;
      }
      else {
         attrName.copy(attr.findChild(lxTerminal).identifier());
         attrName.append('#');
         attrName.appendInt(prefixCounter);

         attrRef = scope.moduleScope->attributes.get(attrName);
      }
   }

   if (!attrRef)
      scope.raiseError(errInvalidHint, node);

   if (expr == lxSize) {
      SNode sizeNode = expr.firstChild(lxTerminalMask);
      if (sizeNode == lxInteger) {
         size = sizeNode.findChild(lxTerminal).identifier().toInt();
      }
      else if (sizeNode == lxHexInteger) {
         size = sizeNode.findChild(lxTerminal).identifier().toLong(16);
      }
      if (size <= 0)
         scope.raiseError(errInvalidHint, node);
   }


   // if it is an autogenerated class
   if (templateMode) {
      // template in template should be copied "as is" (resolving all references)
      writer.newNode(lxTemplateVar, -1);
      writer.appendNode(lxTemplate, attrName.c_str());

      if (classMode) {
         copyTemplateAttributeTree(writer, current, scope);
      }
      else copyTemplateAttributeTree(writer, node, scope);
      copyIdentifier(writer, ident);      
      
      if (size != 0) {
         writer.appendNode(lxAttribute, size);
      }
      else {
         writer.closeNode();

         writer.newNode(lxExpression);

         copyIdentifier(writer, ident);
         writer.appendNode(lxAssign);
         generateExpressionTree(writer, expr.findChild(lxExpression), scope, 0);
      }

      writer.closeNode();

      return;
   }
   else if (attrRef == V_TYPETEMPL) {
      attr = current.findChild(lxIdentifier, lxPrivate, lxReference);

      typeRef = scope.mapTerminal(attr, true);
      if (typeRef == 0)
         typeRef = scope.mapTerminal(attr);
   }
   else {
      DerivationScope templateScope(&scope, attrRef);
      if (classMode) {
         templateScope.loadAttributeValues(current.findChild(lxObject), true);
      }
      else templateScope.loadAttributeValues(node.findChild(lxExpression), false);

      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);
      generateTemplate(bufferWriter, templateScope, true);

      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      typeRef = templateScope.reference;
   }
   writer.newNode(lxVariable);
   writer.newNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(typeRef));
   copyIdentifier(writer, attr);
   writer.closeNode();

   copyIdentifier(writer, ident);

   if (size == -1) {
      writer.appendNode(lxAttribute, V_OBJARRAY);
   }
   else if (size != 0) {
      writer.appendNode(lxAttribute, size);
   }

   if (expr == lxAssigning) {
      writer.closeNode();

      writer.newNode(lxExpression);

      copyIdentifier(writer, ident);
      writer.appendNode(lxAssign);
      generateExpressionTree(writer, expr, scope, 0);
   }

   writer.closeNode();
}

void DerivationReader :: generateArrayVariableTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   // check if the first token is attribute
   SNode attr = node.findChild(lxIdentifier, lxPrivate);
   ref_t attrRef = 0;
   SNode ident = node.findChild(lxAttributeValue).findChild(lxIdentifier, lxPrivate);
   SNode size = node.findChild(lxSize).firstChild(lxTerminalMask);

   attrRef = (attr != lxPrivate) ? scope.mapAttribute(attr) : 0;
   if (attrRef != 0 && !isPrimitiveRef(attrRef)) {
      writer.newNode(lxVariable);

      copyIdentifier(writer, ident);

      writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(attrRef));
      if (size == lxInteger) {
         writer.appendNode(lxAttribute, size.findChild(lxTerminal).identifier().toInt());
      }
      else if (size == lxHexInteger) {
         writer.appendNode(lxAttribute, size.findChild(lxTerminal).identifier().toLong(16));
      }

      writer.closeNode();

      return;
   }

   generateExpressionTree(writer, node, scope);
}

bool DerivationReader :: generateTemplateCode(SyntaxWriter& writer, DerivationScope& scope)
{
   _Memory* body = scope.loadTemplateTree();
   if (body == NULL)
      return false;

   SyntaxTree templateTree(body);

   scope.loadAttributeValues(templateTree.readRoot()/*, false*/);

   SNode current = templateTree.readRoot().findChild(lxCode).firstChild();
   while (current != lxNone) {
      if (current.type == lxLoop || current.type == lxExpression || current.type == lxExtern)
         copyExpressionTree(writer, current, scope);

      current = current.nextNode();
   }

   return true;
}

void DerivationReader :: generateCodeTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   // check if the first token is attribute
   SNode loperand = node.firstChild();
   SNode attr = node.findChild(lxIdentifier);
   if (attr != lxNone) {
      IdentifierString attrName(attr.findChild(lxTerminal).identifier());
      attrName.append('#');
      attrName.appendInt(SyntaxTree::countChild(node, lxCode, lxNestedClass));

      ref_t attrRef = scope.moduleScope->attributes.get(attrName);
      if (attrRef != 0) {
         DerivationScope templateScope(&scope, attrRef);
         templateScope.exprNode = node.findChild(lxExpression);
         templateScope.codeNode = node.findChild(lxCode);
         templateScope.nestedNode = node.findChild(lxNestedClass);
         if (templateScope.nestedNode == lxNone || templateScope.codeNode != lxNone) {
            // if there is else code block
            templateScope.elseNode = templateScope.codeNode.nextNode();
         }
            
         templateScope.type = DerivationScope::ttCodeTemplate;
            
         if (!generateTemplateCode(writer, templateScope))
            scope.raiseError(errInvalidHint, node);

         return;
      }
      else scope.raiseError(errInvalidHint, node);
   }   

   generateExpressionTree(writer, node, scope);
}

void DerivationReader :: generateCodeTree(SyntaxWriter& writer, SNode node, DerivationScope& scope)
{
   writer.newNode(node.type, node.argument);

   bool withBreakpoint = (node == lxReturning || node == lxResendExpression);
   if (withBreakpoint)
      writer.newBookmark();

   SNode current = node.firstChild();
   while (current != lxNone) {
      if (current == lxExpression || current == lxReturning) {
         if (isTemplateDeclaration(current)) {
            generateTemplateVariableTree(writer, current, scope, scope.reference == INVALID_REF);
         }
         else if (current.existChild(lxAssigning)) {
            generateVariableTree(writer, current, scope);
         }
         else if (isArrayDeclaration(current)) {
            generateArrayVariableTree(writer, current, scope);
         }
         else if (current.existChild(lxCode) || current.existChild(lxNestedClass)) {
            generateCodeTemplateTree(writer, current, scope);
         }
         else generateExpressionTree(writer, current, scope);
      }
      else if (current == lxEOF) {
         writer.newNode(lxEOF);

         SNode terminal = current.firstChild();
         SyntaxTree::copyNode(writer, lxRow, terminal);
         SyntaxTree::copyNode(writer, lxCol, terminal);
         SyntaxTree::copyNode(writer, lxLength, terminal);
         writer.closeNode();
      }
      else if (current == lxLoop || current == lxCode || current == lxExtern) {
         generateCodeTree(writer, current, scope);
      }
      else if (current == lxObject && checkNode(current.nextNode(), lxOperator, (ref_t)-3) && current.nextNode().nextNode() != lxAttributeValue) {
         // check if it is new operator
         writer.newBookmark();
         generateNewOperator(writer, current, scope);
         writer.removeBookmark();

         break;
      }
      else generateObjectTree(writer, current, scope);

      current = current.nextNode();
   }

   if (withBreakpoint)
      writer.removeBookmark();

   writer.closeNode();
}

bool DerivationReader :: generateFieldTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, SyntaxTree& buffer, bool/* templateMode*/)
{
//   if (node == lxClassField && node.argument == INVALID_REF) {
//      SNode ident = goToNode(attributes, lxNameAttr);
//
//      int prefixCounter = SyntaxTree::countChild(node, lxAttributeValue);
//      IdentifierString attrName(attributes.findChild(lxIdentifier).findChild(lxTerminal).identifier());
//      attrName.append('#');
//      attrName.appendInt(prefixCounter);
//
//      ref_t attrRef = scope.moduleScope->resolveAttributeRef(attrName, false);
//      if (!attrRef || scope.moduleScope->subjectHints.get(attrRef) != INVALID_REF)
//         scope.raiseError(errInvalidHint, node);
//
//      // if it is an autogenerated class
//      if (templateMode) {
//         // template in template should be copied "as is" (resolving all references)
//         writer.newNode(lxTemplateField, -1);
//         writer.appendNode(lxReference, scope.moduleScope->module->resolveSubject(attrRef));
//         copyAttributeTree(writer, node.firstChild(), scope);
//         if (scope.type == TemplateScope::ttFieldTemplate) {
//            SNode name = ident.findChild(lxIdentifier, lxPrivate);
//
//            scope.fields.add(name.findChild(lxTerminal).identifier(), scope.fields.Count() + 1);
//
//            writer.newNode(lxTemplateField, scope.fields.Count());
//            copyIdentifier(writer, name);
//            writer.closeNode();
//         }
//         else copyIdentifier(writer, ident.findChild(lxPrivate, lxIdentifier));
//         writer.closeNode();
//      }
//      else {
//         TemplateScope templateScope(&scope, attrRef);
//         templateScope.loadAttributeValues(node.firstChild());
//
//         SyntaxWriter bufferWriter(buffer);
//         generateTemplate(bufferWriter, templateScope, true);
//
//         copyAutogeneratedClass(buffer, *scope.autogeneratedTree);
//
//         writer.newNode(lxClassField);
//         writer.appendNode(lxClassRefAttr, scope.moduleScope->module->resolveReference(templateScope.reference));
//         copyIdentifier(writer, ident.findChild(lxPrivate, lxIdentifier));
//         writer.closeNode();
//      }
//   }
//   else {
      // if it is field / method template
      int prefixCounter = 1;
      setIdentifier(attributes);
      SNode propNode = goToNode(attributes, lxNameAttr).prevNode();
      SNode attrNode = propNode.prevNode();
      if (propNode == lxAttribute) {
         if (attrNode == lxAttribute) {
            //if the type attribute provoded
            attrNode = lxAttributeValue;
            attributes.refresh();

            prefixCounter++;
         }
      }
      else scope.raiseError(errInvalidHint, node);

      ref_t attrRef = scope.mapTemplate(propNode, prefixCounter);
      //if (!attrRef || scope.moduleScope->subjectHints.get(attrRef) != INVALID_REF)
      //   scope.raiseError(errInvalidHint, baseNode);

      DerivationScope templateScope(&scope, attrRef);
      templateScope.type = DerivationScope::ttFieldTemplate;
      templateScope.loadFields(node.findChild(lxBaseParent));

      SyntaxTree tempTree;
      templateScope.autogeneratedTree = &tempTree;

      copyTemplateTree(writer, node, templateScope, attributes);

      // copy autogenerated classes
      copyAutogeneratedClass(tempTree, *scope.autogeneratedTree);

      // copy class initializers
      SyntaxWriter initFieldWriter(buffer);
      return SyntaxTree::moveNodes(initFieldWriter, tempTree, lxFieldInit);

//   }
}

bool DerivationReader :: generateFieldTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode)
{
   SyntaxWriter bufferWriter(buffer);
   if (node == lxClassField) {
      writer.newNode(lxClassField, templateMode ? -1 : 0);

      SNode name;
      SNode attrValue = node.findChild(lxAttributeValue);
      if (attrValue == lxAttributeValue && attrValue.argument == (ref_t)-1) {
         name = attrValue.firstChild(lxTerminalObjMask);
      }
      else name = goToNode(attributes, lxNameAttr).findChild(lxIdentifier, lxPrivate);

      if (scope.type == DerivationScope::ttFieldTemplate && name == lxPrivate) {         
         // HOTFIX : template field should be private one
         scope.fields.add(name.findChild(lxTerminal).identifier(), scope.fields.Count() + 1);

         writer.newNode(lxTemplateField, scope.fields.Count());
         copyIdentifier(writer, name);
         writer.closeNode();

         generateAttributes(bufferWriter, SNode(), scope, attributes, templateMode);
      }
      else if (scope.type == DerivationScope::ttMethodTemplate) {
         scope.fields.add(name.findChild(lxTerminal).identifier(), scope.fields.Count() + 1);

         writer.appendNode(lxTemplateMethod, scope.fields.Count());
      }
      else {
         generateAttributes(bufferWriter, node, scope, attributes, templateMode);
         
         if (attrValue == lxAttributeValue && attrValue.argument == (ref_t)-1) {
            // HOTFIX : if it is a primitive array field
            scope.copySubject(bufferWriter, name);

            bufferWriter.appendNode(lxAttribute, V_OBJARRAY);
         }
      }

      // copy attributes
      SyntaxTree::moveNodes(writer, buffer, lxAttribute, lxIdentifier, lxPrivate, lxTemplateParam, lxSize, lxClassRefAttr, lxTemplateAttribute);

      writer.closeNode();

      // copy methods
      SyntaxTree::moveNodes(writer, buffer, lxClassMethod, lxClassField);
   }
   else if (node == lxFieldInit && !templateMode) {
      SNode nameNode = goToNode(attributes, lxNameAttr).firstChild(lxTerminalObjMask);

      writer.newNode(lxFieldInit);
      ::copyIdentifier(writer, nameNode);
      writer.closeNode();
   }

   // copy inplace initialization
   SNode bodyNode = node.findChild(lxAssigning);
   if (bodyNode != lxNone) {
      SNode nameNode = goToNode(attributes, lxNameAttr).firstChild(lxTerminalObjMask);

      bufferWriter.newNode(lxFieldInit);
      ::copyIdentifier(bufferWriter, nameNode);
      bufferWriter.appendNode(lxAssign);
      generateExpressionTree(bufferWriter, bodyNode.findChild(lxExpression), scope);
      bufferWriter.closeNode();

      return true;
   }
   else return false;
}

void DerivationReader :: generateMethodTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, SyntaxTree& buffer, bool templateMode)
{
   SyntaxWriter bufferWriter(buffer);

   writer.newNode(lxClassMethod);
   if (templateMode) {
      writer.appendNode(lxSourcePath, scope.sourcePath);
      writer.appendNode(lxTemplate, scope.templateRef);
   }      

   generateAttributes(bufferWriter, node, scope, attributes, templateMode);

   // copy attributes
   SyntaxTree::moveNodes(writer, buffer, lxAttribute, lxIdentifier, lxPrivate, lxTemplateParam, lxClassRefAttr, lxTemplateAttribute);

   // copy method signature
   SNode current = goToNode(attributes, lxNameAttr);
   while (current == lxNameAttr || current == lxMessage) {
      if (current == lxMessage) {
         writer.newNode(lxMessage);
         scope.copySubject(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }

      current = current.nextNode();
   }

   // copy method arguments
   current = node.firstChild();
   while (current != lxNone) {
      if (current == lxMethodParameter || current == lxMessage) {
         writer.newNode(current.type, current.argument);
         if (current == lxMessage) {
            scope.copySubject(writer, current.firstChild(lxTerminalMask));
         }
         else copyIdentifier(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }
      else if (current == lxAttributeValue) {
         // if it is an explicit type declaration
         ref_t ref = scope.moduleScope->mapTerminal(current.findChild(lxIdentifier, lxReference), true);
         if (!ref) {
            ref = scope.mapAttribute(current.findChild(lxIdentifier, lxReference));
            if (ref == 0) {
               ref = scope.moduleScope->mapTerminal(current.findChild(lxIdentifier, lxReference), false);
            }
            else if ((int)ref < 0)
               scope.raiseError(errInvalidHint, current);
         }            

         writer.newNode(lxParamRefAttr, scope.moduleScope->module->resolveReference(ref));
         copyIdentifier(writer, current.firstChild(lxTerminalMask));
         writer.closeNode();
      }

      current = current.nextNode();
   }

   SNode bodyNode = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
   if (bodyNode != lxNone) {
      if (templateMode)
         scope.reference = INVALID_REF;

      generateCodeTree(writer, bodyNode, scope);
   }

   writer.closeNode();

   // copy methods
   SyntaxTree::moveNodes(writer, buffer, lxClassMethod);
}


bool DerivationReader :: declareType(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   bool classMode = false;
   SNode expr = node.findChild(lxExpression);
   if (expr == lxExpression) {
      expr = expr.findChild(lxObject);
      if (expr.nextNode() == lxOperator) {
         classMode = true;
         SNode roperand = expr.nextNode().findChild(lxObject);

         if (!roperand.existChild(lxAngleOperator))
            return false;
      }
      else if (expr != lxObject || expr.nextNode() != lxNone)
         return false;
   }

   SNode nameNode = goToNode(attributes, lxNameAttr).findChild(lxIdentifier, lxPrivate);

   bool internalSubject = nameNode == lxPrivate;
   bool invalid = true;

   // map a full type name
   ident_t typeName = nameNode.findChild(lxTerminal).identifier();
   ref_t classRef = 0;

   SNode classNode = expr.findChild(lxIdentifier, lxPrivate, lxReference);
   if (classNode != lxNone) {
      SNode param = classMode ? expr.nextNode().firstChild() : classNode.nextNode(lxObjectMask);
      if (param != lxNone) {
         DerivationScope templateScope(&scope, 0);
         templateScope.templateRef = classMode ? templateScope.mapClassTemplate(expr) : templateScope.mapTemplate(expr);
         templateScope.loadAttributeValues(param, classMode);

         SyntaxTree buffer;
         SyntaxWriter bufferWriter(buffer);
         if (generateTemplate(bufferWriter, templateScope, true/*, true*/)) {
            SyntaxTree::moveNodes(writer, buffer);
      
            classRef = templateScope.reference;
      
            invalid = false;
         }
      }
      else {
         classRef = scope.moduleScope->mapTerminal(classNode);

         invalid = false;
      }
   }

   if (!invalid) {
      scope.moduleScope->saveAttribute(typeName, classRef, internalSubject);
   }

   return !invalid;
}

void DerivationReader :: includeModule(SNode ns, _CompilerScope& scope)
{
   ident_t name = ns.findChild(lxIdentifier, lxReference).findChild(lxTerminal).identifier();
   if (name.compare(STANDARD_MODULE))
      // system module is included automatically - nothing to do in this case
      return;

   bool duplicateExtensions = false;
   bool duplicateAttributes = false;
   if (scope.includeModule(name, duplicateExtensions, duplicateAttributes)) {
      if (duplicateExtensions)
         scope.raiseWarning(WARNING_LEVEL_1, wrnDuplicateExtension, ns);
   }
   else scope.raiseWarning(WARNING_LEVEL_1, wrnUnknownModule, ns);
}

bool DerivationReader :: generateDeclaration(SNode node, DerivationScope& scope, SNode attributes)
{
   // recognize the declaration type
   DeclarationAttr declType = daNone;
   SNode current = attributes;
   while (current == lxAttribute || current == lxAttributeDecl) {
      ref_t attrRef = current.argument;
      if (!attrRef) {
         attrRef = scope.mapAttribute(current);
         if (attrRef == V_ATTRTEMPLATE) {            
            if (scope.moduleScope->mapAttribute(current) == V_TEMPLATE) {
               // HOTFIX : check if it is a template based on the class
               SNode attrValue = current.findChild(lxAttributeValue);

               attrValue = lxBaseParent;
               attrRef = V_TEMPLATE;
            }
         }

         current.setArgument(attrRef);

         DeclarationAttr attr = daNone;
         switch (attrRef) {
            case V_TYPETEMPL:
               attr = daType;
               break;
            case V_CLASS:
            case V_STRUCT:
            //case V_STRING:
               attr = daClass;
               break;
            case V_TEMPLATE:
               attr = daTemplate;
               break;
            case V_FIELD:
               attr = daField;
               break;
            case V_METHOD:
               attr = daMethod;
               break;
            case V_LOOP:
               attr = (DeclarationAttr)(daLoop | daTemplate);
               break;
            case V_IMPORT:
               attr = daImport;
               break;
            case V_EXTERN:
               attr = (DeclarationAttr)(daExtern | daTemplate);
               break;
            default:
               break;
         }         
         declType = (DeclarationAttr)(declType | attr);
      }

      current = current.nextNode();
   }

   attributes.refresh();

   if (declType == daType) {
      SyntaxTree buffer;
      SyntaxWriter bufferWriter(buffer);

      bool retVal = declareType(bufferWriter, node, scope, attributes);

      copyAutogeneratedClass(buffer, *scope.autogeneratedTree);

      return retVal;
   }
   if (declType == daImport) {
      SNode name = goToNode(attributes, lxNameAttr);

      includeModule(name, *scope.moduleScope);

      return true;
   }
   else if (test(declType, daTemplate)) {
      node = lxTemplate;

      SNode name = goToNode(attributes, lxNameAttr);

      int count = SyntaxTree::countChild(node, lxBaseParent);

      IdentifierString templateName(name.findChild(lxIdentifier).findChild(lxTerminal).identifier());
      if (test(declType, daField)) {
         templateName.append("#");

         scope.type = DerivationScope::ttFieldTemplate;
         //count++; // HOTFIX : to include the field itself
      }
      else if (test(declType, daMethod)) {
         templateName.append("#");

         scope.type = DerivationScope::ttMethodTemplate;
      }
      else if (node.existChild(lxExpression)) {
         scope.type = DerivationScope::ttCodeTemplate;

         // HOTFIX : mark the expression as a code
         SNode exprNode = node.findChild(lxExpression);
         exprNode.injectNode(lxExpression);
         if (test(declType, daLoop)) {
            exprNode.injectNode(lxLoop);
         }
         else if (test(declType, daExtern)) {
            exprNode.injectNode(lxExtern);
         }

         exprNode = lxCode;
      }

      templateName.append('#');
      templateName.appendInt(count);

      ref_t templateRef = scope.mapNewReference(templateName);

      // check for duplicate declaration
      if (scope.moduleScope->module->mapSection(templateRef | mskSyntaxTreeRef, true))
         scope.raiseError(errDuplicatedSymbol, name);

      saveTemplate(scope.moduleScope->module->mapSection(templateRef | mskSyntaxTreeRef, false),
         node, *scope.moduleScope, attributes, scope.type, *scope.autogeneratedTree);

      scope.moduleScope->saveAttribute(templateName, templateRef, false);

      return true;
   }
   else if (test(declType, daClass)) {
      return false;
   }
   else return false;
}

void DerivationReader :: generateTemplateTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   SyntaxTree buffer;

   generateAttributes(writer, node, scope, attributes, true);

   SNode current = node.firstChild();
   SNode subAttributes;
   while (current != lxNone) {
      if (current == lxAttribute || current == lxNameAttr) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
      else if (current == lxClassMethod) {
         generateMethodTree(writer, current, scope, subAttributes, buffer, true);
         subAttributes = SNode();
      }
      else if (current == lxClassField || current == lxFieldInit) {
      //   if (current.argument == INVALID_REF) {
      //      generateFieldTemplateTree(writer, current, scope, subAttributes, buffer, true);
      //   }
         /*else */
         if (generateFieldTree(writer, current, scope, subAttributes, buffer, true)) {
            SyntaxTree::moveNodes(writer, buffer, lxFieldInit);
         }

         subAttributes = SNode();
      }
      else if (current == lxFieldTemplate) {
         generateFieldTemplateTree(writer, current, scope, subAttributes, buffer, true);
         subAttributes = SNode();
      }
      else if (current == lxCode) {
         scope.type = DerivationScope::ttCodeTemplate;

         generateCodeTree(writer, current, scope);
      }

      current = current.nextNode();
   }
}

bool DerivationReader :: generateSingletonScope(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   SNode expr = node.findChild(lxExpression);
   SNode object = expr.findChild(lxObject);
   SNode closureNode = object.findChild(lxNestedClass);
   if (closureNode != lxNone && isSingleStatement(expr)) {
      generateScopeMembers(closureNode, scope);

      SNode terminal = object.firstChild(lxTerminalMask);

      writer.newNode(lxClass);
      writer.appendNode(lxAttribute, V_SINGLETON);

      if (terminal != lxNone) {
         writer.newNode(lxBaseParent);
         copyIdentifier(writer, terminal);
         writer.closeNode();
      }

      generateAttributes(writer, node, scope, attributes, false);

      // NOTE : generateClassTree closes the class node and copies auto generated classes after it
      generateClassTree(writer, closureNode, scope, SNode(), -2);

      return true;
   }
   else return false;
}

bool DerivationReader :: generateMethodScope(SNode node, DerivationScope& scope, SNode attributes)
{
   SNode current = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);
   if (current != lxNone) {
      // try to resolve the message name
      SNode lastAttr = findLastAttribute(attributes);
      SNode firstMember = node.findChild(lxMethodParameter, lxAttribute, lxAttributeValue);

      if (scope.isImplicitAttribute(lastAttr.findChild(lxIdentifier, lxPrivate)) && (firstMember == lxAttributeValue || firstMember == lxMethodParameter || firstMember == lxNone)) {
         // HOTFIX : recognize explicit / generic attributes
      }
      else {
         if (node.firstChild(lxExprMask) == lxMethodParameter) {
            // HOTFIX : recognize type
            current = lastAttr.prevNode();
            if (current == lxAttribute && (scope.isTypeAttribute(lastAttr.findChild(lxIdentifier, lxPrivate)) 
               || scope.isSubject(current.findChild(lxIdentifier, lxPrivate)))) 
            {
               if (!scope.isAttribute(current.findChild(lxIdentifier, lxPrivate))) {
                  lastAttr = lxMessage;
                  lastAttr = current;

                  current = current.prevNode();

                  if (!isVerbAttribute(lastAttr) && isVerbAttribute(current)) {
                     // HOTFIX : to support "verb subject type[]"
                     lastAttr = lxMessage;
                     lastAttr = current;
                  }
               }
            }
         }

         // mark the last message as a name
         lastAttr = lxNameAttr;
      }
      
      node = lxClassMethod;

      // !! HOTFIX : the node should be once again found
      current = node.findChild(lxCode, lxExpression, lxDispatchCode, lxReturning, lxResendExpression);

      if (current == lxExpression)
         current = lxReturning;

      return true;
   }
   return false;
}

void DerivationReader :: generateClassTree(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes, int nested)
{
   SyntaxTree buffer;

   if (!nested) {
      writer.newNode(lxClass);

      generateAttributes(writer, node, scope, attributes, false);
   }

   SNode current = node.firstChild();
   SNode subAttributes;
   bool withInPlaceInit = false;
   while (current != lxNone) {
      if (current == lxAttribute || current == lxNameAttr) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
      else if (current == lxBaseParent) {
         if (current.existChild(lxAttributeValue)) {
            ref_t attrRef = scope.mapTemplate(current);

            DerivationScope templateScope(&scope, attrRef);
            copyTemplateTree(writer, current, templateScope, current.firstChild());
         }
         else {
            writer.newNode(lxBaseParent);
            copyIdentifier(writer, current.firstChild(lxTerminalMask));
            writer.closeNode();
         }
      }
      else if (current == lxClassMethod) {
         generateMethodTree(writer, current, scope, subAttributes, buffer);
         subAttributes = SNode();
      }
      else if (current == lxClassField || current == lxFieldInit) {
         /*if (current.argument == INVALID_REF) {
            generateFieldTemplateTree(writer, current, scope, subAttributes, buffer);
         }
         else */withInPlaceInit |= generateFieldTree(writer, current, scope, subAttributes, buffer);
         subAttributes = SNode();
      }
      else if (current == lxFieldTemplate) {
         withInPlaceInit |= generateFieldTemplateTree(writer, current, scope, subAttributes, buffer);
         subAttributes = SNode();
      }

      current = current.nextNode();
   }

   if (withInPlaceInit) {
      current = goToNode(buffer.readRoot(), lxFieldInit);
      writer.newNode(lxClassMethod);
      writer.appendNode(lxAttribute, V_SEALED);
      writer.appendNode(lxAttribute, V_CONVERSION);
      writer.newNode(lxCode);
      while (current != lxNone) {
         if (current == lxFieldInit) {
            writer.newNode(lxExpression);
            SyntaxTree::copyNode(writer, current);
            writer.closeNode();
         }
         current = current.nextNode();
      }
      writer.closeNode();
      writer.closeNode();
   }
   
   if (nested == -1)
      writer.insert(lxNestedClass);

   writer.closeNode();
}

void DerivationReader :: generateScopeMembers(SNode node, DerivationScope& scope)
{
   SNode current = node.firstChild();
   SNode subAttributes;
   while (current != lxNone) {
      if (current == lxAttribute) {
         if (subAttributes == lxNone)
            subAttributes = current;
      }
      else if (current == lxScope) {
         if (!generateMethodScope(current, scope, subAttributes)) {
            // recognize the field template if available
            SNode fieldTemplate = current.findChild(lxBaseParent);
            if (fieldTemplate != lxNone) {
               current = lxFieldTemplate;
            }
            else if (checkNode(current.findChild(lxAttributeValue), lxAttributeValue, -1)) {
               // HOTFIX : if it is a primitive array field
               current = lxClassField;
            }
            else if (setIdentifier(subAttributes)) {
               subAttributes.refresh();

               if (goToNode(subAttributes, lxNameAttr).firstChild(lxTerminalObjMask) == lxMemberIdentifier) {
                  current = lxFieldInit;
               }
               else {
                  current = lxClassField;

                  if (current.existChild(lxAttributeValue)) {
                     current.setArgument(-1);
                  }
                  else if (scope.type == DerivationScope::ttMethodTemplate)
                     current.setArgument(V_METHOD);
               }
            }
         }
         subAttributes = SNode();
      }

      current = current.nextNode();
   }
}

void DerivationReader :: generateScope(SyntaxWriter& writer, SNode node, DerivationScope& scope, SNode attributes)
{
   SNode body = node.findChild(lxExpression, lxAttributeDecl, lxAssigning);
   if (body == lxExpression) {
      // if it could be compiled as a symbol
      if (setIdentifier(attributes)) {
         attributes.refresh();

         // check if it is a code template
         if (!generateDeclaration(node, scope, attributes)) {
            // check if it could be compiled as a singleton
            if (!generateSingletonScope(writer, node, scope, attributes)) {
               node = lxSymbol;
               attributes.refresh();

               generateSymbolTree(writer, node, scope, attributes);
            }
         }
      }
   }
   else if (body == lxAttributeDecl) {
      if (setIdentifier(attributes)) {
         attributes.refresh();

         SNode nameAttr = goToNode(attributes, lxNameAttr);
         ident_t name = nameAttr.findChild(lxIdentifier).findChild(lxTerminal).identifier();

         ref_t attrRef = scope.mapAttribute(body);

         if(!scope.moduleScope->saveAttribute(name, attrRef, false))
            scope.raiseError(errDuplicatedDefinition, nameAttr);
      }
   }
   else if (body == lxAssigning) {
      scope.raiseError(errInvalidSyntax, body);
   }
   else {
      // it is is a template
      if (node == lxTemplate) {
         generateScopeMembers(node, scope);

         generateTemplateTree(writer, node, scope, attributes);
      }
      else {
         setIdentifier(attributes);
         // try to recognize general declaration
         if (!generateDeclaration(node, scope, attributes)) {
            // otherwise it will be compiled as a class
            node = lxClass;
            attributes.refresh();

            generateScopeMembers(node, scope);

            generateClassTree(writer, node, scope, attributes);
         }
      }
   }
}

void DerivationReader :: generateSyntaxTree(SyntaxWriter& writer, SNode node, _CompilerScope& scope, SyntaxTree& autogenerated)
{
   SNode attributes;
   SNode current = node.firstChild();
   while (current != lxNone) {
      switch (current.type) {
         case lxAttribute:
         case lxAttributeDecl:
            if (attributes == lxNone) {
               attributes = current;
            }
            break;
         case lxScope:
         {
            DerivationScope rootScope(&scope);
//            rootScope.templateRef = INVALID_REF;
            rootScope.autogeneratedTree = &autogenerated;
            generateScope(writer, current, rootScope, attributes);
            attributes = SNode();
            break;
         }
//         case lxTemplate:
//            generateNewAttribute(current, scope, attributes);
//            attributes = SNode();
//            break;
      }
      current = current.nextNode();
   }
}

void DerivationReader :: saveTemplate(_Memory* target, SNode node, _CompilerScope& scope, SNode attributes, DerivationScope::Type type, SyntaxTree& autogenerated)
{
   SyntaxTree tree;
   SyntaxWriter writer(tree);

   writer.newNode(lxTemplate);

   // HOTFIX : save the template source path
   IdentifierString fullPath(scope.module->Name());
   fullPath.append('\'');
   fullPath.append(scope.sourcePath);

   DerivationScope rootScope(&scope);
   rootScope.autogeneratedTree = &autogenerated;
   rootScope.loadParameters(node);
   rootScope.sourcePath = fullPath;
   rootScope.type = type;

   generateScope(writer, node, rootScope, attributes);

   writer.closeNode();
   
   SyntaxTree::saveNode(tree.readRoot(), target);
}

void DerivationReader :: generateSyntaxTree(SyntaxWriter& writer, _CompilerScope& scope)
{
   SyntaxTree autogeneratedTree;
   writer.newNode(lxRoot);
   generateSyntaxTree(writer, _root, scope, autogeneratedTree);

   SyntaxTree::moveNodes(writer, autogeneratedTree, lxClass);

   writer.closeNode();
}