#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Dimensiunile celulei
#define LAT 7
#define INALT 3
// Dimensiunile bordurii
#define BORD_LAT (4 * LAT + 10)
#define BORD_INALT (4 * INALT + 10)

#define CULOARE_2 1
#define CULOARE_4 2
#define CULOARE_8 3
#define CULOARE_16 4
#define CULOARE_32 5
#define CULOARE_64 6
#define CULOARE_128 7
#define CULOARE_256 8
#define CULOARE_512 9
#define CULOARE_1024 10
#define CULOARE_2048 11

typedef struct Istoric_joc {
    int val_cel[4][4], scor, com, joc_prec, nr_m_val;
    char comenzi[1000], m_val[10];
} joc;

void ipair() {
    init_pair(CULOARE_2, 0, 1);
    init_pair(CULOARE_4, 0, 2);
    init_pair(CULOARE_8, 0, 3);
    init_pair(CULOARE_16, 0, 4);
    init_pair(CULOARE_32, 0, 5);
    init_pair(CULOARE_64, 0, 6);
    init_pair(CULOARE_128, 0, 7);
    init_pair(CULOARE_256, 0, 8);
    init_pair(CULOARE_512, 0, 9);
    init_pair(CULOARE_1024, 0, 10);
    init_pair(CULOARE_2048, 0, 11);
}

// Functie pentru afisarea titlului
void titlu(WINDOW *wind) {
    int print_x, print_y, x;

    for (x = 0; x < 4; x++) {
        /* Determinarea pozitiilor la care se afiseaza astfel incat
        tabela sa fie centrata */
        print_x = (BORD_LAT - 4 * LAT) / 2 + x * LAT;
        print_y = 4;

        WINDOW *celula = newwin(INALT, LAT, print_y, print_x);

        switch (x) {
            case 0:
                wbkgd(celula, COLOR_PAIR(CULOARE_2));
                mvwprintw(celula, INALT / 2, (LAT - 2) / 2 + 1, "2");
                break;
            case 1:
                wbkgd(celula, COLOR_PAIR(CULOARE_4));
                mvwprintw(celula, INALT / 2, (LAT - 2) / 2 + 1, "0");
                break;
            case 2:
                wbkgd(celula, COLOR_PAIR(CULOARE_8));
                mvwprintw(celula, INALT / 2, (LAT - 2) / 2 + 1, "4");
                break;
            case 3:
                wbkgd(celula, COLOR_PAIR(CULOARE_16));
                mvwprintw(celula, INALT / 2, (LAT - 2) / 2 + 1, "8");
                break;
        }

        box(celula, 0, 0);  // Bordarea celulei
        wrefresh(celula);
        refresh();
    }

    wrefresh(wind);
}

// Fereastra Meniu
int menu(WINDOW *wind) {
    int alegere, mark, poz_menu, i;
    char menu[10], varianta[4][15];
    alegere = 0;
    mark = 0;

    werase(wind);
    box(wind, 0, 0);

    refresh();
    wrefresh(wind);

    strcpy(menu, "Main Menu");
    poz_menu = (BORD_LAT - strlen(menu)) / 2;
    mvwprintw(wind, 8, poz_menu, "%s", menu);

    strcpy(varianta[0], "New Game");
    strcpy(varianta[1], "Resume");
    strcpy(varianta[2], "Reguli");
    strcpy(varianta[3], "Quit");

    // Afisarea titlului
    titlu(wind);

    // Navigarea prin meniu pana se face o alegere
    while (1) {
        for (i = 0; i < 4; i++) {
            // Aplica highlight pe optiunea curenta
            if (i == mark) wattron(wind, A_STANDOUT);
            mvwprintw(wind, i + 10, (BORD_LAT - strlen(varianta[i])) / 2, "%s",
                      varianta[i]);
            // Elimina highlight-ul
            wattroff(wind, A_STANDOUT);
        }

        // Citirea tastelor
        alegere = wgetch(wind);

        switch (alegere) {
            case 119:
            case 87:
            case KEY_UP:
                if (mark != 0) mark--;
                break;
            case 115:
            case 83:
            case KEY_DOWN:
                if (mark != 3) mark++;
                break;
            default:
                break;
        }

        // Cand se apasa tasta Enter se returneaza optinea aleasa
        if (alegere == 10) return mark;
    }
}

