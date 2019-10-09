//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <windows.h>
#include "elena.h"

#define TITLE "ELENA Standard Library 4.1: Module "
#define TITLE2 "ELENA&nbsp;Standard&nbsp;Library<br>4.1"

//#define OPERATORS "+-*/=<>?!"

using namespace _ELENA_;

char buffer[BLOCK_SIZE];

struct ApiFieldInfo
{
   IdentifierString name;
   IdentifierString type;
};

struct ApiMethodInfo
{
   bool prop, special, convertor, isAbstract;
   bool isMultidispatcher;

   IdentifierString       prefix;
   IdentifierString       name;
   IdentifierString       retType;
   
   List<IdentifierString> params;

   ApiMethodInfo()
   {
      special = prop = convertor = false;
      isAbstract = isMultidispatcher = false;
   }
};

struct ApiClassInfo
{
   bool withStaticProps;
   bool withProps;
   bool withConstructors;
   bool withConversions;
   bool templateBased;

   IdentifierString prefix;
   IdentifierString fullName;
   IdentifierString name;

   List<IdentifierString> parents;
   List<ApiFieldInfo*> fields;
   List<ApiMethodInfo*> methods;
   List<ApiMethodInfo*> constructors;
   List<ApiMethodInfo*> extensions;

   ApiClassInfo()
      : fields(nullptr, freeobj), methods(nullptr, freeobj), constructors(nullptr, freeobj)
   {
      withStaticProps = withProps = false;
      withConstructors = false;
      withConversions = false;
      templateBased = false;
   }
   ~ApiClassInfo()
   {

   }
};

struct ApiModuleInfo
{
   IdentifierString name;

   List<ApiClassInfo*> classes;
   ApiModuleInfo()
      : classes(nullptr, freeobj)
   {

   }
   ~ApiModuleInfo()
   {

   }
};

//typedef String<char, 255> ParamString;

inline bool isTemplateBased(ident_t reference)
{
   for (int i = 0; i < getlength(reference); i++) {
      if (reference[i] == '#' && reference[i + 1] >= '0' && reference[i + 1] <= '9')
         return true;
   }

   return false;
}

void writeNs(IdentifierString& name, ApiModuleInfo* info)
{
   for (int i = 1; i < info->name.Length(); i++)
   {
      if (info->name[i] == '\'') {
         name.append('-');
      }
      else name.append(info->name[i]);
   }
}

void writeNs(TextFileWriter& writer, ident_t ns)
{
   for (int i = 0; i < getlength(ns); i++)
   {
      if (ns[i] == '\'') {
         writer.writeChar('-');
      }
      else writer.writeChar(ns[i]);
   }
}

void writeHeader(TextFileWriter& writer, const char* package, const char* packageLink)
{
   writer.writeLiteralNewLine("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Frameset//EN\"\"http://www.w3.org/TR/REC-html40/frameset.dtd\">");
   writer.writeLiteralNewLine("<HTML>");
   writer.writeLiteralNewLine("<HEAD>");
   writer.writeLiteralNewLine("<TITLE>");
   writer.writeLiteral(TITLE);
   writer.writeLiteralNewLine(package);
   writer.writeLiteralNewLine("</TITLE>");
   writer.writeLiteralNewLine("<meta name=\"collection\" content=\"api\">");
   writer.writeLiteralNewLine("</HEAD>");
   writer.writeLiteralNewLine("<BODY BGCOLOR=\"white\">");

   writer.writeLiteralNewLine("<A NAME=\"navbar_top\"><!-- --></A>");
   writer.writeLiteralNewLine("<TABLE BORDER=\"0\" WIDTH=\"100%\" CELLPADDING=\"1\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TD COLSPAN=2 BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">");
   writer.writeLiteralNewLine("<A NAME=\"navbar_top_firstrow\"><!-- --></A>");
   writer.writeLiteralNewLine("<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"3\">");
   writer.writeLiteralNewLine("  <TR ALIGN=\"center\" VALIGN=\"top\">");
   writer.writeLiteralNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <A HREF=\"index.html\"><FONT CLASS=\"NavBarFont1\"><B>Overview</B></FONT></A>&nbsp;</TD>");
   if (!emptystr(packageLink)) {
      writer.writeLiteral("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\"><A HREF=\"");
      writer.writeLiteral(packageLink);
      writer.writeLiteralNewLine("\"><FONT CLASS=\"NavBarFont1\"><B>Module</B></FONT></A>&nbsp;</TD>");
   }
   else writer.writeLiteralNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <FONT CLASS=\"NavBarFont1\">Module</FONT>&nbsp;</TD>");
   writer.writeLiteralNewLine("  </TR>");
   writer.writeLiteralNewLine("</TABLE>");
   writer.writeLiteralNewLine("</TD>");

   writer.writeLiteralNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" ROWSPAN=3><EM><b>");
   writer.writeLiteralNewLine(TITLE2);
   writer.writeLiteralNewLine("</b></EM>");
   writer.writeLiteralNewLine("</TD>");
   writer.writeLiteralNewLine("</TR>");
   writer.writeLiteralNewLine("</TABLE>");

   writer.writeLiteralNewLine("<DL>");
   writer.writeLiteralNewLine("<HR>");
}

