oldPath=%PATH%
if ! python build.py $1 $2 $3 $4 $5 $6 $7 $8 $9 ; then
 exit 1
fi
PATH=%oldPath%