/* Functiile de mutare sunt realizate cu acelasi rationament.
In variabila imin, respectiv jmin, se retine prima pozitie libera unde se pot
afisa valorile nenule */

// Functie pentru mutarea in sus
void key_up(int v[4][4], int *scor) {
    int i, j, imin, x;
    for (j = 0; j < 4; j++) {
        imin = 0;
        for (i = 0; i < 4; i++)
            // se ignora valorile nule
            if (v[i][j] != 0) {
                x = i + 1;
                // Se cauta urmatoarea valoare nenula
                while (x < 4 && v[x][j] == 0) {
                    x++;
                }
                // Unirea celulelor cu valori identice si calcularea scorului
                if (x < 4 && v[i][j] == v[x][j]) {
                    v[imin][j] = v[i][j] * 2;
                    v[x][j] = 0;
                    *scor = *scor + v[imin][j];
                } else {
                    /* Daca valorile difera, se afiseaza prima valoare in prima
                     * pozitie disponibila */
                    v[imin][j] = v[i][j];
                }
                if (i != imin) {
                    v[i][j] = 0;
                }
                imin++;
                i = x - 1;
            }
    }
}

// Functie pentru mutarea in jos
void key_down(int v[4][4], int *scor) {
    int i, j, imin, x;
    for (j = 0; j < 4; j++) {
        imin = 3;
        for (i = 3; i >= 0; i--)
            if (v[i][j] != 0) {
                x = i - 1;
                while (x > 0 && v[x][j] == 0) {
                    x--;
                }

                if (x >= 0 && v[i][j] == v[x][j]) {
                    v[imin][j] = v[i][j] * 2;
                    v[x][j] = 0;
                    *scor = *scor + v[imin][j];
                } else {
                    v[imin][j] = v[i][j];
                }
                if (i != imin) {
                    v[i][j] = 0;
                }
                imin--;
                i = x + 1;
            }
    }
}

// Functie pentru mutarea la stanga
void key_left(int v[4][4], int *scor) {
    int i, j, jmin, x;
    for (i = 0; i < 4; i++) {
        jmin = 0;
        for (j = 0; j < 4; j++)
            if (v[i][j] != 0) {
                x = j + 1;
                while (x < 4 && v[i][x] == 0) {
                    x++;
                }

                if (x < 4 && v[i][j] == v[i][x]) {
                    v[i][jmin] = v[i][j] * 2;
                    v[i][x] = 0;
                    *scor = *scor + v[i][jmin];
                } else {
                    v[i][jmin] = v[i][j];
                }
                if (j != jmin) {
                    v[i][j] = 0;
                }
                jmin++;
                j = x - 1;
            }
    }
}

// Functia pentru mutarea la dreapta
void key_right(int v[4][4], int *scor) {
    int i, j, jmin, x;
    for (i = 0; i < 4; i++) {
        jmin = 3;
        for (j = 3; j >= 0; j--)
            if (v[i][j] != 0) {
                x = j - 1;
                while (x > 0 && v[i][x] == 0) {
                    x--;
                }

                if (x >= 0 && v[i][j] == v[i][x]) {
                    v[i][jmin] = v[i][j] * 2;
                    v[i][x] = 0;
                    *scor = *scor + v[i][jmin];
                } else {
                    v[i][jmin] = v[i][j];
                }
                if (j != jmin) {
                    v[i][j] = 0;
                }
                jmin--;
                j = x + 1;
            }
    }
}

/* Functie pentru determinarea mutarilor valide disponibile si pentru mutarea
 automata */