void writeSummaryHeader(TextFileWriter& writer, const char* name, const char* shortDescr)
{
   writer.writeLiteralNewLine("<H2>");
   writer.writeLiteral("Module ");
   writer.writeLiteralNewLine(name);
   writer.writeLiteralNewLine("</H2>");
   writer.writeLiteralNewLine(shortDescr);
   writer.writeLiteralNewLine("<P>");

   writer.writeLiteralNewLine("<TABLE BORDER=\"1\" CELLPADDING=\"3\" CELLSPACING=\"0\" WIDTH=\"100%\">");
   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Class Summary</B></FONT></TD>");
   writer.writeLiteralNewLine("</TR>");
}

void writeRefName(TextFileWriter& writer, ident_t name)
{
   for (int i = 0; i < getlength(name); i++)
   {
      if (name[i] == '\'') {
         writer.writeChar('-');
      }
      else writer.writeChar(name[i]);
   }
}

void writeClassName(TextFileWriter& writer, ApiClassInfo* info)
{
   const char* descr = /*config.getSetting(name, "#shortdescr", NULL)*/nullptr;
   if (!emptystr(descr)) {
      writer.writeLiteral(descr);
   }
   else {
      writer.writeLiteral(info->prefix.c_str());
      writer.writeLiteral(info->name.c_str());
   }
}

void writeSummaryTable(TextFileWriter& writer, ApiClassInfo* info, const char* bodyFileName)
{
   writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
   writer.writeLiteral("<TD WIDTH=\"15%\"><B><A HREF=\"");
   writer.writeLiteral(bodyFileName);
   writer.writeLiteral("#");
   writeRefName(writer, info->name.c_str());
   writer.writeLiteral("\">");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("</A></B></TD>");
   writer.writeLiteral("<TD>");
   writeClassName(writer, info);

   writer.writeLiteral("</TD>");
   writer.writeLiteralNewLine("</TR>");
}

inline void repeatStr(TextFileWriter& writer, const char* s, int count)
{
   for(int i = 0 ; i < count ; i++) writer.writeLiteral(s);
}

//inline const char* find(const char* s, char ch)
//{
//   if (emptystr(s)) {
//      return NULL;
//   }
//   else {
//      int index = StringHelper::find(s, ch);
//      if (index==-1)
//         index = getlength(s) - 1;
//
//      return s + index + 1;
//   }
//}
//
//inline bool exists(const char* s, char ch)
//{
//   if (emptystr(s)) {
//      return false;
//   }
//   else return StringHelper::find(s, ch) != -1;
//}
//
//inline void writeLeft(TextFileWriter& writer, const char* s, const char* right)
//{
//   if (emptystr(right)) {
//      writer.writeLiteral(s);
//   }
//   else writer.write(s, right - s - 1);
//}
//
//void writeLink(TextFileWriter& writer, const char* link, const char* file = NULL)
//{
//   const char* body = find(link, ':');
//
//   writer.writeLiteral("<A HREF=\"");
//
//   if (StringHelper::find(link, '#') == -1) {
//      if (!emptystr(file)) {
//         writer.writeLiteral(file);
//      }
//      writer.writeChar('#');
//      writeLeft(writer, link, body);
//   }
//   else writeLeft(writer, link, body);
//
//   writer.writeLiteral("\">");
//   if (emptystr(body)) {
//      writer.writeLiteral(link);
//   }
//   else writeLeft(writer, body, find(body, ';'));
//
//   writer.writeLiteralNewLine("</A>");
//}
//
//void writeParamLink(TextFileWriter& writer, const char* link, const char* right, const char* file)
//{
//   writer.writeLiteral("<A HREF=\"");
//
//   int pos = StringHelper::find(link, '#');
//
//   if (pos == -1 || pos > (right - link)) {
//      writer.writeLiteral(file);
//      writer.writeChar('#');
//	   writeLeft(writer, link, right);
//   }
//   else {
//      writeLeft(writer, link, right);
//	   link += StringHelper::find(link, '#') + 1;
//   }
//   writer.writeLiteral("\">");
//   writeLeft(writer, link, right);
//   writer.writeLiteralNewLine("</A>");
//}
//
//void writeSignature(TextFileWriter& writer, const char* parameters)
//{
//   if (parameters[0] != '&') {
//      const char* param = parameters;
//      const char* next_subj = find(param, '&');
//
//      writer.writeLiteral(" : ");
//      writeParamLink(writer, param, next_subj, "protocol.html");
//
//      parameters = next_subj;
//   }
//
//   while (!emptystr(parameters)) {
//      const char* subj = parameters;
//      const char* param = find(parameters, ':');
//      const char* next_subj = find(param, '&');
//
//      if (subj[0] == ':' || (subj[0] == '&' && subj[1] == ':')) {
//         writer.writeLiteral(": ");
//         writeParamLink(writer, param, next_subj, "protocol.html");
//      }
//      else {
//         if (parameters[0]!='&')
//            writer.writeLiteral("&");
//
//         writeLeft(writer, subj, param);
//         writer.writeLiteral(":");
//         writeParamLink(writer, param, next_subj, "protocol.html");
//      }
//
//      parameters = next_subj;
//   }
//}

