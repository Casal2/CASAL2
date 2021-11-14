import string
import re

p = re.compile('\{.*?\}',re.IGNORECASE)
q = re.compile('\}.*?\{',re.IGNORECASE)
r = re.compile('[label]',re.IGNORECASE)

FILE = [open('AutoSyntax/AdditionalPrior.tex', 'r'),
        open('AutoSyntax/AddressableTransformation.tex', 'r'),
        open('AutoSyntax/AgeingError.tex', 'r'),
        open('AutoSyntax/AgeLength.tex', 'r'),
        open('AutoSyntax/AgeWeight.tex', 'r'),
        open('AutoSyntax/Assert.tex', 'r'),
        open('AutoSyntax/Catchability.tex', 'r'),
        open('AutoSyntax/Categories.tex', 'r'),
        open('AutoSyntax/DerivedQuantity.tex', 'r'),
        open('AutoSyntax/Estimate.tex', 'r'),
        open('AutoSyntax/EstimateTransformation.tex', 'r'),
        open('AutoSyntax/InitialisationPhase.tex', 'r'),
        open('AutoSyntax/LengthWeight.tex', 'r'),
        open('AutoSyntax/Likelihood.tex', 'r'),
        open('AutoSyntax/MCMC.tex', 'r'),
        open('AutoSyntax/Minimiser.tex', 'r'),
        open('AutoSyntax/Model.tex', 'r'),
        open('AutoSyntax/Observation.tex', 'r'),
        open('AutoSyntax/Penalty.tex', 'r'),
        open('AutoSyntax/Process.tex', 'r'),
        open('AutoSyntax/Profile.tex', 'r'),
        open('AutoSyntax/Project.tex', 'r'),
        open('AutoSyntax/Report.tex', 'r'),
        open('AutoSyntax/Selectivity.tex', 'r'),
        open('AutoSyntax/Simulate.tex', 'r'),
        open('AutoSyntax/TimeStep.tex', 'r'),
        open('AutoSyntax/TimeVarying.tex', 'r')]
OUTFILE = open('QuickReference.tex', 'w')
VERSION = open('Version.tex', 'r')
SYNTAX_OUTFILE_TEXTPAD = open('CASAL2.syn', 'w')
SYNTAX_OUTFILE_NOTEPADPLUS = open('CASAL2.xml', 'w')

OUTFILE.write('\section{Quick reference}\label{sec:QuickReference}\n')
OUTFILE.write('\\small\n')  

Keywords1 = []
Keywords2 = []
Keywords3 = ["F","False","T","True","t","f","true","false"]
Keywords4 = []
Keywords5 = []
Keywords6 = []

for i in range(len(FILE)):
  count=0
  Command = ''
  for line in FILE[i]:
    if(line.startswith(('\\section', '\\subsection', '\\defComLab', '\\subsubsection', '\\commandlabsubarg', '\\defSub'))):
      # if a line has a label, then delete it
      line = re.sub('\\.label','',line)
      # if a line has a ref, then delete it
      line = re.sub('\\.defRef','',line)
      line = re.sub('\\.ref','',line)
      # strip white space at start & end
      line = line.strip()
      ###############################################################
      # Scan for Keywords for the syntax files
      ###############################################################
      if (line.startswith('\\defCom')) :
        m = p.search(line)
        Command = m.group()[1:(len(m.group())-1)]
        Keywords1.append(m.group()[1:(len(m.group())-1)])
      if (line.startswith('\\commandlabsubarg')) :
        result = line.split('{')
        Keywords2.append(result[3].replace('}','').replace('.','').replace('\\',''))
      if (line.startswith('\\defSub')) :
        result = line.split('{')
        Keywords4.append(result[1].replace('}','').replace('.','').replace('\\',''))
      
      ###############################################################
      # Reformat the LateX for the manual
      ###############################################################
      if (line.startswith('\\defComLab')):
        line = '\n' + line + '\n'
        line = re.sub('\.$', '',line)
      if (line.startswith('\\subsubsection')):
        line = ''
      if (line.startswith('\\commandlabsubarg')):
        line = re.sub('\.$', '',line)
        line = line + '\n'
      ###############################################################
      # Print to the output file
      ###############################################################
      OUTFILE.write(line)
      OUTFILE.write('\n')
      
