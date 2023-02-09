#include <unistd.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>

//une liste chainee de clients avec un fd, un id
typedef struct s_client
{
	int				fd;
	int				id;
	struct s_client	*next;
}	t_client;

t_client	*g_clients = NULL;//<-

int		sock_fd, g_id = 0//<-
//cpy_* sont des listes de fd qui verif qu'on peut les utiliser.
//curr_sock est la liste des sockets utilisées
fd_set	curr_sock, cpy_read, cpy_write;//<-
char	msg[42];//<-
//str est ce que le serv recoit
str[42 * 4096], tmp[42 * 4096], buf[42 * 4096 + 42];//<-

//une fct pour aff une fatal error et sortir
void	fatal_error(void){
	write(2, "Fatal error\n", 12);
	close(sock_fd);//attention au leak de fd
	exit(1);
}

//une fct pour recup l'id d'un client en fonction du fd
int	get_id(int fd){
	t_client	*t_cli = g_clients;

	while (t_cli){
		if (t_cli->fd == fd)
			return (t_cli->id);
		t_cli = t_cli->next;
	}
	return (-1);//<-
}

//une fct pour recup le plus grand fd co
int	get_max_fd(void){
	int			max = sock_fd;//<-
	t_client	*t_cli = g_clients;

	while (t_cli){
		if (t_cli->fd > max)
			max = t_cli->fd;
		t_cli = t_cli->next;
	}
	return (max);
}

//une fct pour ajouter un client à la liste
int add_client(int fd){
	t_client	*t_cli = g_clients;
	t_client	*new;

	if(!(new = calloc(1, sizeof(t_client))))//<-
		fatal_error();
	new->id = g_id++;
	new->fd = fd;
	new->next = NULL;
	if(!g_clients)
		g_clients = new;
	else{
		while (t_cli->next)
			t_cli = t_cli->next;
		t_cli->next = new;
	}
	return (new->id);
}

//une fct pour suppr un client
int	rm_client(int fd){
	t_client	*t_cli = g_clients;
	t_client	*to_del;
	int			id = get_id(fd);

	if (t_cli->fd == fd){
		g_clients = t_cli->next;
		free(t_cli);
	} else {
		//penser à travailler avec le next !!
		while (t_cli && t_cli->next && t_cli->next->fd != fd)
			t_cli = t_cli->next;
		del = t_cli->next;
		t_cli->next = t_cli->next->next;
		free(del);
	}
	return(id);
}

//une fct pour accepter un client
void	accept_client(void){
	struct sockaddr_in	clientaddr;//<-
	socklen_t			len = sizeof(clientaddr);
	int					cli_fd;

	if((cli_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len)) < 0)//<-
		fatal_error();
	//prep le msg serveur
	sprintf(msg, "server: client %d just arrived\n", add_client(cli_fd));
	send_all(cli_fd, msg);
	//on ajoute le cli_fd à curr_sock
	FD_SET(cli_fd, &curr_sock);//<-
}

//une fct pour end à ts les client
void	send_all(int fd, char *ze_msg){
	t_client	*t_cli = g_clients;

	while (t_cli){
		//check si on peut écrire
		if (t_cli->fd != fd && FD_ISSET(t_cli->fd, &cpy_write)){//<-
			if(send(t_cli->fd, ze_msg, strlen(ze_msg), 0) < 0)//<-
				fatal_error(void);
		}
		t_cli = t_cli->next;
	}
}

void	send_msg(int fd){
	int	i = 0;
	int	j = 0;

	while (str[i]){
		tmp[j] = str[i];//<-
		j++;
		if (str[i] == '\n'){
			//prep de buf tel qu'on va l'afficher
			sprintf(buf, "client %d: %s", get_id(fd), tmp);
			send_all(fd, buf);
			j = 0;//<-
			//reset de tmp et buf avec bzero
			bzero(&tmp, sizeof(tmp));
			bzero(&buf, sizeof(buf));
		}
		i++
	}
	//reset de str
	bzero(&str, strlen(str));
}

//main
int	main(int ac, char** av){
	//test du nb d'arg
	if (ac != 2){
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	
	//declaration de servaddr de type sockaddr_in et port de type uint16_t
	struct sockaddr_in	servaddr;
	uint16_t			port = atoi(av[1]);

	//bzero de la struct servaddr
	bzero(&servaddr, sizeof(servaddr));
	
	//pour def le protocol TCP //presque dans main
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);//127.0.0.1
	servaddr.sin_port = htons(port);

	//creation de la socket principale qui detecte les co
	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)//<-
		fatal_error();
	//bind de cette socket
	if (bind(sock_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)//<-
		fatal_error();
	//on commence à écouter
	if (listen(sock_fd, 0) < 0)//<-
		fatal_error();
	
	//bzero special
	FD_ZERO(&curr_sock);//<-
	//ajout du fd à curr_sock
	FD_SET(sock_fd, &curr_sock);//<-
	//ini de 3 tab de char avec bzero
	bzero(&tmp, sizeof(tmp));
	bzero(&buf, sizeof(buf));
	bzero(&str, sizeof(str));
	
	while(1){
		//???
		cpy_write = cpy_read = curr_sock;//<-
		//on ini les cpy
		if (select(get_max_fd() + 1, &cpy_read, &cpy_write, NULL, NULL) < 0)//<-
			continue ;
		//pour chaque fd
		for (int fd = 0; fd <= get_max_fd(); fd++){
			//on verif si le fd est lisible
			if (FD_ISSET(fd, &cpy_read)){//<-
				//si la socket en question est la principale alors on a un new client
				if (fd == sock_fd){
					//reset msg
					bzero(&msg, sizeof(msg));
					//accept client
					accept_client();
					break ;
				} else {
					int ret_recv = 1000;

					while(ret_recv == 1000 || str[strlen(str) - 1] != '\n'){//<-
						ret_recv = recv(fd, str + strlen(str), 1000, 0);//<-
						if (ret_recv <= 0)//ret_recv est <=0 quand un client a deco
							break ;
					}
					if(ret_recv <= 0){
						//reset de msg
						bzero(&msg, sizeof(msg));
						//prep du msg serveur et suppr du client
						sprintf(msg, "server: client %d just left\n", rm_client(fd));
						//send_all de msg
						send_all(fd, msg);
						//liberation du fd de curr_sock
						FD_CLR(fd, &curr_sock);//<-
						close(fd);//pas de leak de fd
						break ;
					} else {
						send_msg(fd);
					}
				}
			}
		}
	}
	return (0);
}

