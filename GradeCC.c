#include<stdio.h>
#include<stdlib.h>
#include<locale.h>

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

/*Esta função tenta ler o arquivo csv bd.txt, que tem que estar salvo na mesma pasta que o código está, e armazena os dados das disciplinas na variável global Disciplinas.
Caso haja qualquer tipo de erro será retornado 1, caso tudo tenha ocorrido bem será retornado 0.*/
int LoadDatabase(){
    FILE *file; //variável local do tipo ponteiro para armazenar o arquivo bd.txt
    file = fopen("bd.txt","r"); //Abre o arquivo em modo de leitura
    if (file == NULL) { //Checando se o arquivo bd.txt foi encontrado
        printf("Arquivo bd.txt não encontrado, encerrando aplicação!\n");
        return 1;
    }

    int record = 0; //esta variável local será utilizada para contar quantas disciplinas lemos na tabela bd.txt
    int read = 0; //esta variável local será utilizada para contar quantas informações foram lidas corretamente em cada linha da tabela bd.txt
    //enquanto não chegar ao fim do arquivo bd.text, faça:
    do{
        char tempPrerequisite[20]; //variavel utilizada para ler a coluna pre requisitos como string
        read = fscanf(file,"%d,%65[^,],%3[^,],%19[^,],%d,%d,%d,%7[^,],%d\n",
                        &Disciplinas[record].num,
                        Disciplinas[record].name,
                        Disciplinas[record].institute,
                        tempPrerequisite,
                        &Disciplinas[record].hours,
                        &Disciplinas[record].nucleos,
                        &Disciplinas[record].type,
                        Disciplinas[record].code,
                        &Disciplinas[record].semester);
        
        if (read == 9) record++; // se nove leituras : OK, se não: erro!
        else if (!feof(file)){
            printf ("Arquivo em formato incorreto na linha %d.\n Read = %d\n", record + 1, read);
            printf ("%d,%s,%s,%s,%d,%d,%d,%s,%d\n", 
                        Disciplinas[record].num,
                        Disciplinas[record].name,
                        Disciplinas[record].institute,
                        tempPrerequisite,
                        &Disciplinas[record].hours,
                        &Disciplinas[record].nucleos,
                        &Disciplinas[record].type,
                        Disciplinas[record].code,
                        &Disciplinas[record].semester);;
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
    printf("Leitura de arquivo bem ó sucedida: %d linhas\n", record);
    return 0;   
}

int main(){
    system("cls");
    setlocale(LC_ALL,"Portuguese");
    if (LoadDatabase() == 1) return 1; //Chama a função de ler o arquivo bd.txt, se der erro sai da aplicação
    return 0;
}