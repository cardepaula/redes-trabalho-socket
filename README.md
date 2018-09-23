# redes-trabalho-socket

<i><b>Sockets!!!</b></i>
<div style="display: inline-block;">
  <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/3/3b/French-power-socket.jpg/1200px-French-power-socket.jpg" alt="sohcatXd" title="sohCatxD" width="400px" height="300px">
  <img src="https://raw.githubusercontent.com/cardepaula/redes-trabalho-socket/master/Images/sohCat.jpg" alt="sohCatxD2" title="sohCatxD2" width="400px" height="300px">
</div>


### Tutorial de executar os códigos com sucesso!
    OBS.: O tutorial vai ser feito com o foco no ambiente Linux, pois nele que foi
    desenvolvido toda a implementação do trabalho. Logo se for usar o mesmo tutorial
    em outro ambiente/SO possivelmente terá problemas. Um exemplo disso é o caminho
    do arquivos de registros de log do sistema e das credenciais de usuário que se 
    encontram em uma pasta junto com os arquivos fontes onde seu caminho usando o 
    sistema linux é: 
    ./database/nomeArquivo.txt

#### Compilação dos arquivos .c:

    Os arquivos serverside.c e clientside.c são os códigos fontes que contém as
    codificações realizadas. Para compilá-los siga os seguintes passos:

    1) Abra o terminal Linux exatamente no diretório em que se encontra os arquivos
       .c (serverside.c e clientside.c) ou abra o terminal Linux em qualquer local
       de sua máquina e vá até o diretório que se encontram os arquivo utilizando a
       comando cd do Linux;

    2) Compile o arquivo serverside.c no terminal da seguinte maneira:
      gcc -o server.exe serverside.c

    3) Compile o arquivo clientside.c no terminal da seguinte maneira:
      gcc -o client.exe clientside.c

#### Como executar os arquivos .exe:

    4) Perceba que no mesmo diretório surgirá dois arquivos executáveis (.exe)
       chamados server.exe e client.exe. Para executá-los faça o seguinte:
       * Primeiro execute o server.exe, pois ele é o servidor e não tem como o
       cliente se conectar ao servidor sem ele estar em funcionamento. Para
       executar o servidor faça o seguinte:
          ./serverside.exe

       * Após isso abra um outro terminal Linux no mesmo diretório que compilou
       e executou o serverside.exe e execute o arquivo clientside.exe da seguinte
       maneira:
          ./clientside.exe 3

       OBS.: O número 3 que aparece como segundo argumento na execução poderia ser
       qualquer número inteiro e positivo de 1 a 5. O número 3 que aparece é só como
       exemplo. Lembrando que esse número indica o número da porta que o usuário cliente
       deseja entrar.

    5) Após realizar essas 4 etapas com sucesso verá que o software do cliente guiará
    através da interface pedindo código de identificação do cliente/usuário e retornando
    uma resposta se o usuário está apto ou não a entrar pela porta desejada.

    6) Para finalizar a execução tanto do cliente quanto do servidor use o comando 
    Ctrl + c. Na execução do cliente todo momento que a interface pedir o código do
    usuário que deseja entrar na porta pode ser digita o valor -1 que também funciona
    semelhante ao comando Ctrl + c;

#### Como visualizar o arquivo de registro?

    7) Para visualizar o arquivo de registro pode-se fazer das seguintes maneiras:

      - A partir do terminal Linux em que foi executado o cliente vá até a pasta database
      utilizando o comando cd database seguido do comando cat logDeRegistros.txt, assim
      aparecerá no terminal todos os registros de tentativa dos usuários;
      - A outra maneira é pela interface gráfica, onde basta entrar na pasta database e
      selecionar e executar o arquivo logDeRegistros.txt;



