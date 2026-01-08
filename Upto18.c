#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


struct BSTnode {
    char *word;
    struct BSTnode *left;
    struct BSTnode *right;
    
};


struct gameVincolo {
    char *greenPattern;
    
    int vincoliPerChar[128];

    
    char *redPattern;

    unsigned char lastVincolo[64*3*66];
};

struct nodoFiltrata{
    char* s;
    struct nodoFiltrata *next;
};

struct nodoFiltrata *listaFiltrate;


struct BSTnode *rootDizionario;
//struct RBnode tNIL;

struct gameVincolo *vincolo;


int numParole = 0;
unsigned int lunghezzaParole;
bool abo = false;
char currentCommand = '?';

char *text;

int validCharSet[64];
char missCharSet[64];


void getStringFromLine(char *s) {
    char c;
    int i = 0;
    c = getchar_unlocked();
    if (c == '+') {
        s[0] = c;
        c = getchar_unlocked();
        s[1] = c;

        /*
         *
         *      +i    ---> inserisci_inizio
         *      +i    ----> inserisci_fine
         *      +n    ----> nuova_partita
         *      +s    ----> stampa_filtrate
         *
         */

        while (c != '\n' && c != EOF)
            c = getchar_unlocked();


    } else {

        do {
            s[i] = c;
            c = getchar_unlocked();
            i++;

        } while (c != '\n' && c != EOF);

    }

    if (c == EOF)
        abo = true;

    //sostituisco \n con \0
    //s[i - 1] = '\0';
}

char mapIntegerToChar(int p) {
    if (p <= 9)
        return p + 48;
    if (p >= 10 && p <= 35)
        return p + 55;
    if (p >= 36 && p <= 61)
        return p + 61;
    if (p == 62)
        return 45;

    return 95;
}

int mapCharToInteger(char c) {
    if (c == 95)
        return 63;
    if (c == 45)
        return 62;
    if (c >= 36 + 61 && c <= 61 + 61)
        return c - 61;
    if (c >= 10 + 55 && c <= 35 + 55)
        return c - 55;
    if (c <= 9 + 48)
        return c - 48;
    return 63;
}

void copyString(char *a, const char *b) {
    for (int i = 0; i < lunghezzaParole; i++) {
        a[i] = b[i];
    }
}

//implementazione di strCompare

//si potrebbe utilizzare tranquillamente anche strcmp() in string.h
int strCompare(const char *a, const char *b) {
    //-1: parola 1 è maggiore
    //0: parole uguali
    //1: parole 2 è maggiore
    for (int i = 0; i < lunghezzaParole; ++i) {
        if (a[i] < b[i])
            return 1;
        if (a[i] > b[i])
            return -1;
    }
    return 0;
}


//stampa di una stringa utilizzando putChar()
void myPrintString(const char *s, unsigned int lungh) {
    for (int i = 0; i < lungh; i++) {
        putchar_unlocked(s[i]);
    }
    putchar_unlocked('\n');
}


void initializeDictionary();


void updateDictionary();
void updateDictionaryDuringGame(int *numAmmis);

void handlePartita();

void initializeVincoli();

void resetVincoli();


bool processWord(char *rif, char *p, char *output);

void updateVincolo(char *parola, char *output);


bool rispettaVincoli(char *parola);
bool rispettaLastVincolo(char *parola);

void stampaVincolo();
void stampaUltimoVincolo();



void insertIntoDictionary(char *text);


void stampaTree(struct BSTnode *T);

bool wordExistsInDictionary(char *s, unsigned int lungh);






/**
 *
 * -----------------------------
 * INIZIO IMPLEMENTAZIONE METODI
 * -----------------------------
 */

void inserisciFiltrataInTesta(char *parola) {

    struct nodoFiltrata *newNode = malloc(sizeof(struct nodoFiltrata));
    newNode->s = parola;
    if (listaFiltrate == NULL) {
        listaFiltrate = newNode;
        newNode->next = NULL;

    } else {
        newNode->next = listaFiltrate;
        listaFiltrate = newNode;
    }

}

