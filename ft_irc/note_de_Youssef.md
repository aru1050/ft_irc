# Network Programming

## Chapitre 1 : Les Socket
### A) C'est quoi une Socket

- Une manière de parler à d'autres programmes en utilisant des descripteurs de fichiers Unix standards.

#### Tout dans Unix est un fichier !

- Lorsque les programmes Unix effectuent n'importe quel type d'E/S (Entrées/Sorties), ils le font en lisant ou en écrivant dans un descripteur de fichier.

- Une opération d'entrée-sortie (E/S ou I/O) sous Unix désigne la façon dont un programme lit des données (entrée) et écrit des résultats ou des messages (sortie).

- Le système traite tous ces flux de données et périphériques (clavier, écran, fichiers) de la même manière.

- Un descripteur de fichier est simplement un entier associé à un fichier ouvert.

- Ce fichier peut être une connexion réseau, une FIFO, un pipe, un terminal, un vrai fichier sur disque, ou à peu près n'importe   quoi d'autre.

- Conclusion : Tout dans Unix est un fichier !
    - quand vous voulez communiquer avec un autre programme sur Internet, vous allez le faire à travers un descripteur de fichier

#### Questions 
- Où est-ce que j'obtiens ce descripteur de fichier pour la communication réseau ?
    - vous faites appel à la routine système socket(). 
    - Elle renvoie le descripteur de socket
    - et vous communiquez à travers lui en utilisant les appels de socket spécialisés send() et recv()

- Si c'est un descripteur de fichier, pourquoi ne puis-je pas simplement utiliser les appels normaux read() et write() pour communiquer à travers la socket ?
    - La réponse courte est : « Vous pouvez ! » 
    - La réponse longue est : « Vous pouvez, mais send() et recv() offrent un contrôle beaucoup plus grand sur votre transmission de données.

### B) Deux Types de Sockets Internet (il y’en a plus mais ma3lich)
- L'un est « Stream Sockets » (Sockets de flux)  SOCK_STREAM
- l'autre est « Datagram Sockets » (Sockets de datagrammes), SOCK_DGRAM
#### Socket de flux
- Des flux de communication connectés, bidirectionnels et fiables
- Si vous injectez deux éléments dans la socket dans l'ordre « 1, 2 », ils arriveront dans l'ordre « 1, 2 »
- Les navigateurs web utilisent aussi le protocole de transfert hypertexte (HTTP) qui utilise des sockets de flux pour obtenir des pages
- Comment les sockets de flux atteignent-elles ce haut niveau de qualité de transmission de données ?
    - Elles utilisent un protocole appelé « The Transmission Control Protocol », autrement connu sous le nom de « TCP »

#### Socket de datagrammes
- Utilisent aussi IP pour le routage, mais elles n'utilisent pas TCP ; elles utilisent le « User Datagram Protocol », ou « UDP »
- Pourquoi sont-elles sans connexion ?
    - vous n'avez pas besoin de maintenir une connexion ouverte comme vous le faites avec les sockets de flux. 
    - 1) Vous construisez simplement un paquet
    - 2) vous collez un en-tête IP dessus avec les informations de destination
    - 3) et vous l'envoyez => Aucune connexion nécessaire.

- Pourquoi utiliseriez-vous un protocole sous-jacent non fiable ?
    - la vitesse
    - Si vous envoyez des messages de discussion (chat), TCP est excellent
    - Si vous envoyez 40 mises à jour de position par seconde des joueurs dans le monde, peut-être que cela n'a pas tant d'importance si une ou deux sont perdues, et UDP est un bon choix.

## Chapitre 2 : Introduction
### A) Adresses IP
#### The Internet Protocol Version 4 (IPv4)
- adresses composées de 4 octets = 32 bits (192.0.2.111)
- potentiellement à court d adresse
#### The Internet Protocol Version 6 (IPv6)
- adresses composées de 16 octets = 128 bits (2001:0db8:c9d2:aee5:73e3:934a:a5ae:9551)
- un mode de compatibilité IPv4 pour les adresses IPv6 192.0.2.33 = ::ffff:192.0.2.33

### B) Sous-réseaux (Subnets)
- Pour des raisons organisationnelles, il est parfois pratique de déclarer : « cette première partie de cette adresse IP jusqu'à ce bit précis est la partie réseau de l'adresse IP, et le reste est la partie hôte (host) ».
- Dans les Temps Anciens, il existait des « classes » de sous-réseaux, où le premier, les deux ou les trois premiers octets de l'adresse formaient la partie réseau.
    - Si vous aviez la chance d'avoir un octet pour le réseau et trois pour les hôtes, vous pouviez disposer de 24 bits d'hôtes sur votre réseau (environ 16 millions). C'était un réseau de « Classe A »
    - « Classe C », avec trois octets de réseau et un octet d'hôte (256 hôtes, moins quelques-uns qui étaient réservés)
- La partie réseau de l'adresse IP est décrite par ce qu'on appelle le masque de sous-réseau (netmask) (voir cahier chapitre netpractice)
- il s'est avéré que ce n'était pas assez granulaire pour les besoins futurs d'Internet ; nous manquons très vite de réseaux de Classe C, et nous n'avions absolument plus de Classe A

