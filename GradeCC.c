#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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
int Records = 1; //esta variável  global será utilizada para contar quantas disciplinas lemos na tabela bd.txt

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

/*Esta função tenta ler o arquivo csv bd.txt, que tem que estar salvo na mesma pasta que o código está, e armazena os dados das disciplinas na variável global Disciplinas.
Caso haja qualquer tipo de erro será retornado 1, caso tudo tenha ocorrido bem será retornado 0.*/
int LoadDatabase(){
    printf("Carregando banco de dados...\n");
    FILE *file; //variável local do tipo ponteiro para armazenar o arquivo bd.txt
    file = fopen("bd.txt","r"); //Abre o arquivo em modo de leitura
    if (file == NULL) { //Checando se o arquivo bd.txt foi encontrado
        printf("Arquivo bd.txt não encontrado, encerrando aplicação!\n");
        return 1;
    }

    
    int read = 0; //esta variável local será utilizada para contar quantas informações foram lidas corretamente em cada linha da tabela bd.txt
    //enquanto não chegar ao fim do arquivo bd.text, faça:
    do{
        char tempPrerequisite[20]; //variavel utilizada para ler a coluna pre requisitos como string
        read = fscanf(file,"%d,%65[^,],%3[^,],%19[^,],%d,%d,%d,%7[^,],%d\n",
                        &Disciplinas[Records].num,
                        Disciplinas[Records].name,
                        Disciplinas[Records].institute,
                        tempPrerequisite,
                        &Disciplinas[Records].hours,
                        &Disciplinas[Records].nucleos,
                        &Disciplinas[Records].type,
                        Disciplinas[Records].code,
                        &Disciplinas[Records].semester);
        
        if (read == 9) Records++; // se nove leituras : OK, se não: erro!
        else if (!feof(file)){
            printf ("Arquivo em formato incorreto na linha %d.\n Read = %d\n", Records + 1, read); //Impressão de onde apresentou erro na tabela
            printf ("%d,%s,%s,%s,%d,%d,%d,%s,%d\n", 
                        Disciplinas[Records].num,
                        Disciplinas[Records].name,
                        Disciplinas[Records].institute,
                        tempPrerequisite,
                        &Disciplinas[Records].hours,
                        &Disciplinas[Records].nucleos,
                        &Disciplinas[Records].type,
                        Disciplinas[Records].code,
                        &Disciplinas[Records].semester);;
            return 1;
        }

         
        //TO DO: transformar a string de pre requisitos (tempPrerequisite) em uma array de ints para salvar na variavel Disciplinas

        if (ferror(file)) //Se qualquer outro tipo de erro detectado com o arquivo
        {
            printf("Erro ao ler arquivo.\n");
            return 1;
        }
    }while (!feof(file));

    fclose(file); //Fecha o arquivo pois não precisaremos mais dele
    SetTextStyle("green",0);
    printf("Leitura de arquivo bem sucedida: %d linhas.\n", Records - 1);
    return 0;   
}



int PrintMainList(){
    SetTextStyle("def",1);
    printf("Num     Nome                                                              CH   Status        Situacao\n");
    int i;
    char stateName[12];
    char blockName[12];
    SetTextStyle("def",0);
    for (i = 1; i<=Records - 1; i++){
        switch (States[i])
        {
        case 0:
            strcpy(stateName,"Nao cursada");
            break;
        case 1:
            strcpy(stateName,"Cursada");
            break;
        case 2:
            strcpy(stateName,"Aprovado");
            break;
        }
        switch (Block[i])
        {
        case 0:
            strcpy(blockName,"Liberada");
            break;
        case 1:
            strcpy(blockName,"Bloqueada");
            break;
        case 2:
            strcpy(blockName,"Prox Sem");
            break;
        }

        printf ("%-2d      %-65s %-3d  %-13s %-s\n",Disciplinas[i].num, Disciplinas[i].name, Disciplinas[i].hours, stateName, blockName);
    }
    return 0;
}

int main(){
    system("cls");
    if (LoadDatabase() == 1) return 1; //Chama a função de ler o arquivo bd.txt, se der erro sai da aplicação
    PrintMainList();
    return 0;
}