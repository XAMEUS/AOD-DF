#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "frechet.h"

struct chemins *lire_fichier(FILE* fichier) {
    struct chemins *data = malloc(sizeof(struct chemins));
    data->t[0] = malloc(sizeof(struct tableau));
    data->t[1] = malloc(sizeof(struct tableau));
    fscanf(fichier, "%lu %lu\n", &(data->t[0]->len), &(data->t[1]->len));
    data->t[0]->tab = malloc(data->t[0]->len * sizeof(struct point));
    data->t[1]->tab = malloc(data->t[1]->len * sizeof(struct point));
    for (size_t i = 0; !feof(fichier) && i < data->t[0]->len; i++)
        fscanf(fichier, "%ld %ld", &(data->t[0]->tab[i]).x,
                                   &(data->t[0]->tab[i]).y);
    for (size_t i = 0; !feof(fichier) && i < data->t[1]->len; i++)
        fscanf(fichier, "%ld %ld", &(data->t[1]->tab[i]).x,
                                   &(data->t[1]->tab[i]).y);
    return data;
}

void liberer_chemins(struct chemins *data) {
    free(data->t[0]->tab);
    free(data->t[0]);
    free(data->t[1]->tab);
    free(data->t[1]);
}

void liberer_pre_calc(struct tab_sol_2d *data, int debut, int fin) {
    for(int i = debut; i < fin; i++) {
        struct tab_sol *cur_i = data->t[i];
        for(size_t j = 0; j < cur_i->len; j++)
            free(cur_i->t[j].tab);
        free(cur_i->t);
        free(cur_i);
    }
}
/*
Le repère est selon (P, Q) avec Origine le point d'origine du parcours;
Arguments:
data        Les deux chemins du fichier d'entrée (data->t[0] pour P et 1 pour Q);
depart      Pre_calc correspond à P = depart.x et Q = depart.y;
arrive      Faire tourner l'alorithme jusqu'à P = arrive.x et Q = arrive.y;
pre_calc    Valeurs précalculées:
                pre_calc.t[0] est la ligne en bas
                pre_calc.t[1] est la ligne à gauche
                pre_calc.t[0]->t[0] est le point de départ
                pre_calc.t[i]->len est ignoré;
res         Valeurs de retour:
                res->t[0] est la ligne en haut
                res->t[1] est la ligne à droite
                res->t[1]->t[res->t[1]->len - 1] est le point d'arrivée;
                res->t[i]->t et res->t[i]->len doivent être initialisés
*/
void frechet_iteratif(struct chemins data,
                      struct point depart,
                      struct point arrive,
                      struct tab_sol_2d pre_calc,
                      struct tab_sol_2d *res) {
    #if F_DEBUG == 1
    fprintf(stderr,">(%ld %ld) (%ld %ld) itératif\n",
          depart.x, depart.y, arrive.x, arrive.y);
    #endif
    struct tableau sols[arrive.y - depart.y + 1][arrive.x - depart.x + 1];
    for(int i = 0; i < 2 ; i++) {
        size_t borne = 1 + (arrive.y - depart.y) * i + (arrive.x - depart.x) * !i;
        for(size_t j = i; j < borne; j++) {
            sols[j * i][j * !i].len = pre_calc.t[i]->t[j].len;
            sols[j * i][j * !i].distance = pre_calc.t[i]->t[j].distance;
            sols[j * i][j * !i].tab = malloc(sizeof(*sols[j * i][j * !i].tab)
                                             * sols[j * i][j * !i].len);
            memcpy(sols[j * i][j * !i].tab,
                   pre_calc.t[i]->t[j].tab,
                   sizeof(*pre_calc.t[i]->t[j].tab) * sols[j * i][j * !i].len);
            }
        }
    for(size_t j = 1; j <= arrive.y - depart.y; j++)
        for(size_t i = 1; i <= arrive.x - depart.x; i++) {
            struct tableau *candidat = &sols[j-1][i-1];
            if(candidat->distance > sols[j-1][i].distance)
                candidat = &sols[j-1][i];
            if(candidat->distance > sols[j][i-1].distance)
                candidat = &sols[j][i-1];
            sols[j][i].distance = max(candidat->distance,
                                      dEC(data.t[0]->tab[i + depart.x],
                                          data.t[1]->tab[j + depart.y]));
            sols[j][i].len = candidat->len + 1;
            sols[j][i].tab = malloc(sizeof(*sols[j][i].tab) * sols[j][i].len);
            memcpy(sols[j][i].tab,
                   candidat->tab,
                   sizeof(*candidat->tab) * candidat->len);
            sols[j][i].tab[sols[j][i].len - 1].x = depart.x + i;
            sols[j][i].tab[sols[j][i].len - 1].y = depart.y + j;
        }
    for(size_t j = 0; j < arrive.y - depart.y; j++)
        for(size_t i = 0; i < arrive.x - depart.x; i++)
            free(sols[j][i].tab);
    for(int i = 0; i < 2 ; i++) {
        for(size_t j = 0; j < res->t[i]->len; j++) {
            size_t Y = i * j + !i * (arrive.y - depart.y);
            size_t X = !i * j + i * (arrive.x - depart.x);
            res->t[i]->t[j].len = sols[Y][X].len;
            res->t[i]->t[j].distance = sols[Y][X].distance;
            res->t[i]->t[j].tab = sols[Y][X].tab;
        }
    }
}

