#include <unistd.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <setjmp.h>

//per continuare a giocare anche dopo aver vinto/perso
jmp_buf buffer;

//per la gestione della lista che rappresenterà il serprente
typedef struct Anello {
    int posizione_y;
    int posizione_x;
    struct Anello *next;
} Anello;

void creaMenu(WINDOW **, char **);
void sceltaMenu(WINDOW **, char **);
int reverseSceltaMenu(WINDOW **, char **, int);
void eliminaMenu(WINDOW**);
void aggiungiInCoda(Anello *);
void muoviSerpente(Anello *testa, int key, int *, WINDOW **, int mossa_precedente, int *ultimo_punteggio);
void gestisciDirezione(Anello *, int, int*, WINDOW **, int*);
void stampaMele();
void finestraPunteggio(WINDOW **, int*, int*);
void finestraPerso(WINDOW**, Anello *testa);
void finestraVinto(WINDOW**, Anello *testa);
int checkPrimaDiMuovere(Anello *, int key, int *, WINDOW **, int, int, int, int *);

int main() {
    int ultimo_punteggio = 0;
    //punto di salto dopo aver vinto/perso
    setjmp(buffer);
    char *opzioni[] = {"GIOCA", "ESCI"};
    int scelta_movimento, punteggio = 0;
    bool condizione = true;

    //inizializzo la testa della lista concatenata che rappresenta il serpente, e il primo anello, che rappresenta la testa del serprente
    Anello *testa;
    Anello *primo_elemento = (Anello*)malloc(sizeof(Anello));
    testa = primo_elemento;
    //inizializzo un puntatore an Anello che mi servirà successivamente per la pulizia della memoria
    Anello *tmp = NULL;
    
    //imposto il seme per la generazione delle mele in modo casuale
    srand(time(NULL));

        initscr();
        
            curs_set(0);
            noecho();
            keypad(stdscr, TRUE);
            cbreak();
            start_color();

            //inizializzo i colori
            init_pair(1, COLOR_GREEN, COLOR_BLACK);
            init_pair(2, COLOR_CYAN, COLOR_GREEN);
            init_pair(3, COLOR_RED, COLOR_YELLOW);  

                    //inizializzo lo schermo menu
                    WINDOW *menu = newwin(8, 20, (LINES - 8)/2, (COLS - 20)/2);
                    creaMenu(&menu, opzioni);
                    sceltaMenu(&menu, opzioni);
                    curs_set(0);
        
                    //aggiungo i bordi a stdscr
                    clear();
                    box(stdscr, 0, 0);
                    refresh();
        
                    //stampo a schermo la finestra del punteggio 
                    WINDOW *finestra_punteggio = newwin(3, COLS, 0, 0);
                    finestraPunteggio(&finestra_punteggio, &punteggio, &ultimo_punteggio);
                    //stampo il primo anello della lista (ovvero la testa del serpente)
                    primo_elemento->posizione_y = 3;
                    primo_elemento->posizione_x = 1;
        
                    //stampo il primo anello e faccio la prima mossa 
                    nodelay(stdscr, TRUE);
                    timeout(0);
                    mvaddch(primo_elemento->posizione_y, primo_elemento->posizione_x, 'O');
                        
                    //stampo le prime 4 mele in modo randomico
                        for(size_t i = 0; i < 4; i++) {
                            stampaMele();
                        }
        
                    //ciclo per non far scomparire la finestra del punteggio
                        while((scelta_movimento = getch()) == ERR) {
                            finestraPunteggio(&finestra_punteggio, &punteggio, &ultimo_punteggio);
                        }
        
                    gestisciDirezione(testa, scelta_movimento, &punteggio, &finestra_punteggio, &ultimo_punteggio);
        
                    refresh();    
        
                    //mi assicuro di ripulire la memoria allocata per la lista (il serpente)
                    while(testa != NULL) {
                        tmp = testa;
                        testa = testa->next;
                        free(tmp);
                    }

        endwin();

    return 0;
}


