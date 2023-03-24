BIG_FILE="big_file.txt";
SMALL_FILE="small_file.txt";
DISC="some_disc.vd"

echo "==== Data extraction ===="
./lab6 extract $DISC root/thisfiledoesnotexist out.txt
./lab6 extract $DISC root/folder out.txt
./lab6 extract $DISC root/duplicate_name.txt out.txt
echo "DIFF: [$(diff out.txt $SMALL_FILE)]"
./lab6 extract $DISC root/this_one_is_split.txt out.txt
echo "DIFF: [$(diff out.txt $BIG_FILE)]"
./lab6 extract $DISC root/folder/duplicate_name.txt out.txt
echo "DIFF: [$(diff out.txt $BIG_FILE)]"
