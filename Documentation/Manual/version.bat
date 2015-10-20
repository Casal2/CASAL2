"../../BuildSystem/bin/win32/release/iSAM.exe" -v > SAMversion.tex
svn_version --path ..\ --format tex --recursive --quiet --suffix Doc
python QuickReference.py

