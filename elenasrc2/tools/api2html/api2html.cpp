//---------------------------------------------------------------------------
//              E L E N A   p r o j e c t
//                Command line syntax generator main file
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <windows.h>
#include "config.h"
#include "elena.h"

#define TITLE "ELENA Standard Library 2.0: Package "
#define TITLE2 "ELENA&nbsp;Standard&nbsp;Library<br>2.0"

#define OPERATORS "+-*/=<>?!"

using namespace _ELENA_;

void writeHeader(TextFileWriter& writer, const _text_t* package, const _text_t* packageLink)
{
   writer.writeTextNewLine("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Frameset//EN\"\"http://www.w3.org/TR/REC-html40/frameset.dtd\">");
   writer.writeTextNewLine("<HTML>");
   writer.writeTextNewLine("<HEAD>");
   writer.writeTextNewLine("<TITLE>");
   writer.writeText(TITLE);
   writer.writeTextNewLine(package);
   writer.writeTextNewLine("</TITLE>");
   writer.writeTextNewLine("<meta name=\"collection\" content=\"api\">");
   writer.writeTextNewLine("</HEAD>");
   writer.writeTextNewLine("<BODY BGCOLOR=\"white\">");

   writer.writeTextNewLine("<A NAME=\"navbar_top\"><!-- --></A>");
   writer.writeTextNewLine("<TABLE BORDER=\"0\" WIDTH=\"100%\" CELLPADDING=\"1\" CELLSPACING=\"0\">");
   writer.writeTextNewLine("<TR>");
   writer.writeTextNewLine("<TD COLSPAN=2 BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">");
   writer.writeTextNewLine("<A NAME=\"navbar_top_firstrow\"><!-- --></A>");
   writer.writeTextNewLine("<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"3\">");
   writer.writeTextNewLine("  <TR ALIGN=\"center\" VALIGN=\"top\">");
   writer.writeTextNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <A HREF=\"index.html\"><FONT CLASS=\"NavBarFont1\"><B>Overview</B></FONT></A>&nbsp;</TD>");
   if (!emptystr(packageLink)) {
      writer.writeText("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\"><A HREF=\"");
      writer.writeText(packageLink);
      writer.writeTextNewLine("\"><FONT CLASS=\"NavBarFont1\"><B>Package</B></FONT></A>&nbsp;</TD>");
   }
   else writer.writeTextNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <FONT CLASS=\"NavBarFont1\">Package</FONT>&nbsp;</TD>");
   writer.writeTextNewLine("  </TR>");
   writer.writeTextNewLine("</TABLE>");
   writer.writeTextNewLine("</TD>");

   writer.writeTextNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" ROWSPAN=3><EM><b>");
   writer.writeText(TITLE2);
   writer.writeTextNewLine("</b></EM>");
   writer.writeTextNewLine("</TD>");
   writer.writeTextNewLine("</TR>");
   writer.writeTextNewLine("</TABLE>");

   writer.writeTextNewLine("<DL>");
   writer.writeTextNewLine("<HR>");
}

void writeSummaryHeader(TextFileWriter& writer, const _text_t* name, const _text_t* shortDescr)
{
   writer.writeTextNewLine("<H2>");
   writer.writeText("Package ");
   writer.writeTextNewLine(name);
   writer.writeTextNewLine("</H2>");
   writer.writeTextNewLine(shortDescr);
   writer.writeTextNewLine("<P>");

   writer.writeTextNewLine("<TABLE BORDER=\"1\" CELLPADDING=\"3\" CELLSPACING=\"0\" WIDTH=\"100%\">");
   writer.writeTextNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeTextNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeTextNewLine("<B>Class Summary</B></FONT></TD>");
   writer.writeTextNewLine("</TR>");
}

void writeSummaryTable(TextFileWriter& writer, IniConfigFile& config, const _text_t* name, const _text_t* bodyFileName)
{
   writer.writeTextNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
   writer.writeText("<TD WIDTH=\"15%\"><B><A HREF=\"");
   writer.writeText(bodyFileName);
   writer.writeText("#");
   writer.writeText(name);
   writer.writeText("\">");
   writer.writeText(name);
   writer.writeTextNewLine("</A></B></TD>");
   writer.writeText("<TD>");
   const _text_t* descr = config.getSetting(name, _T("#shortdescr"), NULL);
   if (!emptystr(descr)) {
      writer.writeText(descr);
   }
   else writer.writeText(name);

   writer.writeText("</TD>");
   writer.writeTextNewLine("</TR>");
}

inline void repeatStr(TextFileWriter& writer, const char* s, int count)
{
   for(int i = 0 ; i < count ; i++) writer.writeText(s);
}

inline const _text_t* find(const _text_t* s, char ch)
{
   if (emptystr(s)) {
      return NULL;
   }
   else {
      int index = StringHelper::find(s, ch);
      if (index==-1)
         index = getlength(s) - 1;

      return s + index + 1;
   }
}