void finestraVinto(WINDOW **win_punteggio, Anello *testa) {
    //pulisco le finestre punteggio e standard
    wclear(*win_punteggio);
    wrefresh(*win_punteggio);
    wclear(stdscr);
    wrefresh(stdscr);
    delwin(*win_punteggio);
    wrefresh(stdscr);

    //inizializzo un nuova finestra al centro dello schermo
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int altezza_finestra = 7;
    int larghezza_finestra = 20;
    int start_y = (max_y - altezza_finestra) / 2;
    int start_x = (max_x - larghezza_finestra) / 2;

    WINDOW *win_vinto = newwin(altezza_finestra, larghezza_finestra, start_y, start_x);
    box(win_vinto, 0, 0); // Aggiungi un bordo alla finestra
    wattron(win_vinto, A_REVERSE);
    mvwprintw(win_vinto, 3, 4, "HAI VINTO!!!"); //Scrive nella finestra
    wattroff(win_vinto, A_REVERSE);
    wrefresh(win_vinto);

    sleep(2);

    wclear(win_vinto);
    wrefresh(win_vinto);
    delwin(win_vinto);
    wclear(stdscr);
    wrefresh(stdscr);

    //libero la memoria
    Anello *tmp = testa;    
        while(testa != NULL) {
            tmp = testa;
            testa = testa->next;
            free(tmp);
        }

    endwin();
    longjmp(buffer, 1);
}

void finestraPerso(WINDOW **win_punteggio, Anello *testa) {
    //pulisco le finestre punteggio e standard
    wclear(*win_punteggio);
    wrefresh(*win_punteggio);
    wclear(stdscr);
    wrefresh(stdscr);
    delwin(*win_punteggio);
    wrefresh(stdscr);

    //inizializzo un nuova finestra al centro dello schermo
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int altezza_finestra = 7;
    int larghezza_finestra = 20;
    int start_y = (max_y - altezza_finestra) / 2;
    int start_x = (max_x - larghezza_finestra) / 2;

    WINDOW *win_perso = newwin(altezza_finestra, larghezza_finestra, start_y, start_x);
    box(win_perso, 0, 0); // Aggiungi un bordo alla finestra
    wattron(win_perso, A_REVERSE);
    mvwprintw(win_perso, 3, 4, "HAI PERSO!!!"); //Scrive nella finestra
    wattroff(win_perso, A_REVERSE);
    wrefresh(win_perso);

    sleep(2);

    wclear(win_perso);
    wrefresh(win_perso);
    delwin(win_perso);
    wclear(stdscr);
    wrefresh(stdscr);

    //libero la memoria
    Anello *tmp = testa;    
        while(testa != NULL) {
            tmp = testa;
            testa = testa->next;
            free(tmp);
        }

    //chiudo la finestra e faccio la jump al punto stabilito in partenza
    endwin();
    longjmp(buffer, 1);
}

int checkPrimaDiMuovere(Anello *testa, int key, int *punteggio_ptr, WINDOW **win_punteggio, int mossa_precedente, int posizione_y, int posizione_x, int *ultimo_punteggio) {
    int verifica, x, y, contatore_area = 0;
    Anello *tmp = testa;
    getmaxyx(stdscr, y, x);

    //controllo se il giocatore ha vinto
    int areaPunteggio = 3 * COLS;
    int areaTotale = x * y;

    int areaGiocabile = areaTotale - areaPunteggio;

    //controllo l'area occupata dal serpente
    while(tmp != NULL) {
        tmp = tmp->next;
        contatore_area++;
    }

    //la variabile verifica deve cambiare in base alla mossa eseguita
    switch(key) {
        case KEY_UP:
        verifica = KEY_DOWN;
        break;
        case KEY_DOWN:
        verifica = KEY_UP;
        break;
        case KEY_LEFT:
        verifica = KEY_RIGHT;
        break;
        case KEY_RIGHT:
        verifica = KEY_LEFT;
        break;
    }

        //impedisco al serpente di ''compenetrarsi''
    if(mvinch(posizione_y, posizione_x) == 'O' && mossa_precedente == verifica) {
        return 1;
        //come gestire la ''mangiata'' di una mela
    } else if((mvinch(posizione_y, posizione_x) & A_CHARTEXT) == 'X') {
        aggiungiInCoda(testa);
        stampaMele();
        *punteggio_ptr += 1;
        finestraPunteggio(win_punteggio, punteggio_ptr, ultimo_punteggio);
        return 0;
        //condizioni per perdere
    } else if(mvinch(posizione_y, posizione_x) == ACS_HLINE || mvinch(posizione_y, posizione_x) == ACS_VLINE || posizione_y == 2 || mvinch(posizione_y, posizione_x) == 'O') {
        *ultimo_punteggio = *punteggio_ptr;
        finestraPerso(win_punteggio, testa);
        //condizione per vincere
    } else if(contatore_area == areaGiocabile - 1) {
        *ultimo_punteggio = *punteggio_ptr;
        finestraVinto(win_punteggio, testa);
    }

    //valore di ritorno di default
    return 0;
}

