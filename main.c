/**
**  SNAKE GAME MANIPULATED BY ARTIFICIAL INTELLIGENCE, MADE BY VIKTOR LENZ.
**  THE AIM FOR THIS APPLICATION IS EXECUTE A SNAKE GAME BEING CONTROLLED BY A PRE-PROGRAMMED AI,
**  THE GAME GRAPHICAL INTERFACE WAS MADE WITH SDL 2, YOU MUST HAVE THIS .dll TO RUN THE GAME ON YOUR COMPUTER.
**  COMMENTS ARE IN PORTUGUESE LANGUAGE, I WANT TO DOCUMENTATE IT IN ENGLISH LANGUAGE, BUT, FOR NOW, YOU
**  CAN TRANSLATE THE TEXT WITH A TRANSLATE APPLIANCE TO READ IT.
**
**
**  --PORTUGUESE-----------------------------------------------------------------------
**  JOGO DA COBRA MANIPULADO POR INTELIGENCIA ARTIFICIAL, FEITO POR VIKTOR LENZ.
**  O INTUITO DA APLICA��O � EXECUTAR O JOGO DA COBRA SENDO CONTROLADO POR UMA IA PR� PROGRAMADA,
**  FOI UTILIZADA A INTERFACE GR�FICA SDL 2 PARA SEU FUNCIONAMENTO, LOGO PARA EXECUTAR O JOGO NECESSITA-SE
**  DA .dll INSTALADA.
**/

//Importa��es essenciais
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> //Biblioteca fundamental para a utiliza��o de Booleanas em C
#include <time.h> //Biblioteca para manipula��o de time
#include <SDL2/SDL.h> //Biblioteca importada SDL2, utilizada para a interface gr�fica
#include <locale.h> //Biblioteca para traduzir o texto para uma codifica��o padr�o

typedef enum Direcao {RIGHT=1, LEFT, UP, DOWN} direcao;//Enumerador para indicar dire��o
typedef enum {VAZIO, COBRA, FOOD, CABECA, BORDA} Ocupacao;//Enumerador para indicar se a c�lula est� ocupada

//Declara��o de m�todos
void onInit();
void update();
bool gameOverCheck();
bool checkAte();
void spawnFoodCoord();
void posAte();
void updateCobraCoord();
void IA();
void basicMoveIA();
void setMove(direcao d);
bool checkMove(direcao d);
void render(SDL_Renderer *renderer);
void renderCobra(SDL_Renderer *renderer);
void renderCobraPixel(SDL_Renderer *renderer, short px, short py);
void renderFood(SDL_Renderer *renderer);
void renderCelula(SDL_Renderer *renderer, const short *cor, short *pos);

//declara��o de constantes (vari�veis com valor fixo)
#define WINDOW_SIZE_X 900 //DEVE SER DIVIS�VEL PELO CELULA_SIZE EM INTEIRO
#define WINDOW_SIZE_Y 600 //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#define CELULA_SIZE 15 //TAMANHO DE CADA C�LULA
#define DELAY 32 //DELAY DEFINIDO PARA CADA ATUALIZA��O DO JOGO
const short BACKGROUND_COLOR [3] = {10,10,55}; //VETOR QUE DEFINE A COR DE FUNDO DO JOGO
const short COBRA_COLOR [3] = {107, 50, 107}; //VETOR QUE DEFINE A COR DA COBRA
const short FOOD_COLOR [3] = {100,150,100}; //VETOR QUE DEFINE A COR DA COMIDA


//Structs para auxiliar no c�digo
struct CelulaTela{ //Struct para indicar cada c�lula da tela
    Ocupacao ocupado;
};

struct Tela{ //Struct para representar a tela
    //Dentro da mesma temos uma matriz de c�lulas, seu tamanho � proporcional ao tamnho da tela dividida pelo tamanho
    //das c�lulas
    struct CelulaTela celula[WINDOW_SIZE_X/(CELULA_SIZE/2)][WINDOW_SIZE_Y/(CELULA_SIZE/2)];
};

struct CelulaCobra{//Indica uma c�lula da cobra
    short x, y;
};

struct Cobra{//Indica a estrutura da cobra
    short x,y, size; //x e y nese caso � o espa�o onde se localiza a cabe�a
    struct CelulaCobra celula [(WINDOW_SIZE_X*WINDOW_SIZE_Y)/CELULA_SIZE]; //Quantidade de espa�o reservado em vetor para a cobra "caber" na programa��o
};

struct Food{//� a comida da cobra
    short x,y;
};