### C) Numéros de Port
- Il s'avère qu'en plus d'une adresse IP (utilisée par la couche IP), il existe une autre adresse utilisée par TCP (stream sockets) et, par coïncidence, par UDP (datagram sockets). C'est le numéro de port. C'est un nombre de 16 bits qui agit comme l'adresse locale pour la connexion.
- Voyez l'adresse IP comme l'adresse postale d'un hôtel, et le numéro de port comme le numéro de chambre.
- Vous vouliez avoir un ordinateur qui gère le courrier entrant ET les services web, comment différenciez-vous les deux sur une machine avec une seule adresse IP ?
    -  well-known ports
        - HTTP (le web) est le port 80
        - telnet est le port 23
        - SMTP est le port 25
        - DOOM utilisait le port 666
    - Les ports inférieurs à 1024 sont souvent considérés comme spéciaux et nécessitent généralement des privilèges administratifs (root) pour être utilisés.
 
### D) Ordre des octets (Byte Order / Endianness)
### E) Structs

####  struct addrinfo
```C 
    struct addrinfo {
    int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    int              ai_protocol;  // utiliser 0 pour "n'importe lequel"
    size_t           ai_addrlen;   // taille de ai_addr en octets
    struct sockaddr *ai_addr;      // struct sockaddr_in ou _in6
    char            *ai_canonname; // nom d'hôte canonique complet
    struct addrinfo *ai_next;      // liste chaînée, nœud suivant 
}; 
```
- utilisée pour préparer les structures d'adresses de socket pour un usage ultérieur.
- utilisée lors des résolutions de noms d'hôtes (DNS) et de noms de services
- Vous remplirez un peu cette structure, puis vous appellerez getaddrinfo(). Elle renverra un pointeur vers une nouvelle liste chaînée de ces structures remplies avec toutes les informations dont vous avez besoin.
- Vous pouvez forcer l'utilisation d'IPv4 ou d'IPv6 dans le champ ai_family, ou le laisser à ```AF_UNSPEC```
- Notez qu'il s'agit d'une liste chaînée : ai_next pointe vers l'élément suivant
- il peut y avoir plusieurs résultats parmi lesquels choisir. J'utiliserais le premier résultat fonctionnel
- le champ ```ai_addr``` dans la struct addrinfo est un pointeur vers une ```struct sockaddr```.
    - C'est là que nous entrons dans les détails concrets de ce qui se trouve à l'intérieur d'une structure d'adresse IP.
- Vous n'aurez généralement pas besoin d'écrire manuellement dans ces structures
    - un appel à ```getaddrinfo()``` pour remplir votre struct addrinfo fera tout le travail.
####  struct sockaddr
```C
struct sockaddr {
    unsigned short sa_family; // famille d'adresses, AF_xxx
    char           sa_data[14]; // 14 octets d'adresse de protocole
};
```
- la ```struct sockaddr``` contient des informations d'adresse pour de nombreux types de sockets.
- ```sa_family ```peut être diverses choses mais ce sera :
    - ```AF_INET``` (IPv4) ou ```AF_INET6``` (IPv6) ici

####  struct sockaddr_in
- Pour contourner la struct sockaddr, les programmeurs ont créé une structure parallèle : struct sockaddr_in à utiliser avec IPv4
    - « in » pour « Internet »
```C
struct sockaddr_in {
    short int          sin_family;  // Famille d'adresses, AF_INET
    unsigned short int sin_port;    // Numéro de port (en Network Byte Order !)
    struct in_addr     sin_addr;    // Adresse Internet
    unsigned char      sin_zero[8]; // Même taille globale que struct sockaddr
};
```
- Voici le point crucial : un pointeur vers une ```struct sockaddr_in ```peut être casté en un pointeur vers une ```struct sockaddr``` et vice-versa.
    - Ainsi, même si une fonction comme ```connect()``` ou ```bind()``` attend un struct sockaddr*, vous pouvez toujours utiliser une struct sockaddr_in et la caster au dernier moment !
