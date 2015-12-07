oldPath=%PATH%
python build.py $1 $2 $3 $4 $5 $6 $7 $8 $9 ;
PROJECT_SUCCESS=$?
if [ $PROJECT_SUCCESS -ne 0 ] ; then
 echo "Error Occurred"
 exit 1
fi
PATH=%oldPath%
exit