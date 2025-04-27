/*=====================================================================
 *  MORPION 3×3 – Deluxe v2 (Joueur vs IA)
 *=====================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#if defined(_WIN32)
#include <windows.h>
#endif
#include <locale.h>

/* ──────────── Types & constantes ──────────── */
#define N 3
#define VIDE ' '

typedef enum { FACILE = 0, MOYEN = 1, DIFFICILE = 2 } niveau_t;
typedef enum { FAUX = 0, VRAI = 1 } bool_t;

static bool_t ansi_ok = FAUX;
#define CLR_RED     (ansi_ok ? "\033[31m" : "")
#define CLR_BLUE    (ansi_ok ? "\033[34m" : "")
#define CLR_RESET   (ansi_ok ? "\033[0m"  : "")
#define CLR_CLEAR   (ansi_ok ? "\033[H\033[J" : "")

/* ──────────── Prototypes ──────────── */
void   clear_screen(void);
void   initialiser_grille(char g[N][N]);
void   afficher_grille(const char g[N][N]);
int    grille_pleine(const char g[N][N]);
int    victoire(const char g[N][N], char s);

void   jouer_humain(char g[N][N], char symb);
void   jouer_ia(char g[N][N], char ia_symb, char hum_symb, niveau_t diff);

void   nettoyer_buffer(void);
int    demander_entier(const char *prompt, int min, int max);

int    minimax(char g[N][N], char ia, char joueur, int profondeur,
               bool_t is_max, int alpha, int beta);
int    coup_optimal(char g[N][N], char ia_symb, char hum_symb);

/* ──────────── Outils ──────────── */
void clear_screen(void) {
    fputs(CLR_CLEAR, stdout);
}

void nettoyer_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int demander_entier(const char *prompt, int min, int max)
{
    int v;
    printf("%s", prompt);
    while (scanf("%d", &v) != 1 || v < min || v > max) {
        printf("Entrée invalide. Recommencez : ");
        nettoyer_buffer();
    }
    nettoyer_buffer();
    return v;
}

/* ──────────── Grille & affichage ──────────── */
void initialiser_grille(char g[N][N])
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            g[i][j] = VIDE;
}

void afficher_grille(const char g[N][N])
{
    clear_screen();
    puts("");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            char c = g[i][j];
            const char *col = (c == 'X') ? CLR_RED : CLR_BLUE;
            if (c == VIDE) col = "";
            printf(" %s%c%s ", col, c, CLR_RESET);
            if (j < N - 1) printf("|");
        }
        puts("");
        if (i < N - 1) puts("---+---+---");
    }
    puts("");
}

int grille_pleine(const char g[N][N])
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (g[i][j] == VIDE)
                return FAUX;
    return VRAI;
}

int victoire(const char g[N][N], char s)
{
    for (int i = 0; i < N; ++i) {
        if (g[i][0]==s && g[i][1]==s && g[i][2]==s) return VRAI;
        if (g[0][i]==s && g[1][i]==s && g[2][i]==s) return VRAI;
    }
    if (g[0][0]==s && g[1][1]==s && g[2][2]==s) return VRAI;
    if (g[0][2]==s && g[1][1]==s && g[2][0]==s) return VRAI;
    return FAUX;
}

/* ──────────── IA utilitaires ──────────── */
static const int win_lines[8][3] = {
    {0,1,2},{3,4,5},{6,7,8},
    {0,3,6},{1,4,7},{2,5,8},
    {0,4,8},{2,4,6}
};

int trouver_coup_ligne(char g[N][N], char s)
{
    for (int l = 0; l < 8; ++l) {
        int a=win_lines[l][0], b=win_lines[l][1], c=win_lines[l][2];
        char A=g[a/3][a%3], B=g[b/3][b%3], C=g[c/3][c%3];
        if (A==s && B==s && C==VIDE) return c;
        if (A==s && B==VIDE && C==s) return b;
        if (A==VIDE && B==s && C==s) return a;
    }
    return -1;
}

int coup_prefere(char g[N][N])
{
    /* Centre puis coins */
    const int ordre[5] = {4,0,2,6,8};
    for (int i = 0; i < 5; ++i) {
        int id = ordre[i];
        if (g[id/3][id%3] == VIDE) return id;
    }
    return -1;
}

int coup_alea(char g[N][N])
{
    int libres[9], n = 0;
    for (int i = 0; i < 9; ++i)
        if (g[i/3][i%3] == VIDE)
            libres[n++] = i;
    return n ? libres[rand()%n] : -1;
}

/* ──────────── IA Minimax ──────────── */
int score_fin(char g[N][N], char ia, char joueur, int profondeur)
{
    if (victoire(g, ia))     return 10 - profondeur;
    if (victoire(g, joueur)) return profondeur - 10;
    return 0;
}

