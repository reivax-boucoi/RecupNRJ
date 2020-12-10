# RecupNRJ

Working repository for Energy Harvesting project with Vincent Boitier, LAAS-CNRS

--- 

### Etat actuel du projet :

ADC, WD, GPIO, sleep fonctionnels. 

--- 

### Microcontrolleur et programmation

Microcontrolleur : Attiny25 (2K Flash, 8 Pin package (DIP, SOIC), ADC 10 bit, ***Conso 2µA@5V en mode sleep, 1.2mA run mode***, 1$ en qté individuelle)

Programmation via une sonde type Usbasp/Usbtiny. Compilation avec `avr-gcc`, upload avec `avrdude`. Un script de programmation est fourni `code/simple_makefile_t85.bat`.

Une sonde de programmation (Usbasp) avec adaptateur MiniProg ainsi que 2µC de test oont été fournis.

--- 
