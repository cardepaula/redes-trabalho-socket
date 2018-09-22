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
#include <ctype.h>

/* Constantes */
#define FALSE 0;
#define TRUE 1;
#define DIGITOSCODUSER 4;

typedef struct mensagem
{
	short port;
	time_t td;
	char autorizacao;
	char nomeUsuario[50];
	int codUsuario;
} Message;

/* Protótipo das funções utilizadas */
int leituraUsuario();
int isAllDigit();

/* Esta função reporta erro e finaliza do programa */
static void
bail(const char *on_what)
{
	if (errno != 0)
	{
		fputs(strerror(errno), stderr);
		fputs("	 ", stderr);
	}
	fputs(on_what, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char *argv[])
{
	int z;
	char *srvr_addr = "127.0.0.1"; /* Usando endereço padrão */
	char *srvr_port = "9099";	  /* Usando porta como padrão */
	struct sockaddr_in adr_srvr;   /* AF_INET */
	int len_inet;				   /* comprimento */
	int s;						   /* Socket */
	char dtbuf[128];			   /* Informação de data e hora */
	time_t td;					   /* Data e hora atual */
	int n;
	message msg;

	/* Usuário só pode passar uma porta como argumento */
	if (argc != 2)
	{
		printf("Precisa passar uma porta que deseja entrar como argumento!\n");
		exit(1);
	}

	/* Checa se o usuário passou na linha de comando digíto numérico válido para a identificação da porta */
	if (!isAllDigit(argv[1]))
	{
		printf("Favor passar somente dígitos numéricos como argumento para a identificação da porta!\n");
		exit(2);
	}

	/* Cria um socket do tipo TCP */
	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s == -1)
	{
		bail("socket()");
	}

	/* Preenche a estrutura do socket com a porta e endereço do servidor */
	memset(&adr_srvr, 0, sizeof(adr_srvr));
	adr_srvr.sin_family = AF_INET;
	adr_srvr.sin_port = htons(atoi(srvr_port));
	adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
	if (adr_srvr.sin_addr.s_addr == INADDR_NONE)
	{
		bail("bad address.");
	}

	/* Conecta com o servidor */
	len_inet = sizeof(adr_srvr);

	z = connect(s, (struct sockaddr *)&adr_srvr, len_inet);
	if (z == -1)
	{
		bail("connect(2): Server isn't working.");
	}

	/* Lê o código do usuário pelo teclado */
	int codigoUsuario = leituraUsuario();

	/* Preenche a estrutura mensagem com a porta e o código do usuário */
	msg.port = (short)atoi(argv[1]);
	msg.codUsuario = codigoUsuario;

	/* Escreve a mensagem para o socket do servidor */
	z = write(s, (const void *)&msg, sizeof(message));
	if (z == -1)
	{
		bail("write(2): It's not possible to write on socket.");
	}

	/* Pegando a informação de hora e data que o cliente faz a requisição ao servidor */
	// time(&msg.td); // Pega hora atual
	// n = (int)strftime(dtbuf,sizeof dtbuf,"%A %b %d %H:%M:%S %Y\n", localtime(&msg.td));
	// dtbuf[n] = 0;

	/* Lê as informações de data e hora do servidor */
	// z = read(s, &msg, sizeof(message));
	// if ( z == -1 ) {
	// 	bail("read(2)");
	// }
	// printf("Usuário: %s\n", msg.nomeUsuario);
	// printf("Porta: %d\n", msg.port);
	// printf("Horário: %s\n", dtbuf);
	// printf("Autorização: %c\n", msg.autorizacao);

	// n = (int) strftime(dtbuf,sizeof dtbuf,"%A %b %d %H:%M:%S %Y\n", localtime(&msg.td));
	// dtbuf[n] = 0; /* Adiciona um NULL no final da string */

	// /*
	// * Imprimi a data e a hora do servidor */
	// printf("Nome: %s, data/hora: %s\n",msg.nome, dtbuf);

	/* Fecha o socket */
	close(s);
	putchar('\n');

	return 0;
}

/* Faz a leitura da entrada de dados do usuário */
int leituraUsuario()
{
	int codigoUsuario = 0;
	char strCodigoUsuario[1000];
	int qntDigitosCodUsuario = DIGITOSCODUSER;

	printf("Digite o código de usuário: ");
	scanf("%s", strCodigoUsuario);

	/* Usuário não pode digitar mais que 4 caracteres */
	while ((strlen(strCodigoUsuario) != qntDigitosCodUsuario || !isAllDigit(strCodigoUsuario)) && strcmp(strCodigoUsuario, "-1") != 0)
	{
		printf("Favor digite somente %d dígitos numéricos.\n", qntDigitosCodUsuario);
		printf("Digite o código de usuário: ");
		scanf("%s", strCodigoUsuario);
	}
	codigoUsuario = atoi(strCodigoUsuario);

	return codigoUsuario;
}

/* Checa se a string passada é de somente dígitos numéricos */
int isAllDigit(char *strDigit)
{
	for (int i = 0; i < strlen(strDigit); i++)
	{
		if (!isdigit(strDigit[i]))
		{
			return FALSE;
		}
	}
	return TRUE;
}
