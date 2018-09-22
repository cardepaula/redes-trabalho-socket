/* Server side code */	

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

typedef struct mensagem {
	short port;
	time_t td;
	char autorizacao; 
	char nomeUsuario[50];
	int codUsuario;
} message;

/* Esta função reporta erro e finaliza do programa: */
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
	char *srvr_addr = "127.0.0.1"; /* Endereço padrão */
	char *srvr_port = "9099"; /* Porta padrão */
	struct sockaddr_in adr_srvr;/* AF_INET */
	struct sockaddr_in adr_clnt;/* AF_INET */
	int len_inet; /* comprimento  */
	int s; /* Socket do servidor */
	int c; /* Socket par ao cliente */
	int n; /* número de bytes lidos */
	time_t td; /* Data e hora atual */
	char dtbuf[128]; /* Informações de data e hora */
	message msg;

 	/* Cria um socket do tipo TCP */
	s = socket(PF_INET, SOCK_STREAM, 0);
	if ( s == -1 ) {
		bail("socket()");
	}

	/* Preenche a estrutura do socket com a porta e endereço do servidor */
	memset(&adr_srvr, 0, sizeof(adr_srvr));
	adr_srvr.sin_family = AF_INET;
	adr_srvr.sin_port = htons(atoi(srvr_port));
	if ( strcmp(srvr_addr,"*") != 0 ) {
		/* Endereço normal */
		adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
		if ( adr_srvr.sin_addr.s_addr == INADDR_NONE ) {
			bail("bad address."); 
		}
	} else {
		/* Qualquer endereço */
		adr_srvr.sin_addr.s_addr = INADDR_ANY;
	}

	/* Liga (bind) o socket com o endereço/porta */
	len_inet = sizeof(adr_srvr);
	z = bind(s, (struct sockaddr *)&adr_srvr, len_inet);
	if ( z == -1 ) {
		bail("bind(2)");
	}

	/* Coloca o socket do servidor em estado de "escuta" com até 10 clientes simultâneos*/
	z = listen(s, 10);
	if ( z == -1 ) {
		bail("listen(2)");
	}

	/* Inicia o loop do servido: */
	while(1) {

		/* Aguarda por uma solitação de conexão: */
		len_inet = sizeof adr_clnt;
		c = accept(s,(struct sockaddr *)&adr_clnt,&len_inet);

		if ( c == -1 ) {
			bail("accept(2)");
		}
		
		/* Gera a data e hora atual, onde o cliente faz requisições ao servidor: */
		time(&msg.td);
		n = (int) strftime(dtbuf,sizeof dtbuf, "%A %b %d %H:%M:%S %Y\n", localtime(&msg.td));
		dtbuf[n] = 0;

		z = read(c, &msg, sizeof(message));
		if ( z == -1 ) {
			bail("read(2): Its not possible to read the socket");
		}

		//printf("Número da porta lida: %d\n", msg.port);
		//printf("Código do cliente lido: %d\n", msg.codUsuario);

		/* Informações escritas para o usuário */
		// strcpy(msg.nomeUsuario, "SEHLORO 157");
		// msg.port = 157;
		// msg.autorizacao = 'A';
		// msg.td = time(&msg.td);
		
		/* Imprimi a data e a hora do servidor */
	    //printf("Nome: %s, data/hora: %s\n",msg.nomeUsuario, dtbuf);
		//printf("It's working!!!\n");

		/* Escreve o resultado no socket do cliente */
		//z = write(c,dtbuf,n);
		// z = write(c,(const void *) &td,sizeof(time_t));
		// z = write(c,(const void *) &msg,sizeof(struct mensagem));
		// if ( z == -1 ) {
		// 	bail("write(2)");
		// }
		
		/* Fecha a conexão com o cliente corrente */
		close(c);
	}

	return 0;
}