void stampaMele() {
    int y, x, rigaCasuale, colonnaCasuale;
    
    getmaxyx(stdscr, y, x);

    //inizialisso rigaCasuale e colonnaCasuale a valori casuali e mi assicuro che in quella posizione non ci sia una porzione di serpente
    do {
        rigaCasuale = 3 + (rand() % (y - 4));
        colonnaCasuale = 2 + (rand() % (x - 3));
    } while(mvinch(rigaCasuale, colonnaCasuale) == 'O');

    //stampo la mela in quella posizione
    mvaddch(rigaCasuale, colonnaCasuale, 'X' | A_BOLD | A_BLINK | A_REVERSE | A_STANDOUT | COLOR_PAIR(3));
    refresh();
}

void finestraPunteggio(WINDOW **win_punteggio, int *punteggio_ptr, int *ultimo_punteggio) {
    wattron(*win_punteggio, A_REVERSE);
    mvwprintw(*win_punteggio, 1, 2, "PUNTEGGIO: %d", *punteggio_ptr);
    wattroff(*win_punteggio, A_REVERSE);
    wattron(*win_punteggio, COLOR_PAIR(2) | A_BOLD | A_STANDOUT);
    mvwprintw(*win_punteggio, 1, COLS/2 - 9, "GIOCO DEL SERPENTE");
    mvwprintw(*win_punteggio, 1, COLS - 22, "ULTIMO PUNTEGGIO: %d", *ultimo_punteggio);
    wattroff(*win_punteggio, COLOR_PAIR(2) | A_BOLD | A_STANDOUT);
    box(*win_punteggio, 0, 0);
    wrefresh(*win_punteggio);
    refresh();
}

void gestisciDirezione(Anello *testa, int mossa, int *punteggio_ptr, WINDOW **win_punteggio, int *ultimo_punteggio) {
    int mossa_successiva, mossa_precedente;
    mossa_precedente = mossa;
        //ciclo per la gestione dell'input dell'utente
        while(true) {
            switch(mossa) {
                case KEY_UP:
                case KEY_DOWN:
                case KEY_RIGHT:
                case KEY_LEFT:
                muoviSerpente(testa, mossa, punteggio_ptr, win_punteggio, mossa_precedente, ultimo_punteggio);
                finestraPunteggio(win_punteggio, punteggio_ptr, ultimo_punteggio);
                break;
            }

            //gestisco le mosse dell'utente
            mossa_successiva = getch();

            if(mossa_successiva != ERR) {
                    mossa_precedente = mossa;
                    mossa = mossa_successiva;
                }
        }
}

void muoviSerpente(Anello *testa, int key, int *punteggio_ptr, WINDOW **win_punteggio, int mossa_precedente, int *ultimo_punteggio) {
    bool condizione = true;
    int risultato;
    Anello *tmp = testa;
    Anello prev, prev_cpy;
    
    //in base al tasto premuto i parametri assumono valori diversi
    while(condizione) { //gestisco anche i casi particolari di movimento come svoltare a dx quando stai andanado a sx etc...
        switch(key) {
            case KEY_UP: //case KEY_UP
            risultato = checkPrimaDiMuovere(testa, key, punteggio_ptr, win_punteggio, mossa_precedente, tmp->posizione_y - 1, tmp->posizione_x, ultimo_punteggio);
                if(risultato) {
                    key = KEY_DOWN;
                    break;
                } else if(!risultato) {
                    prev.posizione_x = tmp->posizione_x;
                    prev.posizione_y = tmp->posizione_y - 1;
                    condizione = false;
                    break; 
                } 
            case KEY_DOWN: //case KEY_DOWN
            risultato = checkPrimaDiMuovere(testa, key, punteggio_ptr, win_punteggio, mossa_precedente, tmp->posizione_y + 1, tmp->posizione_x, ultimo_punteggio);
                if(risultato) {
                    key = KEY_UP;
                    break;
                } else if(!risultato) {
                    prev.posizione_x = tmp->posizione_x;
                    prev.posizione_y = tmp->posizione_y + 1;
                    condizione = false;
                    break; 
                } 
            case KEY_RIGHT: //case KEY_RIGHT
            risultato = checkPrimaDiMuovere(testa, key, punteggio_ptr, win_punteggio, mossa_precedente, tmp->posizione_y, tmp->posizione_x + 1, ultimo_punteggio);
                if(risultato) {
                    key = KEY_LEFT;
                    break;
                } else if(!risultato) {
                    prev.posizione_x = tmp->posizione_x + 1;
                    prev.posizione_y = tmp->posizione_y;
                    condizione = false;
                    break; 
                } 
            break;
            case KEY_LEFT: //case KEY_LEFT
            risultato = checkPrimaDiMuovere(testa, key, punteggio_ptr, win_punteggio, mossa_precedente, tmp->posizione_y, tmp->posizione_x - 1, ultimo_punteggio);
                if(risultato) {
                    key = KEY_RIGHT;
                    break;
                } else if(!risultato) {
                    prev.posizione_x = tmp->posizione_x - 1;
                    prev.posizione_y = tmp->posizione_y;
                    condizione = false;
                    break; 
                } 
        }
    }
     
    //ciclo per spostare tutti gli anelli del serprente
    while(tmp != NULL) {
        
        mvaddch(prev.posizione_y, prev.posizione_x, 'O');
        mvaddch(tmp->posizione_y, tmp->posizione_x, ' ');

        prev_cpy.posizione_y = prev.posizione_y;
        prev_cpy.posizione_x = prev.posizione_x;

        prev.posizione_x = tmp->posizione_x;
        prev.posizione_y = tmp->posizione_y;

        tmp->posizione_x = prev_cpy.posizione_x;
        tmp->posizione_y = prev_cpy.posizione_y;


        tmp = tmp->next;
    }

    //fermo un attimo sennò l'animazione non viene
    usleep(130000);
    refresh();
}

