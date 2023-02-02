#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

typedef struct s_client
{
	int				fd;
	int				id;
	struct s_client	*next;
}	t_client;

t_client	*g_clients = NULL;

int 	sock_fd, g_id = 0;
fd_set	curr_sock, cpy_read, cpy_write;
char 	msg[42];
char 	str[42*4096], tmp[42*4096], buf[42*4096 + 42];

void	fatal(void)
{
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	close(sock_fd);//pas de leaks de fd
	exit(1);
}

int	get_id(int fd)//recup id d'un client par rapport a un fd
{
	t_client	*temp = g_clients;

	while (temp)
	{
		if (temp->fd == fd)
			return (temp->id);
		temp = temp->next;
	}
	return (-1);
}

int	get_max_fd(void) //get le plus grand fd co, pour le select dans le main
{
	int			max = sock_fd;
	t_client	*temp = g_clients;

	while (temp)
	{
		if (temp->fd > max)
			max = temp->fd;
		temp = temp->next;
	}
	return (max);
}

void	send_all(int fd, char *str_req)//send a tous les clients
{
	t_client	*temp = g_clients;

	while (temp)
	{
		if (temp->fd != fd && FD_ISSET(temp->fd, &cpy_write))//on check si on peut ecrire
		{
			if (send(temp->fd, str_req, strlen(str_req), 0) < 0)
				fatal();
		}
		temp = temp->next;
	}
}

int	add_client_to_list(int fd)//mettre le new client sur lq liste chainee
{
	t_client *temp = g_clients;
	t_client *new;

	if (!(new = calloc(1, sizeof(t_client))))
		fatal();
	new->id = g_id++;
	new->fd = fd;
	new->next = NULL;
	if (!g_clients)
	{
		g_clients = new;
	}
	else
	{
		while (temp->next)
			temp = temp->next;
		temp->next = new;
	}
	return (new->id);
}

void	accept_client(void)//accepter un client
{
	struct sockaddr_in	clientaddr;
	socklen_t			len = sizeof(clientaddr);
	int					client_fd;

	if ((client_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len)) < 0)
		fatal();
	sprintf(msg, "server: client %d just arrived\n", add_client_to_list(client_fd));//msg info
	send_all(client_fd, msg);
	FD_SET(client_fd, &curr_sock);//on ajoute le new client dans la liste de socket
}

int	rm_client(int fd)//supp le cli
{
	t_client	*temp = g_clients;
	t_client	*del;
	int 		id = get_id(fd);

	if (temp && temp->fd == fd)
	{
		g_clients = temp->next;
		free(temp);
	}
	else
	{
		while (temp && temp->next && temp->next->fd != fd)
			temp = temp->next;
		del = temp->next;
		temp->next = temp->next->next;
		free(del);
	}
	return (id);
}

void	send_msg(int fd)//envoyer un msg
{
	int	i = 0;
	int	j = 0;

	while (str[i])
	{
		tmp[j] = str[i];
		j++;
		if (str[i] == '\n')
		{
			sprintf(buf, "client %d: %s", get_id(fd), tmp);
			send_all(fd, buf);
			j = 0;
			bzero(&tmp, strlen(tmp));
			bzero(&buf, strlen(buf));
		}
		i++;
	}
	bzero(&str, strlen(str));
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}

	struct sockaddr_in	servaddr;//struct aui contient toutes les info de connexion y compris addr
	uint16_t			port = atoi(av[1]);

	bzero(&servaddr, sizeof(servaddr));//initialisation a 0
	//pour def le protocol TCP
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433);//127.0.0.1
	servaddr.sin_port = htons(port);

	if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)//creer la ssocket principal qui detect les co
		fatal();
	if (bind(sock_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)//bind cette socket
		fatal();
	if (listen(sock_fd, 0) < 0)//on commence a ecouter
		fatal();

	FD_ZERO(&curr_sock);//un bzero special
	FD_SET(sock_fd, &curr_sock);//on ajoute le fd a curr_sock
	//ini a 0 des 3 tableaux
	bzero(&tmp, sizeof(tmp));
	bzero(&buf, sizeof(buf));
	bzero(&str, sizeof(str));
	while (1)
	{
		cpy_write = cpy_read = curr_sock;//cpy_* sont des listes de fd qui verif qu'on peut les utiliser
		if (select(get_max_fd() + 1, &cpy_read, &cpy_write, NULL, NULL) < 0)//remplir cpy_* 
			continue ;
		for (int fd = 0; fd <= get_max_fd(); fd++)
		{
			if (FD_ISSET(fd, &cpy_read))//verif pour chaque fd si c'est lisible
			{
				if (fd == sock_fd)//si la socket en question est la principale, on a un nouveau client
				{
					bzero(&msg, sizeof(msg));//reini un msg
					accept_client();//new client
					break ;
				}
				else
				{
					int	ret_recv = 1000;//a apprendre
					while (ret_recv == 1000 || str[strlen(str) - 1] != '\n')//tant que le msg n'est pas vide
					{
						ret_recv = recv(fd, str + strlen(str), 1000, 0);
						if (ret_recv <= 0)//dans le cas ou le client a deco
							break ;
					}
					if (ret_recv <= 0)//dans le cas ou le client a deco
					{
						bzero(&msg, sizeof(msg));
						sprintf(msg, "server: client %d just left\n", rm_client(fd));//permet de recup dans le buffer msg
						send_all(fd, msg);//envoyer le msg a tous les clients
						FD_CLR(fd, &curr_sock);//on libere le fd de curr_sock
						close(fd);//pas de leaks de fd
						break ;
					}
					else
					{
						send_msg(fd);//send msg
					}
				}
			}
		}
	}
	return (0);
}