int mutare(joc *joc) {
    int i, j, k, max_cel, max_scor, move, move_val;
    int nr_cel[2], v[4][4], scor[2] = {0}, dif;

    srand(time(NULL));
    max_scor = 0;
    max_cel = 0;
    move = 4;
    strcpy(joc->m_val, "");
    joc->nr_m_val = 0;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            if (joc->val_cel[i][j] == 0) max_cel++;

    for (k = 0; k < 4; k++) {
        dif = 0;

        // Realizarea unei copii pentru stadiul actual al jocului
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++) v[i][j] = joc->val_cel[i][j];

        /* Verificarea tuturor mutarilor (acestea functioneaza dupa acelasi
         * principiu) */
        switch (k) {
            case 0:
                key_up(v, &scor[k]);
                // Verifica daca mutarea este valida
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                /* Daca mutarea este valida, abrevierea sa este copiata intr-un
                 * sir de caractere pentru mutari valide si se actualizeaza
                 * numarul mutarilor valide*/
                if (dif == 1) {
                    strcat(joc->m_val, "U ");
                    joc->nr_m_val++;
                }
                break;
            case 1:
                key_down(v, &scor[k]);
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                if (dif == 1) {
                    strcat(joc->m_val, "D ");
                    joc->nr_m_val++;
                }
                break;
            case 2:
                key_left(v, &scor[k]);
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                if (dif == 1) {
                    strcat(joc->m_val, "L ");
                    joc->nr_m_val++;
                }
                break;
            case 3:
                key_right(v, &scor[k]);
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                if (dif == 1) {
                    strcat(joc->m_val, "R");
                    joc->nr_m_val++;
                }
                break;
        }

        // Verificarea celulelor cu valori nule dupa mutare
        nr_cel[k] = 0;
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                if (v[i][j] == 0) nr_cel[k]++;

        /* Determinarea celei mai eficiente mutari din punct de vedere al
         * celulelor eliberate si a scorului*/
        if (dif == 1) {
            move_val = k;
            if (nr_cel[k] > max_cel ||
                (nr_cel[k] == max_cel && scor[k] > max_scor)) {
                max_cel = nr_cel[k];
                max_scor = scor[k];
                move = k;
            }
            /* Miscarile sus/jos, respectiv stanga/dreapta sunt echivalente ca
             * punctaj, acestea se genereaza aleatoriu daca sunt valide*/
            if (k == 1 && move == 0) move = rand() % 2;
            if (k == 3 && move == 2) move = rand() % 2 + 2;
        }
    }

    /* Daca nu exista mutari care sa elibereze celule, atunci se ia drept unica
     * conditie ca mutarea sa fie valida*/
    if (move == 4) {
        if (joc->nr_m_val == 4)
            move = rand() % 4;
        else
            move = move_val;
    }
    switch (move) {
        case 0:
            move = 119;
            break;
        case 1:
            move = 115;
            break;
        case 2:
            move = 97;
            break;
        case 3:
            move = 100;
            break;
    }

    // Se returneaza cea mai eficienta mutare
    return move;
}