void inserisciFiltrataInOrdine( char *parola){
    struct nodoFiltrata *newNode = malloc(sizeof(struct nodoFiltrata));
    newNode->s = parola;
    newNode->next = NULL;
    //newNode->s = parola;

    if (listaFiltrate == NULL) {
        listaFiltrate = newNode;
        return;
    }

    struct nodoFiltrata *prec = NULL;
    struct nodoFiltrata *curr = listaFiltrate;


    //se sostituisc strcmp devi cambaire anche il segno

    while (curr != NULL && strCompare(parola, curr->s) < 0) {
        prec = curr;
        curr = curr->next;
    }
    //struct nodoFiltrata *newNode = (struct *nodoFiltrata) malloc(sizeof(word));


    if (prec == NULL) {
        newNode->next = listaFiltrate;
        listaFiltrate = newNode;
    } else {
        prec->next = newNode;
        newNode->next = curr;
    }
}

void freeList(){


    struct nodoFiltrata *temp;


    while (listaFiltrate != NULL) {
        temp = listaFiltrate;
        listaFiltrate = listaFiltrate->next;
        free(temp);
    }

    listaFiltrate=NULL;

}



void aggiungiAFiltrate(int *count, struct BSTnode *T){


    if (T == NULL)
        return;
    else {
        aggiungiAFiltrate(count, T->right);

        if (rispettaVincoli(T->word)) {
            //myPrintString(T->word, lunghezzaParole);

            *count = *count +1;
            inserisciFiltrataInTesta( T->word);

        }


        aggiungiAFiltrate(count,  T->left);
    }
}


void initializeFiltrate( struct BSTnode *T){
    if (T == NULL)
        return;
    else {
        initializeFiltrate(T->right);



        inserisciFiltrataInTesta(T->word);



        initializeFiltrate(T->left);
    }
}
void initializeFiltrateConVincoli( struct BSTnode *T , int *numAmmis){

    if (T == NULL)
        return;
    else {
        initializeFiltrateConVincoli(T->right, numAmmis);


        if(rispettaLastVincolo(T->word)){
            inserisciFiltrataInTesta(T->word);
            *numAmmis = *numAmmis + 1;
        }



        initializeFiltrateConVincoli(T->left, numAmmis);
    }
}
void updateFiltrate(int *count, struct BSTnode *T){
    if(listaFiltrate==NULL){
        //se la lista è vuota percorri tutto l'albero e visita qua
        *count=0;
        aggiungiAFiltrate(count, T);

    }else{
        /*
        struct nodoFiltrata *prec = NULL;
        struct nodoFiltrata *curr =listaFiltrate;





        //sono quante ne elimino
        int i = 0;


        //scorri la lista e elimina quelli che non rispettano i vinxoli

        while(curr!=NULL){
            if (rispettaVincoli(curr->s)) {
                prec = curr;
                curr = curr->next;
                if(curr==NULL)
                    return;

            }
            else{
                if(prec==NULL)
                    listaFiltrate= curr->next;
                else
                    prec->next = curr->next;

                free(curr);
                i++;
                if(prec!=NULL)
                    curr = prec->next;


            }
        }

        *count = *count - i;

*/

        int i = 0;

        struct nodoFiltrata *prec = NULL;
        struct nodoFiltrata *curr = listaFiltrate;

        do {
            if (curr != NULL && rispettaVincoli(curr->s)) {
                prec = curr;
                curr = curr->next;

            } else {
                if (curr == NULL) {
                    *count = *count -i;
                    return;
                    printf("Impossibile passare da qui!");

                } else {
                    if (prec != NULL) {

                        //non verificato
                        prec->next = curr->next;
                        free(curr);
                        curr= prec->next;

                    }
                    else {
                        /*free(filtrate);
                        filtrate = curr->next;
                        curr=filtrate;*/


                        curr= curr->next;
                        free(listaFiltrate);
                        listaFiltrate = curr;
                    }
                    i++;
                }
            }

            //stampaVincolo();
            //fflush(stdout);
        } while (i < *count);

        *count = *count -i;
    }

}
void updateFiltrateOnlyLastVincolo(int *count, struct BSTnode *T){
    if(listaFiltrate==NULL){
        //se la lista è vuota percorri tutto l'albero e visita qua
        *count=0;
        aggiungiAFiltrate(count, T);

    }else{
        /*
        struct nodoFiltrata *prec = NULL;
        struct nodoFiltrata *curr =listaFiltrate;





        //sono quante ne elimino
        int i = 0;


        //scorri la lista e elimina quelli che non rispettano i vinxoli

        while(curr!=NULL){
            if (rispettaVincoli(curr->s)) {
                prec = curr;
                curr = curr->next;
                if(curr==NULL)
                    return;

            }
            else{
                if(prec==NULL)
                    listaFiltrate= curr->next;
                else
                    prec->next = curr->next;

                free(curr);
                i++;
                if(prec!=NULL)
                    curr = prec->next;


            }
        }

        *count = *count - i;

*/

        int i = 0;

        struct nodoFiltrata *prec = NULL;
        struct nodoFiltrata *curr = listaFiltrate;

        do {
            if (curr != NULL && rispettaLastVincolo(curr->s)) {
                prec = curr;
                curr = curr->next;

            } else {
                if (curr == NULL) {
                    *count = *count -i;
                    return;
                    printf("Impossibile passare da qui!");

                } else {
                    if (prec != NULL) {

                        //non verificato
                        prec->next = curr->next;
                        free(curr);
                        curr= prec->next;

                    }
                    else {
                        /*free(filtrate);
                        filtrate = curr->next;
                        curr=filtrate;*/


                        curr= curr->next;
                        free(listaFiltrate);
                        listaFiltrate = curr;
                    }
                    i++;
                }
            }

            //stampaVincolo();
            //fflush(stdout);
        } while (i < *count);

        *count = *count -i;
    }
}
