- Cette structure facilite la référence aux éléments de l'adresse de socket.
- ```sin_zero```  (inclus pour aligner la taille de la structure sur celle d'une ``` struct sockaddr``` ) doit être initialisé à des zéros avec la fonction ``` memset()```
- sin_family correspond à sa_family et doit être défini à AF_INET
- sin_port doit être en Network Byte Order (en utilisant htons() !)
- sin_addr est une struct in_addr
    - c'était l'une des unions les plus effrayantes de tous les temps
```C
// (IPv4 uniquement -- voir struct in6_addr pour IPv6)
// Adresse Internet (une structure pour des raisons historiques)
struct in_addr {
    uint32_t s_addr; // c'est un entier 32 bits (4 octets)
};
```
#### IPV6
```C
// (IPv6 uniquement)
struct sockaddr_in6 {
    u_int16_t       sin6_family;   // famille d'adresses, AF_INET6
    u_int16_t       sin6_port;     // port, en Network Byte Order
    u_int32_t       sin6_flowinfo; // informations de flux IPv6
    struct in6_addr sin6_addr;     // adresse IPv6
    u_int32_t       sin6_scope_id; // Scope ID
};

struct in6_addr {
    unsigned char s6_addr[16]; // adresse IPv6 (16 octets / 128 bits)
};
```
####  struct sockaddr_storage
- conçue pour être suffisamment grande pour contenir à la fois des structures IPv4 et IPv6.

- pour certains appels, vous ne savez pas à l'avance si le système va remplir votre structure avec une adresse IPv4 ou IPv6.
```C
struct sockaddr_storage {
    sa_family_t ss_family; // famille d'adresses

    // Tout ceci est du padding spécifique à l'implémentation, ignorez-le :
    char        __ss_pad1[_SS_PAD1SIZE];
    int64_t     __ss_align;
    char        __ss_pad2[_SS_PAD2SIZE];
};
```
- vous pouvez inspecter la famille d'adresses dans le champ ss_family pour vérifier s'il s'agit de AF_INET ou AF_INET6
- Ensuite, vous pouvez la caster en struct sockaddr_in ou struct sockaddr_in6 si vous le souhaitez.

### F) Adresses IP, Deuxième Partie (à completer plus tard)

## Chapitre 3 : Syscall
### getaddrinfo() — Paré au lancement !

- Elle permet de préparer les structures (struct) dont vous aurez besoin par la suite.
- vous disposez désormais de la fonction getaddrinfo() qui s'occupe de tout un tas de bonnes choses pour vous
    - les résolutions DNS
    - noms de services
    - les structures dont vous avez besoin
```C
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,     // ex: "www.example.com" ou adresse IP
                const char *service,  // ex: "http" ou numéro de port
                const struct addrinfo *hints,
                struct addrinfo **res);
```
- Vous donnez à cette fonction trois paramètres en entrée, et elle vous donne un pointeur vers une liste chaînée de résultats, res
    - Le paramètre node est le nom d'hôte auquel se connecter, ou une adresse IP.
    - le paramètre service, qui peut être un numéro de port, comme "80", ou le nom d'un service particulier
    - le paramètre hints pointe vers une struct addrinfo déjà remplie avec les informations pertinentes.
### socket() — Obtenez le descripteur de fichier !
```C
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
```
- Mais quels sont ces arguments ?
    - Ils vous permettent de spécifier quel genre de socket vous désirez (IPv4 ou IPv6, flux ou datagramme, et TCP ou UDP).
- Auparavant, les développeurs écrivaient ces valeurs en dur
    - domain est PF_INET ou PF_INET6
    - type est SOCK_STREAM ou SOCK_DGRAM
    - protocol peut être mis à 0 pour choisir le protocole approprié au type donné
- Ce que vous voulez réellement faire, c'est utiliser les valeurs issues des résultats de l'appel à getaddrinfo(), et les transmettre directement à socket() comme ceci :
```C
int s;
struct addrinfo hints, *res;

// effectue la recherche
// [faisons comme si nous avions déjà rempli la structure "hints"]
getaddrinfo("www.example.com", "http", &hints, &res);

// encore une fois, vous devriez vérifier les erreurs de getaddrinfo(), et parcourir
// la liste chaînée "res" en quête d'entrées valides au lieu de supposer que la
// première est bonne (comme le font nombre de ces exemples).
// Voir la section client/serveur pour de vrais exemples.

s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```
- socket() vous renvoie simplement un descripteur de socket que vous pourrez utiliser dans de futurs appels système, ou -1 en cas d'erreur.
- à quoi sert cette socket ?
    - elle-même, et vous devez poursuivre votre lecture et exécuter d'autres appels système pour qu'elle prenne tout son sens.
### bind() — Sur quel port suis-je ?
- vous devrez peut-être associer cette socket à un port sur votre machine locale.
    - (Cela se fait couramment si vous prévoyez d'écouter avec listen() les connexions entrantes sur un port spécifique — les jeux réseau multijoueurs font cela lorsqu'ils vous demandent de vous « connecter à 192.168.5.10 sur le port 3490 »)
- Le numéro de port est utilisé par le noyau pour faire correspondre un paquet entrant au descripteur de socket d'un processus particulier.
- Si vous prévoyez uniquement de faire un connect() (parce que vous êtes le client, pas le serveur), cela est probablement inutile.
```C
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
```
- sockfd est le descripteur de fichier de la socket renvoyé par socket()
- my_addr est un pointeur vers une struct sockaddr contenant des informations sur votre adresse
- addrlen est la longueur en octets de cette adresse
```C
getaddrinfo(NULL, "3490", &hints, &res);

// crée une socket :

sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

// lie la socket au port passé à getaddrinfo() :

bind(sockfd, res->ai_addr, res->ai_addrlen);
```
- bind() renvoie également -1 en cas d'erreur et affecte le code d'erreur à la variable errno
- n'allez pas trop bas avec vos numéros de port. Tous les ports inférieurs à 1024 sont RÉSERVÉS à sudo
- il y a des moments où vous ne serez absolument pas obligé de l'appeler
    - Si vous effectuez un connect() vers une machine distante 
    - et que vous ne vous souciez pas de votre numéro de port local
    - vous pouvez simplement appeler connect()
    - Il vérifiera si la socket n'est pas liée, et la liera (bind()) à un port local inutilisé si nécessaire.
### connect() — Hé, toi !
- Faisons semblant quelques minutes que vous êtes une application telnet
    - Votre utilisateur vous ordonne d'obtenir un descripteur de fichier de socket.
    - Ensuite, l'utilisateur vous ordonne de vous connecter à "10.12.110.57" sur le port "23"
    - Heureusement pour vous, vous consultez actuellement la section sur connect()
```C
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen);
```
- sockfd est notre bon vieux descripteur de fichier de socket
- serv_addr est une struct sockaddr contenant : 
    - le port de destination 
    - l'adresse IP
- addrlen est la longueur en octets de la structure d'adresse du serveur
- Toutes ces informations peuvent être chopée directement à partir des résultats de l'appel à getaddrinfo()
- Assurez-vous de vérifier la valeur de retour de connect() — elle renverra -1 en cas d'erreur
- Remarquez également que nous n'avons pas appelé bind()
    - Fondamentalement, nous ne nous soucions pas de notre numéro de port local
    - seul l'endroit où nous allons nous importe
    - Le noyau choisira un port local pour nous
    - et le site auquel nous nous connectons recevra automatiquement cette information de notre part
### listen() — Quelqu'un pourrait-il m'appeler, s'il vous plaît ?
- Supposons, que vous vouliez attendre des connexions entrantes et les traiter d'une manière ou d'une autre.
- Le processus se déroule en deux étapes : 
    - d'abord vous faites listen()
    - puis vous faites accept()
- L'appel listen() est assez simple, mais nécessite une petite explication :
```C
int listen(int sockfd, int backlog);
```
- sockfd est le descripteur de fichier de socket habituel issu de l'appel système socket()
- backlog est le nombre de connexions autorisées dans la file d'attente entrante
    - les connexions entrantes vont attendre dans cette file d'attente jusqu'à ce que vous les acceptiez avec accept()
    - La plupart des systèmes limitent silencieusement ce nombre à environ 20 ; vous pouvez probablement vous en sortir en le définissant à 5 ou 10.
- Encore une fois, comme d'habitude, listen() renvoie -1 et positionne errno en cas d'erreur.
- nous devons appeler bind() avant d'appeler listen() afin que le serveur tourne sur un port spécifique.
- Donc, si vous prévoyez d'écouter les connexions entrantes, la séquence d'appels système que vous effectuerez est :
```C
    1 getaddrinfo();
    2 socket();
    3 bind();
    4 listen();
    5 /* accept() vient ici */
```
### accept() — « Merci d'avoir appelé le port 3490. »
- l'appel accept() est un peu bizarre
- quelqu'un de très, très loin va essayer de se connecter avec connect() à votre machine sur un port que vous écoutez avec listen()
- Sa connexion sera mise en attente dans la file pour être acceptée avec accept()
- Vous appelez accept() et vous lui dites de récupérer la connexion en attente
- Il va vous renvoyer un tout nouveau descripteur de fichier de socket à utiliser pour cette unique connexion
- soudainement vous avez deux descripteurs de socket pour le prix d'un
    - L'original continue d'écouter pour de nouvelles connexions
    - et celui nouvellement créé est enfin prêt pour send() et recv()
```C
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- sockfd est le descripteur de socket en écoute (listen()).
- addr sera généralement un pointeur vers une struct sockaddr_storage locale
- addrlen est une variable entière locale qui doit être initialisée à sizeof(struct sockaddr_storage)
    - accept() n'écrira pas plus d'octets que cette limite dans addr
    - S'il en écrit moins, il modifiera la valeur de addrlen pour refléter cela.
- Devinez quoi ? accept() renvoie -1 et positionne errno si une erreur survient
- voici donc un extrait de code
```C
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define MYPORT "3490" // le port auquel les utilisateurs vont se connecter
#define BACKLOG 10    // taille maximale de la file d'attente des connexions

int main(void)
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, new_fd;

    // !! n'oubliez pas vos vérifications d'erreurs pour ces appels !!

    // d'abord, charger les structures d'adresses avec getaddrinfo():
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // utilise IPv4 ou IPv6, peu importe
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // remplit mon IP pour moi

    getaddrinfo(NULL, MYPORT, &hints, &res);

    // crée une socket, lie-la au port, et passe-la en écoute :
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);

    // maintenant, accepte une connexion entrante :
    addr_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);

    // prêt à communiquer sur le descripteur de socket new_fd !
    // ...
