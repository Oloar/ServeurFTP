Sorin Gaëtan - G1

### Introduction aux Systèmes et Réseaux
#TP n°8 : Implémentation d’un pseudo serveur FTP
--

*Fonctionnalités implémentées:*

- **Serveur FTP concurent**: Au lancement le serveur crée un certain nombre de processus fils. Ces processus ont pour rôle de prendre en charge les clients. Un processus fils et un client fonctionnent en pair. Une fois les requètes du clients traitées le processus fils peutse connecter à un autre client. Diverses ajout ont été implémentés. Lors d'un transfert le temps de ce dernier est mesuré et afficher, ainsi que le nombre d'octets transférés.

- Les commandes **get**, **put**, **ls** et **pwd** sont fonctionnelles.

- Les requêtes portant sur **plusieurs fichiers** sont acceptées. Elles sont décomposées par le client qui envoit une requête par fichier. (ex: la requête `get fich1 fich2 fich3` sera décomposée par le client en 3 requêtes, `get fich1`, `get fich2`, `get fich3`).

- Envoi des fichiers par block de 512 octets.


*Problèmes rencontrés:*

- Un manque de temps. (Causé notamment par le nombre de projets demandés, en fin de semestre, dans plusieurs UEs.)