int minimax(char g[N][N], char ia, char joueur, int profondeur,
            bool_t is_max, int alpha, int beta)
{
    int score = score_fin(g, ia, joueur, profondeur);
    if (score != 0 || grille_pleine(g))
        return score;

    if (is_max) {
        int best = -100;
        for (int i = 0; i < 9; ++i) {
            int r=i/3, c=i%3;
            if (g[r][c]!=VIDE) continue;
            g[r][c]=ia;
            int val=minimax(g,ia,joueur,profondeur+1,FAUX,alpha,beta);
            g[r][c]=VIDE;
            if (val>best) best=val;
            if (best>alpha) alpha=best;
            if (beta<=alpha) break;
        }
        return best;
    } else {
        int best=100;
        for (int i = 0; i < 9; ++i) {
            int r=i/3, c=i%3;
            if (g[r][c]!=VIDE) continue;
            g[r][c]=joueur;
            int val=minimax(g,ia,joueur,profondeur+1,VRAI,alpha,beta);
            g[r][c]=VIDE;
            if (val<best) best=val;
            if (best<beta) beta=best;
            if (beta<=alpha) break;
        }
        return best;
    }
}

int coup_optimal(char g[N][N], char ia_symb, char hum_symb)
{
    int best_val = -100, best_move = -1;
    /* Ordre centre -> coins -> autres pour accélérer l'élagage */
    const int ordre[9] = {4,0,2,6,8,1,3,5,7};
    for (int k=0;k<9;++k) {
        int i=ordre[k];
        int r=i/3, c=i%3;
        if (g[r][c]!=VIDE) continue;
        g[r][c]=ia_symb;
        int move_val=minimax(g, ia_symb, hum_symb, 0, FAUX, -100, 100);
        g[r][c]=VIDE;
        if (move_val>best_val) {
            best_val=move_val;
            best_move=i;
            if (best_val==10) break; /* coup gagnant */
        }
    }
    return best_move;
}

/* ──────────── IA : point d'entrée ──────────── */
void jouer_ia(char g[N][N], char ia_symb, char hum_symb, niveau_t diff)
{
    int coup=-1;
    if (diff==FACILE) {
        coup=coup_alea(g);
    } else if (diff==MOYEN) {
        coup=trouver_coup_ligne(g, ia_symb);
        if (coup==-1) coup=trouver_coup_ligne(g, hum_symb);
        if (coup==-1) coup=coup_prefere(g);
        if (coup==-1) coup=coup_alea(g);
    } else { /* DIFFICILE */
        coup=coup_optimal(g, ia_symb, hum_symb);
    }
    g[coup/3][coup%3]=ia_symb;
}

/* ──────────── Joueur humain ──────────── */
void jouer_humain(char g[N][N], char symb)
{
    int lig, col;
    while (1) {
        lig=demander_entier("Ligne (1-3) : ",1,3)-1;
        col=demander_entier("Colonne (1-3) : ",1,3)-1;
        if (g[lig][col]==VIDE) { g[lig][col]=symb; break; }
        puts("Cette case est déjà occupée.");
    }
}

/* ──────────── Programme principal ──────────── */
int main(void)

{
#if defined(_WIN32)
    setlocale(LC_ALL, ".65001");      /* UTF-8 locale */
    if (SetConsoleOutputCP(65001))
        ansi_ok = VRAI;               /* vrai terminal + UTF-8 → bordures jolies */
#endif

    const char *term = getenv("TERM");
    ansi_ok = term && (strstr(term,"xterm") || strstr(term,"ansi") ||
                       strstr(term,"color") || strstr(term,"linux"));
#if defined(_WIN32)
    ansi_ok = FAUX;
#endif
    srand((unsigned)time(NULL));

    char grille[N][N];
    int score_joueur=0, score_ia=0, nuls=0;

    puts("╔══════════════════════════════╗");
    puts("║          MORPION 3×3         ║");
    puts("╚══════════════════════════════╝");

    char joueur_symb = (demander_entier("Voulez-vous jouer X (1) ou O (2) ? ",1,2)==1)?'X':'O';
    char ia_symb     = (joueur_symb=='X')?'O':'X';
    niveau_t diff    = (niveau_t)demander_entier("Difficulté 0=Facile 1=Moyen 2=Difficile : ",0,2);

    bool_t joueur_commence = demander_entier("Commencez-vous ? 1=Oui 0=Non : ",0,1);

    bool_t rejouer = VRAI;
    while (rejouer) {
        initialiser_grille(grille);
        bool_t tour_joueur = joueur_commence;
        bool_t partie_finie = FAUX;

        while (!partie_finie) {
            afficher_grille(grille);
            if (tour_joueur)
                jouer_humain(grille, joueur_symb);
            else
                jouer_ia(grille, ia_symb, joueur_symb, diff);

            if (victoire(grille, tour_joueur ? joueur_symb : ia_symb)) {
                afficher_grille(grille);
                if (tour_joueur) {
                    printf("%sVous gagnez cette manche !%s\n", CLR_RED,  CLR_RESET);
                    ++score_joueur;
                } else {
                    printf("%sL'IA gagne cette manche.%s\n",   CLR_BLUE, CLR_RESET);
                    ++score_ia;
                }
                partie_finie = VRAI;
            } else if (grille_pleine(grille)) {
                afficher_grille(grille);
                puts("Match nul.");
                ++nuls;
                partie_finie = VRAI;
            }

            if (!partie_finie) tour_joueur = (bool_t)!tour_joueur;
        }

        printf("Score : Vous %d  |  IA %d  |  Nuls %d\n",
               score_joueur, score_ia, nuls);

        rejouer = demander_entier("Rejouer ? 1=Oui 0=Non : ",0,1);
        if (rejouer) joueur_commence = (bool_t)!joueur_commence; /* alterne */
    }

    puts("Merci d'avoir joué !");
    return 0;
}