```
- notez que nous utiliserons le descripteur de socket new_fd pour tous les appels send() et recv()
- Si vous ne prévoyez de recevoir qu'une seule et unique connexion, vous pouvez fermer le socket d'écoute sockfd avec close() 
    - afin d'empêcher de nouvelles connexions entrantes sur le même port, si vous le désirez.
### send() et recv() — Parle-moi
- Ces deux fonctions servent à communiquer sur des sockets de flux (stream sockets) ou des sockets de datagrammes connectées.
- Si vous voulez utiliser des sockets de datagrammes classiques non connectées, vous devrez consulter la section sur sendto() et recvfrom()
- ce sont des appels bloquants.
    - C'est-à-dire que recv() va bloquer jusqu'à ce qu'il y ait des données prêtes à être reçues.
    - Cela signifie que votre programme va s'arrêter là, sur cet appel système, jusqu'à ce que quelqu'un vous envoie quelque chose 
    - Le jargon technique de l'OS pour "s'arrêter" dans cette phrase est en fait sleep
    - send() peut aussi bloquer si les données que vous envoyez sont complètement saturées d'une façon ou d'une autre
```C
int send(int sockfd, const void *msg, int len, int flags);
```
- sockfd est le descripteur de socket auquel vous voulez envoyer des données 
    - qu'il s'agisse de celui renvoyé par socket() ou de celui obtenu avec accept()
- msg est un pointeur vers les données à transmettre
- len est la longueur de ces données en octets
- Mettez simplement flags à 0 (Consultez le manuel de send() pour plus d'informations concernant les drapeaux).
```C
char *msg = "Beej was here!";
int len, bytes_sent;
// ...
len = strlen(msg);
bytes_sent = send(sockfd, msg, len, 0);
// ...
```
- send() renvoie le nombre d'octets réellement envoyés
    - cela peut être inférieur au nombre que vous lui avez demandé d'envoyer
- Voyez-vous, parfois vous lui demandez d'envoyer un gros paquet de données et il ne peut tout simplement pas tout gérer d'un coup
    - il expédiera autant de données qu'il le peut
- si la valeur renvoyée par send() ne correspond pas à la valeur de len, c'est à vous d'envoyer le reste de la chaîne.
- si le paquet est petit (moins de 1 Ko environ), il réussira probablement à tout envoyer en une seule fois
- Encore une fois, -1 est renvoyé en cas d'erreur, et errno prend la valeur de l'erreur.
- L'appel à recv() est similaire à bien des égards :
```C
int recv(int sockfd, void *buf, int len, int flags);
// ...
```
- sockfd est le descripteur de socket depuis lequel lire
- buf est le tampon dans lequel lire les informations
- len est la longueur maximale du tampon
- flags peut à nouveau être mis à 0
- recv() renvoie le nombre d'octets effectivement lus dans le tampon, ou -1 en cas d'erreur
- recv() peut renvoyer 0. Cela ne peut signifier qu'une seule chose :
    - le côté distant a fermé la connexion !
    - Une valeur de retour de 0 est la manière pour recv() de vous faire savoir que cela s'est produit.
### sendto() et recvfrom() — Parle-moi façon DATAGRAMME (à completer plus tard)
### close() et shutdown() — Dégage de ma vue !
- Vous êtes prêt à clore la connexion sur votre descripteur de socket
- C'est simple. Vous pouvez juste utiliser la fonction classique de descripteur de fichier Unix close()
    - close(sockfd); 
- Cela empêchera toute lecture ou écriture ultérieure sur la socket.
- Au cas où vous voudriez un peu plus de contrôle sur la manière dont la socket se ferme :
    - vous pouvez utiliser la fonction shutdown()
    - Elle vous permet de couper la communication dans une direction donnée
    - int shutdown(int sockfd, int how);
        - 0 : Les réceptions ultérieures sont interdites
        - 1 : Les transmissions ultérieures sont interdites
        - 2 : toutes deux interdites (comme close())
    - shutdown() renvoie 0 en cas de succès, et -1 en cas d'erreur (avec errno positionné en conséquence).
- shutdown() ne ferme pas réellement le descripteur de fichier: il modifie simplement son utilisabilité
- Pour libérer un descripteur de socket, vous devez impérativement utiliser close()

## Chapitre 4 : Contexte Client-Serveur
### introduction
- À peu près tout sur le réseau implique des processus clients qui communiquent avec des processus serveurs et inversement
- Prenez telnet, par exemple. Lorsque vous vous connectez à un hôte distant sur le port 23 avec telnet (le client), un programme sur cet hôte (appelé telnetd, le serveur) s'active
    - l gère la connexion telnet entrante, vous fournit une invite de connexion, etc.
```C
Client                         Serveur
        --- requête --->
        <-- réponse ---
