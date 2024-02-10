# Magnetometr MMC56x3
- Specyfikacja

    https://cdn-learn.adafruit.com/assets/assets/000/113/957/original/MMC5603NJ_RevB_7-12-18.pdf?1659554945

- Opis API i2c w RIOT OS

    https://doc.riot-os.org/group__drivers__periph__i2c.html

# Kalibracja
na podstawie https://thepoorengineer.com/en/calibrating-the-magnetometer/
- na płytce uruchamiamy polecenie `cal`
- odłączamy terminal jeśli jakiś był podłączony
- uruchamiamy `python plot.py`
    - punkty powinny pokryć równo elipsoidę
    - ctrl-c przerywa i dodaje wyniki do `data.csv`
- resetujemy płytkę czarnym przyciskiem
- uruchamiamy `python calibrateMag.py`
- przepisujemy wartości do `HARD_IRON (b)` i `SOFT_IRON (A_inv)`