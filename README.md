# RecupNRJ

Working repository for Energy Harvesting project with Vincent Boitier, LAAS-CNRS

--- 

### Etat actuel du projet :

Ebauche de code C disponible dans le dossier `code`. Code périphériques testé fonctionnel (WD, sleep, GPIO & ADC).

Proposition de schéma électrique (incomplet) : `schematic.pdf`

Chronogramme réel dans `code/logic`, visible avec Logic 1.2.18


### Microcontrolleur et programmation

Microcontrolleur : Attiny25 (2K Flash, 8 Pin package (DIP, SOIC), ADC 10 bit, ***Conso 2µA@5V en mode sleep, 1.2mA run mode***, 1$ en qté individuelle)

Programmation via une sonde type Usbasp/Usbtiny. Compilation avec `avr-gcc`, upload avec `avrdude`. Un script de programmation est fourni `code/makefile_t85.bat`.

Je peux fournir microcontrolleur, sonde de programmation et câble (le tout pour moins de 10$).

### Déroulement du code (interpétation de la spécification)

Wakeup périodique du microncontrolleur (max toutes les 8s, rallongement possible avec un compteur software).

Seuil de mesure de la tension des capas: réglable grâce au #define VCAP_THRESHOLD

Si mesure inférieure au seuil -> sleep (8s).

Allumage de l'ATIM

Délai ATIM_WAKEUP_DELAY

Allumage de l'ETAS

Démarrage compteur.

Attente de la fin de l'erreur ETAS

Quand fin erreur : lancement de la mesure (IN1)

Attente de la fin du compteur (22s)

Extinction de l'ETAS, ATIM et IN1.

Sleep(8s).


### Questions : 

Une alimentation pour le microcontrolleur est-elle disponible ? (Entre 2.5 et 5.5V) Sinon je peux prévoir un LDO 5V ou 3v3 avec low-iq(<1µA).  

Est-il possible de switcher les charges ATIM et ETAS en low side comme suggéré dans le schéma? Ou disposez vous de switch high side intégrés ?

Quel est le délai entre l'allumage ATIM et ETAS ?

Faut-il alimenter l'ETAS 22s au total, ou pendant un certain délai après le lancement de la mesure ?

Le power-down doit il respecter un séquencement précis ?

Le signal IN1 est-il bien de niveau logique ?

Quel est le temps haut minimum pour le signal IN1 ?

