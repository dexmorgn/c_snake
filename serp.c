#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

typedef struct Anello {
    int riga;
    int colonna;
    struct Anello *next;
} Anello;

void muoviCarattere(Anello *, char, int, int, WINDOW**);
void aggiungiInCoda(Anello *, WINDOW **);
void aggiornaPunteggio(int, WINDOW**);
void movimento(Anello*, Anello , Anello);
int stampaMelaRightDown(Anello, Anello*, int, WINDOW**);
int stampaMelaLeftUp(Anello, Anello*, int, WINDOW**);
void vinto(WINDOW**);
void perso();

int main(void) {
    Anello *testa = (Anello*)malloc(sizeof(Anello));
    testa = NULL;
    int muovi, rigaCasuale, colonnaCasuale, punteggio = 0;
    srand(time(NULL));

    //anelli del serpente
    Anello *primo = (Anello*)malloc(sizeof(Anello));

    testa = primo;

    primo->riga = 4;
    primo->colonna = 2;
    primo->next = NULL;

    initscr();

    //windows settings
        noecho();
        keypad(stdscr, TRUE);
        cbreak();
        curs_set(0);

        box(stdscr, 0 , 0);
        refresh();

                //finestra del punteggio
        WINDOW *winPunteggio = newwin(3, 17, 1, 1);
        aggiornaPunteggio(punteggio, &winPunteggio);

        Anello *temp = testa;
            while(temp != NULL) {
                mvaddch(temp->riga, temp->colonna, 'O');
                temp = temp->next;
                refresh();
            } 

            //printo le prime mele casuali

            rigaCasuale = 1 + (rand() % (LINES - 2));
            colonnaCasuale = 18 + (rand() % ((COLS - 2) -17));
            mvaddch(rigaCasuale, colonnaCasuale, 'X'| A_BOLD | A_BLINK | A_REVERSE | A_STANDOUT);
            rigaCasuale = 1 + (rand() % (LINES - 2));
            colonnaCasuale = 18 + (rand() % ((COLS - 2) -17));
            mvaddch(rigaCasuale, colonnaCasuale, 'X'| A_BOLD | A_BLINK | A_REVERSE | A_STANDOUT);
            rigaCasuale = 4 + (rand() % ((LINES - 2) - 3));
            colonnaCasuale = 1 + (rand() % ((18 - 1) + 1));
            mvaddch(rigaCasuale, colonnaCasuale, 'X'| A_BOLD | A_BLINK | A_REVERSE | A_STANDOUT);
            rigaCasuale = 4 + (rand() % ((LINES - 2) - 3));
            colonnaCasuale = 1 + (rand() % ((18 - 1) + 1));
            mvaddch(rigaCasuale, colonnaCasuale, 'X'| A_BOLD | A_BLINK | A_REVERSE | A_STANDOUT);

        muovi = getch();
        nodelay(stdscr, TRUE);
        timeout(0);
        muoviCarattere(testa, 'O', muovi, punteggio, &winPunteggio);

        getch();

        temp = testa;
        while(testa != NULL) {
            temp = testa;
            testa = testa->next;
            free(temp);
        }
    endwin();
}


void movimento(Anello *temp, Anello prev, Anello prevI) {

    while(temp != NULL) {
        mvaddch(prev.riga, prev.colonna, 'O');
        mvaddch(temp->riga, temp->colonna, ' ');

        prevI.colonna = prev.colonna;
        prevI.riga = prev.riga;

        prev.riga = temp->riga;
        prev.colonna = temp->colonna;

        temp->riga = prevI.riga;
        temp->colonna = prevI.colonna;


        temp = temp->next;
    }

    usleep(130000);
    wrefresh(stdscr);

}