send()                         recv()
recv()                         send()
               Le Réseau
```
- Notez que la paire client-serveur peut utiliser SOCK_STREAM, SOCK_DGRAM, ou n'importe quoi d'autre tant qu'ils emploient le même protocole
- De bons exemples de paires client-serveur sont :
    - telnet / telnetd
    - ftp / ftpd
    - Firefox / Apache
- Chaque fois que vous utilisez ftp, un programme distant, ftpd, est là pour vous servir.
- Souvent, il n'y aura qu'un seul serveur sur une machine 
    - et ce serveur gérera plusieurs clients en utilisant fork()
### Un Serveur de Flux Simple (Simple Stream Server)
- Tout ce que fait ce serveur est d'envoyer la chaîne "Hello, world!" à travers une connexion de flux.
- Pour tester ce serveur, il vous suffit de le lancer dans une fenêtre et de vous y connecter avec telnet depuis une autre avec :
```C
$ telnet remotehostname 3490
```
- où remotehostname est le nom de la machine sur laquelle vous l'exécutez.
```C
/*
** server.c -- démonstration d'un serveur de socket de flux
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490" // le port auquel les utilisateurs vont se connecter
#define BACKLOG 10  // nombre de connexions en attente que la file peut contenir

void sigchld_handler(int s)
{
    (void)s; // supprime l'avertissement de variable inutilisée

    // waitpid() peut écraser errno, donc nous le sauvegardons et le restaurons :
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

// extrait le sockaddr, IPv4 ou IPv6 :
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    // écoute sur sockfd, nouvelle connexion sur new_fd
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // informations d'adresse du connecteur
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // utilise mon IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // boucle à travers tous les résultats et se lie au premier possible
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // libère cette structure, nous en avons terminé avec elle

    if (p == NULL) {
        fprintf(stderr, "server: échec de liaison (bind)\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // élimine tous les processus morts (zombies)
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: en attente de connexions...\n");

    while(1) { // boucle principale d'accept()
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
            &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: connexion reçue de %s\n", s);

        if (!fork()) { // ceci est le processus enfant
            close(sockfd); // l'enfant n'a pas besoin de la socket d'écoute
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
                perror("send");
            close(new_fd);
            exit(0);
        }
        close(new_fd); // le parent n'a pas besoin de ce descripteur
    }

    return 0;
}
```
### Un Client de Flux Simple (Simple Stream Client)
- Ce client est encore plus simple que le serveur.
- out ce qu'il fait, c'est se connecter à l'hôte que vous indiquez en ligne de commande
```C
/*
** client.c -- démonstration d'un client de socket de flux
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "3490" // le port auquel le client va se connecter
#define MAXDATASIZE 100 // nombre maximal d'octets que nous pouvons recevoir en une fois

// extrait le sockaddr, IPv4 ou IPv6 :
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // boucle à travers tous les résultats et se connecte au premier possible
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        inet_ntop(p->ai_family,
            get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
        printf("client: tentative de connexion vers %s\n", s);

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("client: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: échec de connexion\n");
        return 2;
    }

    inet_ntop(p->ai_family,
        get_in_addr((struct sockaddr *)p->ai_addr),
        s, sizeof s);
    printf("client: connecté à %s\n", s);

    freeaddrinfo(servinfo); // libère cette structure, tout est terminé avec elle

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: reçu '%s'\n", buf);

    close(sockfd);

    return 0;
}
```
### Sockets de Datagrammes (Datagram Sockets) (puls tard)
## Chapitre 5 : Techniques Légèrement Avancées
- Ces techniques ne sont pas réellement avancées, mais elles sortent des niveaux plus élémentaires que nous avons déjà abordés
### Le Blocage (Blocking)
- Le blocage. Vous en avez entendu parler (sleep)
- Beaucoup de fonctions bloquent.
    - accept() bloque
    - Toutes les fonctions recv() bloquent.
- La raison pour laquelle elles peuvent faire cela est qu'elles en ont l'autorisation
- Lorsque vous créez pour la première fois le descripteur de socket avec socket() le noyau le configure en mode bloquant
    - Si vous ne voulez pas qu'une socket soit bloquante, vous devez faire un appel à fcntl()
```C
#include <unistd.h>
#include <fcntl.h>
// ...
sockfd = socket(PF_INET, SOCK_STREAM, 0);
fcntl(sockfd, F_SETFL, O_NONBLOCK);
// ...
```
- En réglant une socket en non-bloquant, vous pouvez effectivement la « sonder » (poll) pour obtenir des informations.
- Si vous tentez de lire sur une socket non-bloquante et qu'aucune donnée n'est présente, elle n'est pas autorisée à bloquer — elle renverra -1 et errno sera positionné à EAGAIN ou EWOULDBLOCK
- cependant, ce type de scrutation active (busy-waiting) est une mauvaise idée.
    - Si vous placez votre programme dans une boucle d'attente active scrutant les données sur la socket, vous allez dévorer le temps processeur (CPU) comme si c'était la fin du monde.
- Une solution bien plus élégante pour vérifier s'il y a des données en attente de lecture arrive dans la section suivante sur poll().
### poll() — Multiplexage d'E/S Synchrone (Synchronous I/O Multiplexing)
- Ce que vous voulez réellement pouvoir faire, c'est surveiller d'une manière ou d'une autre un groupe entier de sockets en même temps puis traiter uniquement celles qui ont des données prêtes
- De cette façon, vous n'avez pas besoin de scruter continuellement toutes ces sockets pour déterminer lesquelles sont prêtes pour la lecture.
- poll() est horriblement lent lorsqu'il s'agit de gérer un nombre gigantesque de connexions.
    - vous obtiendrez de bien meilleures performances grâce à une bibliothèque événementielle telle que libevent
- Alors, comment éviter la scrutation active (polling) ?
    - vous pouvez éviter le polling en utilisant l'appel système poll()
    - En résumé, nous allons demander au système d'exploitation de faire tout le sale boulot pour nous, et de nous avertir simplement lorsque des données sont prêtes à être lues sur telle ou telle socket
    - Pendant ce temps, notre processus peut s'endormir, économisant ainsi les ressources du système.
- Le plan général consiste à conserver un tableau de structures struct pollfd contenant les informations sur les descripteurs de sockets que nous souhaitons surveiller, et le type d'événements que nous voulons guetter.
- L'OS bloquera sur l'appel poll() jusqu'à ce que l'un de ces événements survienne
    - par exemple : « socket prête à être lue ! »
- ou jusqu'à ce qu'un délai d'attente (timeout) défini par l'utilisateur arrive à expiration.
- Détail très pratique : une socket en écoute (listen()) signalera qu'elle est « prête à lire » lorsqu'une nouvelle connexion entrante est prête à être acceptée avec accept().
```C
#include <poll.h>

int poll(struct pollfd fds[], nfds_t nfds, int timeout);
```
- fds est notre tableau d'informations (quelles sockets surveiller et pour quel motif)
- nfds est le nombre d'éléments dans le tableau
- timeout est un délai d'attente exprimé en millisecondes
- La fonction renvoie le nombre d'éléments du tableau pour lesquels un événement s'est produit.
```C
struct pollfd {
    int   fd;      // le descripteur de socket
    short events;  // masque binaire des événements qui nous intéressent
    short revents; // au retour de l'appel, masque binaire des événements survenus
};
```
- Nous allons donc disposer d'un tableau de ces structures, et nous initialiserons le champ fd de chaque élément avec le descripteur de socket que nous souhaitons surveiller.
- Ensuite, nous configurerons le champ events pour indiquer le type d'événements qui nous intéresse.
- Le champ events est la combinaison par OU logique binaire (|) des éléments suivants :
    - POLLIN : Alerte-moi quand des données sont prêtes à être lues avec recv() sur cette socket.
    - POLLOUT : Alerte-moi quand je peux envoyer des données avec send() sur cette socket sans bloquer.
    - POLLHUP : Alerte-moi quand l'extrémité distante a fermé la connexion.
- Une fois que votre tableau de struct pollfd est prêt, vous pouvez le passer à poll(), en précisant également la taille du tableau ainsi qu'une valeur de timeout en millisecondes.
    - Vous pouvez spécifier une valeur de timeout négative pour attendre indéfiniment
- Après le retour de poll(), vous pouvez inspecter le champ revents pour vérifier si POLLIN ou POLLOUT est activé, indiquant que cet événement précis s'est produit.
```C
#include <stdio.h>
#include <poll.h>

int main(void)
{
    struct pollfd pfds[1]; // Davantage si vous voulez surveiller plus de choses

    pfds[0].fd = 0;          // Entrée standard (stdin)
    pfds[0].events = POLLIN; // Avertis-moi quand c'est prêt à lire

    // Si vous deviez surveiller d'autres choses également :
    //pfds[1].fd = some_socket; // Un descripteur de socket
    //pfds[1].events = POLLIN;  // Avertis-moi quand c'est prêt à lire

    printf("Appuyez sur ENTRÉE ou attendez 2.5 secondes le timeout\n");

    int num_events = poll(pfds, 1, 2500); // timeout de 2.5 secondes

    if (num_events == 0) {
        printf("Le poll a expiré (timeout) !\n");
    } else {
        int pollin_happened = pfds[0].revents & POLLIN;

        if (pollin_happened) {
            printf("Le descripteur de fichier %d est prêt pour la lecture\n",
                pfds[0].fd);
        } else {
            printf("Événement inattendu survenu : %d\n",
                pfds[0].revents);
        }
    }

    return 0;
}
```
- Remarquez à nouveau que poll() renvoie le nombre d'éléments dans le tableau pfds pour lesquels des événements sont survenus
- l ne vous dit pas quels éléments dans le tableau mais il vous indique combien d'entrées ont un champ revents différent de zéro
- comment ajouter de nouveaux descripteurs de fichiers à l'ensemble passé à poll() ?
    - Pour cela, assurez-vous simplement d'avoir suffisamment de place allouée dans le tableau pour tous vos besoins, ou utilisez realloc() pour étendre l'espace au besoin.
- Qu'en est-il de la suppression d'éléments de l'ensemble ?
    - Pour cela, vous pouvez copier le dernier élément du tableau par-dessus celui que vous supprimez.
    - Et ensuite, passer une valeur décrémentée de 1 pour le décompte transmis à poll()
    - Une autre possibilité est de régler n'importe quel champ fd sur un nombre négatif, et poll() l'ignorera purement et simplement.
- Comment rassembler tout cela dans un serveur de chat auquel on peut se connecter avec telnet ?
    - démarrer une socket d'écoute (listener)
    - l'ajouter à l'ensemble des descripteurs de fichiers surveillés par poll()
    - Ensuite, nous ajouterons les nouvelles connexions à notre tableau de struct pollfd
    - nous l'agrandirons dynamiquement si nous venons à manquer d'espace. 
    - Lorsqu'une connexion sera fermée, nous la supprimerons du tableau.
    - lorsqu'une connexion sera prête pour la lecture, nous lirons les données qu'elle contient et nous les réexpédierons à toutes les autres connexions afin qu'elles puissent voir ce que les autres utilisateurs ont tapé.
```C
/*
** pollserver.c -- un serveur de chat rudimentaire pour plusieurs personnes
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>

#define PORT "9034" // Port d'écoute

/*
 * Convertit une socket en chaîne d'adresse IP.
 * addr: struct sockaddr_in ou struct sockaddr_in6
 */