OUTFILE.write('\n\\normalsize\n')  

# Add additional keywords
Keywords5 = ['table','end_table']

###############################################################
# Additional sorting and reformatting of Keywords
###############################################################
Keywords1 = list(set(Keywords1))
Keywords1.sort()
Keywords2 = list(set(Keywords2))
Keywords2.sort()
Keywords3 = list(set(Keywords3))
Keywords3.sort()
Keywords4 = list(set(Keywords4))
Keywords4.sort()
Keywords5 = list(set(Keywords5))
Keywords5.sort()
Keywords6 = list(set(Keywords6))
Keywords6.sort()

for i in range(len(Keywords1)):
  Keywords1[i] = Keywords1[i].replace('\\','')
  

###############################################################
# SYNTAX for TEXTPAD
###############################################################
SYNTAX_OUTFILE_TEXTPAD.write("; TextPad syntax definitions for CASAL2; This is auto generated from the QuickReference.py script do not edit\n")
SYNTAX_OUTFILE_TEXTPAD.write("; ===================================================================================\n;\n;\n")
SYNTAX_OUTFILE_TEXTPAD.write("C=1\n")
SYNTAX_OUTFILE_TEXTPAD.write("[Syntax]\n")
SYNTAX_OUTFILE_TEXTPAD.write("Namespace1 = 6\n")
SYNTAX_OUTFILE_TEXTPAD.write("IgnoreCase = No\n")
SYNTAX_OUTFILE_TEXTPAD.write("KeyWordLength = 0\n")
SYNTAX_OUTFILE_TEXTPAD.write("BracketChars = {[]}\n")
SYNTAX_OUTFILE_TEXTPAD.write("OperatorChars = -+*/!~%^&|=$:!_<>\n")
SYNTAX_OUTFILE_TEXTPAD.write("PreprocStart =\n")
SYNTAX_OUTFILE_TEXTPAD.write("HexPrefix = 0x\n")
SYNTAX_OUTFILE_TEXTPAD.write("SyntaxStart =\n")
SYNTAX_OUTFILE_TEXTPAD.write("SyntaxEnd =\n")
SYNTAX_OUTFILE_TEXTPAD.write("CommentStart = /*\n")
SYNTAX_OUTFILE_TEXTPAD.write("CommentEnd = */ \n")
SYNTAX_OUTFILE_TEXTPAD.write("CommentStartAlt =\n")
SYNTAX_OUTFILE_TEXTPAD.write("CommentEndAlt =\n")
SYNTAX_OUTFILE_TEXTPAD.write("SingleComment = #\n")
SYNTAX_OUTFILE_TEXTPAD.write("SingleCommentCol =\n")
SYNTAX_OUTFILE_TEXTPAD.write("SingleCommentAlt = @comment\n")
SYNTAX_OUTFILE_TEXTPAD.write("SingleCommentColAlt =\n")
SYNTAX_OUTFILE_TEXTPAD.write("SingleCommentEsc =\n")
SYNTAX_OUTFILE_TEXTPAD.write("StringsSpanLines = No\n")
SYNTAX_OUTFILE_TEXTPAD.write("StringStart = \"\n")
SYNTAX_OUTFILE_TEXTPAD.write("StringEnd = \"\n")
SYNTAX_OUTFILE_TEXTPAD.write("StringAlt =\n")
SYNTAX_OUTFILE_TEXTPAD.write("StringEsc = \\ \n")
SYNTAX_OUTFILE_TEXTPAD.write("CharStart = '\n")
SYNTAX_OUTFILE_TEXTPAD.write("CharEnd = '\n")
SYNTAX_OUTFILE_TEXTPAD.write("CharEsc = \\ \n")

SYNTAX_OUTFILE_TEXTPAD.write("\n[Keywords 1]\n")
for i in Keywords1 :
  SYNTAX_OUTFILE_TEXTPAD.write("@" + i)
  SYNTAX_OUTFILE_TEXTPAD.write("\n")

