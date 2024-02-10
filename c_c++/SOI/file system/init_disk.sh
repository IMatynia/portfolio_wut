DISC="some_disc.vd";
SIZE=320000;

echo "==== INIT ===="
./lab6 disc_remove $DISC;
./lab6 create $DISC $SIZE;