const char *inet_ntop2(void *addr, char *buf, size_t size)
{
    struct sockaddr_storage *sas = addr;
    struct sockaddr_in *sa4;
    struct sockaddr_in6 *sa6;
    void *src;

    switch (sas->ss_family) {
        case AF_INET:
            sa4 = addr;
            src = &(sa4->sin_addr);
            break;
        case AF_INET6:
            sa6 = addr;
            src = &(sa6->sin6_addr);
            break;
        default:
            return NULL;
    }

    return inet_ntop(sas->ss_family, src, buf, size);
}

/*
 * Renvoie une socket en écoute.
 */
int get_listener_socket(void)
{
    int listener; // Descripteur de la socket d'écoute
    int yes = 1;  // Pour setsockopt() SO_REUSEADDR ci-dessous
    int rv;

    struct addrinfo hints, *ai, *p;

    // Récupère une socket et lie-la
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "pollserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        // Supprime l'erreur agaçante "Address already in use"
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    // Si on arrive ici, c'est qu'on n'a pas pu lier la socket
    if (p == NULL) {
        return -1;
    }

    freeaddrinfo(ai); // Tout est terminé avec ceci

    // Écoute
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener;
}

/*
 * Ajoute un nouveau descripteur de fichier à l'ensemble.
 */