void writeType(TextFileWriter& writer, ident_t type)
{
   if (type.find('\'') != NOTFOUND_POS) {
      writer.writeLiteral("<A HREF=\"");

      NamespaceName ns(type);
      if (emptystr(ns.c_str())) {
         writer.writeLiteral("#");
      }
      else {
         writeNs(writer, ns.ident());
         writer.writeLiteral(".html#");
      }
      writeRefName(writer, type.c_str() + ns.Length() + 1);

      writer.writeLiteral("\">");
      writer.writeLiteral(type.c_str() + ns.Length() + 1);
      writer.writeLiteral("</A>");
   }
   else writer.writeLiteral(type.c_str());
}

void writeParents(TextFileWriter& writer, ApiClassInfo* info, ident_t moduleName)
{
   writer.writeLiteralNewLine("<PRE>");
   int indent = 0;
   auto it = info->parents.start();
   while (!it.Eof()) {
      repeatStr(writer, "  ", indent - 1);
      if (indent > 0) writer.writeLiteralNewLine(" |");
      repeatStr(writer, "  ", indent - 1);
      if (indent > 0) writer.writeLiteral(" +--");

      writeType(writer, (*it).ident());
      writer.writeNewLine();

      indent++;

      it++;
   }
   repeatStr(writer, "  ", indent - 1);
   writer.writeLiteralNewLine(" |");
   repeatStr(writer, "  ", indent - 1);
   writer.writeLiteral(" +--<B>");
   writer.writeLiteral(moduleName);
   writer.writeLiteral("'");
   writer.writeLiteral(info->name);
   writer.writeLiteralNewLine("</B>");

   writer.writeLiteralNewLine("</PRE>");
}

void writeFields(TextFileWriter& writer, ApiClassInfo* info)
{
   // field section
   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Field Summary</B></FONT></TD>");

   auto it = info->fields.start();
   while (!it.Eof()) {
      writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
      writer.writeLiteralNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");
      writer.writeLiteralNewLine("<CODE>");
      writeType(writer, (*it)->type);
      writer.writeLiteralNewLine("&nbsp;</CODE>");
      writer.writeLiteralNewLine("</TD>");
      writer.writeLiteral("<TD><CODE>");
      writer.writeLiteral((*it)->name);
      writer.writeLiteralNewLine("</CODE>");
      writer.writeLiteralNewLine("</TD>");
      writer.writeLiteralNewLine("</TR>");

      it++;
   }
}

bool isMethod(ApiMethodInfo* info)
{
   return !info->special && !info->prop;
}

void writeFirstColumn(TextFileWriter& writer, ApiMethodInfo* info)
{
   writer.writeLiteralNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");
   writer.writeLiteralNewLine("<CODE>&nbsp;");
   if (info->prefix.Length() != 0) {
      writer.writeLiteral(info->prefix.ident());
      writer.writeLiteral("&nbsp;");
   }
   if (info->retType.Length() != 0) {
      writeType(writer, info->retType);
   }
   
   writer.writeLiteralNewLine("</CODE></TD>");
}

