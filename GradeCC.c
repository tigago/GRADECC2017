#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

int IncludeMessage = 0; //utilizada para incluir ou nao a mensagem de alterações feitas ao fim do menu
int IncludeOpt = 0; //utilizada para incluir ou nao as disciplinas optativas na lista principal

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
int Priority[100];
/*Esta array é utilizada para saber se uma disciplina pode ou não ser cursada
0- Liberada
1- Falta Pre Requisitos
2- Proximo Semestre*/
int Block[100] = {0};
int Records = 0; //esta variável  global será utilizada para contar quantas disciplinas lemos na tabela bd.txt

//Esta funcao compara duas strings sem se preocupar com maiusculas e minusculas
int stricmp(const char str1[], const char str2[]) {
    int i = 0;
    if (strlen(str1) != strlen(str2)) return 0;
    while (str1[i] && str2[i]) {
        if (tolower((unsigned char)str1[i]) != tolower((unsigned char)str2[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}


void ClearScreen(){
	#ifdef _WIN32 // Includes both 32 bit and 64 bit
        system("cls");
	#else
	    system("clear");
	#endif
}

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
    printf("%s", str);
    SetTextStyle("def", 0);
}

int CountTotalDependencies(int disciplineNumber){
    int compromised[100] = {-1};
    int comproCount = 0;
    int searchLen = 0;
    int i;
    for (i = 0; i<Records; i++){
        if (Disciplinas[i].type == 1) continue;
        if(disciplineNumber == Disciplinas[i].prerequisite[0] - 1
            || disciplineNumber == Disciplinas[i].prerequisite[1] - 1
            || disciplineNumber == Disciplinas[i].prerequisite[2] - 1
            || disciplineNumber == Disciplinas[i].prerequisite[3] - 1){
                compromised[searchLen] = i;
                comproCount++;
                searchLen++;
            }
    }
    int lastSearchIndex = 0;
    while (searchLen > 0)
    {
        int count = 0;
        //printf("\nDebug: checando de %d ate %d:\n", lastSearchIndex, lastSearchIndex + searchLen - 1);
        for (i = lastSearchIndex; i < lastSearchIndex + searchLen; i++){
            int j;
            for (j = 0; j <Records; j++){
                if (Disciplinas[j].type == 1) continue;
                if(compromised[i] == Disciplinas[j].prerequisite[0] - 1
                || compromised[i] == Disciplinas[j].prerequisite[1] - 1
                || compromised[i] == Disciplinas[j].prerequisite[2] - 1
                || compromised[i] == Disciplinas[j].prerequisite[3] - 1){
                    int k;
                    int check = 0; //checar se o comprometido ja foi adicionado ao caminho
                    for (k = 0; k < lastSearchIndex + count + searchLen; k ++){
                        if (compromised[k] == j) {
                            check = 1;
                            break;
                        }
                    }
                    if (check) continue;
                    count++;
                    compromised[lastSearchIndex + count + searchLen - 1] = j;
                    comproCount++;
                }
            }
        }
        lastSearchIndex += searchLen;
        searchLen = count;
    }
    return comproCount;
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
    int i;
    for (i = 0; i < Records; i++){
        Priority[i] = CountTotalDependencies(i);
    }
    return 0;   
}


//Esta funcao imprime as informacoes de uma disciplina de forma conforme à tabela
void PrintTableLineForDiscipline(int disciplineIndex, int includeSemester){
    char stateName[12];
    char blockName[25];
    switch (States[disciplineIndex])
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
    if (States[disciplineIndex] == 2){
        strcpy(blockName,"-");
    }
    else if (States[disciplineIndex] == 1){
        strcpy(blockName, "\033[0;34mCursando\033");
    }
    else{
        switch (Block[disciplineIndex])
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
    SetTextStyle("def",0);
    if (includeSemester){
        if (Disciplinas[disciplineIndex].semester > 0) printf("%-4d", Disciplinas[disciplineIndex].semester);
        else printf("Opt ");
    }
    printf ("%-7s %-65s %-3d  %-13s %-11d %-s",Disciplinas[disciplineIndex].code, Disciplinas[disciplineIndex].name, Disciplinas[disciplineIndex].hours, stateName, Priority[disciplineIndex], blockName);
    if (Disciplinas[disciplineIndex].type == 1) printf(" OPT");
    printf("\n");
}

//Função que imprime a lista de disciplinas separada por periodos
int printMainList(){
    
    int p;
    for (p = 1; p<10; p++)
    {
        SetTextStyle("blue",1);
        printf("%d PERIODO:\n",p);
        SetTextStyle("def",1);
        printf("Cod     Nome                                                              CH   Status        Prioridade  Situacao\n");
        char stateName[12];
        char blockName[25];
        SetTextStyle("def",0);
        int i;
        for (i = 0; i<Records; i++){
                if (Disciplinas[i].semester != p) continue;
                if (Disciplinas[1].type != 0) continue;
                PrintTableLineForDiscipline(i, 0);
        }
        SetTextStyle("yellow", 0);
        if (p == 7 || p == 8) printf("O fluxo recomenda 1 optativa neste periodo\n");
        if (p == 9) printf("O fluxo recomenda 2 optativas neste periodo\n");
        printf("\n");
    }
    SetTextStyle("blue",1);
    printf("OPTATIVAS:\n");
    if (IncludeOpt)  PrintStringInStyle("Cod     Nome                                                              CH   Status        Prioridade  Situacao\n", "def", 1);
    int chOpt = 0;
    for (int i = 0; i< Records; i++){
        if (Disciplinas[i].type != 1) continue;
        if (IncludeOpt) PrintTableLineForDiscipline(i, 0);
        if (States[i] == 2) chOpt += Disciplinas[i].hours;
    }
    SetTextStyle("def", 0);
    printf("\nHoras de optativas cursadas: %d/256\n",chOpt);
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
    if (stricmp(code,"X")){
        PrintStringInStyle("Operacao cancelada pelo usuario.\n", "yellow", 0);
        return 1;
    }
    int i;
    int found = -1;
    for (i = 0; i<Records; i++){
        if (stricmp(code,Disciplinas[i].code)){
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
    IncludeMessage = 1;
    UpdateBlockSituations();
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
    UpdateBlockSituations();
    IncludeMessage = 1;
    return 0;
}

int CmdInspectdiscipline(){
    char code[8];
    PrintStringInStyle("Codigo da disciplina a ser isnpecionada: (X para cancelar)\n", "cyan", 0);
    scanf("%s", code);
    if (stricmp(code,"X")){
        PrintStringInStyle("Operacao cancelada pelo usuario.\n", "yellow", 0);
        return 1;
    }
    int i;
    int found = -1;
    for (i = 0; i<Records; i++){
        if (stricmp(code,Disciplinas[i].code)){
            found = i;
            break;
        }
    }
    if (found == -1){
        PrintStringInStyle("Codigo invalido, operacao cancelada.\n","red",0);
        return 1;
    }
    ClearScreen(); //Limpa a tela
    PrintStringInStyle("Detalhes da disciplina:\n", "blue", 1);
    PrintStringInStyle("Sem Cod     Nome                                                              CH   Status        Prioridade  Situacao\n", "def", 1);
    PrintTableLineForDiscipline(found, 1);
    if (Disciplinas[found].prerequisite[0] != 0)
    {
        PrintStringInStyle("\nPre-requisitos diretos desta disciplina:\n", "blue", 1);
        PrintStringInStyle("Sem Cod     Nome                                                              CH   Status        Prioridade  Situacao\n", "def", 1);
        for (i = 0; i<4; i++){
            if (Disciplinas[found].prerequisite[i] == 0) continue;
            PrintTableLineForDiscipline(Disciplinas[found].prerequisite[i] - 1, 1);
        }
    }
    int header = 0;
    for (i = 0; i<Records; i++){
        if(found == Disciplinas[i].prerequisite[0] - 1
            || found == Disciplinas[i].prerequisite[1] - 1
            || found == Disciplinas[i].prerequisite[2] - 1
            || found == Disciplinas[i].prerequisite[3] - 1){
                if (header == 0){
                    header = 1;
                    PrintStringInStyle("\nEsta disciplina e pre-requisito direto para:\n", "blue", 1);
                    PrintStringInStyle("Sem Cod     Nome                                                              CH   Status        Prioridade  Situacao\n", "def", 1);
                }
                PrintTableLineForDiscipline(i, 1);
            }
    }
    int path[100] = {-1,-1};
    path[0] = found;
    int searchLen = 1;
    int lastSearchIndex = 0;
    header = 0;
    while(searchLen > 0){
        int count = 0;
        //printf("\nDebug: checando de %d ate %d:\n", lastSearchIndex, lastSearchIndex + searchLen - 1);
        for (i = lastSearchIndex; i < lastSearchIndex + searchLen; i++){
            int j;
            //printf("\nDebug: procurando em: %s\n", Disciplinas[path[i]].name);
            for (j = 0; j < 4; j++){
                if (Disciplinas[path[i]].prerequisite[j] != 0){
                    int num = Disciplinas[path[i]].prerequisite[j] - 1;
                    int k;
                    int check = 0; //checar se o prerequisito ja foi adicionado ao caminho
                    for (k = 0; k < lastSearchIndex + count; k ++){
                        if (path[k] == num) {
                            check = 1;
                            //printf("\nDebug: achou repetido: %s\n", Disciplinas[num].name);
                            break;
                        }
                    }
                    if (check) continue; //se pre requisito ja esta no caminho va para o proximo j
                    count++;
                    path[lastSearchIndex + count + searchLen - 1] = num;
                    //printf("\nDebug: adicionando: %s no index %d\n", Disciplinas[num].name, lastSearchIndex + count + searchLen - 1);
                    if (header == 0){
                        header = 1;
                        PrintStringInStyle("\nTodas as disciplinas que voce precisa passar ate chegar a esta:\n", "yellow", 1);
                        PrintStringInStyle("Sem Cod     Nome                                                              CH   Status        Prioridade  Situacao\n", "def", 1);
                    }
                    PrintTableLineForDiscipline(num, 1);
                }
            }
        }
        lastSearchIndex += searchLen;
        searchLen = count;
    }

    int compromised[100] = {-1};
    searchLen = 0;
    header = 0;
    for (i = 0; i<Records; i++){
        if (Disciplinas[i].type == 1) continue;
        if(found == Disciplinas[i].prerequisite[0] - 1
            || found == Disciplinas[i].prerequisite[1] - 1
            || found == Disciplinas[i].prerequisite[2] - 1
            || found == Disciplinas[i].prerequisite[3] - 1){
                compromised[searchLen] = i;
                searchLen++;
                if (header == 0){
                    header = 1;
                    PrintStringInStyle("\nReprovacao nesta disciplina atrasa todas essas outras: (nao incluindo optativas)\n", "red", 1);
                    PrintStringInStyle("Sem Cod     Nome                                                              CH   Status        Prioridade  Situacao\n", "def", 1);
                }
                PrintTableLineForDiscipline(i, 1);
                //printf("\nDebug: adicionando: %s no index %d\n", Disciplinas[i].name, searchLen - 1);
            }
    }
    lastSearchIndex = 0;
    while (searchLen > 0)
    {
        int count = 0;
        //printf("\nDebug: checando de %d ate %d:\n", lastSearchIndex, lastSearchIndex + searchLen - 1);
        for (i = lastSearchIndex; i < lastSearchIndex + searchLen; i++){
            int j;
            for (j = 0; j <Records; j++){
                if (Disciplinas[j].type == 1) continue;
                if(compromised[i] == Disciplinas[j].prerequisite[0] - 1
                || compromised[i] == Disciplinas[j].prerequisite[1] - 1
                || compromised[i] == Disciplinas[j].prerequisite[2] - 1
                || compromised[i] == Disciplinas[j].prerequisite[3] - 1){
                    int k;
                    int check = 0; //checar se o comprometido ja foi adicionado ao caminho
                    for (k = 0; k < lastSearchIndex + count + searchLen; k ++){
                        if (compromised[k] == j) {
                            check = 1;
                            break;
                        }
                    }
                    if (check) continue;
                    count++;
                    compromised[lastSearchIndex + count + searchLen - 1] = j;
                    //printf("\nDebug: adicionando: %s no index %d\n", Disciplinas[j].name, lastSearchIndex + count + searchLen - 1);
                    PrintTableLineForDiscipline(j, 1);
                }
            }
        }
        lastSearchIndex += searchLen;
        searchLen = count;
    }
    




    char opt;
    PrintStringInStyle("\nEntre qualquer valor para voltar ao menu: ", "cyan" , 0);
    scanf(" %c", &opt);
    IncludeMessage = 0;
    return 0;
}




int CmdSaveProfile(){
    PrintStringInStyle("Max 15 characteres\nAo salvar com o nome de um perfil existente, este sera substituido\n", "yellow", 0);
    PrintStringInStyle("Salvar perfil com o nome:\n","cyan",0);
    char name[20];
    scanf("%15s",name);
    SetTextStyle("cyan",0);
    printf("Tem certeza que quer salvar o perfil como %s.perfil na pasta do programa? Arquivos de nomes iguais serao susbtituidos!\n0-SIM\n1-NAO\n",name);
    SetTextStyle("def",0);
    int opt;
    scanf("%d",&opt);
    if (opt < 0 || opt > 1){
        PrintStringInStyle("Opcao Invalida, operacao cancelada\n","red",0);
        return 1;
    }
    if (opt == 1){
        PrintStringInStyle("Operacao cancelada pelo usuario.\n","yellow",0);
        return 1;
    }
    FILE *file;
    file = fopen(strcat(name,".perfil"),"w"); //Abre ou cria um arquivo em modo de escrita
    if (file == NULL){
        PrintStringInStyle("Erro ao abrir ou criar arquivo de perfil, operacao cancelada.\n","red",0);
        return 1;
    }
    int i;
    for (i = 0; i<Records;i++){
        if (States[i] == 0) fputs("0\n",file);
        else if (States[i] == 1) fputs("1\n",file);
        else fputs("2\n",file);
    }
    fclose(file);
    IncludeMessage = 1;
    return 0;
}

int CmdLoadProfile(){
    PrintStringInStyle("Nao inclua o sufixo .perfil\n", "yellow", 0);
    PrintStringInStyle("Nome do perfil a ser carregado:\n","cyan",0);
    char name[20];
    scanf("%15s",name);
    FILE *file;
    file = fopen(strcat(name,".perfil"),"r"); //Abre ou cria um arquivo em modo de leitura
    if (file == NULL){
        PrintStringInStyle("Nao foi encontrado um perfil com este nome, operacao cancelada.\n","red",0);
        return 1;
    }

    int i=0;
    do{ 
        fscanf(file,"%d\n",&States[i]);
        printf("%d\n",States[i]);
        i++;
        if (ferror(file)) //Se qualquer outro tipo de erro detectado com o arquivo
        {
            SetTextStyle("red",0);
            printf("Erro desconhecido ao ler arquivo.\n");
            return 2;
        }
     } while (!feof(file));
    if (i != Records){
        PrintStringInStyle("Numero de informacoes no arquivo e incompativel!\n","red", 0);
        return 2;
    }
    fclose(file);
    UpdateBlockSituations();
    IncludeMessage = 1;
    return 0;
}

int CmdChangeCursandoToAprovado(){
    PrintStringInStyle("Quer mudar todas as disciplinas que estao como CURSANDO para APROVADO?\n 0 - SIM\n 1 - NAO\n","cyan",0);
    int r;
    scanf("%d", &r);
    if (r == 0){
        int i;
        for (i = 0; i < Records; i++){
            if (States[i] == 1) States[i] = 2;
        }
        UpdateBlockSituations();
        IncludeMessage = 1;
    }else if (r == 1){
        PrintStringInStyle("Operacao cancelada pelo usuario.\n", "red", 0);
        return  1;
    } else{
        PrintStringInStyle("0 ou 1 manezao! Operacao Cancelada.\n", "red", 0);
        return  1;
    }
    return 0;
}
//Função que da informacoes sobre o projeto
int CmdAbout(){
    ClearScreen();
    PrintStringInStyle("Auxiliar de fluxo Ciencia da Computacao PPC 2017 v1.0:\n","blue",1);
    printf("Este programa tem como objetivo auxiliar alunos que estão com a grade baguncada, seja por motivos de reprovacao ou aproveitamento de disciplina.\n");
    printf("Quando o aluno altera o status das disciplinas de acordo com a sua situacao, o programa ressalta quais disciplinas ja estao liberadas e quais disciplinas estarao disponiveis no proximo semestre caso haja aprovacao em todas as diciplinas que estao sendo cursadas.\n");
    printf("O usuario pode salvar as informacoes dos status das disciplinas em um perfil para utilizar novamente quando o programa for aberto uma outra vez.\n");
    printf("As alteracoes nao sao salvas automaticamente, possibilitando a simulacao de varios cenarios pelo usuario sem comprometer os seus dados salvos\n");
    PrintStringInStyle("\nProjeto desenvolvido para a aula de IP 2023/2 ministrada pelo professor Hebert.\n","yellow",1);
    PrintStringInStyle("GRUPO 7:\n", "def", 1);
    printf("Hawryson Do Nascimento Mesquita\nIgor Gabriel Mario de Oliveira Martins\nLorena Ganzer\nThiago Nascimento Nogueira\n");
    char opt;
    PrintStringInStyle("\nEntre qualquer valor para voltar ao menu: ", "cyan" , 0);
    scanf(" %c", &opt);
    IncludeMessage = 0;
    return 0;
}

//Imprime o menu principal com a tabela principal e dá as opções
int EnterMainMenu(){
    printf("\n");
    PrintStringInStyle("Esta e a sua situacao academica no curso CC PPC 2017:\n", "blue" , 1);
    printf("\n");
    printMainList();
    if (IncludeOpt == 0) PrintStringInStyle("\nOpcoes:\n M - Mudar Status de uma disciplina\n P - Mudar status de todas as disciplinas de um periodo\n I - Inspecionar Disciplina\n O - Visualizar Optativas\n S - Salvar Perfil\n C - Carregar Perfil\n A - Sobre o Programa\n W - Mudar todos os status de cursando para aprovado\n X - Finalizar Programa\n","cyan", 0);
    else PrintStringInStyle("\nOpcoes:\n M - Mudar Status de uma disciplina\n P - Mudar status de todas as disciplinas de um periodo\n I - Inspecionar Disciplina\n O - Esconder Optativas\n S - Salvar Perfil\n C - Carregar Perfil\n A - Sobre o Programa\n W - Mudar todos os status de cursando para aprovado\n X - Finalizar Programa\n","cyan", 0);
        
    if (IncludeMessage) PrintStringInStyle("\nAlteracoes feitas com sucesso\n", "green", 0);
    char cmd;
    while (1){ //Loop infinito que dura enquanto o usuario não entrar um comando válido
        printf("Opcao: ");
        scanf(" %c", &cmd);
        if (cmd == 'M' || cmd == 'm'){
            if (CmdChangeSingleDiscipline() == 0) break;
        } else if (cmd == 'p' || cmd == 'P'){
            if (CmdChangeWholeSemester() == 0) break;
        }else if (cmd == 'i' || cmd == 'I'){
            if (CmdInspectdiscipline() == 0) break;
        }else if (cmd == 'o' || cmd == 'O'){
            IncludeMessage = 0;
            break;
        }else if (cmd == 's' || cmd == 'S'){
            if (CmdSaveProfile() == 0) break;
        }else if (cmd == 'a' || cmd == 'A'){
            if (CmdAbout() == 0) break;
        }else if (cmd == 'c' || cmd == 'C'){
            int a = CmdLoadProfile();
            if (a == 0) break;
            else if (a == 2) return 1;
        }else if (cmd == 'w' || cmd == 'W'){
            if (CmdChangeCursandoToAprovado() == 0) break;
        }else if (cmd == 'X' || cmd == 'x'){
            return 1;
        }else{
            PrintStringInStyle("Comando nao reconhecido, tente novamente:\n", "red", 0);
        }
    }
    if ((cmd == 'O' || cmd == 'o') && IncludeOpt == 0) IncludeOpt = 1;
    else IncludeOpt = 0;
    ClearScreen(); //Limpa a tela
    return 0;
}

int main(){
    ClearScreen(); //Limpa a tela
    SetTextStyle("def",0); //reseta a fonte para o padrão
    if (LoadDatabase() == 1) return 1; //Chama a função de ler o arquivo bd.txt, se der erro sai da aplicação
    int i = 0;
    ChangeWholeSemesterStatus(1,1); //muda todas as disciplinas do primeiro periodo para cursando, o que é o padrão antes de carregar um perfil
    UpdateBlockSituations();//Atualiza a situação das materias bloqueadas ou liberadas antes de entrar no menu
    while (i == 0){
        i = EnterMainMenu(); //enquanto o menu retornar 0, entraremos no menu novamente
    }
    PrintStringInStyle("Finalizando Programa...\n", "yellow" , 0);
    return 0;
}
