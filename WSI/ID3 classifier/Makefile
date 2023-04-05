WSI_ROZW_ZIP='./igor matynia wsi lab4.zip'

zip_solution:
	@ if [ -f $(WSI_ROZW_ZIP) ]; then rm $(WSI_ROZW_ZIP); echo 'Deleting old file'; fi;
	@ zip $(WSI_ROZW_ZIP) -r './data set' './wykresy' data_handling.py dokumentacja.ipynb id3classifier.py plot_utils.py
	@ echo 'done! uwu'

open_zip:
	@ xdg-open $(WSI_ROZW_ZIP)