void initializeDictionary() {

    //char temp[lunghezzaParola+1];
    //char *temp;


    //char temp[lunghezzaParole+1];


    /*while(!abo){

    }*/

    do {
        getStringFromLine(text);

        if (text[0] == '+') {
            if (text[1] == 'i') //può essere solo inserisci_inizio
                updateDictionary();
            else {

                //termino non appena arriva un comando +nuova_partita

                currentCommand = text[1];
                return;
            }

        } else {

            //addWordToDictionary(temp);
            char *nuovaParola = malloc(lunghezzaParole * sizeof(char));
            copyString(nuovaParola, text);
            insertIntoDictionary(nuovaParola);

            numParole++;


            //inserisciFiltrataInOrdine(nuovaParola);

        }

        //}while(!abo);
    } while (1);

}

void updateDictionary() {


    //char temp[lunghezzaParole+1];


    /*while(!abo){

    }*/


    do {
        getStringFromLine(text);

        if (text[0] == '+') {

            //puo essere solo inserisci_fine
            currentCommand = '?';
            return;
        } else {


            char *nuovaParola = malloc(lunghezzaParole * sizeof(char));
            copyString(nuovaParola, text);
            insertIntoDictionary(nuovaParola);

            numParole++;



            //inserisciFiltrataInOrdine(nuovaParola);


        }

        //}while(!abo);
    } while (1);




    //}while(!abo);
}

void updateDictionaryDuringGame(int *numAmmis) {


    //char temp[lunghezzaParole+1];


    /*while(!abo){

    }*/


    do {
        getStringFromLine(text);

        if (text[0] == '+') {

            //puo essere solo inserisci_fine
            currentCommand = '?';
            return;
        } else {


            char *nuovaParola = malloc(lunghezzaParole * sizeof(char));
            copyString(nuovaParola, text);
            insertIntoDictionary(nuovaParola);

            numParole++;


            if(rispettaVincoli(nuovaParola)) {
                inserisciFiltrataInOrdine(nuovaParola);
                *numAmmis = *numAmmis +1;
            }


        }

        //}while(!abo);
    } while (1);




    //}while(!abo);
}


