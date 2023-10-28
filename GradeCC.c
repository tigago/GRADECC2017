#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*Esta variável acompanha o estado em que o programa está
0 - Menu Inicial
1 - Inspecionando Disciplina
2 - Visualisando Optativas
*/
int Mode = 0;

/*Uma struct pode ser interpretada como um conjunto de variáveis de diferentes tipos em um só lugar
Aqui criamos uma struct chamada Disciplina para armazenar os dados de cada disciplina do banco de dados.*/
typedef struct 
{
    int num; //número da disciplina
    char name[66]; //nome da disciplina com até 65 caracteres (precisamos de um char para determinar o fim da string)
    char institute[5]; //nome do instituto com até 3 caracteres
    int prerequisite[4]; //números das disciplinas que são pre requisitos desta, assumindo que nunca teremos mais de 4 pre requisitos por disciplina
    int hours; //carga horaria da disciplina
    int nucleos; //0 -> núcleo comum 1-> nucleo especifico
    int type; //0 -> obrigatória 1 -> optativa
    char code[8]; //código da disciplina no Sigaa com até 7 characteres
    int semester; //qual é o período desta disciplina de acordo com o fluxo sugerido
}Disciplina;

Disciplina Disciplinas[100];//Variável array global do tipo Disciplina (o struct acima) que vai armazenar as informações de todas as disciplina do bd.txt (max 100)

/*Esta array é utilizada para armazenar o estado de cada disciplina
0- Não cursada
1- Cursando
2- Aprovado*/
int States[100] = {0};

/*Esta array é utilizada para saber se uma disciplina pode ou não ser cursada
0- Liberada
1- Falta Pre Requisitos
2- Proximo Semestre*/
int Block[100] = {0};
int Records = 0; //esta variável  global será utilizada para contar quantas disciplinas lemos na tabela bd.txt

/*Função para mudar a cor e estilo dos textos do console usando o nome em ingles
red - purple - green - yellow - cyan - blue
Se a string inserida não for nenhuma destas o sistema usará a cor padrão
Estilo: 0-> padrao 1-> Negrito*/
void SetTextStyle(char color[], int style){
    printf("\033[0m");
    if (color == "red"){
        printf("\033[%d;31m",style);
    } else if (color == "purple")
    {
        printf("\033[%d;35m",style);
    }
    else if (color == "green")
    {
        printf("\033[%d;32m",style);
    }
    else if (color == "yellow")
    {
        printf("\033[%d;33m",style);
    }
    else if (color == "cyan")
    {
        printf("\033[%d;36m",style);
    }else if (color == "blue")
    {
        printf("\033[%d;34m",style);
    }
    else
    {
        printf("\033[%dm",style);
    }
    
}

//Imprime um texto com um estilo e logo em seguida volta o estilo para default
void PrintStringInStyle(char str[], char color[], int style){
    SetTextStyle(color,style);
    printf(str);
    SetTextStyle("def", 0);
}

/*Esta função tenta ler o arquivo csv bd.txt, que tem que estar salvo na mesma pasta que o código está, e armazena os dados das disciplinas na variável global Disciplinas.
Caso haja qualquer tipo de erro será retornado 1, caso tudo tenha ocorrido bem será retornado 0.*/
int LoadDatabase(){
    printf("Carregando banco de dados...\n");
    FILE *file; //variável local do tipo ponteiro para armazenar o arquivo bd.txt
    file = fopen("bd.txt","r"); //Abre o arquivo em modo de leitura
    if (file == NULL) { //Checando se o arquivo bd.txt foi encontrado
        SetTextStyle("red",0);
        printf("Arquivo bd.txt não encontrado, encerrando aplicação!\n");
        return 1;
    }

    
    int read = 0; //esta variável local será utilizada para contar quantas informações foram lidas corretamente em cada linha da tabela bd.txt
    //enquanto não chegar ao fim do arquivo bd.text, faça:
    do{
        read = fscanf(file,"%d,%65[^,],%3[^,],%d,%d,%d,%d,%d,%d,%d,%7[^,],%d\n",
                        &Disciplinas[Records].num,
                        Disciplinas[Records].name,
                        Disciplinas[Records].institute,
                        &Disciplinas[Records].prerequisite[0],
                        &Disciplinas[Records].prerequisite[1],
                        &Disciplinas[Records].prerequisite[2],
                        &Disciplinas[Records].prerequisite[3],
                        &Disciplinas[Records].hours,
                        &Disciplinas[Records].nucleos,
                        &Disciplinas[Records].type,
                        Disciplinas[Records].code,
                        &Disciplinas[Records].semester);
        
        if (read == 12) Records++; // se doze leituras : OK, se não: erro!
        else if (!feof(file)){
            SetTextStyle("red",0);
            printf ("Arquivo em formato incorreto na linha %d.\n Sucesso ate o %dº valor...\n", Records + 1, read); //Impressão de onde apresentou erro na tabela
            return 1;
        }

        if (ferror(file)) //Se qualquer outro tipo de erro detectado com o arquivo
        {
            SetTextStyle("red",0);
            printf("Erro ao ler arquivo.\n");
            return 1;
        }
    }while (!feof(file));

    fclose(file); //Fecha o arquivo pois não precisaremos mais dele
    SetTextStyle("green",0);
    printf("Leitura de arquivo bem sucedida: %d linhas.\n", Records);
    return 0;   
}

