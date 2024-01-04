#!bash
errors=0
files=0
mkdir -p out
for json in *.json
do
 ((files++))
 ./jsonparser-single $json > out/$json
 python3 -m json.tool out/$json >/dev/null 2>failed.result
 if [ -s failed.result ]; then
  ((errors++))
  printf "\n$json\n"
  charnum=${#json}
  for ((i=1; i<=$charnum; i++)); do printf '-'; done;echo
  line=$( cat failed.result|sed -e 's/.*line \(.*\) column.*/\1/' )
  column=$( cat failed.result|sed -e 's/.*column \(.*\) (.*/\1/' )
  printf "line $line, column $column ( "
  cat failed.result|tr -d '\n\r'
  echo " )"
  linenum=$((line - 3))
  if [ "$linenum" -lt "1" ]; then
    linenum=1
  fi
  for (( ; linenum < (($line + 3)); linenum++ ))
  do
    printf "%05d: " ${linenum}
    command="sed -n ${linenum}p out/$json"
    ${command}
    if [[ "$linenum" == "$line" ]]; then
      for ((i=0; i<=(($column+5)); i++)); do printf '-'; done;echo "^"
    fi
  done
 fi
 rm failed.result
done
if [ "$errors" -eq "0" ]; then
   echo "everything went well. $files json file processed.";
else
   printf "\nproblems found in $errors/$files json files.\n";
fi