static struct Tela tela;    //DECLARA��O DAS STRUCTS PARA SEREM UTILIZADAS
static struct Cobra cobra;
static struct Food food;
bool running; //Booleana que define se o ciclo da aplica��o ainda est� em ativa
short x, y; //Define para onde ir� a cobra

/**
int main � o m�todo principal da linguagem C, no caso vai inicializar o SDL e sua interface gr�fica, al�m de iniciar
o ciclo do jogo, at� ser encerrado.
int argc e char** argv s�o argumentos imperativos para a execu��o do SDL
*/
int main(int argc, char** argv)
{
    //Indica que o texto desse programa � em portugu�s, essencial para rodar caracteres especiais(^~�)
    setlocale(LC_ALL,"Portuguese");

    // Inicializa o SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Comando SDL para declarar a janela gr�fica, e define sua posi��o inicial e tamanho
    SDL_Window* window = SDL_CreateWindow("Jogo da Cobra com IA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_SIZE_X-CELULA_SIZE, WINDOW_SIZE_Y-CELULA_SIZE, 0);// -CELULA_SIZE pois as bordas n�o s�o desenhadas
    //Cria o renderizador gr�fico
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //Vari�vel de evento para o SDL
    SDL_Event event;

    //Vari�veis para manipula��o do tempo, no caso o updateTime vai atualizar o jogo a cada vez que passar o tempo de DELAY
    Uint32 old_time = 0, updateTime = DELAY, new_time;
    running = true; //Avisando que o jogo est� ligado
    onInit(); //Chama um procedimento para iniciar o estado do jogo

    while (running) //Enquanto estiver rodando...
    {
        //Verifica varios tipos de eventos (Teclado, Mouse, toque, etc)
        while (SDL_PollEvent(&event))//enquanto tiver evento
        {
            if (event.type == SDL_KEYDOWN) //Se o evento for o toque de uma tecla
            {
                //Declara o ponteiro de uma char que ser� igual a chave que foi teclada no momento
                const char* key = SDL_GetKeyName(event.key.keysym.sym);
                //Nota: strcmp � uma fun��o que compara se duas strings s�o iguais
                if (strcmp(key, "Q") == 0) //Se a chave for igual "Q"
                    running = false; //O jogo ser� desligado
                else if (strcmp(key, "Escape") == 0) //tecla "Esc"
                    running = false; //O jogo ser� desligado
//Deixei a possibilidade de se movimentar independentemente da AI, entretanto, enquanto o m�todo AI estiver ligado, vai sobreescrever seus comandos
                else if(strcmp(key,"D") == 0)
                    setMove(RIGHT);
                else if(strcmp(key,"A") == 0)
                    setMove(LEFT);
                else if(strcmp(key,"W") == 0)
                    setMove(UP);
                else if(strcmp(key,"S") == 0)
                    setMove(DOWN);
            }
            else if (event.type == SDL_QUIT) //Se a janela gr�fica for fechada
            {
                running = false; //O jogo ser� desligado
            }
        }

        new_time = SDL_GetTicks(); //Temporiza o tempo
        if(new_time - old_time > updateTime) //Verifica se o tempo � superior a atualiza��o de DELAY
        { //Se passou do Delay
            update();//Atualiza
            render(renderer);//Renderiza
            old_time = new_time; //Tempo antigo ser� igual o tempo atual no qual passou o tempo de DELAY
        }
    }
    //P�s sair do ciclo de jogo ativo


    // Destr�i a interface gr�fica
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
/**
Provedimento de inicializa��o do jogo, inicializa as vari�veis necess�rias para execu��o do jogo
*/
void onInit(){
    int j,k; //Ind�ces para fazer uma matriz de repeti��o
    //Pega todas as c�lulas poss�veis para estata-las como vazias.
    for (j=0;j<=(WINDOW_SIZE_X/CELULA_SIZE);j++){
        for(k=0;k<=(WINDOW_SIZE_Y/CELULA_SIZE);k++){
            if((j==0||k==0)||(j==WINDOW_SIZE_X/CELULA_SIZE)||(k==WINDOW_SIZE_Y/CELULA_SIZE))
                tela.celula[j][k].ocupado = BORDA;
            else
                tela.celula[j][k].ocupado = VAZIO; //VAZIO � o estado dado devido ao enumerador "Ocupacao"
        }
    }

    srand(time(NULL)); //Declara a fun��o de randomiza��o da linguagem C
    x = CELULA_SIZE; //Movimento padr�o inicial para a direita, nota-se que todo movimento ser� igual de uma c�lula
    y = 0; //Move-se apenas para uma dire��o, logo o movimento aqui pode ser nulo mesmo
    //abaixo coloca-se a posi��o inicial da cobra
    cobra.x = (rand() % (WINDOW_SIZE_X/CELULA_SIZE-2)+2)*CELULA_SIZE;//rand() = Express�o para randomizar um n�mero
    cobra.y = (rand() % (WINDOW_SIZE_Y/CELULA_SIZE-2)+2)*CELULA_SIZE;// depois de % ter� o n�mero m�ximo no qual o rand poder� gerar
    printf("Posi��o X da Cobra: %d\n Posi��o Y da Cobra: %d\n\n", cobra.x, cobra.y); //Logs sempre s�o bons para ajudar em tratamento de bugs
    cobra.size = 3; //tamanho da cobra
    short i = 0;
    while (i<cobra.size){//Preenchendo a cauda da cobra, alocando cada pixel c�lula atr�s de c�lula assim por diante
        cobra.celula[i].x = cobra.x-(i+1)*CELULA_SIZE;
        cobra.celula[i].y = cobra.y;
        i++;
    }
    spawnFoodCoord(); //Procedimento para surgir a primeira comida do jogo

}
/**
Procedimento demarcado para atualizar as vari�veis de jogo
*/
void update(){
    running = gameOverCheck(); //Verifica se aconteceu as condi��es de derrota
    if (running) //Caso esteja rodando, atualiza a coordenada da cobra
        updateCobraCoord();
}

/**
Fun��o de verifica��o de derrota, caso uma das condi��es sejam encaixadas, game over.
return retorna true para que n�o houve Game Over e false para caso o jogo tenha que se encerrar
*/
bool gameOverCheck(){
    //Se a celula onde a cabe�a da cobra se localiza j� est� ocupado por uma cauda
    if(tela.celula[cobra.x/CELULA_SIZE][cobra.y/CELULA_SIZE].ocupado==COBRA||tela.celula[cobra.x/CELULA_SIZE][cobra.y/CELULA_SIZE].ocupado==BORDA){
        printf("Caiu no rabo ou borda\n");
        return false;
        }
return true;
}

/**
Fun��o que verifica se a cabe�a da cobra est� no mesmo local que a comida, declarando que foi comida
struct CelulaTela celula � a c�lula que vai ser inspecionada, no caso deve-se acionar a fun��o com a c�lula
na qual a cabe�a da cobra se localiza.
return true caso a condi��o seja verdade
*/
bool checkAte(struct CelulaTela celula){
    if(celula.ocupado == FOOD){
            printf("FOOD COMIDA!\n\n");
        return true;
    }
    return false;
}
/**
Procedimento para gerar a c�lula de comida, gera em um espa�o aleat�rio.
*/
void spawnFoodCoord(){
    //Dentro do do para garantir que n�o choque uma comida dentro da cobra
    do{
        food.x = (rand() % WINDOW_SIZE_X/CELULA_SIZE)*CELULA_SIZE;
        food.y = (rand() % WINDOW_SIZE_Y/CELULA_SIZE)*CELULA_SIZE;
    }
    while((tela.celula[food.x/CELULA_SIZE][food.y/CELULA_SIZE].ocupado == COBRA)||(tela.celula[food.x/CELULA_SIZE][food.y/CELULA_SIZE].ocupado == BORDA));
    tela.celula[food.x/CELULA_SIZE][food.y/CELULA_SIZE].ocupado = FOOD;//Estatar como c�lula comida ao espa�o designado
    printf("COORDENADA X: %d\n COORDENADA Y:%d\n", food.x, food.y);//Logs s�o sempre �teis :D
    printf("ESTADO DA CELULA: %d\n\n", tela.celula[food.x/CELULA_SIZE][food.y/CELULA_SIZE].ocupado);
}

/**
Procedimento de a��o p�s uma comida ser atingida, a cobra ganha 1 de tamanho
*/
void posAte(){
    cobra.size++;
    cobra.celula[cobra.size-1].x = cobra.x;//Posicionando a c�lula nova
    cobra.celula[cobra.size-1].y = cobra.y;
    spawnFoodCoord(); //Spawnando a nova comida
}

/**
Procedimento para atualizar as coordenadas da cobra, no caso, movimenta-la.
 */
void updateCobraCoord(){
    IA(); //Acionar intelig�ncia artificial
    //De acordo com processos do procedimento IA, cobra.x e cobra.y v�o receber o valor de o movimento
    //em uma c�lula (c�digo atual precisa de reestrutura��o)
    cobra.x += x;
    cobra.y += y;
    short i = 0; //�ndice
    while(i<cobra.size-1){//movendo a cauda da cobra
        cobra.celula[i].x = cobra.celula[i+1].x;
        cobra.celula[i].y = cobra.celula[i+1].y;
        i++;
    }
    cobra.celula[i].x = cobra.x;//Movendo por fim a cabe�a da cobra
    cobra.celula[i].y = cobra.y;//--------------------------------
    tela.celula[cobra.celula[i-1].x/CELULA_SIZE][cobra.celula[i-1].y/CELULA_SIZE].ocupado = COBRA;
    if(checkAte(tela.celula[cobra.x/CELULA_SIZE][cobra.y/CELULA_SIZE]))//Se a c�lula na qual a cobra moveu era comida
        posAte();
    tela.celula[cobra.celula[0].x/CELULA_SIZE][cobra.celula[0].y/CELULA_SIZE].ocupado = VAZIO;//A ultima c�lula do rabo de move, deixando o espa�o vazio

    if(tela.celula[cobra.x/CELULA_SIZE][cobra.y/CELULA_SIZE].ocupado==VAZIO||tela.celula[cobra.x/CELULA_SIZE][cobra.y/CELULA_SIZE].ocupado==FOOD) //Se for cobra ou borda o local onde a cabe�a se mover
        tela.celula[cobra.x/CELULA_SIZE][cobra.y/CELULA_SIZE].ocupado = CABECA;//� game over, o estado permanece como cobra, para prevenir bugs
}

/**
Procedimento de IA, atualmente leva apenas para outro procedimento
*/
void IA(){
    basicMoveIA();
}
/**
Procedmento para mover via IA, � analisado a dist�ncia mais pr�xima da comida, e � verificado
se a a��o o levar� a morte, no caso translocando para outra dire��o
*/
void basicMoveIA(){
    short xDist = cobra.x - food.x;//Analisar o mais pr�ximo
    short yDist = cobra.y - food.y;



    if(abs(xDist)>abs(yDist)){ //SE A DIST�NCIA HORIZONTAL FOR MAIOR QUE A DIST�NCIA VERTICAL
       if (xDist <= 0){ //SE A DIST�NCIA HORIZONTAL FOR NEGATIVA
            if(checkMove(RIGHT)){
                setMove(RIGHT);
            }
            else if(checkMove(UP)){
                setMove(UP);
            }
            else if(checkMove(DOWN)){
                setMove(DOWN);
            }
            else{
                setMove(LEFT);
            }

        }
        else{ //SE A DIST�NCIA FOR POSITIVA(x da cobra ser maior que x da comida)
            if(checkMove(LEFT)){
                setMove(LEFT);
            }
            else if(checkMove(UP)){
                setMove(UP);
            }
            else if(checkMove(DOWN)){
                setMove(DOWN);
            }
            else{
                setMove(RIGHT);
            }
        }
    }
    else{ //SE A COBRA ESTIVER MAIS LONGE VERTICALMENTE, DO QUE HORIZONTALMENTE
        if (yDist <= 0){ //SE A DIST�NCIA VERTICAL FOR NEGATIVA
            if(checkMove(DOWN)){
                setMove(DOWN);
            }
            else if(checkMove(LEFT)){
                setMove(LEFT);
            }
            else if(checkMove(RIGHT)){
                setMove(RIGHT);
            }
            else{
                setMove(UP);
            }
        }
        else{
            if(checkMove(UP)){
                setMove(UP);
            }
            else if(checkMove(LEFT)){
                setMove(LEFT);
            }
            else if(checkMove(RIGHT)){
                setMove(RIGHT);
            }
            else{
                setMove(DOWN);
            }
        }
    }

}

/**
Define o movimento da cobra.
direcao d � a enum para verificar o movimento
*/
void setMove(direcao d){
    //Nota: A esse ponto deve-se ficar mais que claro que X � horizontal e Y vertical
    switch (d){
    case RIGHT:
        y=0;
        x = CELULA_SIZE;
        break;
    case LEFT:
        y=0;
        x = -CELULA_SIZE;
        break;
    case UP:
        x=0;
        y = -CELULA_SIZE;
        break;

    case DOWN:
        x=0;
        y = CELULA_SIZE;
        break;
    }
}

/**
Procedimento para conferir se o movimento n�o levar� a fim de jogo
direcao d � a dire��o designada
bool � true ou false para verificar se o movimento � poss�vel
*/
bool checkMove(direcao d){
    bool retorno = false;
    //Cada caso verifica se a c�lula a se locomover est� ocupada
    switch (d){
    case RIGHT:
        if(((tela.celula[(cobra.x+CELULA_SIZE)/CELULA_SIZE][(cobra.y)/CELULA_SIZE].ocupado)!=COBRA)&&((tela.celula[(cobra.x+CELULA_SIZE)/CELULA_SIZE][(cobra.y)/CELULA_SIZE].ocupado)!=BORDA))
                retorno = true;
    break;
    case LEFT:
        if(((tela.celula[(cobra.x-CELULA_SIZE)/CELULA_SIZE][(cobra.y)/CELULA_SIZE].ocupado)!=COBRA)&&((tela.celula[(cobra.x-CELULA_SIZE)/CELULA_SIZE][(cobra.y)/CELULA_SIZE].ocupado)!=BORDA))
            retorno = true;
    break;
    case UP:
        if(((tela.celula[(cobra.x)/CELULA_SIZE][(cobra.y-CELULA_SIZE)/CELULA_SIZE].ocupado)!=COBRA)&&((tela.celula[(cobra.x)/CELULA_SIZE][(cobra.y-CELULA_SIZE)/CELULA_SIZE].ocupado)!=BORDA))
            retorno = true;
    break;
    case DOWN:
        if(((tela.celula[(cobra.x)/CELULA_SIZE][(cobra.y+CELULA_SIZE)/CELULA_SIZE].ocupado)!=COBRA)&&((tela.celula[(cobra.x)/CELULA_SIZE][(cobra.y+CELULA_SIZE)/CELULA_SIZE].ocupado)!=BORDA))
            retorno = true;
    break;
    }
    return retorno;
}

/**
Procedimento para renderizar os gr�ficos
SDL_Renderer *renderer � um ponteiro do renderizador da biblioteca SDL
*/
void render(SDL_Renderer *renderer){
    //renderizando o plano de fundo
    SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR[0], BACKGROUND_COLOR[1], BACKGROUND_COLOR[2] , 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 200, 30, 50 , 255);
    //Renderizando a c�lula de comida
    renderFood(renderer);
    //Renderizando a cobra
    renderCobra(renderer);
    SDL_RenderPresent(renderer);
}

/**
Procedimento que renderiza a cobra
SDL_Renderer *renderer � o renderizador da biblioteca SDL
 */
void renderCobra(SDL_Renderer *renderer){
    short i = 0;
    while(i<cobra.size){ //Pintando cada c�lula
        renderCobraPixel(renderer, cobra.celula[i].x, cobra.celula[i].y); //Renderizar a c�lula espec�fica da cobra
        i++;
    }
}

/**
Procedimento que renderiza as c�lulas da cobra, renderiza-se ret�ngulo por ret�ngulo
SDL_Renderer *renderer � o renderizador da biblioteca SDL
 */
void renderCobraPixel(SDL_Renderer *renderer, short px, short py){
    short pos[] = {px, py, CELULA_SIZE, CELULA_SIZE};
    renderCelula(renderer, COBRA_COLOR, pos);
}

/**
Procedimento que renderiza a c�lula da comida
SDL_Renderer *renderer � o renderizador da biblioteca SDL
 */
void renderFood(SDL_Renderer *renderer){
    short pos[] = {food.x, food.y, CELULA_SIZE, CELULA_SIZE};
    renderCelula(renderer, FOOD_COLOR, pos);
}

/**
Procedimento para renderizar as c�lulas do tabuleiro, ajusta o gr�fico em rela��o a falta de bordas
(as bordas n�o s�o impressas graficamente, ficam fora da interface gr�fica)
SDL_Renderer *renderer � um ponteiro do renderizador da biblioteca SDL
const  short *cor � um ponteiro de um vetor de 3 n�meros at� 255 para definir a cor da c�lula em RGB
short *pos � um ponteiro da posi��o a ser desenhada a c�lula
*/
void renderCelula(SDL_Renderer *renderer, const short *cor, short *pos){
    SDL_SetRenderDrawColor(renderer, cor[0], cor[1], cor[2] , 255);
    SDL_Rect rect = {pos[0]-CELULA_SIZE, pos[1]-CELULA_SIZE, pos[2], pos[3]};
    SDL_RenderFillRect(renderer, &rect);
}