void initializeVincoli() {
    vincolo = (struct gameVincolo *) malloc(sizeof(struct gameVincolo));
    vincolo->greenPattern = (char *) malloc((lunghezzaParole) * sizeof(char));

    //vincolo->vincoliPerChar = (struct rule *) malloc(64 * sizeof(struct rule));


    /*
     * Ho:
     * c---> caratter
     * j---> posizione nella parola
     * accedo come una matrice con redPattern[mapCharToInteger(c)][j]=  redPattern(mapCharToInteger(c)+ 64*j)
     */
    vincolo->redPattern = (char *) malloc(64 * lunghezzaParole * sizeof(char));


    resetVincoli();

}

void resetVincoli() {

    memset((vincolo->greenPattern),'?',lunghezzaParole);
    memset((vincolo->redPattern),'C',64*lunghezzaParole);

    for (int i = 0; i < 128; i++) {
        //(vincolo->vincoliPerChar)[i].carattere = mapIntegerToChar(i);
        vincolo->vincoliPerChar[i]=0;

        /*
        for (int j = 0; j < lunghezzaParole; j++) {
            (vincolo->redPattern)[i + 64 * j] = 'C'; //significa che può esserci
        }

         */

    }

}


bool processWord(char *rif, char *p, char *output) {

    int i = 0;
    bool v = true;


    for (int i = 0; i < 64; i++) {
        validCharSet[i]=0;
    }
//{inizializza array a 0}

    do {
        if (rif[i] == p[i]){
            output[i] = '+';
        }
        else {
            output[i] = '?';
            validCharSet[mapCharToInteger(rif[i])]++;
            v = false;
        }

        i++;
    } while (i < lunghezzaParole);

    i=0;
    do{
        if (output[i] == '?') {
            char c = p[i];
            int charMappato = mapCharToInteger(c);
            int rifCount = validCharSet[charMappato];

            if (rifCount <= 0) {
                output[i] = '/';
            }
            else{
                output[i] = '|';
                validCharSet[charMappato]--;
            }
        }



        i++;
    } while (i<lunghezzaParole);

    return v;


}