// Functie pentru afisarea jocului
int show_game(WINDOW *wind, joc *joc, int ok) {
    int x, y, poz_x, poz_y, nr_cel;
    nr_cel = 0;

    // Se calculeaza numarul de celule cu valori nenule
    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++)
            if (joc->val_cel[y][x] != 0) nr_cel++;

    // Daca toate celulele au valori nenule, jocul se incheie
    if (nr_cel == 16) {
        joc->joc_prec = 0;
        // Se afiseaza un mesaj sugestiv
        mvwprintw(wind, 4 * INALT + 6, (BORD_LAT - 11) / 2, "Ai pierdut!");
        wrefresh(wind);
        return 0;
    }

    wrefresh(wind);

    // Afisare valorii 2 sau 4 intr-o celula cu valoare nula in mod aleator
    if (ok == 1) {
        do {
            x = rand() % 4;
            y = rand() % 4;
        } while (joc->val_cel[y][x] != 0);
        joc->val_cel[y][x] = (rand() % 2 + 1) * 2;
    }

    // Afisarea scorului
    mvwprintw(wind, 4 * INALT + 3, 1, "Scor: %d", joc->scor);

    // Verificarea mutarilor valide posibile si afisarea lor
    mutare(joc);
    mvwprintw(wind, 4 * INALT + 4, 1, "Mutari posibile: %s", "           ");
    mvwprintw(wind, 4 * INALT + 4, 1, "Mutari posibile: %s", joc->m_val);

    // Afisarea ultimelor comenzi valide
    if (strlen(joc->comenzi) < 18)
        mvwprintw(wind, 4 * INALT + 5, 1, "Ultimele mutari: %s", joc->comenzi);
    else
        mvwprintw(wind, 4 * INALT + 5, 1, "Ultimele mutari: %s",
                  joc->comenzi + strlen(joc->comenzi) - 18);

    // Generarea tablei de joc cu 16 celule
    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++) {
            /* Determinarea pozitiilor la care se afiseaza astfel incat
            tabela sa fie centrata */
            poz_x = (BORD_LAT - 4 * LAT) / 2 + x * LAT;
            poz_y = 2 + y * INALT;

            WINDOW *celula = newwin(INALT, LAT, poz_y, poz_x);

            // Colorarea celulei in fuctie de valoarea pe care o contine
            switch (joc->val_cel[y][x]) {
                case 2:
                    wbkgd(celula, COLOR_PAIR(CULOARE_2));
                    break;
                case 4:
                    wbkgd(celula, COLOR_PAIR(CULOARE_4));
                    break;
                case 8:
                    wbkgd(celula, COLOR_PAIR(CULOARE_8));
                    break;
                case 16:
                    wbkgd(celula, COLOR_PAIR(CULOARE_16));
                    break;
                case 32:
                    wbkgd(celula, COLOR_PAIR(CULOARE_32));
                    break;
                case 64:
                    wbkgd(celula, COLOR_PAIR(CULOARE_64));
                    break;
                case 128:
                    wbkgd(celula, COLOR_PAIR(CULOARE_128));
                    break;
                case 256:
                    wbkgd(celula, COLOR_PAIR(CULOARE_256));
                    break;
                case 512:
                    wbkgd(celula, COLOR_PAIR(CULOARE_512));
                    break;
                case 1024:
                    wbkgd(celula, COLOR_PAIR(CULOARE_1024));
                    break;
                case 2048:
                    wbkgd(celula, COLOR_PAIR(CULOARE_2048));
                    break;
                default:
                    wbkgd(celula, COLOR_PAIR(COLOR_BLACK));
                    break;
            }

            box(celula, 0, 0);  // Bordarea celulei
            refresh();

            // Afisarea valorii in celula doar daca aceasta este nenula
            if (joc->val_cel[y][x] != 0)
                mvwprintw(celula, INALT / 2, (LAT - 2) / 2,
                          "%d",  //+1 la lat
                          joc->val_cel[y][x]);

            // Jocul se incheie cand se ajunge la valoarea 2048
            if (joc->val_cel[y][x] == 2048) joc->joc_prec = 0;

            wrefresh(celula);
            delwin(celula);
            wrefresh(wind);
        }

    // Incheierea jocului daca acesta este castigat
    if (joc->joc_prec == 0) {
        // Afisarea unui mesaj sugestiv
        mvwprintw(wind, 4 * INALT + 6, (BORD_LAT - 12) / 2, "Ai castigat!");
        wrefresh(wind);
        return 0;
    }

    // Continuarea jocului
    return 1;
}

// Afisarea datei si orei curente
void afisare_data(WINDOW *wind) {
    time_t now;
    time(&now);
    char format_data[80];
    struct tm *timp = localtime(&now);

    // Formatarea datei si a orei
    strftime(format_data, 80, "%d.%m.%Y %H:%M:%S", timp);
    mvwprintw(wind, 4 * INALT + 7, (BORD_LAT - strlen(format_data)) / 2, "%s",
              format_data);
    wrefresh(wind);
}

