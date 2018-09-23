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
#include <string.h>

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

typedef struct usuario
{
	int codUsuario;
	char nomeUsuario[50];
	int nivelAcesso;
} User;

/* Protótipos das funções */
void verificaCredenciais(User **users, Message *msg, int qntUsuariosCredenciados);
void atualizaRegistro(FILE *arqRegistros, Message *msg);
User **criarBufferUsuarios(FILE *arqCredenciais, int qntUsuariosCredenciados);
void apagarBufferUsuarios(User **users, int qntUsuariosCredenciados);
int quantidadeUsuariosCredenciados(FILE *arqCredenciais);

/* Esta função reporta erro e finaliza do programa: */
static void bail(const char *on_what)
{
	if (errno != 0)
	{
		fputs(strerror(errno), stderr);
		fputs(": ", stderr);
	}
	fputs(on_what, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main(int argc, char **argv)
{
	int z;
	char *srvr_addr = ENDERECOPADRAO; /* Endereço padrão */
	char *srvr_port = PORTAPADRAO;	  /* Porta padrão */
	struct sockaddr_in adr_srvr;   /* AF_INET */
	struct sockaddr_in adr_clnt;   /* AF_INET */
	int len_inet;				   /* comprimento  */
	int s;						   /* Socket do servidor */
	int c;						   /* Socket par ao cliente */
	int n;						   /* número de bytes lidos */
	time_t td;					   /* Data e hora atual */
	Message msg;
	FILE *arqRegistros;
	FILE *arqCredenciais;
	char caminhoArqCredenciais[] = "./database/credenciaisDeUsuario.txt";
	char caminhoArqRegistros[] = "./database/logDeRegistros.txt";
	char cTmp[1000];
	int a = 0;
	int qntUsuariosCredenciados = 0;
	User **users; /* Funciona como um "cache" de usuários */

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
	if (strcmp(srvr_addr, "*") != 0)
	{
		/* Endereço normal */
		adr_srvr.sin_addr.s_addr = inet_addr(srvr_addr);
		if (adr_srvr.sin_addr.s_addr == INADDR_NONE)
		{
			bail("bad address.");
		}
	}
	else
	{
		/* Qualquer endereço */
		adr_srvr.sin_addr.s_addr = INADDR_ANY;
	}

	/* Liga (bind) o socket com o endereço/porta */
	len_inet = sizeof(adr_srvr);
	z = bind(s, (struct sockaddr *)&adr_srvr, len_inet);
	if (z == -1)
	{
		bail("bind(2)");
	}

	/* Coloca o socket do servidor em estado de "escuta" com até 10 clientes simultâneos*/
	z = listen(s, 10);
	if (z == -1)
	{
		bail("listen(2)");
	}

	arqCredenciais = fopen(caminhoArqCredenciais, "r");
	if (arqCredenciais == NULL)
	{
		printf("Error ao abrir o arquivo de Credenciais.\n");
		exit(1);
	}

	arqRegistros = fopen(caminhoArqRegistros, "a");
	if (arqRegistros == NULL)
	{
		printf("Error ao abrir o arquivo de registros.\n");
		exit(2);
	}
	fclose(arqRegistros);

	printf("Servidor online\n");

	qntUsuariosCredenciados = quantidadeUsuariosCredenciados(arqCredenciais);

	users = criarBufferUsuarios(arqCredenciais, qntUsuariosCredenciados);

	/* Inicia o loop do servidor: */
	while (1)
	{
		/* Aguarda por uma solitação de conexão */
		len_inet = sizeof(adr_clnt);
		c = accept(s, (struct sockaddr *)&adr_clnt, &len_inet);

		if (c == -1)
		{
			bail("accept(2)");
		}

		z = read(c, &msg, sizeof(Message));
		if (z == -1)
		{
			bail("read(2): It's not possible to read the socket");
		}

		/* Verificando se usuário existe no "banco" e escrevendo registros de acesso ao
		sistema no log de arquivos do sistema */
		verificaCredenciais(users, &msg, qntUsuariosCredenciados);

		arqRegistros = fopen(caminhoArqRegistros, "a");
		if (arqRegistros == NULL)
		{
			printf("Error ao abrir o arquivo de registros.\n");
			exit(2);
		}
		atualizaRegistro(arqRegistros, &msg);
		fclose(arqRegistros);

		/* Reenvia o pacote Message para o soquete do cliente com as informações preenchidas */
		z = write(c, (const void *) &msg, sizeof(Message));
		if ( z == -1 ) 
		{
			bail("write(2): It's not possible to write on socket");
		}

		printf("Serviço realizado com sucesso.\n");

		/* Fecha a conexão com o cliente corrente */
		close(c);
	}

	/* Fechando arquivos e limpando a memória alocada */
	apagarBufferUsuarios(users, qntUsuariosCredenciados);
	fclose(arqCredenciais);

	return 0;
}

int quantidadeUsuariosCredenciados(FILE *arqCredenciais)
{
	int cont = 0;
	char c[100];

	while (!feof(arqCredenciais))
	{
		fgets(c, 100, arqCredenciais);
		cont++;
	}

	/* Aponta para o início do arquivo */
	rewind(arqCredenciais);

	return cont;
}

/* Cria uma espécie de cache de usuário do arquivo de credenciais de usuário */
User **criarBufferUsuarios(FILE *arqCredenciais, int qntUsuariosCredenciados)
{
	char str[80];
	const char separador[2] = ",";
	char *token;
	char bufferLinhaArquivo[3][100];
	int qtd = qntUsuariosCredenciados;
	User **users = (User **)malloc(qtd * sizeof(User *));
	int posicaoUsers = 0;

	for (int i = 0; i < qtd; i++)
	{
		users[i] = (User *)malloc(sizeof(User));
	}

	while (!feof(arqCredenciais))
	{
		fgets(str, 1000, arqCredenciais);

		token = strtok(str, separador);
		int i = 0;

		while (token != NULL)
		{
			strcpy(bufferLinhaArquivo[i], token);

			if (bufferLinhaArquivo[i][0] == ' ')
			{
				for (int contador = 0; contador < strlen(bufferLinhaArquivo[i]); contador++)
				{
					bufferLinhaArquivo[i][contador] = bufferLinhaArquivo[i][contador + 1];
				}
			}

			token = strtok(NULL, separador);
			i++;
		}

		users[posicaoUsers]->codUsuario = atoi(bufferLinhaArquivo[0]);
		strcpy(users[posicaoUsers]->nomeUsuario, bufferLinhaArquivo[1]);
		users[posicaoUsers]->nivelAcesso = atoi(bufferLinhaArquivo[2]);

		posicaoUsers++;
	}

	/* Aponta para o início do arquivo */
	rewind(arqCredenciais);

	return users;
}

/* Libera o cache de usuários alocados */
void apagarBufferUsuarios(User **users, int qntUsuariosCredenciados)
{
	if (users != NULL)
	{
		for (int i = 0; i < qntUsuariosCredenciados; i++)
		{
			free(users[i]);
		}

		free(users);
	}
}

/* Verifica se o usuário existe no "banco de dados" do sistema e preenche as informações do
usuário na estrutura mensagem*/
void verificaCredenciais(User **users, Message *msg, int qntUsuariosCredenciados)
{
	time(&msg->td);

	for (int i = 0; i < qntUsuariosCredenciados; i++)
	{
		if (users[i]->codUsuario == msg->codUsuario)
		{
			/* Preenche o campo da mensagem com o nome do usuário */
			strcpy(msg->nomeUsuario, users[i]->nomeUsuario);

			/* Verifica o acesstao a porta */
			if (msg->port <= users[i]->nivelAcesso)
			{
				msg->autorizacao = 'A';
			}
			else
			{
				msg->autorizacao = 'N';
			}

			return;
		}
	}
	/* Caso procure no loop todo o usuário e não encontra é enviado um caracter I 
	I == Inexistente, ou seja, usuário inexistente no sistema */
	msg->autorizacao = 'I';
}

/* Registra no log do sistema as tentativas de sucesso ou falha de acessar a porta */
void atualizaRegistro(FILE *arqRegistros, Message *msg)
{
	char dtbuf[256];
	int n = (int)strftime(dtbuf, sizeof(dtbuf), "%d/%m/%Y - %H:%M:%S", localtime(&msg->td));
	dtbuf[n] = 0;
	char strAutorizacao[30];

	switch (msg->autorizacao)
	{
		case 'A':
			strcpy(strAutorizacao, "autorizado");
			break;
		case 'N':
			strcpy(strAutorizacao, "negado");
			break;
		case 'I':
			strcpy(strAutorizacao, "inexistente");
			break;
		default:
			printf("Erro de tipo de autorização. Favor checar o sistema.");
			exit(3);
	}
	
	fprintf(arqRegistros, "%s, p%d, %d, %s\n", dtbuf, msg->port, msg->codUsuario, strAutorizacao);
}