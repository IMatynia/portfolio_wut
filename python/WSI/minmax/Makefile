WSI_ROZW_ZIP='./igor matynia wsi lab3.zip'

zip_solution:
	@ if [ -f $(WSI_ROZW_ZIP) ]; then rm $(WSI_ROZW_ZIP); echo 'Deleting old file'; fi;
	@ zip $(WSI_ROZW_ZIP) -r *.png './dokumentacja.ipynb' './constants.py' './game_board.py' './graphing_utils.py' './minmax.py' './multiprocess_experiment.py'
	@ echo 'done! uwu'

open_zip:
	@ xdg-open $(WSI_ROZW_ZIP)