void writeSecondColumn(TextFileWriter& writer, ApiMethodInfo* info)
{
   writer.writeLiteralNewLine("<TD VALIGN=\"top\" WIDTH=\"30%\">");
   writer.writeLiteralNewLine("<CODE>&nbsp;");
   writer.writeLiteral(info->name);
   writer.writeLiteral("(");

   bool first = true;
   auto it = info->params.start();
   while (!it.Eof()) {
      if (!first) {
         writer.writeLiteral(", ");
      }
      else first = false;

      writeType(writer, (*it).c_str());

      it++;
   }

   writer.writeLiteralNewLine(")");

   writer.writeLiteralNewLine("</CODE>");

   writer.writeLiteralNewLine("<DIV>");
   //   const char* descr = emptystr(result) ? find(parameter, ';') : find(result, ';');
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("</TD>");
}

void writeSecondPropColumn(TextFileWriter& writer, ApiMethodInfo* info)
{
   writer.writeLiteralNewLine("<TD VALIGN=\"top\" WIDTH=\"30%\">");
   writer.writeLiteralNewLine("<CODE>&nbsp;");
   writer.writeLiteral(info->name);

   if (info->params.Count() > 0) {
      writer.writeLiteral("(");

      bool first = true;
      auto it = info->params.start();
      while (!it.Eof()) {
         if (!first) {
            writer.writeLiteral(", ");
         }
         else first = false;

         writeType(writer, (*it).c_str());

         it++;
      }

      writer.writeLiteralNewLine(")");
   }

   writer.writeLiteralNewLine("</CODE>");

   writer.writeLiteralNewLine("<DIV>");
   //   const char* descr = emptystr(result) ? find(parameter, ';') : find(result, ';');
   writer.writeLiteralNewLine("</DIV>");

   writer.writeLiteralNewLine("</TD>");
}

