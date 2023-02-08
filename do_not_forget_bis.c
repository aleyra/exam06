void	fatal_error(void){

int	rm_client(int fd){
}

void	accept_client(void){
	//
	FD_SET(cli_fd, &curr_sock);//!!!!!!!!!!!
}

void	send_all(int fd, char *ze_msg){
	while (t_cli){
		if (t_cli->fd != fd && FD_ISSET(t_cli->fd, &cpy_write))//FD_ISSET ICI pas dans le while
			//
		//
	}
}

void	send_msg(int fd){
}

//main
int	main(int ac, char** av){
	//
	bzero(&servaddr, sizeof(servaddr));//avant def du TCP
	//
	
	while(1){
		//
		for (int fd = 0; fd <= get_max_fd(); fd++){
			if (FD_ISSET(fd, &cpy_read)){
				if (fd == sock_fd){
					//
					break ;//!!!!!!!
				} else {
					//
					while(ret_recv == 1000 || str[strlen(str) - 1] != '\n'){
						//un jour ce serait biende remplir ce recv
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