void updateVincolo(char *parola, char *output) {




    for (int i = 0; i < 64; i++) {
        validCharSet[i] = 0;
    }
    memset(missCharSet, 'F',64);

    int testina=0;
    //memset(vincolo->lastVincolo, '?', 64*4*100);



    for (int j = 0; j < lunghezzaParole; ++j) {

        int charMappato = mapCharToInteger(parola[j]);

        if (output[j] == '+') {
            if((vincolo->greenPattern)[j] == '?'){

                //la posizione 3 è la 4° posizione
                //il carattere z è assicurato in posizione 3   {+}{3}{z}{)}

                vincolo->lastVincolo[testina]='+';
                vincolo->lastVincolo[testina+1]= j+48 ;
                vincolo->lastVincolo[testina+2]= parola[j];


                testina+=3;
            }
            (vincolo->greenPattern)[j] = parola[j];
            validCharSet[charMappato]++;
            /*
            for(int k=0;k<64;k++){
                if(k!= mapCharToInteger(parola[j]))
                    (vincolo->redPattern)[mapIntegerToChar(parola[j]) + 64 * j] = 'F';
            }
             */
        } else if (output[j] == '/') {

            missCharSet[charMappato] = 'T';

            if((vincolo->redPattern)[charMappato + 64 * j] == 'C'){

                //la posizione 3 è la 4° posizione
                //il carattere z è assicurato in posizione 3   {+}{3}{z}{)}

                vincolo->lastVincolo[testina]='/';
                vincolo->lastVincolo[testina+1]= j+48 ;
                vincolo->lastVincolo[testina+2]= parola[j];


                testina+=3;
            }
            //perchè restrittivo
            (vincolo->redPattern)[charMappato + 64 * j] = 'F';




        } else /* (output[j]=='|') */{

            if((vincolo->redPattern)[charMappato + 64 * j] == 'C'){

                //la posizione 3 è la 4° posizione
                //il carattere z è assicurato in posizione 3   {+}{3}{z}{)}

                vincolo->lastVincolo[testina]='/';
                vincolo->lastVincolo[testina+1]= j+48 ;
                vincolo->lastVincolo[testina+2]= parola[j];
                //vincolo->lastVincolo[testina+3]=')';

                testina+=3;
            }

            //perchè restrittivo
            (vincolo->redPattern)[charMappato + 64 * j] = 'F';

            validCharSet[charMappato]++;
        }

    }
    for (int j = 0; j < lunghezzaParole; ++j) {

        int charMappato = mapCharToInteger(parola[j]);
        bool soEsatto=false;

        if (missCharSet[charMappato]=='T') {

            //si sa il numero esatto!


                //la posizione 3 è la 4° posizione
                //il carattere z è assicurato in posizione 3   {+}{3}{z}{)}





            if (validCharSet[charMappato] > 0 &&
                validCharSet[charMappato] > vincolo->vincoliPerChar[2*charMappato+1]) {

                vincolo->vincoliPerChar[2*charMappato+1] = validCharSet[charMappato];



            }
            if(vincolo->vincoliPerChar[2*charMappato] == 0) {
                vincolo->lastVincolo[testina] = 'E';
                vincolo->lastVincolo[testina + 1] = mapCharToInteger(parola[j]);
                vincolo->lastVincolo[testina + 2] = validCharSet[charMappato]+48;

                testina += 3;
            }
            vincolo->vincoliPerChar[2*charMappato] = 1;



        soEsatto=true;
        }

        if (validCharSet[charMappato] > vincolo->vincoliPerChar[2*charMappato+1]) {
            if(!soEsatto){
                vincolo->lastVincolo[testina] = 'M';
                vincolo->lastVincolo[testina + 1] = mapCharToInteger(parola[j]);
                vincolo->lastVincolo[testina + 2] = validCharSet[charMappato]+48;

                testina += 3;
            }
            vincolo->vincoliPerChar[2 * charMappato + 1] = validCharSet[charMappato];
        }









        vincolo->lastVincolo[testina]='\0';
    }

}


void stampaFiltrate() {
    struct nodoFiltrata *temp = listaFiltrate;
    while(temp != NULL){
        myPrintString(temp->s, lunghezzaParole);
        temp = temp->next;
    }

}
void stampaTutteLeParole(struct BSTnode *T){
    if (T == NULL)
        return;
    else {
        stampaTutteLeParole(T->left);

        myPrintString(T->word, lunghezzaParole);

        stampaTutteLeParole(T->right);
    }

}

//non è gestito il caso in cui sono possibili commandi prima e tra la parola di riferimento e il numero massimo di tentativi
void handlePartita() {


    bool senzaVincolo=true;
    int numAmmis = numParole;


    char rif[lunghezzaParole];

    int tentativiRimasti = 0;


    //char text[lunghezzaParole];
    char output[lunghezzaParole];

    getStringFromLine(rif);
    if (scanf("%d", &tentativiRimasti)) {}
    char r = getchar_unlocked();
    if (r == 0) {}
    do {

        if (tentativiRimasti == 0) {
            fputs("ko\n", stdout);
            currentCommand = '?';
            return;
        }

        getStringFromLine(text);

        if (text[0] == '+') {
            if (text[1] == 'n') {
                currentCommand = 'n';
                return;
            } else if (text[1] == 'i') {
                if(!senzaVincolo)
                    updateDictionaryDuringGame(&numAmmis);
                else {
                    updateDictionary();
                    numAmmis=numParole;
                }
                //newWordsSignal=true;
            } else {
                /*if(newWordsSignal) {
                    freeList();
                    numAmmis=0;
                    aggiungiAFiltrate(&numAmmis, RBrootDizionario);
                    newWordsSignal=false;

                }*/

                if(!senzaVincolo)
                    stampaFiltrate(listaFiltrate);
                else
                    stampaTutteLeParole(rootDizionario);

            }
        } else {


            if (wordExistsInDictionary(text, lunghezzaParole)) {

                if (processWord(rif, text, output) == true) {
                    fputs("ok\n", stdout);
                    currentCommand = '?';
                    return;
                }


                myPrintString(output, lunghezzaParole);
                //fputc('\n', stdout);
                //stampaVincolo();
                updateVincolo(text, output);

                //stampaUltimoVincolo();

                //stampaVincolo();




                if(!senzaVincolo)
                    updateFiltrateOnlyLastVincolo(&numAmmis, rootDizionario);
                else{
                    numAmmis=0;
                    initializeFiltrateConVincoli(rootDizionario, &numAmmis);
                }

                senzaVincolo=false;
                //numAmmis = getAmmissibili(RBrootDizionario);

                fprintf(stdout, "%d", numAmmis);
                fputc('\n', stdout);


                tentativiRimasti--;


            } else {
                fputs("not_exists\n", stdout);
            }


        }
    } while (!abo);


}

