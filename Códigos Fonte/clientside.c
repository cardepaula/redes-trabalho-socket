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
#define QNTMAXPORTAS 5;
#define ENDERECOPADRAO "127.0.0.1";
#define PORTAPADRAO "9099";

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
void mensagemAutorizacaoUsuario(Message *msg);

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
	char *srvr_addr = ENDERECOPADRAO; /* Usando endereço padrão */
	char *srvr_port = PORTAPADRAO;	  /* Usando porta como padrão */
	struct sockaddr_in adr_srvr;   /* AF_INET */
	int len_inet;				   /* comprimento */
	int s;						   /* Socket */
	char dtbuf[128];			   /* Informação de data e hora */
	time_t td;					   /* Data e hora atual */
	int n;
	Message msg;

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

	/* Checa se o número da porta passado pelo usuário existe no contexto do ambiente */
	short idPortaUsuario = (short)atoi(argv[1]);
	int qntMaxPortas = QNTMAXPORTAS;

	if (idPortaUsuario > qntMaxPortas || idPortaUsuario == 0)
	{
		printf("Digite a identificação da porta na linha de comando de 1 a %d somente.\n", qntMaxPortas);
		exit(3);
	}

	int codigoUsuario = leituraUsuario();
	while (codigoUsuario != -1) 
	{
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
			bail("connect(2)");
		}

		/* Preenche a estrutura mensagem com a porta e o código do usuário */
		msg.port = idPortaUsuario;
		msg.codUsuario = codigoUsuario;

		/* Escreve a mensagem para o socket do servidor */
		z = write(s, (const void *)&msg, sizeof(Message));
		if (z == -1)
		{
			bail("write(2): It's not possible to write on socket.");
		}

		/* Lê as informações do pacote Message enviado ao servidor com a autorização do
		usuário corrente */
		z = read(s, &msg, sizeof(Message));
		if ( z == -1 ) 
		{
			bail("read(2): It's not possible to read the socket");
		}

		/* Imprime mensagem informando autorização do cliente/usuário */
		putchar('\n');
		mensagemAutorizacaoUsuario(&msg);
		puts("\n\n");

		/* Fecha o socket do cliente */
		close(s);

		/* Lê o código do usuário pelo teclado */
		codigoUsuario = leituraUsuario();
	}

	/* Caso o usuário digite -1 fecha o socket previamente aberto no loop while(1) */
	//close(s);

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

/* Checa se a string passada é de somente dígitos numéricos e positivos */
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

/* Mensagem para o usuário com sua autorização a determinada porta atualizada */
void mensagemAutorizacaoUsuario(Message *msg) 
{
	switch(msg->autorizacao) 
	{
		case 'A':
			printf("\tAcesso autorizado. Bem-vindo %s.", msg->nomeUsuario);
			break;
		case 'N':
			printf("\tAcesso negado. %s suas credencias não permitem acesso a esta sala.", msg->nomeUsuario);
			break;
		case 'I':
			printf("\tInexistente. Usuário com código %d não se encontra cadastrado no sistema.", msg->codUsuario);
			break;
		default:
			printf("Erro de tipo de autorização. Favor checar o sistema.");
			exit(4);
	}
}