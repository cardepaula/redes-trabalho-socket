/* server.c:
*
* Exemplo de servidor de data e hora:
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>



struct mensagem{
	time_t td; /* Data e hora atual */
	char nome[128]; /* nome do servidor */
};

/*
* Esta função reporta erro e finaliza do programa:
*/
static void
bail(const char *on_what) {
	if ( errno != 0 ) {
		fputs(strerror(errno),stderr);
		fputs(": ",stderr);
	}
	fputs(on_what,stderr);
	fputc('\n',stderr);
	exit(1);
}

int main(int argc,char **argv) {
	int z;
	char *srvr_addr = NULL;
	char *srvr_port = "9099";
	struct sockaddr_in adr_srvr;/* AF_INET */
	struct sockaddr_in adr_clnt;/* AF_INET */
	int len_inet; /* comprimento  */
	int s; /* Socket do servidor */
	int c; /* Socket par ao cliente */
	int n; /* número de bytes lidos */
	time_t td; /* Data e hora atual */
	char dtbuf[128]; /* Informações de data e hora */
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
	if ( strcmp(srvr_addr,"*") != 0 ) {
		/* Endereço normal */
		adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
		if ( adr_srvr.sin_addr.s_addr == INADDR_NONE )
			bail("bad address."); 
	} else {
		/* Qualquer endereço */
		adr_srvr.sin_addr.s_addr = INADDR_ANY;
	}

/*
* Liga (bind) o socket com o endereço/porta
*/
	len_inet = sizeof adr_srvr;
	z = bind(s,(struct sockaddr *)&adr_srvr, len_inet);
	if ( z == -1 )
		bail("bind(2)");

/*
* Coloca o socket do servidor em estado de "escuta"
*/
	z = listen(s,10);
	if ( z == -1 )
		bail("listen(2)");

/*
* Inicia o loop do servido:
*/
	for (;;) {
		/*
		* Aguarda por uma solitação de conexão:
		*/
			len_inet = sizeof adr_clnt;
			c = accept(s,(struct sockaddr *)&adr_clnt,&len_inet);

			if ( c == -1 )
				bail("accept(2)");

		/*
		* Informar o nome do servidor:
		*/
		strcpy(msg.nome, "SERVER RCBSI");
				
		/*
		* Gera a data e hora atual:
		*/
		time(&msg.td);
		n = (int) strftime(dtbuf,sizeof dtbuf, "%A %b %d %H:%M:%S %Y\n", localtime(&msg.td));
		dtbuf[n] = 0;
		
		/* Imprimi a data e a hora do servidor */
	    printf("Nome: %s, data/hora: %s\n",msg.nome, dtbuf);

		/* Escreve o resultado no socket do cliente
		*/
		//z = write(c,dtbuf,n);
		// z = write(c,(const void *) &td,sizeof(time_t));
		z = write(c,(const void *) &msg,sizeof(struct mensagem));
		
		if ( z == -1 )
			bail("write(2)");
		
		/*
		* Fecha a conexão com o cliente
		*/
		close(c);
	}

	return 0;
}
