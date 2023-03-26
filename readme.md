## Communication avec différents types d'appareils via UDP

Ce code permet de communiquer avec différents types d'appareils en utilisant le protocole UDP. Les appareils supportés sont :

Une LED connectée utilisant dev/light.h, accessible sur le port 3000, qui passe au vert quand elle est allumée et au rouge quand elle est éteinte. Elle prend en paramètre les valeurs ON ou OFF et change l'état de la LED. Elle renvoie une réponse JSON.
Une radio utilisant dev/radio-sensor.h, accessible sur le port 3000, qui renvoie la valeur courante sous format JSON.
Un bouton connecté utilisant dev/button-sensor.h, accessible sur le port 3000, qui envoie un message "bouton cliqué" au clic, sous format JSON.
Un capteur de température utilisant dev/sht11.h, accessible sur le port 3000, qui envoie la valeur actuelle du capteur sous format JSON.

## Explication 

Ce code met en place un serveur UDP qui écoute sur le port 3000. Lorsqu'un message est reçu, il est parsé en JSON et traité en fonction du type de l'appareil. Les différentes actions sont :

Si le type est "led", le code modifie l'état de la LED en fonction de la valeur passée en paramètre ("ON" ou "OFF") et renvoie une réponse JSON indiquant l'état actuel de la LED.
Si le type est "radio", le code lit la valeur actuelle du capteur radio et renvoie une réponse JSON avec cette valeur.

Si le type est "button", le code renvoie simplement un message JSON indiquant que le bouton a été cliqué.

Si le type est "temperature", le code lit la valeur actuelle du capteur de température et renvoie une réponse JSON avec cette valeur.

Le serveur envoie ensuite une réponse JSON au client qui a envoyé le message initial.

Pour tester ce code, vous pouvez utiliser un simulateur comme Cooja pour simuler différents nœuds communicant avec le serveur. Vous pouvez envoyer des messages JSON en utilisant une bibliothèque comme "libjson-c" ou en utilisant un client UDP simple en ligne de commande.







## Licence

Ce code est sous licence MIT.

## Makefile

TARGET = udp-server

CONTIKI_PROJECT = $(TARGET)

all: $(CONTIKI_PROJECT)

Define the platform

PLATFORMS_ONLY = cc26x0-cc13x0
BOARDS_ONLY = sensortag/cc2650



## Include the main contiki Makefile

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include