inline bool exists(const _text_t* s, char ch)
{
   if (emptystr(s)) {
      return false;
   }
   else return StringHelper::find(s, ch) != -1;
}

inline void writeLeft(TextFileWriter& writer, const _text_t* s, const _text_t* right)
{
   if (emptystr(right)) {
      writer.writeText(s);
   }
   else writer.write(s, right - s - 1);
}

void writeLink(TextFileWriter& writer, const _text_t* link, const _text_t* file = NULL)
{
   const _text_t* body = find(link, ':');

   writer.writeText("<A HREF=\"");

   if (StringHelper::find(link, '#') == -1) {
      if (!emptystr(file)) {
         writer.writeText(file);
      }
      writer.writeChar('#');
      writeLeft(writer, link, body);
   }
   else writeLeft(writer, link, body);

   writer.writeText("\">");
   if (emptystr(body)) {
      writer.writeText(link);
   }
   else writeLeft(writer, body, find(body, ';'));

   writer.writeTextNewLine("</A>");
}

void writeParamLink(TextFileWriter& writer, const _text_t* link, const _text_t* right, const _text_t* file)
{
   writer.writeText("<A HREF=\"");

   int pos = StringHelper::find(link, '#');

   if (pos == -1 || pos > (right - link)) {
      writer.writeText(file);
      writer.writeChar('#');
	   writeLeft(writer, link, right);
   }
   else {
      writeLeft(writer, link, right);
	   link += StringHelper::find(link, '#') + 1;
   }
   writer.writeText("\">");
   writeLeft(writer, link, right);
   writer.writeTextNewLine("</A>");
}

void writeSignature(TextFileWriter& writer, const _text_t* parameters)
{
   if (parameters[0] != '&') {
      const _text_t* param = parameters;
      const _text_t* next_subj = find(param, '&');

      writer.writeText(" : ");
      writeParamLink(writer, param, next_subj, _T("protocol.html"));

      parameters = next_subj;
   }

   while (!emptystr(parameters)) {
      if (parameters[0]!='&')
         writer.writeText("&");

      const _text_t* subj = parameters;
      const _text_t* param = find(parameters, ':');
      const _text_t* next_subj = find(param, '&');

      writeLeft(writer, subj, param);
      writer.writeText(" : ");
      writeParamLink(writer, param, next_subj, _T("protocol.html"));

      parameters = next_subj;
   }
}

void writeMessage(TextFileWriter& writer, const _text_t* message)
{
   const _text_t* parameter = find(message, ',');
   const _text_t* result = find(parameter, ',');
   const _text_t* descr = emptystr(result) ? find(parameter, ';') : find(result, ';');

   if (emptystr(result)) {
      result = descr;
   }

   bool oper = (StringHelper::find(OPERATORS, message[0])!=-1);

   if (message[0]=='<' && message[1] == 'a') {
      oper = false;
   }
   else if (message[0]=='<') {
      writer.writeText("&lt;");
      message++;
   }
   else if (message[0]=='>') {
      writer.writeText("&gt;");
      message++;
   }
   writeLeft(writer, message, parameter);
   if (!emptystr(parameter) && result != parameter && parameter[0] != ',') {
      if (!oper) {
         if (exists(parameter, '&')) {
            IdentifierString signature(parameter, result - parameter - 1);

            writer.writeText(" ");
            writeSignature(writer, signature);
         }
         else {
            writer.writeText(" : ");

            writeParamLink(writer, parameter, result, _T("protocol.html"));
         }
      }
      else writer.writeText(" ");
   }
   if (!emptystr(result) && result != descr && result[0] != ';') {
      writer.writeText(" = ");
      writeParamLink(writer, result, descr, _T("protocol.html"));
   }
}

void writeParents(TextFileWriter& writer, IniConfigFile& config, const _text_t* name,  const _text_t* moduleName)
{
   writer.writeTextNewLine("<PRE>");
   int indent = 0;
   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), _T("#parent"))) {
         repeatStr(writer, "  ", indent - 1);
         if (indent > 0) writer.writeTextNewLine(" |");
         repeatStr(writer, "  ", indent - 1);
         if (indent > 0) writer.writeText(" +--");

         writeLink(writer, *it);

         indent++;
      }
      it++;
   }
   repeatStr(writer, "  ", indent - 1);
   writer.writeTextNewLine(" |");
   repeatStr(writer, "  ", indent - 1);
   writer.writeText(" +--<B>");
   writer.writeText(moduleName);
   writer.writeText("'");
   writer.writeText(name);
   writer.writeTextNewLine("</B>");

   writer.writeTextNewLine("</PRE>");
}

