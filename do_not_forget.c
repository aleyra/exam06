void	fatal_error(void){//<-
	//
	close(sock_fd);//sock_fd car pas de fd ds param
	//
}

int	rm_client(int fd){
	if (t_cli->fd == fd){
		//
	} else {
		while (t_cli && t_cli->next && t_cli->next->fd != fd)//<-
			//
		//
	}
	//
}

void	accept_client(void){//<-c'est le accept qui def le fd
	//
	int					cli_fd;

	if((cli_fd = accept(sock_fd, (struct sockaddr *)&clientaddr, &len)) < 0)//<-
		//
	FD_SET(cli_fd, &curr_sock);
}

void	send_all(int fd, char *ze_msg){
	while (t_cli){
		if (t_cli->fd != fd && FD_ISSET(t_cli->fd, &cpy_write))//<-
			//
		//
	}
}

void	send_msg(int fd){//<-
	//
	bzero(&str, strlen(str));
}

//main
int	main(int ac, char** av){
	//
	if (listen(sock_fd, 0) < 0)//<- ici c'est 0 et pas 10
		//
	
	FD_SET(sock_fd, &curr_sock);//sock_fd est aussi a mettre dans curr_sock
	
	while(1){
		//
		for (int fd = 0; fd <= get_max_fd(); fd++){
			if (FD_ISSET(fd, &cpy_read)){//<-
				if (fd == sock_fd){
					//
					break ;
				} else {
					//
					while(ret_recv == 1000 || str[strlen(str) - 1] != '\n'){
						//
						if (ret_recv <= 0)
							//
					}
					if(ret_recv <= 0){
						//
					} else {
						//
					}
				}
			}
		}
	}
	return (0);
}