void writeMethods(TextFileWriter& writer, ApiClassInfo* info)
{
//   if (emptystr(config.getSetting(name, "#method", NULL)))
//      return;

   // method section
   writer.writeLiteralNewLine("<A NAME=\"method_summary\"><!-- --></A>");

   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Method Summary</B></FONT></TD>");
   writer.writeLiteralNewLine("</TR>");

   auto it = info->methods.start();
   while (!it.Eof()) {
      if (isMethod(*it)) {
         writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }
}

void writeProperties(TextFileWriter& writer, ApiClassInfo* info)
{
   // property section
   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Property Summary</B></FONT></TD>");

   auto it = info->methods.start();
   while (!it.Eof()) {
      if ((*it)->prop) {
         writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");

         writeFirstColumn(writer, *it);
         writeSecondPropColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }
}

void writeStaticProperties(TextFileWriter& writer, ApiClassInfo* info)
{
   // property section
   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Static Property Summary</B></FONT></TD>");

   auto it = info->constructors.start();
   while (!it.Eof()) {
      if ((*it)->prop) {
         writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");

         writeFirstColumn(writer, *it);
         writeSecondPropColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }
}

void writeExtensions(TextFileWriter& writer, ApiClassInfo* info)
{
   // method section
   writer.writeLiteralNewLine("<A NAME=\"method_summary\"><!-- --></A>");

   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Extension Summary</B></FONT></TD>");
   writer.writeLiteralNewLine("</TR>");

   auto it = info->extensions.start();
   while (!it.Eof()) {
      if (!(*it)->special) {
         writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }

      it++;
   }
}

void writeConstructors(TextFileWriter& writer, ApiClassInfo* info)
{
//   if (emptystr(config.getSetting(name, "#constructor", NULL)))
//      return;

   // method section
   writer.writeLiteralNewLine("<A NAME=\"constuctor_summary\"><!-- --></A>");

   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Constructor Summary</B></FONT></TD>");
   writer.writeLiteralNewLine("</TR>");

   auto it = info->constructors.start();
   while (!it.Eof()) {
      if (isMethod(*it)) {
         writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }
}

void writeConversions(TextFileWriter& writer, ApiClassInfo* info)
{
   //   if (emptystr(config.getSetting(name, "#constructor", NULL)))
   //      return;

      // method section
   writer.writeLiteralNewLine("<A NAME=\"conversion_summary\"><!-- --></A>");

   writer.writeLiteralNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeLiteralNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeLiteralNewLine("<B>Conversion Summary</B></FONT></TD>");
   writer.writeLiteralNewLine("</TR>");

   auto it = info->methods.start();
   while (!it.Eof()) {
      if ((*it)->convertor) {
         writer.writeLiteralNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");

         writeFirstColumn(writer, *it);
         writeSecondColumn(writer, *it);

         writer.writeLiteralNewLine("</TR>");
      }
      it++;
   }
}

void writeBody(TextFileWriter& writer, ApiClassInfo* info, const char* moduleName)
{
   const char* title = /*config.getSetting(name, "#title", NULL)*/nullptr;
   if (title==NULL)
      title = info->name.c_str();

   writer.writeLiteral("<A NAME=\"");
   writer.writeLiteral(info->name.c_str());
   writer.writeLiteralNewLine("\"/>");
   writer.writeLiteralNewLine("<HR/>");
   writer.writeLiteralNewLine("<!-- ======== START OF CLASS DATA ======== -->");
   writer.writeLiteralNewLine("<H2>");
   writer.writeLiteralNewLine("<FONT SIZE=\"-1\">");
   writer.writeLiteral(moduleName);
   writer.writeLiteral("'");
   writer.writeLiteralNewLine("</FONT>");
   writer.writeLiteralNewLine("<BR>");
   writer.writeLiteral(title);
   writer.writeLiteralNewLine("</H2>");

//   if (!emptystr(config.getSetting(name, "#parent", NULL))) {
   if (info->parents.Count() > 0) {
      writeParents(writer, info, moduleName);
   }

   const char* descr = /*config.getSetting(name, "#descr", NULL)*/nullptr;
//   if (emptystr(descr)) {
//      descr = config.getSetting(name, "#shortdescr", NULL);
//   }
//   if (!emptystr(descr)) {
//      writer.writeLiteralNewLine("<P>");
//      writer.writeLiteralNewLine(descr);
//      writer.writeLiteralNewLine("<P>");
//   }
   /*else*/ writeClassName(writer, info);

//   writer.writeLiteralNewLine("<P>");

   writer.writeLiteralNewLine("<TABLE BORDER=\"1\" CELLPADDING=\"3\" CELLSPACING=\"0\" WIDTH=\"100%\">");

   if (info->fields.Count() > 0) {
      writer.writeLiteralNewLine("<!-- =========== FIELD SUMMARY =========== -->");
      writeFields(writer, info);
   }

   if (info->withStaticProps) {
      writer.writeLiteralNewLine("<!-- ========== CONSTRUCTOR SUMMARY =========== -->");
      writeStaticProperties(writer, info);
   }

   if (info->withConstructors) {
      writer.writeLiteralNewLine("<!-- ========== CONSTRUCTOR SUMMARY =========== -->");
      writeConstructors(writer, info);
   }

   if (info->withConversions) {
      writer.writeLiteralNewLine("<!-- ========== COVERSION SUMMARY =========== -->");
      writeConversions(writer, info);
   }

   if (info->withProps) {
      writer.writeLiteralNewLine("<!-- =========== PROPERTY SUMMARY =========== -->");
      writeProperties(writer, info);
   }

   writer.writeLiteralNewLine("<!-- ========== METHOD SUMMARY =========== -->");
   writeMethods(writer, info);

   if (info->extensions.Count() > 0) {
      writer.writeLiteralNewLine("<!-- ========== EXTNSION SUMMARY =========== -->");
      writeExtensions(writer, info);
   }

   writer.writeLiteralNewLine("</TABLE>");
}

void writeFooter(TextFileWriter& writer, const char* packageLink)
{
   writer.writeLiteralNewLine("<HR>");
   writer.writeLiteralNewLine("<A NAME=\"navbar_top\"><!-- --></A>");
   writer.writeLiteralNewLine("<TABLE BORDER=\"0\" WIDTH=\"100%\" CELLPADDING=\"1\" CELLSPACING=\"0\">");
   writer.writeLiteralNewLine("<TR>");
   writer.writeLiteralNewLine("<TD COLSPAN=2 BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">");
   writer.writeLiteralNewLine("<A NAME=\"navbar_top_firstrow\"><!-- --></A>");
   writer.writeLiteralNewLine("<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"3\">");
   writer.writeLiteralNewLine("  <TR ALIGN=\"center\" VALIGN=\"top\">");
   writer.writeLiteralNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <A HREF=\"index.html\"><FONT CLASS=\"NavBarFont1\"><B>Overview</B></FONT></A>&nbsp;</TD>");
   if (!emptystr(packageLink)) {
      writer.writeLiteral("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\"><A HREF=\"");
      writer.writeLiteral(packageLink);
      writer.writeLiteralNewLine("\"><FONT CLASS=\"NavBarFont1\"><B>Module</B></FONT></A>&nbsp;</TD>");
   }
   else writer.writeLiteralNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <FONT CLASS=\"NavBarFont1\">Module</FONT>&nbsp;</TD>");
   writer.writeLiteralNewLine("  </TR>");
   writer.writeLiteralNewLine("</TABLE>");
   writer.writeLiteralNewLine("</TD>");

   writer.writeLiteralNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" ROWSPAN=3><EM><b>");
   writer.writeLiteral(TITLE2);
   writer.writeLiteralNewLine("</b></EM>");
   writer.writeLiteralNewLine("</TD>");
   writer.writeLiteralNewLine("</TR>");
   writer.writeLiteralNewLine("</TABLE>");
}

void writeSummaryFooter(TextFileWriter& writer)
{
   writer.writeLiteralNewLine("</TABLE>");
}

bool readLine(String<char, LINE_LEN>& line, TextFileReader& reader)
{
   if (reader.readLine(line, buffer)) {
      line.trim('\n');
      line.trim('\r');

      return true;
   }
   else return false;
}

ApiModuleInfo* findModule(List<ApiModuleInfo*>& modules, ident_t ns)
{
   auto it = modules.start();
   while (!it.Eof()) {
      if ((*it)->name.compare(ns))
         return *it;

      it++;
   }

   return nullptr;
}

ApiClassInfo* findClass(ApiModuleInfo* module, ident_t name)
{
   auto it = module->classes.start();
   while (!it.Eof()) {
      if ((*it)->name.compare(name))
         return *it;

      it++;
   }

   return nullptr;
}

void parseTemplateName(IdentifierString& line, int index)
{
   IdentifierString temp(line);

   line.truncate(index);

   int last = index;
   bool first = true;
   for (int i = index; i < temp.Length(); i++) {
      if (temp[i] == '@') {
         temp[i] = '\'';
         last = i;
      }
      else if (temp[i] == '#') {
         line.append(temp.c_str() + last + 1, i - last - 1);
      }
      else if (temp[i] == '&') {
         if (first) {
            line.append("&lt;");
            first = false;
         }
         else {
            line.append(temp.c_str() + last + 1, i - last - 1);
            line.append(',');
         }
      }
   }

   line.append(temp.c_str() + last + 1);
   line.append("&gt;");
}

void validateTemplateType(IdentifierString& type)
{
   if (isTemplateBased(type)) {
      NamespaceName ns(type);
      ns.append('\'');

      parseTemplateName(type, ns.Length());
   }
   else if (type.ident().find("$private'T") != NOTFOUND_POS) {
      int index = type.ident().findLast('\'');
      type.cut(0, index + 1);
   }

}

void parseName(ApiMethodInfo* info, bool templateBased)
{
   int sign_index = info->name.ident().find("<");
   if (sign_index != NOTFOUND_POS) {
      IdentifierString paramType;
      for (int i = sign_index + 1; i < info->name.Length(); i++) {
         if (info->name[i] == ',' || info->name[i] == '>') {
            if (templateBased) {
               validateTemplateType(paramType);
            }

            info->params.add(paramType.c_str());

            paramType.clear();
         }
         else paramType.append(info->name[i]);
      }

      info->name.truncate(sign_index);
   }

}

void parseMethod(ApiMethodInfo* info, ident_t messageLine, bool staticOne, bool extensionOne, bool templateBased)
{
   int paramCount = 0;

   info->isAbstract = messageLine.find("@abstract") != NOTFOUND_POS;
   info->isMultidispatcher = messageLine.find("@multidispatcher") != NOTFOUND_POS;

   pos_t retPos = messageLine.find(" of ");
   if (retPos != NOTFOUND_POS) {
      info->retType.copy(messageLine.c_str() + retPos + 4);

      if (templateBased) {
         validateTemplateType(info->retType);
      }
   }
   else {
      info->retType.copy("system'Object");
      retPos = getlength(messageLine);
   }

   pos_t index = messageLine.find('.') + 1;
   int propIndex = messageLine.find(".prop#");
   if (propIndex != NOTFOUND_POS) {
      info->prop = true;

      info->name.copy(messageLine.c_str() + propIndex + 6, retPos - propIndex - 6);
   }
   else info->name.copy(messageLine.c_str() + index, retPos - index);

   pos_t end = info->name.ident().find('[') + 1;
   if (end != 0)  {
      pos_t size_end = info->name.ident().find(']') + 1;

      String<char, 4> tmp;
      tmp.copy(info->name.c_str() + end, size_end - end - 1);
      paramCount = ident_t(tmp.c_str()).toInt();

      info->name.truncate(end - 1);
   }

   parseName(info, templateBased);

   if (info->params.Count() < paramCount) {
      // if weak message
      for (int i = 0; i < paramCount; i++) {
         info->params.add("system'Object");
      }
   }

   if (info->name.compare("#dispatch"))
      info->special = true;
   else if (info->name.compare("#new"))
      info->special = true;
   else if (info->name.compare("#constructor")) {
      info->name.copy("<i>constructor</i>");
   }
   else if (info->name.compare("#cast")) {
      info->special = true;
      info->convertor = true;
      info->name.copy("<i>cast</i>");
   }

   if (info->isMultidispatcher && !info->isAbstract)
      info->special = true;

   if (extensionOne)
      info->prefix.append("extension ");

   if (info->isAbstract)
      info->prefix.append("abstract ");

   if (staticOne && info->prop)
      info->prefix.append("static ");

   if (info->prop && paramCount == 0) {
      info->prefix.append("get");
   }
   else if (info->prop && paramCount == 1) {
      info->prefix.append("set");

      info->retType.clear();
   }
}

void parseField(ApiClassInfo* info, ident_t line)
{
   auto fieldInfo = new ApiFieldInfo();

   pos_t retPos = line.find(" of ");
   if (retPos != NOTFOUND_POS) {
      fieldInfo->name.copy(line, retPos);

      fieldInfo->type.copy(line.c_str() + 4);

      if (info->templateBased) {
         validateTemplateType(fieldInfo->type);
      }
   }
   else {
      fieldInfo->type.copy("system'Object");
      fieldInfo->name.copy(line);
   }

   info->fields.add(fieldInfo);

}

void readClassMembers(String<char, LINE_LEN>& line, TextFileReader& reader, ApiClassInfo* info, 
   bool& isAbstract, bool& isClosed, bool& isSealed, bool& isStateless, bool& isRole)
{
   while (true) {
      if (!readLine(line, reader))
         return;

      if (ident_t(line).startsWith("@method ")) {
         if (ident_t(line.c_str()).find("#private&") != NOTFOUND_POS || ident_t(line.c_str()).find("auto#") != NOTFOUND_POS) {
            // ignore private methods
         }
         else {
            ApiMethodInfo* methodInfo = new ApiMethodInfo();

            parseMethod(methodInfo, line.c_str() + 8, false, false, info->templateBased);

            if (methodInfo->prop)
               info->withProps = true;

            if (methodInfo->convertor)
               info->withConversions = true;

            info->methods.add(methodInfo);
         }
      }
      else if (ident_t(line).startsWith("@flag ")) {
         if (ident_t(line).endsWith("elAbstract"))
            isAbstract = true;
         else if (ident_t(line).endsWith("elClosed"))
            isClosed = true;
         else if (ident_t(line).endsWith("elSealed"))
            isSealed = true;
         else if (ident_t(line).endsWith("elRole"))
            isRole = true;
         else if (ident_t(line).endsWith("elStateless"))
            isStateless = true;
      }
      else if (ident_t(line).startsWith("@parent ")) {
         info->parents.add(line + 8);
      }
      else if (ident_t(line).startsWith("@field ")) {
         parseField(info, line.c_str() + 7);
      }
      else if (line.Length() == 0)
         return;
   }
}

void readClassClassMembers(String<char, LINE_LEN>& line, TextFileReader& reader, ApiClassInfo* info)
{
   while (true) {
      if (!readLine(line, reader))
         return;

      if (ident_t(line).startsWith("@method ")) {
         if (ident_t(line.c_str()).find("#private&") != NOTFOUND_POS || ident_t(line.c_str()).find("auto#") != NOTFOUND_POS) {
            // ignore private methods
         }
         else {
            ApiMethodInfo* methodInfo = new ApiMethodInfo();

            parseMethod(methodInfo, line.c_str() + 8, true, false, info->templateBased);

            if (isMethod(methodInfo))
               info->withConstructors = true;
            if (methodInfo->prop)
               info->withStaticProps = true;

            info->constructors.add(methodInfo);
         }
      }
      else if (line.Length() == 0)
         return;
   }
}

void readExtensions(String<char, LINE_LEN>& line, TextFileReader& reader, ApiClassInfo* info)
{
   while (true) {
      if (!readLine(line, reader))
         return;

      if (ident_t(line).startsWith("@method ")) {
         ApiMethodInfo* methodInfo = new ApiMethodInfo();

         parseMethod(methodInfo, line.c_str() + 8, true, true, info->templateBased);

         info->extensions.add(methodInfo);
      }
      else if (line.Length() == 0)
         return;
   }
}

bool readClassInfo(String<char, LINE_LEN>& line, TextFileReader& reader, List<ApiModuleInfo*>& modules)
{
   if (emptystr(line)) {
      if (!readLine(line, reader))
         return false;
   }
   
   if (ident_t(line).startsWith("class ")) {
      ApiModuleInfo* moduleInfo = nullptr;

      NamespaceName ns(line + 6);
      ns.append('\'');

      moduleInfo = findModule(modules, ns.c_str());
      if (!moduleInfo) {
         moduleInfo = new ApiModuleInfo();
         moduleInfo->name.copy(ns.c_str());

         modules.add(moduleInfo);
      }

      if (ident_t(line).endsWith("#class")) {
         IdentifierString className(line + 6 + ns.Length());
         className.truncate(className.Length() - 6);

         if (isTemplateBased(className)) {
            parseTemplateName(className, 0);
         }

         ApiClassInfo* info = findClass(moduleInfo, className.c_str());

         readClassClassMembers(line, reader, info);
      }
      else {
         ApiClassInfo* info = new ApiClassInfo();

         info->fullName.copy(line + 6);
         if (isTemplateBased(info->fullName)) {
            info->templateBased = true;

            parseTemplateName(info->fullName, ns.Length());
         }

         info->name.copy(info->fullName.c_str() + ns.Length());

         bool isAbstract = false, isClosed = false, isSealed = false;
         bool isStateless = false, isRole = false;
         readClassMembers(line, reader, info, isAbstract, isClosed, isSealed, isStateless, isRole);

         info->prefix.clear();
         if (isSealed) {
            info->prefix.append("sealed ");
         }

         if (info->templateBased) {
            if (isRole && isStateless) {
               info->prefix.append("Singleton ");
            }
            info->prefix.append("Template ");
         }
         else if (isAbstract && isClosed) {
            info->prefix.append("Interface ");
         }
         else if (isAbstract) {
            info->prefix.append("abstract Class ");
         }
         else if (isRole && isStateless) {
            info->prefix.append("Singleton ");
         }
         else info->prefix.append("Class ");

         moduleInfo->classes.add(info);
      }
   }
   else if (ident_t(line).startsWith("extension ")) {
      ApiModuleInfo* moduleInfo = nullptr;

      pos_t index = ident_t(line).find(" of ");
      IdentifierString targetRef(line + index + 4);
      line[index] = 0;

      NamespaceName ns(line + 10);
      ns.append('\'');

      moduleInfo = findModule(modules, ns.c_str());
      if (!moduleInfo) {
         moduleInfo = new ApiModuleInfo();
         moduleInfo->name.copy(ns.c_str());

         modules.add(moduleInfo);
      }

      NamespaceName targetNs(targetRef);
      targetNs.append('\'');

      IdentifierString targetName(targetRef.c_str() + targetNs.Length());

      ApiClassInfo* info = findClass(moduleInfo, targetName);
      if (info == nullptr) {
         info = new ApiClassInfo();

         info->fullName.copy(targetRef);
         info->name.copy(targetName.c_str());


      }

      readExtensions(line, reader, info);
   }

   return true;
}

int main(int argc, char* argv[])
{
   printf("ELENA command line Html Documentation generator (C)2006-19 by Alexei Rakov\n");

   if (argc != 2) {
      printf("api2html <file>\n");
      return 0;
   }

   Path path;
   path.copy(argv[1]);

   pos_t pos = ident_t(argv[1]).findLast('\\', -1);
   IdentifierString ns;
   ns.copy(argv[1] + pos + 1);

   TextFileReader reader(path.c_str(), _ELENA_::feUTF8, true);
   if (!reader.isOpened()) {
      printf("Cannot open the temporal file %s", path.c_str());
      return -1;
   }

   List<ApiModuleInfo*> modules;

   String<char, LINE_LEN> line;
   line[0] = 0;

   while (readClassInfo(line, reader, modules));

   auto it = modules.start();
   while (!it.Eof()) {
      IdentifierString name(ns);
      writeNs(name, *it);

      name.append(".html");
      
      IdentifierString summaryname(ns);
      writeNs(summaryname, *it);

      summaryname.append("-summary");
      summaryname.append(".html");

      Path outPath;
      outPath.copy(name);
      
      Path outSumPath;
      outSumPath.copy(summaryname);

      TextFileWriter bodyWriter(outPath.str(), feUTF8, false);
      TextFileWriter summaryWriter(outSumPath.str(), feUTF8, false);

      writeHeader(summaryWriter, ns, nullptr);
      writeHeader(bodyWriter, ns, summaryname);

      //
      //	const char* package = config.getSetting("#general#", "#name");
      const char* shortDescr = /*config.getSetting("#general#", "#shortdescr")*/nullptr;

      writeSummaryHeader(summaryWriter, ns, shortDescr);
      //

      auto class_it = (*it)->classes.start();
      while (!class_it.Eof()) {
         writeSummaryTable(summaryWriter, *class_it, name);
         writeBody(bodyWriter, *class_it, ns);
         
         class_it++;
      }

      writeSummaryFooter(summaryWriter);

      writeFooter(summaryWriter, NULL);
      writeFooter(bodyWriter, summaryname);

      it++;
   }

   return 0;
}

