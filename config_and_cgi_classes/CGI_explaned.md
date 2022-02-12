cgi : Common Gateway Interface (littéralement "interface de passerelle commune"), est une interface utilisée par les serveurs HTTP.

Au lieu d'envoyer le contenu d'un fichier (fichier HTML, image, ...), le serveur HTTP exécute un programme, puis retourne le contenu généré. On passe a la CGI les informations via execve(path, args) puis on récupère l'output du CGI. On forme la réponse et on la send().

La propriété method indique la manière dont on transmet les données entrées par l'utilisateur au programme. 

Il existe deux méthodes de transmission des données différentes : method=POST et method=GET. Suivant la méthode choisie, il faudra écrire le programme CGI d'une manière adaptée.

La méthode GET transmet les données de manière visible en les ajoutant a la suite de l'URL conduisant au programme cgi, sous la forme suivante : "www.example.fr/cgi-bin/cgi.exe?name1=value1&ame2=value2&name3=value3...." . Cette méthode est peu pratique et également peu sécurisante dans la mesure ou toutes les données que vous entrez (même votre mot de passe par exemple) sont rendues visibles à tous dans l'URL. Il est donc déconseillé de l'utiliser.

La méthode POST est plus satisfaisante dans la mesure où elle transmet les données de manière invisible au programme. C'est celle qu'il est recommandé d'utiliser.

En plus des données entrées par l'utilisateur, le programme peut récupérer un certain nombre d'informations sur le contexte de travail. On peut en effet obtenir les valeurs des variables d'environnement suivantes :

Variable d'environnement 	Information fournie
SERVER_SOFTWARE 	Nom du logiciel serveur Web
SERVER_NAME 	Nom DNS de la machine hébergeant le serveur Web
GATEWAY_INTERFACE 	Version du protocole CGI utilisée par le serveur
SERVER_PROTOCOL 	Version du protocole HTTP utilisée par le serveur
SERVER_PORT 	Port utilisé par le serveur Web
REQUEST_METHOD 	Méthode (GET ou POST) selon laquelle le formulaire a été soumis
PATH_TRANSLATED 	Chemin complet menant au programme CGI lancé
SCRIPT_NAME 	Nom du programme CGI lancé
QUERY_STRING 	Si method=GET, contient la chaîne CGI, sinon vide
CONTENT_LENGTH 	Si method=POST, contient la longeur de la chaîne CGI, sinon 0
REMOTE_HOST 	Nom DNS de la machine distante qui a soumis le formulaire
REMOTE_ADDR 	Adresse IP de cette machine distante