void writeProtocols(TextFileWriter& writer, const _text_t* name, IniConfigFile& config)
{
   if (!emptystr(config.getSetting(name, _T("#protocol"), NULL))) {
      ConfigCategoryIterator it = config.getCategoryIt(name);
      writer.writeText("<B>All Implemented Protocols:</B>");
      writer.writeTextNewLine("<DL>");
      while (!it.Eof()) {
         if (StringHelper::compare(it.key(), _T("#protocol"))) {
            writer.writeTextNewLine("<DT>");
            writeLink(writer, *it, _T("protocol.html"));
            writer.writeTextNewLine("</DT>");
         }
         it++;
      }
      writer.writeTextNewLine("</DL>");
   }
}

void writeFields(TextFileWriter& writer, IniConfigFile& config, const _text_t* name)
{
   // field section
   writer.writeTextNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeTextNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeTextNewLine("<B>Field Summary</B></FONT></TD>");

   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), _T("#field"))) {
         const _text_t* descr = find(*it, ';');
         writer.writeTextNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
         writer.writeTextNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");
         writer.writeTextNewLine("<CODE>");
         writeLeft(writer, *it, descr);
         writer.writeTextNewLine("&nbsp;</CODE>");
         writer.writeTextNewLine("</TD>");
         writer.writeText("<TD><CODE>");
         writer.writeText(descr);
         writer.writeTextNewLine("</CODE>");
         writer.writeTextNewLine("</TD>");
         writer.writeTextNewLine("</TR>");
      }
      it++;
   }
}

void writeProperties(TextFileWriter& writer, IniConfigFile& config, const _text_t* name)
{
   // property section
   writer.writeTextNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeTextNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeTextNewLine("<B>Property Summary</B></FONT></TD>");

   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), _T("#property"))) {
         const _text_t* descr = find(*it, ';');
         writer.writeTextNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
         writer.writeTextNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");

         writeLink(writer, *it, _T("properties.html"));
         writer.writeTextNewLine("</DT>");

         writer.writeText("<TD><CODE>");
         writer.writeText(descr);
         writer.writeTextNewLine("</CODE>");
         writer.writeTextNewLine("</TD>");
         writer.writeTextNewLine("</TR>");

      }
      it++;
   }
}

void writeMethods(TextFileWriter& writer, IniConfigFile& config, const _text_t* name)
{
   if (emptystr(config.getSetting(name, _T("#method"), NULL)))
      return;

   // method section
   writer.writeTextNewLine("<A NAME=\"method_summary\"><!-- --></A>");

   writer.writeTextNewLine("<TR BGCOLOR=\"#CCCCFF\" CLASS=\"TableHeadingColor\">");
   writer.writeTextNewLine("<TD COLSPAN=2><FONT SIZE=\"+2\">");
   writer.writeTextNewLine("<B>Method Summary</B></FONT></TD>");
   writer.writeTextNewLine("</TR>");

   ConfigCategoryIterator it = config.getCategoryIt(name);
   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), _T("#method"))) {
         writer.writeTextNewLine("<TR BGCOLOR=\"white\" CLASS=\"TableRowColor\">");
         writer.writeTextNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" WIDTH=\"30%\">");
         writer.writeTextNewLine("<CODE>&nbsp;");

         const _text_t* message = *it;
         const _text_t* descr = find(message, ';');

         writeMessage(writer, message);

         writer.writeTextNewLine("</TD>");
         writer.writeText("<TD><CODE>");

         if (!emptystr(descr)) {
            writer.writeText(descr);
         }
         else writer.writeText("&nbsp;");
         writer.writeTextNewLine("</CODE>");
         writer.writeTextNewLine("</TD>");
         writer.writeTextNewLine("</TR>");
      }
      it++;
   }
}

void writeBody(TextFileWriter& writer, IniConfigFile& config, const _text_t* name,  const _text_t* moduleName)
{
   const _text_t* title = config.getSetting(name, _T("#title"), NULL);
   if (title==NULL)
      title = name;

   writer.writeText("<A NAME=\"");
   writer.writeText(name);
   writer.writeTextNewLine("\">");
   writer.writeTextNewLine("<HR>");
   writer.writeTextNewLine("<!-- ======== START OF CLASS DATA ======== -->");
   writer.writeTextNewLine("<H2>");
   writer.writeTextNewLine("<FONT SIZE=\"-1\">");
   writer.writeText(moduleName);
   writer.writeTextNewLine("</FONT>");
   writer.writeTextNewLine("<BR>");
   writer.writeText(title);
   writer.writeTextNewLine("</H2>");

   if (!emptystr(config.getSetting(name, _T("#parent"), NULL))) {
      writeParents(writer, config, name, moduleName);
   }
   writeProtocols(writer, name, config);
   const _text_t* descr = config.getSetting(name, _T("#shortdescr"), NULL);
   if (!emptystr(descr)) {
      writer.writeTextNewLine("<P>");
      writer.writeTextNewLine(descr);
      writer.writeTextNewLine("<P>");
   }
   else writer.writeTextNewLine(name);

   writer.writeTextNewLine("<P>");

   writer.writeTextNewLine("<TABLE BORDER=\"1\" CELLPADDING=\"3\" CELLSPACING=\"0\" WIDTH=\"100%\">");

   if (!emptystr(config.getSetting(name, _T("#field"), NULL))) {
      writer.writeTextNewLine("<!-- =========== FIELD SUMMARY =========== -->");
      writeFields(writer, config, name);
   }

   if (!emptystr(config.getSetting(name, _T("#property"), NULL))) {
      writer.writeTextNewLine("<!-- =========== PROPERTY SUMMARY =========== -->");
      writeProperties(writer, config, name);
   }

   writer.writeTextNewLine("<!-- ========== METHOD SUMMARY =========== -->");
   writeMethods(writer, config, name);

   writer.writeTextNewLine("</TABLE>");
}