// Functie pentru generarea unui nou joc
void new_game(WINDOW *wind, joc *joc) {
    int random_poz[2][2], i, j, x, y;
    joc->comenzi[0] = '\0';
    joc->scor = 0;
    joc->com = 0;
    joc->joc_prec = 1;

    srand(time(NULL));

    // Initializarea valorilor
    for (y = 0; y < 4; y++)
        for (x = 0; x < 4; x++) joc->val_cel[y][x] = 0;

    // Determinarea a doua pozitii aleatorare
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++) random_poz[i][j] = rand() % 4;

    // Asigurarea faptului ca cele doua pozitii determinate sunt diferite
    while (random_poz[0][0] == random_poz[1][0] &&
           random_poz[0][1] == random_poz[1][1])
        for (j = 0; j < 2; j++) random_poz[1][j] = rand() % 4;

    // Generarea a doua numere aleatoare (2 sau 4)
    joc->val_cel[random_poz[0][0]][random_poz[0][1]] = (rand() % 2 + 1) * 2;
    joc->val_cel[random_poz[1][0]][random_poz[1][1]] = (rand() % 2 + 1) * 2;

    // Afisarea jocului
    show_game(wind, joc, 0);
}

/* Functie pentru continuarea jocului / revenirea la ultimul joc (daca este
 * disponibil) */
int resume(WINDOW *wind, joc *joc) {
    int ok, c, mut, v[4][4], i, j, dif;
    joc->joc_prec = 1;

    time_t timp_ultimacom, timp_acum;
    timp_ultimacom = time(NULL);

    mvwprintw(wind, 4 * INALT + 8, 3, "Apasa Q pentru a reveni la meniu");
    srand(time(NULL));
    nodelay(wind, TRUE);
    wrefresh(wind);
    ok = 1;
    mut = 0;

    while (1) {
        // Realizarea unei copii a valorilor
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++) v[i][j] = joc->val_cel[i][j];
        dif = 0;

        // Afisarea datei si orei curente
        afisare_data(wind);

        timp_acum = time(NULL);
        c = wgetch(wind);

        /* Daca dupa 5 secunde nu este apasata o tasta valida, se genereaza
         * cea mai eficienta mutare in mod automat */
        if ((difftime(timp_acum, timp_ultimacom) >= 5) && c == ERR) {
            if (ok == 0) return 0;
            c = mutare(joc);
            timp_ultimacom = time(NULL);
            mut = 1;
        }
        if (c != ERR) timp_ultimacom = time(NULL);

        // Daca se apasa tasta "q"/"Q", se revine la meniu
        if (c == 113 || c == 81) return 0;

        // Cand jocul s-a incheiat (ok este 0) se asteapta apasarea tastei Q
        while (ok == 0)
            if (c == 113 || c == 81)
                return 0;
            else
                c = wgetch(wind);

        /* Realizarea mutarilor valide (toate se realizeaza dupa acelasi
         * principiu) */
        switch (c) {
            // Mutare in sus
            case 119:
            case 87:
            case KEY_UP:
                key_up(joc->val_cel, &joc->scor);
                // Se verifica daca mutarea e valida
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                /* Daca mutarea e valida, se retine si se marcheaza faptul
                 * ca s-a executat o mutare valida*/
                if (dif == 1) {
                    joc->comenzi[joc->com] = 'U';
                    joc->comenzi[++joc->com] = ' ';
                    joc->com++;
                    mut = 1;
                }
                break;

            // Mutare in jos
            case 115:
            case 83:
            case KEY_DOWN:
                key_down(joc->val_cel, &joc->scor);
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                if (dif == 1) {
                    joc->comenzi[joc->com] = 'D';
                    joc->comenzi[++joc->com] = ' ';
                    joc->com++;
                    mut = 1;
                }
                break;

            // Mutare la stanga
            case 97:
            case 65:
            case KEY_LEFT:
                key_left(joc->val_cel, &joc->scor);
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                if (dif == 1) {
                    joc->comenzi[joc->com] = 'L';
                    joc->comenzi[++joc->com] = ' ';
                    joc->com++;
                    mut = 1;
                }
                break;

            // Mutare la dreapta
            case 100:
            case 68:
            case KEY_RIGHT:
                key_right(joc->val_cel, &joc->scor);
                for (i = 0; i < 4 && dif == 0; i++)
                    for (j = 0; j < 4 && dif == 0; j++)
                        if (v[i][j] != joc->val_cel[i][j]) dif = 1;
                if (dif == 1) {
                    joc->comenzi[joc->com] = 'R';
                    joc->comenzi[++joc->com] = ' ';
                    joc->com++;
                    mut = 1;
                }
                break;
        }

        // Daca s-a realizat o mutare valida se afiseaza jocul
        if (mut == 1) {
            joc->comenzi[joc->com] = '\0';
            ok = show_game(wind, joc, 1);
            mut = 0;
        }
        /* Daca se realizeaza o mutare invalida, se verifica daca mai sunt
           mutari valide posibile. In cazul negativ se incheie jocul*/
        else {
            mutare(joc);
            if (joc->nr_m_val == 0) ok = show_game(wind, joc, 1);
        }
    }

    wrefresh(wind);
}

