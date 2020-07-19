/*
 *  udp client program.
 */
#include "client.h"

Socket init_connection(char* host, int port) ;
int		send_message(char* buf, Socket sock) ;
int		receive_message(char *buf, int size, Socket *sock) ;
void	message_loop(FILE* fpin, FILE* fpout, Socket sock) ;
void	close_connection(Socket sock) ;
int		main(int argc, char** argv) ;

/*
 * UDP connection �γ���
 *
 * ���� :
 * host : host name �⤷���� host �� IP address
 * port : port �ֹ�
 * 
 * ����� :
 * error ��ȯ��������硢Socket.socketfd �� -1 ��������֤���
 * 
 */
 
Socket init_connection(char *host, int port)
{
	struct hostent 	*host_ent ;
	struct in_addr	*addr_ptr ;
	struct sockaddr_in	cli_addr ;
	int			sockfd, val ;
	Socket		sock ;

	sock.socketfd = -1 ;

	if((host_ent = (struct hostent *)gethostbyname(host)) == NULL) {
	/* Check if a numeric address */
		if(inet_addr(host) == -1)
			return sock ;
	}
	else{
		addr_ptr = (struct in_addr *) *host_ent->h_addr_list ;
		host = inet_ntoa(*addr_ptr) ;
	}


	/*
	 *  Open UDP socket.
	 */
	if( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return sock ;	/* Can't open socket. */

	val = fcntl(sockfd, F_GETFL, 0) ;
#if defined NewsOS || defined IRIX
	val |= O_NDELAY ;
#else
	val |= O_NONBLOCK ;
#endif
	fcntl(sockfd, F_SETFL, val) ;

	/*
	 *  Bind any local address.
	 */
	bzero((char *) &cli_addr, sizeof(cli_addr)) ;
	cli_addr.sin_family			= AF_INET ;
	cli_addr.sin_addr.s_addr	= htonl(INADDR_ANY) ;
	cli_addr.sin_port			= htons(0) ;


	if(bind(sockfd, (struct sockaddr *) &cli_addr,
		sizeof(cli_addr)) < 0){
	return sock ;	/* Can't bind local address */
	}


	/*
	 *  Fill in the structure with the address of the server.
	 */
	sock.socketfd = sockfd ;

	bzero((char *) &sock.serv_addr, sizeof(sock.serv_addr)) ;
	sock.serv_addr.sin_family		= AF_INET ;
	sock.serv_addr.sin_addr.s_addr	= inet_addr(host) ;
	sock.serv_addr.sin_port			= htons(port) ;

	return sock ;
}


/*
 * socket �� message ������
 *
 * ���� :
 * buf  : message ������
 * sock : socket
 * 
 * ����� :
 * ���Ф˼��Ԥ�����硢-1 ���֤���
 * 
 */

int send_message(char *buf, Socket sock)
{
	int n ;

	n = strlen(buf) ;
	printf( "send %d : %s",
	ntohs(sock.serv_addr.sin_port),buf);
	if( sendto(sock.socketfd, buf, n, 0,
		(struct sockaddr *)&sock.serv_addr, sizeof(sock.serv_addr)) != n )
		return (-1) ;
	else
		return 0 ;
}

/*
 * socket ����� message �μ���
 *
 * ���� :
 * buf  : message ������� buffer
 * size : sizeof(buf)
 * sock : socket
 * 
 * ����� :
 * �����˼��Ԥ�����硢-1 ���֤���
 * �������Ƥ�̵����硢 0 ���֤���
 * �������Ƥ������硢 1 ���֤���
 * 
 */

int receive_message(char *buf, int size, Socket *sock)
{
	int			n,servlen ;
	struct sockaddr_in	serv_addr ;

	servlen = sizeof(serv_addr) ;
	n = recvfrom(sock->socketfd, buf, size, 0,
			 (struct sockaddr *)&serv_addr, &servlen);

	if(n < 0) {
		if( n == -1 && errno == EWOULDBLOCK){
			buf[0] = '\0' ;
			return 0 ;
		}
		else
			return (-1) ;
	}
	else {
		printf( "recv %d : ", ntohs(serv_addr.sin_port));
		sock->serv_addr.sin_port = serv_addr.sin_port ;
		buf[n] = '\0' ;

		if(n == 0)
			return 0 ;
		else
			return 1 ;
	}
}


/*
 * messege ��������
 *
 * ���� :
 * fpin : ����(���Ƥ� socket ��������ޤ��)
 * fpout: ����(socket ����μ������Ƥ�������ޤ��)
 * sock : socket
 * 
 */

void message_loop(FILE* fpin, FILE* fpout, Socket sock)
{
	fd_set	readfds, readfds_bak ;
	int		in, max_fd, n, ret ;
	char	buf[MAXMESG];

	in = fileno(fpin) ;
	FD_ZERO(&readfds) ;
	FD_SET(in, &readfds) ;
	FD_SET(sock.socketfd, &readfds) ;
	readfds_bak = readfds ;
	max_fd = ((in > sock.socketfd) ? in : sock.socketfd) + 1 ;

	while(1){
		readfds = readfds_bak ;
		if((ret = select(max_fd, &readfds, NULL, NULL, NULL)) < 0){
			perror("select") ;
			break ;
		}
		else if (ret != 0) {
			if(FD_ISSET(in, &readfds)){
				fgets(buf, MAXMESG, fpin) ;
				if(send_message(buf, sock) == -1)
					break ;
			}
			if(FD_ISSET(sock.socketfd, &readfds)){
				n = receive_message(buf, MAXMESG, &sock) ;
				if(n == -1)
					break ;
				else if(n > 0){
					fputs(buf, fpout) ;
					fputc('\n', fpout) ;
				}
				fflush(stdout) ;
			}
		}
	}
}

/*
 * socket �� close
 *
 * ���� :
 * sock : socket
 * 
 */

void close_connection(Socket sock)
{
	close(sock.socketfd) ;
}

/* for test */
#ifdef TEST
int main(int argc, char **argv)
{
	Socket sock ;
	char *server ;
	int port ;

	if(argc == 3) {
		server = argv[1] ;
		port = atoi(argv[2]) ;
	} else if (argc == 2) {
		server = argv[1] ;
		port = 6000 ;
	} else {
		server = "localhost" ;
		port = 6000 ;
	}

	sock = init_connection(server,port) ;
	if(sock.socketfd == -1)
		exit(1) ;

	message_loop(stdin, stdout, sock) ;

	close_connection(sock) ;
}
#endif /* TEST */