void writeFooter(TextFileWriter& writer, const _text_t* packageLink)
{
   writer.writeTextNewLine("<HR>");
   writer.writeTextNewLine("<A NAME=\"navbar_top\"><!-- --></A>");
   writer.writeTextNewLine("<TABLE BORDER=\"0\" WIDTH=\"100%\" CELLPADDING=\"1\" CELLSPACING=\"0\">");
   writer.writeTextNewLine("<TR>");
   writer.writeTextNewLine("<TD COLSPAN=2 BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">");
   writer.writeTextNewLine("<A NAME=\"navbar_top_firstrow\"><!-- --></A>");
   writer.writeTextNewLine("<TABLE BORDER=\"0\" CELLPADDING=\"0\" CELLSPACING=\"3\">");
   writer.writeTextNewLine("  <TR ALIGN=\"center\" VALIGN=\"top\">");
   writer.writeTextNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <A HREF=\"index.html\"><FONT CLASS=\"NavBarFont1\"><B>Overview</B></FONT></A>&nbsp;</TD>");
   if (!emptystr(packageLink)) {
      writer.writeText("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\"><A HREF=\"");
      writer.writeText(packageLink);
      writer.writeTextNewLine("\"><FONT CLASS=\"NavBarFont1\"><B>Package</B></FONT></A>&nbsp;</TD>");
   }
   else writer.writeTextNewLine("  <TD BGCOLOR=\"#EEEEFF\" CLASS=\"NavBarCell1\">    <FONT CLASS=\"NavBarFont1\">Package</FONT>&nbsp;</TD>");
   writer.writeTextNewLine("  </TR>");
   writer.writeTextNewLine("</TABLE>");
   writer.writeTextNewLine("</TD>");

   writer.writeTextNewLine("<TD ALIGN=\"right\" VALIGN=\"top\" ROWSPAN=3><EM><b>");
   writer.writeText(TITLE2);
   writer.writeTextNewLine("</b></EM>");
   writer.writeTextNewLine("</TD>");
   writer.writeTextNewLine("</TR>");
   writer.writeTextNewLine("</TABLE>");
}

void writeSummaryFooter(TextFileWriter& writer)
{
   writer.writeTextNewLine("</TABLE>");
}

int main(int argc, char* argv[])
{
   printf("ELENA command line Html Documentation generator (C)2006-13 by Alexei Rakov\n");

   IniConfigFile config(true);

   if (argc != 2) {
      printf("api2html <file>\n");
      return 0;
   }

   config.load(ConstantIdentifier(argv[1]), /*CP_OEMCP*/feAnsi); // !! temporal

   FileName fileName(argv[1]);
   IdentifierString name(fileName);
   name.append(".html");

   IdentifierString summaryname(fileName);
   summaryname.append("-summary");
   summaryname.append(".html");

   TextFileWriter bodyWriter(name, feAnsi, false);
   TextFileWriter summaryWriter(summaryname, feAnsi, false);

	const _text_t* package = config.getSetting(_T("#general#"), _T("#name"));
	const _text_t* shortDescr = config.getSetting(_T("#general#"), _T("#shortdescr"));
	writeHeader(summaryWriter, package, NULL);
   writeHeader(bodyWriter, package, summaryname);
   writeSummaryHeader(summaryWriter, package, shortDescr);

   ConfigCategoryIterator classNode = config.getCategoryIt(_T("#list#"));
   while (!classNode.Eof()) {
      writeSummaryTable(summaryWriter, config, classNode.key(), name);
      writeBody(bodyWriter, config, classNode.key(), package);

      classNode++;
   }
   writeSummaryFooter(summaryWriter);
   writeFooter(summaryWriter, NULL);
   writeFooter(bodyWriter, summaryname);
   return 0;
}