void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size)
{
    // Si on manque de place, agrandir le tableau pfds
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Doubler la taille
        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Vérifier si prêt à lire
    (*pfds)[*fd_count].revents = 0;

    (*fd_count)++;
}

/*
 * Supprime un descripteur de fichier de l'ensemble à un index donné.
 */
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copie celui de la fin par-dessus celui-ci
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}

/*
 * Traite les nouvelles connexions entrantes.
 */
void handle_new_connection(int listener, int *fd_count, int *fd_size, struct pollfd **pfds)
{
    struct sockaddr_storage remoteaddr; // Adresse du client
    socklen_t addrlen;
    int newfd; // Nouveau descripteur de socket issu de accept()
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof remoteaddr;
    newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        add_to_pfds(pfds, newfd, fd_count, fd_size);

        printf("pollserver: nouvelle connexion de %s sur le socket %d\n",
            inet_ntop2(&remoteaddr, remoteIP, sizeof remoteIP),
            newfd);
    }
}

/*
 * Traite les données courantes d'un client ou sa déconnexion.
 */
void handle_client_data(int listener, int *fd_count, struct pollfd *pfds, int *pfd_i)
{
    char buf[256]; // Tampon pour les données du client

    int nbytes = recv(pfds[*pfd_i].fd, buf, sizeof buf, 0);
    int sender_fd = pfds[*pfd_i].fd;

    if (nbytes <= 0) { // Erreur reçue ou connexion fermée par le client
        if (nbytes == 0) {
            // Connexion fermée
            printf("pollserver: le socket %d a raccroché\n", sender_fd);
        } else {
            perror("recv");
        }

        close(pfds[*pfd_i].fd); // Au revoir !

        del_from_pfds(pfds, *pfd_i, fd_count);

        // Réexaminer l'emplacement qu'on vient d'écraser
        (*pfd_i)--;

    } else { // Des données valides ont été reçues du client
        printf("pollserver: recv du fd %d: %.*s", sender_fd, nbytes, buf);

        // Envoyer à tout le monde !
        for(int j = 0; j < *fd_count; j++) {
            int dest_fd = pfds[j].fd;

            // Sauf la socket d'écoute et nous-mêmes
            if (dest_fd != listener && dest_fd != sender_fd) {
                if (send(dest_fd, buf, nbytes, 0) == -1) {
                    perror("send");
                }
            }
        }
    }
}

