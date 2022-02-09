mots clef ( in server{ } ) à implémenter dans le fichier de config :

> listen	        		(port d'écoute)
> host				        (interface réseau)
> server_name	        	(virtual host)
> root		        		(server global root)
> error_page				(custom error pages)
> client_max_body_size		(la taille maximale pour le corps des requêtes envoyées par le client)

in block location { } :
> index				(le fichier d'index à rechercher)
> autoindex			(activation ou non de l'autoindex)
> cgi_extension		(l'extension de fichier signalant la nécessité de transférer au CGI)
> cgi_bin			(le chemin de l'exécutable CGI, Ubuntu standard path: /usr/bin/cgi-bin)
> root				(redéfinit la racine de cette route indépendamment de la racine globale)

? :
[> methods					(les méthodes HTTP acceptées pour cette route)]
[> auth_basic				(activation ou non de l'authentification)]
[> auth_basic_user_file		(le fichier contenant les credentials si l'authentification est activée, format : username:password, 1 par ligne)]
[> language					(le langage préféré pour cette route) ]




liens stylés :

- http://nginx.org/en/docs/beginners_guide.html

- http://nginx.org/en/docs/http/request_processing.html

- http://nginx.org/en/docs/http/server_names.html

- https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts

- https://www.keycdn.com/support/nginx-directory-index#:~:text=By%20default%2C%20Nginx%20tries%20to,is%20if%20it%20has%20permissions