BIG_FILE="big_file.txt";
SMALL_FILE="small_file.txt";
DISC="some_disc.vd"

echo "==== Data insertion ===="
./lab6 insert $DISC $SMALL_FILE root/duplicate_name.txt
./lab6 mkdir $DISC folder
./lab6 ls $DISC root
./lab6 insert $DISC $SMALL_FILE root/folder/delete_this_later
./lab6 insert $DISC $SMALL_FILE root/folder/file_smo2.txt
./lab6 ls $DISC root/folder
./lab6 rm $DISC root/folder/delete_this_later
./lab6 disc_info $DISC
./lab6 insert $DISC $BIG_FILE root/this_one_is_split.txt
./lab6 disc_info $DISC
./lab6 insert $DISC $BIG_FILE root/folder/duplicate_name.txt
./lab6 ls $DISC root
./lab6 ls $DISC root/folder
./lab6 disc_info $DISC