SYNTAX_OUTFILE_TEXTPAD.write("\n[Keywords 2]\n")
for i in Keywords2 :
  SYNTAX_OUTFILE_TEXTPAD.write(i)
  SYNTAX_OUTFILE_TEXTPAD.write("\n")

SYNTAX_OUTFILE_TEXTPAD.write("\n[Keywords 3]\n")
for i in Keywords3 :
  SYNTAX_OUTFILE_TEXTPAD.write(i)
  SYNTAX_OUTFILE_TEXTPAD.write("\n")

SYNTAX_OUTFILE_TEXTPAD.write("\n")
SYNTAX_OUTFILE_TEXTPAD.write("\n[Keywords 4]\n")
for i in Keywords4 :
  SYNTAX_OUTFILE_TEXTPAD.write(i)
  SYNTAX_OUTFILE_TEXTPAD.write("\n")

SYNTAX_OUTFILE_TEXTPAD.write("\n[Keywords 5]\n")
for i in Keywords5 :
  SYNTAX_OUTFILE_TEXTPAD.write(i)
  SYNTAX_OUTFILE_TEXTPAD.write("\n")

SYNTAX_OUTFILE_TEXTPAD.write("\n[Preprocessor keywords]\n")
for i in Keywords6 :
  SYNTAX_OUTFILE_TEXTPAD.write(i)
  SYNTAX_OUTFILE_TEXTPAD.write("\n")

###############################################################
# SYNTAX for Notepad++
###############################################################
SYNTAX_OUTFILE_NOTEPADPLUS.write("<!-- Notepad++ syntax definitions for CASAL2; This is auto generated from the QuickReference.py script do not edit -->\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("<!-- ============================================================================================================= -->\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("<NotepadPlus>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("    <UserLang name=\"Casal2\" ext=\"csl2\" udlVersion=\"2.1\">\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("        <Settings>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Global caseIgnored=\"yes\" allowFoldOfComments=\"no\" foldCompact=\"no\" forcePureLC=\"0\" decimalSeparator=\"0\" />\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Prefix Keywords1=\"no\" Keywords2=\"no\" Keywords3=\"no\" Keywords4=\"no\" Keywords5=\"no\" Keywords6=\"no\" Keywords7=\"no\" Keywords8=\"no\" />\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("        </Settings>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("        <KeywordLists>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Comments\">00# 01 02 03/* 04*/</Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Numbers, prefix1\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Numbers, prefix2\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Numbers, extras1\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Numbers, extras2\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Numbers, suffix1\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Numbers, suffix2\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Numbers, range\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Operators1\">-+*/~%^;|=$:!_&lt;&gt;</Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Operators2\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in code1, open\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in code1, middle\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in code1, close\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in code2, open\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in code2, middle\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in code2, close\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in comment, open\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in comment, middle\"></Keywords>\n")
SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Folders in comment, close\"></Keywords>\n")

SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Keywords1\">")
for i in Keywords1 :
  SYNTAX_OUTFILE_NOTEPADPLUS.write("@" + i)
  SYNTAX_OUTFILE_NOTEPADPLUS.write("&#x000D;&#x000A;")
SYNTAX_OUTFILE_NOTEPADPLUS.write("</Keywords>")

SYNTAX_OUTFILE_NOTEPADPLUS.write("\n            <Keywords name=\"Keywords2\">")
for i in Keywords2 :
  SYNTAX_OUTFILE_NOTEPADPLUS.write(i)
  SYNTAX_OUTFILE_NOTEPADPLUS.write("&#x000D;&#x000A;")
SYNTAX_OUTFILE_NOTEPADPLUS.write("</Keywords>")

SYNTAX_OUTFILE_NOTEPADPLUS.write("\n            <Keywords name=\"Keywords3\">")
for i in Keywords3 :
  SYNTAX_OUTFILE_NOTEPADPLUS.write(i)
  SYNTAX_OUTFILE_NOTEPADPLUS.write("&#x000D;&#x000A;")
SYNTAX_OUTFILE_NOTEPADPLUS.write("</Keywords>")

