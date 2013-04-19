oldPath=%PATH%
python build.py $1 $2 $3 $4 $5 $6 $7 $8 $9 ;
ISAM_SUCCESS=$?
if [ $ISAM_SUCCESS -ne 0 ] ; then
 exit 1
fi
PATH=%oldPath%