/*
Le repère est selon (P, Q) avec Origine le point d'origine du parcours;
Arguments:
data        Les deux chemins du fichier d'entrée (data->t[0] pour P et 1 pour Q);
depart      Pre_calc correspond à P = depart.x et Q = depart.y;
arrive      Faire tourner l'alorithme jusqu'à P = arrive.x et Q = arrive.y;
pre_calc    Valeurs précalculées:
                pre_calc.t[0] est la ligne en bas
                pre_calc.t[1] est la ligne à gauche
                pre_calc.t[0]->t[0] est le point de départ
                pre_calc.t[i]->len est ignoré;
res         Valeurs de retour:
                res->t[0] est la ligne en haut
                res->t[1] est la ligne à droite
                res->t[1]->t[res->t[1]->len - 1] est le point d'arrivée;
*/
void frechet_recursif(struct chemins data,
                      struct point depart,
                      struct point arrive,
                      struct tab_sol_2d pre_calc,
                      struct tab_sol_2d *res) {
    res->t[0]->len = arrive.x - depart.x + 1;
    res->t[1]->len = arrive.y - depart.y + 1;
    res->t[0]->t = malloc(res->t[0]->len * sizeof(*res->t[0]->t));
    res->t[1]->t = malloc(res->t[1]->len * sizeof(*res->t[1]->t));
    if(res->t[0]->len * res->t[1]->len < 200) {
        frechet_iteratif(data, depart, arrive, pre_calc, res);
        return;
    }
    #if F_DEBUG == 1
    fprintf(stderr,">(%ld %ld) (%ld %ld) récursif, vertical: %d\n",
            depart.x, depart.y, arrive.x, arrive.y,
            res->t[0]->len >= res->t[1]->len);
    #endif

    int choix = res->t[0]->len >= res->t[1]->len;
    struct point n_arrive, n_depart;
    if(choix) {
        n_depart.x = (arrive.x + depart.x) / 2;
        n_depart.y = depart.y;
        n_arrive.x = n_depart.x;
        n_arrive.y = arrive.y;
    }
    else {
        n_depart.x = depart.x;
        n_depart.y = (arrive.y + depart.y) / 2;
        n_arrive.x = arrive.x;
        n_arrive.y = n_depart.y;
    }
    struct tab_sol_2d res_a = {malloc(sizeof(struct tableau)),
                               malloc(sizeof(struct tableau))};
    frechet_recursif(data, depart, n_arrive, pre_calc, &res_a);
    struct tab_sol_2d n_pre_calc = {malloc(sizeof(struct tableau)),
                                    malloc(sizeof(struct tableau))};
    n_pre_calc.t[choix]->len = res_a.t[choix]->len;
    n_pre_calc.t[choix]->t = res_a.t[choix]->t;
    n_pre_calc.t[!choix]->len = pre_calc.t[!choix]->len - choix * n_depart.x -
                                                        !choix * n_depart.y;
    n_pre_calc.t[!choix]->t = pre_calc.t[!choix]->t +
                             choix * (n_depart.x - depart.x) +
                             !choix * (n_depart.y - depart.y);
    struct tab_sol_2d res_b = {malloc(sizeof(struct tableau)),
                               malloc(sizeof(struct tableau))};
    frechet_recursif(data, n_depart, arrive, n_pre_calc, &res_b);
    free(n_pre_calc.t[0]);
    free(n_pre_calc.t[1]);
    memcpy(res->t[!choix]->t, res_a.t[!choix]->t, sizeof(*res_a.t[!choix]->t) * res_a.t[!choix]->len);
    memcpy(res->t[!choix]->t + res_a.t[!choix]->len - 1, res_b.t[!choix]->t, sizeof(*res_b.t[!choix]->t) * res_b.t[!choix]->len);
    memcpy(res->t[choix]->t, res_b.t[choix]->t, sizeof(*res_b.t[choix]->t) * res_b.t[choix]->len);
    liberer_pre_calc(&res_a, choix, choix + 1);
    free(res_a.t[!choix]->t);
    free(res_a.t[!choix]);
    free(res_b.t[0]->t);
    free(res_b.t[0]);
    free(res_b.t[1]->t);
    free(res_b.t[1]);
    #if F_DEBUG == 1
        fprintf(stderr, "<(%ld %ld) (%ld %ld)\n",
                depart.x, depart.y, arrive.x, arrive.y);
    #endif
}

