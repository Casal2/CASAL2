iSAM -v > SAMversion.tex
svn_version --path ..\ --format tex --recursive --quiet --suffix Doc
"../../isam" -v > iSAMversion.tex
rem python QuickReference.py
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\pdflatex.exe" SAM
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\bibtex.exe" SAM
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\makeindex.exe" SAM
rem python QuickReference.py
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\pdflatex.exe" SAM
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\bibtex.exe" SAM
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\makeindex.exe" SAM
rem python QuickReference.py
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\pdflatex.exe" SAM
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\bibtex.exe" SAM
"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\makeindex.exe" SAM