bool rispettaVincoli(char *parola) {

    //stampaVincolo();
    int charMappato;

    for (int i = 0; i < 64; i++)
        validCharSet[i] = 0;

    
    for (int j = 0; j < lunghezzaParole; j++) {

        charMappato = mapCharToInteger(parola[j]);

        //G
        if (vincolo->greenPattern[j] != '?' && parola[j] != vincolo->greenPattern[j])
            return false;


        //M
        if (vincolo->vincoliPerChar[2*charMappato]==1 &&
            vincolo->vincoliPerChar[2*charMappato+1] == 0)
            return false;

        //O
        if (vincolo->redPattern[charMappato + 64 * j] == 'F')
            return false;
        validCharSet[charMappato]++;

    }

    for (int j = 0; j < lunghezzaParole; j++) {

        charMappato = mapCharToInteger(parola[j]);


        //D
        if (validCharSet[charMappato] <
            vincolo->vincoliPerChar[2*charMappato+1])
            return false;

        //E
        if (vincolo->vincoliPerChar[2*charMappato]==1&&
            vincolo->vincoliPerChar[2*charMappato+1] > 0 &&
            vincolo->vincoliPerChar[2*charMappato+1] !=
            (validCharSet[charMappato]))
            return false;

    }
    //C
    for (int m = 0; m < 64; m++) {
        if (vincolo->vincoliPerChar[2*m+1] > validCharSet[m])
            return false;
    }

    //N
    for (int m = 0; m < 64; m++) {
        if (vincolo->vincoliPerChar[2*m] == 1)
            if (validCharSet[m] != vincolo->vincoliPerChar[2*m+1])
                return false;
    }
    return true;
}
bool rispettaLastVincolo(char *parola){

    unsigned char *mylv= vincolo->lastVincolo;

    bool gianCont = false;
    char c;
    int testina=0;

    c = mylv[testina];
    while(c!='\0'){
        if(c=='+' && mylv[testina+2] != parola[mylv[testina+1]-48])
            return false;
        if(c=='/' && parola[mylv[testina+1]-48] == mylv[testina+2])
            return false;

        if(!gianCont){

            for (int i = 0; i < 64; i++)
                validCharSet[i] = 0;
            for (int j = 0; j < lunghezzaParole; j++) {
                validCharSet[mapCharToInteger(parola[j])]++;
            }

            gianCont=true;
        }

        if(c=='E' && validCharSet[mylv[testina+1]]!= mylv[testina+2]-48)
            return false;
        if(c=='M' && validCharSet[mylv[testina+1]] < mylv[testina+2]-48)
            return false;

        testina+=3;
        c=mylv[testina];
    }
    return true;
}
void insertIntoDictionary(char *text/*, unsigned int lung*/) {

    //do per socntato che la stringa sia già stata allocata

    struct BSTnode *newNode = malloc(sizeof(struct BSTnode));

    newNode->word = text;
    newNode->left =NULL;
    newNode->right = NULL;



    struct BSTnode *pre = NULL;
    struct BSTnode *curr = rootDizionario;

    while (curr != NULL) {

        pre = curr;

        if (strCompare(newNode->word, curr->word) > 0) {
            curr = curr->left;
        } else {
            curr = curr->right;

        }
    }
    

    if (pre == NULL) {
        rootDizionario = newNode;
    } else if (strCompare(newNode->word, pre->word) > 0) {
        pre->left = newNode;
    } else {
        pre->right = newNode;
    }

}