// Functie pentru afisarea ferestrei cu reguli
int rules(WINDOW *wind) {
    int c;
    werase(wind);
    box(wind, 0, 0);
    refresh();
    mvwprintw(wind, 3, (BORD_LAT - 6) / 2, "Reguli");
    mvwprintw(wind, 5, 4, "Utilizeaza tastele sageti sau");
    mvwprintw(wind, 6, 2, "WASD pentru a deplasa celulele.");
    mvwprintw(wind, 8, 4, "Mutarile disponibile si istoricul");
    mvwprintw(wind, 9, 2, "mutarilor vor fi afisate pe ecran:");
    mvwprintw(wind, 10, 5, "U - up / sus");
    mvwprintw(wind, 11, 5, "D - down / jos");
    mvwprintw(wind, 12, 5, "L - left / stanga");
    mvwprintw(wind, 13, 5, "R - right / dreapta");
    mvwprintw(wind, 15, 4, "Daca dupa 5 secunde nu este");
    mvwprintw(wind, 16, 2, "apasata nicio tasta, se va genera");
    mvwprintw(wind, 17, 2, "o miscare in mod automat.");
    mvwprintw(wind, 4 * INALT + 8, 3, "Apasa Q pentru a reveni la meniu");
    wrefresh(wind);

    // Se asteapta apasarea tastei Q pentru a reveni la meniu
    c = 0;
    while (c != 113 || c != 81) {
        c = wgetch(wind);
        if (c == 113 || c == 81) return 0;
    }
    return 0;
}

int main() {
    int x;
    joc j;
    j.joc_prec = 0;

    initscr();
    cbreak();
    start_color();
    noecho();
    curs_set(0);

    // Initializarea culorilor pentru fiecare numar
    ipair();

    // Crearea unor ferestre pentru meniu si joc
    WINDOW *wind = newwin(BORD_INALT, BORD_LAT, 0, 0);
    box(wind, 0, 0);
    refresh();
    wrefresh(wind);
    keypad(wind, TRUE);

    WINDOW *wnd = newwin(BORD_INALT, BORD_LAT, 0, 0);
    box(wnd, 0, 0);
    refresh();
    wrefresh(wnd);
    keypad(wnd, TRUE);

    while (1) {
        // Afisarea meniului
        x = menu(wind);

        // Selectarea optiunii
        switch (x) {
            // Optiunea "New Game"
            case 0:
                werase(wnd);
                box(wnd, 0, 0);
                refresh();
                wrefresh(wnd);
                new_game(wnd, &j);
                resume(wnd, &j);
                break;
            // Optiunea "Resume"
            case 1:
                // Daca exista un joc inceput, se revine la el
                if (j.joc_prec == 1) {
                    show_game(wnd, &j, 1);
                    resume(wnd, &j);
                }
                // Daca nu exista, se afiseaza o fereastra cun un mesaj
                else {
                    werase(wind);
                    box(wind, 0, 0);
                    refresh();
                    mvwprintw(wind, 10, (BORD_LAT - 25) / 2,
                              "Nu exista un joc anterior");
                    mvwprintw(wind, 11, (BORD_LAT - 19) / 2,
                              "la care poti reveni");
                    wrefresh(wind);
                    sleep(4);
                }
                break;
            // Optiunea "Reguli"
            case 2:
                rules(wind);
                break;
            // Optiunea "Quit"
            case 3:
                clrtoeol();
                refresh();
                endwin();
                return 0;
        }
    }
}