/*
 * Traite l'ensemble des connexions existantes.
 */
void process_connections(int listener, int *fd_count, int *fd_size, struct pollfd **pfds)
{
    for(int i = 0; i < *fd_count; i++) {

        // Vérifier si quelqu'un est prêt à lire
        if ((*pfds)[i].revents & (POLLIN | POLLHUP)) {
            // Nous en tenons un !

            if ((*pfds)[i].fd == listener) {
                // Si nous sommes le listener, c'est une nouvelle connexion
                handle_new_connection(listener, fd_count, fd_size, pfds);
            } else {
                // Sinon, c'est un client classique
                handle_client_data(listener, fd_count, *pfds, &i);
            }
        }
    }
}

/*
 * Main : instancie un listener et un ensemble de connexions,
 * boucle indéfiniment en traitant les connexions.
 */
int main(void)
{
    int listener; // Descripteur de la socket d'écoute

    // Commence avec de l'espace pour 5 connexions
    // (Nous ferons un realloc au fur et à mesure)
    int fd_size = 5;
    int fd_count = 0;
    struct pollfd *pfds = malloc(sizeof *pfds * fd_size);

    // Initialise et obtient une socket en écoute
    listener = get_listener_socket();

    if (listener == -1) {
        fprintf(stderr, "erreur lors de l'obtention de la socket d'écoute\n");
        exit(1);
    }

    // Ajoute le listener à l'ensemble ;
    // Notifier "prêt à lire" lors d'une connexion entrante
    pfds[0].fd = listener;
    pfds[0].events = POLLIN;

    fd_count = 1; // Pour le listener

    puts("pollserver: en attente de connexions...");

    // Boucle principale
    for(;;) {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        // Parcourt les connexions en quête de données à lire
        process_connections(listener, &fd_count, &fd_size, &pfds);
    }

    free(pfds);
}
```
### Gérer les send() partiels (Handling Partial send()s)
- Vous souvenez-vous de la section consacrée à send(), plus haut, lorsque j'ai mentionné que send() pourrait ne pas transmettre tous les octets demandés ?
    - C'est-à-dire que vous souhaitez lui faire envoyer 512 octets, mais il renvoie 412. Qu'est-il advenu des 100 octets restants ?
    - Eh bien, ils sont toujours présents dans votre tampon en attente d'être expédiés.
    - le noyau a décidé de ne pas émettre toutes les données en un seul bloc,
    - il vous appartient d'acheminer ces données jusqu'à destination.
- Vous pourriez d'ailleurs écrire une fonction comme celle-ci pour y parvenir :
```C
#include <sys/types.h>
#include <sys/socket.h>

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // combien d'octets nous avons envoyés
    int bytesleft = *len; // combien il nous reste à envoyer
    int n;

    while(total < *len) {
        n = send(s, buf + total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // renvoie le nombre réellement envoyé ici

    return n == -1 ? -1 : 0; // renvoie -1 en cas d'échec, 0 en cas de succès
}
```
- s est la socket à laquelle vous désirez expédier les données
- buf est le tampon contenant les données
- len est un pointeur vers un entier contenant le nombre d'octets présents dans le tampon.
- La fonction renvoie -1 en cas d'erreur (et errno reste positionné par l'appel à send())
- De plus, le nombre d'octets réellement transmis est retourné dans len
- Il correspondra au même nombre d'octets que vous aviez demandé d'envoyer, à moins qu'une erreur ne soit survenue.
```C
char buf[10] = "Beej!";
int len;

len = strlen(buf);
if (sendall(s, buf, &len) == -1) {
    perror("sendall");
    printf("Nous n'avons envoyé que %d octets en raison de l'erreur !\n", len);
}
```
- Que se passe-t-il à l'autre bout, du côté du destinataire, lorsqu'une portion de paquet arrive ?
- Si les paquets sont de longueur variable, comment le récepteur sait-il où se termine un paquet et où commence le suivant ?
- Oui, les cas d'usage du monde réel sont une véritable plaie
- Vous devez probablement encapsuler vos données (vous vous rappelez de la section sur l'encapsulation tout au début ?). Poursuivez votre lecture pour découvrir les détails !
<!-- - 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
- 
-  -->