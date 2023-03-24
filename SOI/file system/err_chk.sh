BIG_FILE="big_file.txt";
SMALL_FILE="small_file.txt";
DISC="some_disc.vd"

echo "==== Error checking ===="
./lab6 disc_info $DISC
./lab6 insert $DISC $SMALL_FILE root/duplicate_name.txt
./lab6 insert $DISC $SMALL_FILE root/veryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryveryverylongname.txt
./lab6 insert $DISC $SMALL_FILE root/duplicate_name.txt/hmmm.txt
./lab6 insert $DISC $SMALL_FILE root/thisfolderisnotreal/file.txt
./lab6 rm $DISC root/folder
./lab6 disc_info $DISC
./lab6 this_command_does_not_work