bool wordExistsInDictionary(char *s, unsigned int lungh) {

    int esito;

    if (rootDizionario == NULL)
        return false;

    struct BSTnode *temp = rootDizionario;


    while (temp != NULL) {
        esito = strCompare(temp->word, s);
        if (esito == 0)
            return true;
        else if (esito < 0)
            temp = temp->left;
        else
            temp = temp->right;


    }
    return false;


}

//soluzione ricorsiva
void stampaTree(struct BSTnode *T) {
    if (T == NULL) {
        return;
    }


    if (T->left != NULL)
        stampaTree(T->left);

    myPrintString(T->word, lunghezzaParole);
    if (T->right != NULL)
        stampaTree(T->right);


}


int main() {
    bool firstGame = true;

    currentCommand = '?';

    rootDizionario = NULL;
    

    listaFiltrate=NULL;

    int r = scanf("%d", &lunghezzaParole);
    text = malloc(lunghezzaParole);
    
    //skip the /n
    r = getchar_unlocked();

    if (r == 0) {}
    
    
    
    //aggiunge parole alla struttura fino a un comando +nuovaPartita
    //gestisce anche il caso +inserisci_inizio
    //ipotesi: l'unico comando successivo a  +inerisci_inizio è inerisci_fine, non c'è bisogno di controllarlo
    initializeDictionary();
    
    
    
    
    while (!abo) {
        if (currentCommand == 'n') {
            if (firstGame) {
                initializeVincoli(); //fa anche il reset dei vincoli
                firstGame = false;
            } else {
                resetVincoli();
                freeList();
            }
            
            
            
            handlePartita();
            
            
            
        }else if (currentCommand == 'i') {
            updateDictionary();
        } else {
            getStringFromLine(text);
            currentCommand = text[1];
            //leggo commando oppure EOF
        }
    }
    //stampaRBTree(RBrootDizionario);


    return 0;
}

void stampaVincolo() {

    if (vincolo == NULL) {
        printf("VINCOLO VUOTo \n");
        return;
    }

    //int i = 0;
    printf("\n\n--------------------------------STAMPANDO VINCOLI---------------------------------\n");
    //stampo rules
    //stampo greenPattern
    //stampo redPattern

    for (int i = 0; i < 64; i++) {
        printf("%c:  ", mapIntegerToChar(i));
        printf("     ");
        if (vincolo->vincoliPerChar[2*i]==1)
            printf("manca!\n");
        else
            printf("c'e'\n");


        printf("     ");
        printf("compare almeno %d\n", vincolo->vincoliPerChar[2*i +1]);

        printf("\n");

    }
    printf("\n\nGREEN PATTERN: %s\n", vincolo->greenPattern);


    printf("Legenda: F= non c'è in questo posto!          C = ci potrebbe essere!");
    printf("\n\nRED PATTERN: \n");

    for (int i = 0; i < 64; i++) {
        printf("%c ---> ", mapIntegerToChar(i));
        for (int j = 0; j < lunghezzaParole; j++) {
            printf(" %c ", vincolo->redPattern[i + 64 * j]);
        }
        printf("\n");
    }


}

void stampaUltimoVincolo(){
    printf("ULTIMO VINCOLO:  ");
    int i=0;
    while(1){
        fputc(vincolo->lastVincolo[i],stdout);
        if(vincolo->lastVincolo[i]=='\0')
            break;
        i++;
    }
    printf("\n\n");
}

