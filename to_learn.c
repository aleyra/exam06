//includes

//une liste chainee de clients avec un fd, un id

t_client	*g_clients = NULL;//<-

int		sock_fd, g_id = 0//<-
fd_set	curr_sock, cpy_read, cpy_write;//<-
char	msg[42];//<-
str[42 * 4096], tmp[42 * 4096], buf[42 * 4096 + 42];//<-

/*a utiliser pour le socket(), le bind(), le listen(),
le send(), le accept(), et le calloc()*/
void	fatal_error(void){
	//
}

int	get_id(int fd){
	//
	return (-1);//<-
}

int	get_max_fd(void){
	int			max = sock_fd;//<-
	//
	return (max);
}

int add_client(int fd){
	t_client	*t_cli = g_clients;
	t_client	*new;

	calloc(1, sizeof(t_client))//<-
	//
	return (new->id);
}

int	rm_client(int fd){
	t_client	*t_cli = g_clients;
	t_client	*to_del;
	int			id = get_id(fd);

	//g_client n'a qu'un client
	//sinon penser à travailler avec le next !!
	return(id);
}

void	accept_client(void){
	struct sockaddr_in	clientaddr;//<-
	socklen_t			len = sizeof(clientaddr);//<-
	int					cli_fd;//<-

	if((cli_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len))<0)//<-
		fatal_error();
	//sprintf msg serveur avec id = add_client puis send_all
	FD_SET(cli_fd, &curr_sock);//<-
}

void	send_all(int fd, char *msg){
	t_client	*t_cli = g_clients;

	while (t_cli){
		if (t_cli->fd != fd && FD_ISSET(/**/)){//<-
			//send msg avec flag 0
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
			//sprintf ds buf du msg a afficher, et send_all
			j = 0;//<-
			//reset de tmp et buf avec bzero
		}
		i++
	}
	//reset de str
}

//main
int	main(int ac, char** av){
	//test du nb d'arg
	//declaration de servaddr de type sockaddr_in et port de type uint16_t
	//bzero de la struct servaddr
	//pour def le protocol TCP //presque dans main servaddr.*

	//on crée la socket, on la bind et on l'écoute
	
	FD_ZERO(&curr_sock);//<-
	FD_SET(sock_fd, &curr_sock);//<-
	//ini de 3 tab de char avec bzero
	
	while(1){
		cpy_write = cpy_read = curr_sock;//<-
		if (select(/**/) < 0)//<-
			continue ;
		for (int fd = 0; fd <= get_max_fd(); fd++){
			if (FD_ISSET(/**/)){//<-
				if (fd == sock_fd){
					//reset msg
					//accept client
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
						//sprintf msg serveur avec id = suppr du client puis send_all
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