void init_pre_calc(struct tab_sol_2d pre_calc, struct chemins *data) {
    for(int i = 0; i < 2; i++) {
        struct tab_sol *actuel = pre_calc.t[i];
        actuel->len = data->t[i]->len;
        actuel->t = malloc(sizeof(*actuel->t) * actuel->len);
        actuel->t[0].len = 1;
        actuel->t[0].distance = dEC(data->t[0]->tab[0], data->t[1]->tab[0]);
        actuel->t[0].tab = malloc(sizeof(*actuel->t[0].tab));
        actuel->t[0].tab[0].x = actuel->t[0].tab[0].y = 0;
        for(size_t j = 1; j < actuel->len; j++) {
            actuel->t[j].len =  j+1;
            actuel->t[j].tab = malloc(sizeof(*actuel->t[j].tab) * actuel->t[j].len);
            memcpy(actuel->t[j].tab, actuel->t[j-1].tab, sizeof(*actuel->t[j].tab) * j);
            long candidat = dEC(data->t[!i]->tab[0], data->t[i]->tab[j]);
            if(candidat >= actuel->t[j-1].distance)
                actuel->t[j].distance = candidat;
            else
                actuel->t[j].distance = actuel->t[j-1].distance;
            actuel->t[j].tab[j].x = j * (i == 0);
            actuel->t[j].tab[j].y = j * (i != 0);
        }
    }
}

void print_result(struct tab_sol_2d res) {
    printf("Distance: %ld\n", res.t[1]->t[res.t[1]->len - 1].distance);
    printf("Nombre pas: %ld\n", res.t[1]->t[res.t[1]->len - 1].len);
    for(size_t i=0; i < res.t[1]->t[res.t[1]->len - 1].len; i++)
        printf("\t %ld %ld\n", res.t[1]->t[res.t[1]->len - 1].tab[i].x + 1,
                               res.t[1]->t[res.t[1]->len - 1].tab[i].y + 1);
    }

int main(int argc, char const *argv[]) {
    for(int f = 1; f < argc; f++) {
        FILE *input = fopen(argv[f], "r");
        if(input) {
            struct chemins *data = lire_fichier(input);
            fclose(input);
            struct point depart = {0, 0};
            struct point arrive = {data->t[0]->len - 1, data->t[1]->len - 1};
            struct tab_sol_2d res = {malloc(sizeof(struct tableau)),
                                     malloc(sizeof(struct tableau))};
            struct tab_sol_2d pre_calc = {malloc(sizeof(struct tableau)),
                                          malloc(sizeof(struct tableau))};
            init_pre_calc(pre_calc, data);
            frechet_recursif(*data, depart, arrive, pre_calc, &res);
            #if F_DEBUG == 1
                print_result(res);
            #endif
            liberer_pre_calc(&pre_calc, 0, 2);
            res.t[1]->len --;
            liberer_pre_calc(&res, 0, 2);
            liberer_chemins(data);
            free(data);
        }
        else {
            perror("Error ");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
