all: compile run_all_tests

clean:
	rm lab6 & rm out.txt & rm ./*.vd

compile:
	cc main.c virtual_disc.h virtual_disc.c -o lab6

copy_over:
	cp ../lab6/main.c ./main.c
	cp ../lab6/virtual_disc.c ./virtual_disc.c
	cp ../lab6/virtual_disc.h ./virtual_disc.h

run_all_tests:
	sh ./init_disk.sh
	sh ./mk_file_str.sh
	sh ./err_chk.sh
	sh ./data_extr.sh 