//Função que imprime a lista de disciplinas separada pro periodos
int printMainList(){
    
    int p;
    for (p = 1; p<10; p++)
    {
        SetTextStyle("blue",1);
        printf("%d PERIODO:\n",p);
        SetTextStyle("def",1);
        printf("Cod     Nome                                                              CH   Status        Situacao\n");
        char stateName[12];
        char blockName[25];
        SetTextStyle("def",0);
        int i;
        for (i = 0; i<Records; i++){
                if (Disciplinas[i].semester != p) continue;
                if (Disciplinas[1].type != 0) continue;
                switch (States[i])
                {
                case 0:
                    strcpy(stateName,"Nao cursada");
                    break;
                case 1:
                    strcpy(stateName,"Cursando");
                    break;
                case 2:
                    strcpy(stateName,"Aprovado");
                    break;
                }
                if (States[i] == 2){
                    strcpy(blockName,"-");
                }else{
                    switch (Block[i])
                    {
                    case 0:
                        strcpy(blockName,"\033[0;32mLiberada\033[0m");
                        break;
                    case 1:
                        strcpy(blockName,"Bloqueada");
                        break;
                    case 2:
                        strcpy(blockName,"\033[0;33mProx Sem\033[0m");
                        break;
                    }
                }

                printf ("%-7s %-65s %-3d  %-13s %-s\n",Disciplinas[i].code, Disciplinas[i].name, Disciplinas[i].hours, stateName, blockName);
        }
        SetTextStyle("yellow", 0);
        if (p == 7 || p == 8) printf("O fluxo recomenda 1 optativa neste periodo\n");
        if (p == 9) printf("O fluxo recomenda 2 optativas neste periodo\n");
        printf("\n");
    }
    SetTextStyle("blue",1);
    printf("OPTATIVAS:\n");
    int chOpt = 0;
    for (int i = 0; i< Records; i++){
        if (Disciplinas[i].type != 1) continue;
        if (States[i] == 2) chOpt += Disciplinas[i].hours;
    }
    SetTextStyle("def", 0);
    printf("Horas de optativas cursadas: %d/256\n",chOpt);
}


void ChangeWholeSemesterStatus(int semester, int status){
    int i;
    for (i = 0; i<Records;i++){
        if (Disciplinas[i].semester != semester) continue;
        States[i] = status;
    }
}

int CheckState(int prerequisteNumber){
    if (prerequisteNumber == 0) return 2;
    return States[prerequisteNumber - 1];
}

//Esta função verifica todos os pre requisitos das disciplinas pra determinar se está liberada para cursar ou não
int UpdateBlockSituations(){
    int i;
    for (i = 0; i<Records;i++){
        if (CheckState(Disciplinas[i].prerequisite[0]) == 2 && CheckState(Disciplinas[i].prerequisite[1]) == 2 && CheckState(Disciplinas[i].prerequisite[2]) == 2 && CheckState(Disciplinas[i].prerequisite[3]) == 2) Block[i] = 0;
        else if ((CheckState(Disciplinas[i].prerequisite[0]) == 2 || CheckState(Disciplinas[i].prerequisite[0]) == 1) && 
                    (CheckState(Disciplinas[i].prerequisite[1]) == 2 || CheckState(Disciplinas[i].prerequisite[1]) == 1) && 
                    (CheckState(Disciplinas[i].prerequisite[2]) == 2 || CheckState(Disciplinas[i].prerequisite[2]) == 1) &&
                    (CheckState(Disciplinas[i].prerequisite[3]) == 2 || CheckState(Disciplinas[i].prerequisite[3]) == 1)) Block[i] = 2;
        else Block[i] = 1;
    }
    return 0;
}

