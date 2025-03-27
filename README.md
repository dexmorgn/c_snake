# Gioco del Serpente in C con ncurses

Questo progetto implementa un semplice gioco del serpente in C utilizzando la libreria ncurses per la gestione dell'interfaccia a terminale.

## Descrizione

Il gioco consiste nel controllare un serpente che si muove all'interno del terminale. L'obiettivo è mangiare le mele (rappresentate dal carattere 'X') per aumentare la lunghezza del serpente e il punteggio. Il gioco termina quando il serpente colpisce i bordi del terminale, se stesso o quando il serpente riempie completamente l'area di gioco.

## Funzionalità

* **Movimento del Serpente**: Il serpente si muove utilizzando i tasti freccia.
* **Gestione del Punteggio**: Il punteggio aumenta quando il serpente mangia una mela.
* **Generazione di Mele Casuali**: Le mele vengono generate in posizioni casuali all'interno del terminale.
* **Rilevamento delle Collisioni**: Il gioco rileva le collisioni del serpente con i bordi e con se stesso.
* **Condizioni di Vittoria e Sconfitta**: Il gioco termina con una vittoria se il serpente riempie l'area di gioco o con una sconfitta se il serpente colpisce i bordi o se stesso.
* **Interfaccia Utente**: L'interfaccia utente è gestita utilizzando la libreria ncurses.

## Dipendenze

* Libreria ncurses

## Come Compilare ed Eseguire

1.  Assicurati di avere la libreria ncurses installata.
2.  Compila il codice utilizzando un compilatore C:

    ```bash
    gcc nome_del_file.c -lncurses -o serpente 
    ```

    (Sostituisci `nome_del_file.c` con il nome del tuo file sorgente).
3.  Esegui il file eseguibile:

    ```bash
    ./serpente
    ```

## Struttura del Codice

* **`Anello`**: Struttura dati per rappresentare un segmento del serpente.
* **`muoviCarattere()`**: Gestisce il movimento del serpente e le collisioni.
* **`aggiungiInCoda()`**: Aggiunge un segmento al serpente quando mangia una mela.
* **`aggiornaPunteggio()`**: Aggiorna e visualizza il punteggio.
* **`movimento()`**: Aggiorna la posizione del serpente sullo schermo.
* **`stampaMelaRightDown()` e `stampaMelaLeftUp()`**: Gestiscono la generazione delle mele e le collisioni con le mele.
* **`vinto()` e `perso()`**: Gestiscono le condizioni di vittoria e sconfitta.
* **`main()`**: Funzione principale che inizializza il gioco e gestisce il ciclo di gioco.

## Note

* Il codice utilizza `usleep()` per controllare la velocità del serpente.
* Il codice gestisce la generazione casuale di mele all'interno dell'area di gioco.
* Il codice gestisce la fine del gioco in caso di vittoria o sconfitta, mostrando un messaggio appropriato.

## Autore

dexmorgn
## Licenza

Questo progetto è distribuito sotto \[Nome della licenza].