SYNTAX_OUTFILE_NOTEPADPLUS.write("\n            <Keywords name=\"Keywords4\">")
for i in Keywords4 :
  SYNTAX_OUTFILE_NOTEPADPLUS.write(i)
  SYNTAX_OUTFILE_NOTEPADPLUS.write("&#x000D;&#x000A;")
SYNTAX_OUTFILE_NOTEPADPLUS.write("</Keywords>")

SYNTAX_OUTFILE_NOTEPADPLUS.write("\n            <Keywords name=\"Keywords5\">")
for i in Keywords5 :
  SYNTAX_OUTFILE_NOTEPADPLUS.write(i)
  SYNTAX_OUTFILE_NOTEPADPLUS.write("&#x000D;&#x000A;")
SYNTAX_OUTFILE_NOTEPADPLUS.write("</Keywords>")

SYNTAX_OUTFILE_NOTEPADPLUS.write('            <Keywords name="Keywords5"></Keywords>\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <Keywords name="Keywords6"></Keywords>\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <Keywords name="Keywords7"></Keywords>\n')

SYNTAX_OUTFILE_NOTEPADPLUS.write("            <Keywords name=\"Keywords8\">")
SYNTAX_OUTFILE_NOTEPADPLUS.write("!include")
SYNTAX_OUTFILE_NOTEPADPLUS.write("&#x000D;&#x000A;")
SYNTAX_OUTFILE_NOTEPADPLUS.write("</Keywords>")
SYNTAX_OUTFILE_NOTEPADPLUS.write("\n")

SYNTAX_OUTFILE_NOTEPADPLUS.write('            <Keywords name="Delimiters">00&quot; 01 02&quot; 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20 21 22 23</Keywords>\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('        </KeywordLists>\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('        <Styles>\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DEFAULT" fgColor="000000" bgColor="FFFFFF" fontName="&#x6180;&#xC305;&#x0273;" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="COMMENTS" fgColor="808040" bgColor="FFFFFF" fontName="0" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="LINE COMMENTS" fgColor="808040" bgColor="FFFFFF" fontName="" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="NUMBERS" fgColor="000000" bgColor="FFFFFF" fontName="0" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS1" fgColor="FF0000" bgColor="FFFFFF" fontName="4" fontStyle="1" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS2" fgColor="0000FF" bgColor="FFFFFF" fontName="" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS3" fgColor="800000" bgColor="FFFFFF" fontName="Backup" fontStyle="1" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS4" fgColor="8080FF" bgColor="FFFFFF" fontName="" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS5" fgColor="0000FF" bgColor="FFFFFF" fontName="" fontStyle="1" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS6" fgColor="000000" bgColor="FFFFFF" fontName="MRU" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS7" fgColor="000000" bgColor="FFFFFF" fontName="&#x7020;&#xC305;&#x0273;" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="KEYWORDS8" fgColor="FF0000" bgColor="FFFFFF" fontName="4" fontStyle="3" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="FOLDER IN CODE1" fgColor="000000" bgColor="FFFFFF" fontName="&#x9BD0;&#xC312;&#x0273;" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="FOLDER IN CODE2" fgColor="000000" bgColor="FFFFFF" fontName="&#x6D80;&#xC305;&#x0273;" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="FOLDER IN COMMENT" fgColor="000000" bgColor="FFFFFF" fontName="Caret" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS1" fgColor="804000" bgColor="FFFFFF" fontName="no" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS2" fgColor="000000" bgColor="FFFFFF" fontName="&#x6FF0;&#xC305;&#x0273;" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS3" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS4" fgColor="000000" bgColor="FFFFFF" fontName="no" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS5" fgColor="000000" bgColor="FFFFFF" fontName="box" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS6" fgColor="000000" bgColor="FFFFFF" fontName="no" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS7" fgColor="000000" bgColor="FFFFFF" fontName="2" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('            <WordsStyle name="DELIMITERS8" fgColor="000000" bgColor="FFFFFF" fontName="" fontStyle="0" nesting="0" />\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('        </Styles>\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('    </UserLang>\n')
SYNTAX_OUTFILE_NOTEPADPLUS.write('</NotepadPlus>\n')
      