//Comando de mudar status uma disciplina
int CmdChangeSingleDiscipline(){
    char code[8];
    PrintStringInStyle("Codigo da disciplina a ser alterada: (X para cancelar)\n", "cyan", 0);
    scanf("%s", code);
    if (strcmp(code,"X") == 0 || strcmp(code,"x") == 0){
        PrintStringInStyle("Operacao cancelada pelo usuario.\n", "yellow", 0);
        return 1;
    }
    int i;
    int found = -1;
    for (i = 0; i<Records; i++){
        if (strcmp(code,Disciplinas[i].code) == 0){
            found = i;
            break;
        }
    }
    if (found == -1){
        PrintStringInStyle("Codigo invalido, operacao cancelada.\n","red",0);
        return 1;
    }
    int read;
    SetTextStyle("cyan", 0);
    printf("0 - Nao Cursada\n1 - Cursando\n2 - Aprovada\nNovo status para a disciplina %s:\n",Disciplinas[found].name);
    SetTextStyle("def" , 0);
    scanf("%d", &read);
    if (read < 0 || read > 2){
        PrintStringInStyle("Comando invalido, operacao cancelada.\n","red",0);
        return 1;
    }
    States[found] = read;
    printf("Debug 0\n");
    return 0;
}

//Quando o usuario entra o comando para mudas as disciplinas de um semestre, é chamada esta função
int CmdChangeWholeSemester(){
    PrintStringInStyle("Entre o numero do semestre que deseja alterar:\n","cyan",0);
    int sem = -1;
    scanf("%d", &sem);
    if (sem <= 0 || sem > 9){
        PrintStringInStyle("Semestre invalido, operacao cancelada!\n", "red" , 0);
        return 1;
    }
    int opt = -1;
    SetTextStyle("cyan", 0);
    printf("0 - Nao Cursada\n1 - Cursando\n2 - Aprovada\nNovo status para o %d semestre:",sem);
    SetTextStyle("def" , 0);
    scanf("%d", &opt);
    if (opt < 0 || opt > 2){
        PrintStringInStyle("Opcao invalida, operacao cancelada!\n", "red" , 0);
        return 1;
    }
    ChangeWholeSemesterStatus(sem, opt);
    printf("Debug 0\n");
    return 0;
}
//Imprime o menu principal com a tabela principal e dá as opções
int EnterMainMenu(int printMessage){
    Mode = 0;
    UpdateBlockSituations();
    printf("\n");
    PrintStringInStyle("Esta e a sua situacao academica no curso CC PPC 2017:\n", "blue" , 1);
    printf("\n");
    printMainList();
    PrintStringInStyle("\nOpcoes:\n M - Mudar Status de uma disciplina\n P - Mudar status de todas as disciplinas de um periodo\n I - Inspecionar Disciplina\n O - Visualizar Optativas\n S - Salvar Status\n C - Carregar Status\n X - Finalizar Programa\n","cyan", 0);
    if (printMessage) PrintStringInStyle("\nAlteracoes feitas com sucesso\n", "green", 0);
    while (1){ //Loop infinito que dura enquanto o usuario não entrar um comando válido
        char cmd;
        printf("Insira o seu comando: ");
        scanf(" %c", &cmd);
        if (cmd == 'M' || cmd == 'm'){
            if (CmdChangeSingleDiscipline() == 0) break;;
        } else if (cmd == 'p' || cmd == 'P'){
            if (CmdChangeWholeSemester() == 0) break;
        }else if (cmd == 'X' || cmd == 'x'){
            return 1;
        }else{
            PrintStringInStyle("Comando nao reconhecido, tente novamente:\n", "red", 0);
        }
    }
    system("cls"); //Limpa a tela
    return 0;
}

int main(){
    system("cls"); //Limpa a tela
    SetTextStyle("def",0); //reseta a fonte para o padrão
    if (LoadDatabase() == 1) return 1; //Chama a função de ler o arquivo bd.txt, se der erro sai da aplicação
    int i = 0;
    ChangeWholeSemesterStatus(1,1);
    int includeMessage = 0; //utilizada para incluir ou nao a mensagem de alterações feitas ao fim do menu
    while (i == 0){
        i = EnterMainMenu(includeMessage);
        includeMessage = 1;
    }
    PrintStringInStyle("Finalizando Programa...", "yellow" , 0);
    return 0;
}