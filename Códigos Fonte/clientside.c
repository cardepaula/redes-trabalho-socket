 /* Client side code */	
 #include <stdio.h>	
 #include <unistd.h>
 #include <stdlib.h>
 #include <errno.h>	
 #include <string.h>	
 #include <sys/types.h>	
 #include <sys/socket.h>	
 #include <netinet/in.h>	
 #include <arpa/inet.h>	
 #include <netdb.h>	
 #include <time.h>
 
struct mensagem{
	time_t td; /* Data e hora atual */
	char nome[128]; /* nome do servidor */
};
	
 /*	
 * Esta função reporta erro e finaliza do programa
 */	
 static void	
	bail(const char *on_what) {	
	if ( errno != 0 ) {	
		fputs(strerror(errno),stderr);	
		fputs("	 ",stderr);
	}	
	fputs(on_what,stderr);	
	fputc('\n',stderr);	
	exit(1);	
 }	
	
 int main(int argc,char *argv[]) {	
	 int z;
	 char *srvr_addr = NULL;
	 char *srvr_port = "9099";
	 struct sockaddr_in adr_srvr;/* AF_INET */
	 int len_inet; /* comprimento */
	 int s; /* Socket */
	 char dtbuf[128]; /* Informação de data e hora */
	 time_t td; /* Data e hora atual */
	 int n;
	 struct mensagem msg; /* variagem utilizada para armazernar os dados da estrutura */

	 
	 if ( argc >= 2 ) {
		 /* Verifica se o endereço do servidor foi passado como argumento	 */	
		 srvr_addr = argv[1];		
	 } else {		
		/* Usa o valor padrão	 */
		srvr_addr = "127.0.0.1";
	 }		
			
	 /*		
	 * Verifica se o numero da porta foi passado como argumento
	 */		
	 if ( argc >= 3 )		
		 srvr_port = argv[2];		
			
	 /*		
	 * Cria um socket do tipo TCP		
	 */		
	 s = socket(PF_INET,SOCK_STREAM,0);		
	 if ( s == -1 )
		 bail("socket()");

	 /*
	 * Preenche a estrutura do socket com a porta e endereço do servidor
	 */
	 memset(&adr_srvr,0,sizeof adr_srvr);
	 adr_srvr.sin_family = AF_INET;
	 adr_srvr.sin_port = htons(atoi(srvr_port));
	 adr_srvr.sin_addr.s_addr =
	 inet_addr(srvr_addr);
	 if ( adr_srvr.sin_addr.s_addr == INADDR_NONE )
		 bail("bad address.");

	 /*
	 * Conecta com o servidor
	 */
	 len_inet = sizeof adr_srvr;

	 z = connect(s,(struct sockaddr *) &adr_srvr,len_inet);
	 if ( z == -1 )
		 bail("connect(2)");

	 /*
	 * Le as informações de data e hora
	 */
	 z = read(s,&msg,sizeof(struct mensagem));
	 if ( z == -1 )
		 bail("read(2)");

 	 n = (int) strftime(dtbuf,sizeof dtbuf,"%A %b %d %H:%M:%S %Y\n", localtime(&msg.td));
	 dtbuf[n] = 0; /* Adiciona um NULL no final da string */

	 /*
	 * Imprimi a data e a hora do servidor */
	 printf("Nome: %s, data/hora: %s\n",msg.nome, dtbuf);
	 
	 	
	 /*	
	 * Fecha o socket	
	 */	
	 close(s);	
	 putchar('\n');	
		
	 return 0;	
 }	