void muoviCarattere(Anello *testa, char serpente, int muovi, int punteggio, WINDOW **aggiorna) {
    int mossa;
    int rigaCasuale, colonnaCasuale;
    Anello *temp = testa;
    Anello prevI;
    Anello prev;

    switch(muovi) {
            case KEY_RIGHT:
                if(mvinch(temp->riga, temp->colonna + 1) == 'O') {
                    muoviCarattere(testa, serpente, KEY_LEFT, punteggio, &(*aggiorna));
                } else {
                    while((mossa = getch()) == KEY_RIGHT || mossa == ERR ) {
                        temp = testa;
                        prev.colonna = temp->colonna + 1;
                        prev.riga = temp->riga;

                            punteggio = stampaMelaRightDown(prev, testa, punteggio, &(*aggiorna));

                            movimento(temp, prev, prevI);
                        }
                }
            muoviCarattere(testa, serpente, mossa, punteggio, &(*aggiorna));
            break;
            case KEY_LEFT:
                if(mvinch(temp->riga, temp->colonna - 1) == 'O') {
                    muoviCarattere(testa, serpente, KEY_RIGHT, punteggio, &(*aggiorna));
                } else {
                    while((mossa = getch()) == KEY_LEFT|| mossa == ERR ) {
                        temp = testa;
                        prev.colonna = temp->colonna - 1;
                        prev.riga = temp->riga;


                            if(prev.riga < 4 && prev.colonna == 17) {
                                wborder(*aggiorna, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                                delwin(*aggiorna);
                                wborder(stdscr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                                wclear(stdscr);
                                wrefresh(stdscr);
                                temp = testa;
                                while(testa != NULL) {
                                    temp = testa;
                                    testa = testa->next;
                                    free(temp);
                                }
                                perso(&(*aggiorna));
                            }
                            
                        
                            punteggio = stampaMelaLeftUp(prev, testa, punteggio, &(*aggiorna));
                         
                            movimento(temp, prev, prevI);
                    }
                }
            muoviCarattere(testa, serpente, mossa, punteggio, &(*aggiorna));                      
            break;
            case KEY_DOWN:
                if(mvinch(temp->riga + 1, temp->colonna) == 'O') {
                    muoviCarattere(testa, serpente, KEY_UP, punteggio, &(*aggiorna));
                } else {
                    while((mossa = getch()) == KEY_DOWN|| mossa == ERR ) {
                        temp = testa;
                        prev.colonna = temp->colonna;
                        prev.riga = temp->riga + 1;
                        
                            punteggio = stampaMelaRightDown(prev, testa, punteggio, &(*aggiorna));

                            movimento(temp, prev, prevI);
                            usleep(30000);
                    }
                }
            muoviCarattere(testa, serpente, mossa, punteggio, &(*aggiorna));                      
            break;
            case KEY_UP:
                if(mvinch(temp->riga - 1, temp->colonna) == 'O') {
                    muoviCarattere(testa, serpente, KEY_DOWN, punteggio, &(*aggiorna));
                } else {
                    while((mossa = getch()) == KEY_UP|| mossa == ERR ) {
                        temp = testa;
                        prev.colonna = temp->colonna;
                        prev.riga = temp->riga - 1;

                        if(prev.riga == 2 && prev.colonna < 18) {
                            wborder(*aggiorna, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                            delwin(*aggiorna);
                            wborder(stdscr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                            wclear(stdscr);
                            wrefresh(stdscr);
                            while(testa != NULL) {
                                temp = testa;
                                testa = testa->next;
                                free(temp);
                            }
                            perso(&(*aggiorna));
                        }
                            
                            punteggio = stampaMelaLeftUp(prev, testa, punteggio, &(*aggiorna));
                            
                            movimento(temp, prev, prevI);
                            usleep(30000);
                    }
                }
            muoviCarattere(testa, serpente, mossa, punteggio, &(*aggiorna));                      
            break; 
    }

}


void aggiungiInCoda(Anello *testa, WINDOW **punteggio) {
    int x, y;
    int z, k;
    Anello *temp;

    getmaxyx(stdscr, y, x);
    getmaxyx(*punteggio, z, k);

    int area_totale = (y - 2) * (x -2);
    int area_pezzettino = (k * z);

    int area = area_totale - area_pezzettino;

    int contatore = 1;
    Anello *nuovoAnello = (Anello*)malloc(sizeof(Anello));
    while(testa->next != NULL) {
        testa = testa->next;
        contatore ++;
    }

    contatore += 1;

        if(contatore == area - 1) {
            while(testa != NULL) {
                temp = testa;
                testa = testa->next;
                free(temp);
            }
            vinto(&(*punteggio));
        }

    testa->next = nuovoAnello;
    nuovoAnello->next = NULL;
}


void aggiornaPunteggio(int punteggio, WINDOW **winPunteggio) {	
	wattron(*winPunteggio, A_REVERSE);
	mvwprintw(*winPunteggio, 1, 2, "PUNTEGGIO: %d", punteggio);
	wattroff(*winPunteggio, A_REVERSE);
	box(*winPunteggio, 0, 0);
	wrefresh(*winPunteggio);
}


int stampaMelaRightDown(Anello prev, Anello *testa, int punteggio, WINDOW **aggiorna) {
    int y, x, rigaCasuale, colonnaCasuale;
    Anello *temp;

    if((mvwinch(stdscr, prev.riga, prev.colonna) & A_CHARTEXT) == 'X') {
        aggiungiInCoda(testa, &(*aggiorna));
        punteggio += 1;
        getmaxyx(stdscr, y, x);
        rigaCasuale = 4 + (rand() % ((y - 2) - 3));
        colonnaCasuale = 1 + (rand() % ((18 - 1) + 1));
        mvaddch(rigaCasuale, colonnaCasuale, 'X' | A_BOLD | A_BLINK | A_REVERSE | A_STANDOUT);
        aggiornaPunteggio(punteggio, &(*aggiorna));
    } else if(mvwinch(stdscr, prev.riga, prev.colonna) == ACS_VLINE || mvwinch(stdscr, prev.riga, prev.colonna) == ACS_HLINE) {
        wborder(*aggiorna, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        delwin(*aggiorna);
        wborder(stdscr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wclear(stdscr);
        wrefresh(stdscr);
        while(testa != NULL) {
            temp = testa;
            testa = testa->next;
            free(temp);
        }
        perso(&(*aggiorna));
    } else if(mvwinch(stdscr, prev.riga, prev.colonna) == 'O') {
        wborder(*aggiorna, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        delwin(*aggiorna);
        wborder(stdscr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wclear(stdscr);
        wrefresh(stdscr);
        while(testa != NULL) {
            temp = testa;
            testa = testa->next;
            free(temp);
        }
        perso(&(*aggiorna));
    } 


    return punteggio;
}

int stampaMelaLeftUp(Anello prev, Anello *testa, int punteggio, WINDOW **aggiorna) {
    int y, x, rigaCasuale, colonnaCasuale;
    Anello *temp;

    if((mvinch(prev.riga, prev.colonna) & A_CHARTEXT) == 'X') {
        aggiungiInCoda(testa, &(*aggiorna));
        punteggio += 1;
        getmaxyx(stdscr, y, x);
        rigaCasuale = 1 + (rand() % (y - 2));
        colonnaCasuale = 18 + (rand() % ((x - 2) -17));
        mvaddch(rigaCasuale, colonnaCasuale, 'X'| A_BOLD | A_BLINK | A_REVERSE | A_STANDOUT);
        aggiornaPunteggio(punteggio, &(*aggiorna));
    } else if(mvwinch(stdscr, prev.riga, prev.colonna) == ACS_VLINE || mvwinch(stdscr, prev.riga, prev.colonna) == ACS_HLINE) {
        wborder(*aggiorna, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        delwin(*aggiorna);
        wborder(stdscr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wclear(stdscr);
        wrefresh(stdscr);
        while(testa != NULL) {
            temp = testa;
            testa = testa->next;
            free(temp);
        }
        perso(&(*aggiorna));
    } else if(mvwinch(stdscr, prev.riga, prev.colonna) == 'O') {
        wborder(*aggiorna, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        delwin(*aggiorna);
        wborder(stdscr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wclear(stdscr);
        wrefresh(stdscr);
        while(testa != NULL) {
            temp = testa;
            testa = testa->next;
            free(temp);
        }
        perso(&(*aggiorna));
    } 


    return punteggio;
}


void perso() {

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int altezza_finestra = 7;
    int larghezza_finestra = 20;
    int start_y = (max_y - altezza_finestra) / 2;
    int start_x = (max_x - larghezza_finestra) / 2;

    WINDOW *perso = newwin(altezza_finestra, larghezza_finestra, start_y, start_x);
    box(perso, 0, 0); // Aggiungi un bordo alla finestra
    wattron(perso, A_REVERSE);
    mvwprintw(perso, 3, 4, "HAI PERSO!!!"); //Scrive nella finestra
    wattroff(perso, A_REVERSE);
    move(0, 0);
    echo();
    wgetch(perso);
    delwin(perso);
    exit(1);
}


void vinto(WINDOW **punteggio) {

    wborder(*punteggio, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    delwin(*punteggio);
    wborder(stdscr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wclear(stdscr);
    wrefresh(stdscr);

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int altezza_finestra = 7;
    int larghezza_finestra = 20;
    int start_y = (max_y - altezza_finestra) / 2;
    int start_x = (max_x - larghezza_finestra) / 2;

    WINDOW *vinto = newwin(altezza_finestra, larghezza_finestra, start_y, start_x);
    box(vinto, 0, 0); // Aggiungi un bordo alla finestra
    wattron(vinto, A_REVERSE);
    mvwprintw(vinto, 3, 4, "HAI VINTO!!!"); //Scrive nella finestra
    wattroff(vinto, A_REVERSE);

    wgetch(vinto);
    delwin(vinto);
    exit(1);

}