void aggiungiInCoda(Anello *testa) {
    //per la logica del gioco dello snake, conviene aggiungere gli elementi in coda alla lista
    //non controllo che la testa non sia vuota in quanto ogni volta viene inizializzato almeno un anello
    Anello *nuovo_anello = (Anello*)malloc(sizeof(Anello));
    
    nuovo_anello->next = NULL;

        while(testa->next != NULL) {
            testa = testa->next; //scorro gli elementi fino all'ultimo elemento della coda
        }

    testa->next = nuovo_anello;
}

void creaMenu(WINDOW **menu, char *opzioni[]) {
//funzione per creare il menu
    keypad(*menu, TRUE);

    //creo i bordi del menu'
    box(*menu, 0, 0);
    wattron(*menu, COLOR_PAIR(1) | A_REVERSE);
    mvwaddstr(*menu, 3, 7, opzioni[0]);
    wattroff(*menu, A_REVERSE);
    mvwaddstr(*menu, 4, 7, opzioni[1]);
    wattroff(*menu, COLOR_PAIR(1));
    
    wrefresh(*menu);
}

void sceltaMenu(WINDOW **menu, char *opzioni[]) {
    int scelta;
    int posizioneCursore = 0;
    wmove(*menu,3, 7);

    //utilizzo la variabile posizionecursore per tenere traccia di dove aggiungere l'attributi reverse alla scritta che rappresenta l'opzione
        while((scelta = wgetch(*menu)) != KEY_F(1)) {
            switch(scelta) {
                case KEY_DOWN:
                posizioneCursore = reverseSceltaMenu(menu, opzioni, posizioneCursore);
                wrefresh(*menu);
                break;
                case KEY_UP:
                posizioneCursore = reverseSceltaMenu(menu, opzioni, posizioneCursore);
                wrefresh(*menu);
                break;
                case '\n':
                    if(posizioneCursore == 0) {
                        eliminaMenu(menu);
                        return;
                    } else {
                        eliminaMenu(menu);
                        endwin();
                        exit(1);
                    }
            }
        }
}

int reverseSceltaMenu(WINDOW **menu, char *opzioni[], int posizioneCursore) {
    //funzione che mi serve per determinare quale delle due opzioni è selezionata, e successivamente evidenziarla
    if(posizioneCursore == 0) {
        wattron(*menu, A_REVERSE | COLOR_PAIR(1));
        mvwaddstr(*menu, 4, 7, opzioni[1]);
        wattroff(*menu, A_REVERSE);
        mvwaddstr(*menu, 3, 7, opzioni[0]);
        wattroff(*menu, COLOR_PAIR(1));
        wmove(*menu, 4, 7);
        posizioneCursore += 1;
    } else {
        wattron(*menu, A_REVERSE | COLOR_PAIR(1));
        mvwaddstr(*menu, 3, 7, opzioni[0]);
        wattroff(*menu, A_REVERSE);
        mvwaddstr(*menu, 4, 7, opzioni[1]);
        wattroff(*menu, COLOR_PAIR(1));
        wmove(*menu, 3, 7);
        posizioneCursore -= 1;
    }

    return posizioneCursore;
}

void eliminaMenu(WINDOW **menu) {
    //semplice funzione per eliminare la finestra del menu
    wborder(*menu, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    mvwaddstr(*menu, 3, 7, "     ");
    mvwaddstr(*menu, 4, 7,"    ");
    wrefresh(*menu);
    delwin(*menu);
}
