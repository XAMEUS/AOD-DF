#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "frechet.h"

chemins *lire_fichier(FILE* fichier) {
    chemins *data = malloc(sizeof(chemins));
    data->t[0] = malloc(sizeof(tab_pts));
    data->t[1] = malloc(sizeof(tab_pts));
    fscanf(fichier, "%lu %lu\n", &(data->t[0]->len), &(data->t[1]->len));
    data->t[0]->t = malloc(data->t[0]->len * sizeof(point));
    data->t[1]->t = malloc(data->t[1]->len * sizeof(point));
    for (size_t i = 0; !feof(fichier) && i < data->t[0]->len; i++)
        fscanf(fichier, "%ld %ld", &(data->t[0]->t[i]).x,
                                   &(data->t[0]->t[i]).y);
    for (size_t i = 0; !feof(fichier) && i < data->t[1]->len; i++)
        fscanf(fichier, "%ld %ld", &(data->t[1]->t[i]).x,
                                   &(data->t[1]->t[i]).y);
    return data;
}

void liberer_chemins(chemins *data) {
    free(data->t[0]->t);
    free(data->t[0]);
    free(data->t[1]->t);
    free(data->t[1]);
}

void liberer_pre_calc(deux_tab_sol_2d *data, int debut, int fin) {
    for(int i = debut; i < fin; i++) {
        tab_tab_pts *cur_i = data->t[i];
        for(size_t j = 0; j < cur_i->len; j++)
            free(cur_i->t[j].t);
        free(cur_i->t);
        free(cur_i);
    }
}
/*
Le repère est selon (P, Q) avec Origine le point d'origine du parcours;
Arguments:
data        Les deux chemins du fichier d'entrée (data->t[0] pour P, 1 pour Q);
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
void frechet_iteratif(chemins data,
                      point depart,
                      point arrive,
                      deux_tab_sol_2d pre_calc,
                      deux_tab_sol_2d *res) {
    #if F_DEBUG > 1
    fprintf(stderr,">(%ld %ld) (%ld %ld) itératif\n",
          depart.x, depart.y, arrive.x, arrive.y);
    #endif
    tab_pts sols[arrive.y - depart.y + 1][arrive.x - depart.x + 1];
    for(int i = 0; i < 2 ; i++) {
        size_t borne = 1 + (arrive.y - depart.y) * i +
                           (arrive.x - depart.x) * !i;
        for(size_t j = i; j < borne; j++) {
            sols[j * i][j * !i].len = pre_calc.t[i]->t[j].len;
            sols[j * i][j * !i].distance = pre_calc.t[i]->t[j].distance;
            sols[j * i][j * !i].t = malloc(sizeof(*sols[j * i][j * !i].t)
                                             * sols[j * i][j * !i].len);
            memcpy(sols[j * i][j * !i].t,
                   pre_calc.t[i]->t[j].t,
                   sizeof(*pre_calc.t[i]->t[j].t) * sols[j * i][j * !i].len);
            }
        }
    for(size_t j = 1; j <= arrive.y - depart.y; j++)
        for(size_t i = 1; i <= arrive.x - depart.x; i++) {
            tab_pts *candidat = &sols[j-1][i-1];
            if(candidat->distance > sols[j-1][i].distance)
                candidat = &sols[j-1][i];
            if(candidat->distance > sols[j][i-1].distance)
                candidat = &sols[j][i-1];
            sols[j][i].distance = max(candidat->distance,
                                      dEC(data.t[0]->t[i + depart.x],
                                          data.t[1]->t[j + depart.y]));
            sols[j][i].len = candidat->len + 1;
            sols[j][i].t = malloc(sizeof(*sols[j][i].t) * sols[j][i].len);
            memcpy(sols[j][i].t,
                   candidat->t,
                   sizeof(*candidat->t) * candidat->len);
            sols[j][i].t[sols[j][i].len - 1].x = depart.x + i;
            sols[j][i].t[sols[j][i].len - 1].y = depart.y + j;
        }
    for(size_t j = 0; j < arrive.y - depart.y; j++)
        for(size_t i = 0; i < arrive.x - depart.x; i++)
            free(sols[j][i].t);
    for(int i = 0; i < 2 ; i++) {
        for(size_t j = 0; j < res->t[i]->len; j++) {
            size_t Y = i * j + !i * (arrive.y - depart.y);
            size_t X = !i * j + i * (arrive.x - depart.x);
            res->t[i]->t[j].len = sols[Y][X].len;
            res->t[i]->t[j].distance = sols[Y][X].distance;
            res->t[i]->t[j].t = sols[Y][X].t;
        }
    }
}

/*
Le repère est selon (P, Q) avec Origine le point d'origine du parcours;
Arguments:
data        Les deux chemins du fichier d'entrée (data->t[0] pour P, 1 pour Q);
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
void frechet_recursif(chemins data,
                      point depart,
                      point arrive,
                      deux_tab_sol_2d pre_calc,
                      deux_tab_sol_2d *res) {
    res->t[0]->len = arrive.x - depart.x + 1;
    res->t[1]->len = arrive.y - depart.y + 1;
    res->t[0]->t = malloc(res->t[0]->len * sizeof(*res->t[0]->t));
    res->t[1]->t = malloc(res->t[1]->len * sizeof(*res->t[1]->t));
    if(res->t[0]->len * res->t[1]->len < 200) {
        frechet_iteratif(data, depart, arrive, pre_calc, res);
        return;
    }
    #if F_DEBUG > 1
    fprintf(stderr,">(%ld %ld) (%ld %ld) récursif, vertical: %d\n",
            depart.x, depart.y, arrive.x, arrive.y,
            res->t[0]->len >= res->t[1]->len);
    #endif

    int choix = res->t[0]->len >= res->t[1]->len;
    point n_arrive, n_depart;
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
    deux_tab_sol_2d res_a = {malloc(sizeof(tab_pts)),
                               malloc(sizeof(tab_pts))};
    frechet_recursif(data, depart, n_arrive, pre_calc, &res_a);
    deux_tab_sol_2d n_pre_calc = {malloc(sizeof(tab_pts)),
                                    malloc(sizeof(tab_pts))};
    n_pre_calc.t[choix]->len = res_a.t[choix]->len;
    n_pre_calc.t[choix]->t = res_a.t[choix]->t;
    n_pre_calc.t[!choix]->len = pre_calc.t[!choix]->len - choix * n_depart.x -
                                                        !choix * n_depart.y;
    n_pre_calc.t[!choix]->t = pre_calc.t[!choix]->t +
                             choix * (n_depart.x - depart.x) +
                             !choix * (n_depart.y - depart.y);
    deux_tab_sol_2d res_b = {malloc(sizeof(tab_pts)),
                               malloc(sizeof(tab_pts))};
    frechet_recursif(data, n_depart, arrive, n_pre_calc, &res_b);
    free(n_pre_calc.t[0]);
    free(n_pre_calc.t[1]);
    memcpy(res->t[!choix]->t,
           res_a.t[!choix]->t,
           sizeof(*res_a.t[!choix]->t) * res_a.t[!choix]->len);
    memcpy(res->t[!choix]->t + res_a.t[!choix]->len - 1,
           res_b.t[!choix]->t,
           sizeof(*res_b.t[!choix]->t) * res_b.t[!choix]->len);
    memcpy(res->t[choix]->t,
           res_b.t[choix]->t,
           sizeof(*res_b.t[choix]->t) * res_b.t[choix]->len);
    liberer_pre_calc(&res_a, choix, choix + 1);
    free(res_a.t[!choix]->t);
    free(res_a.t[!choix]);
    free(res_b.t[0]->t);
    free(res_b.t[0]);
    free(res_b.t[1]->t);
    free(res_b.t[1]);
    #if F_DEBUG > 1
        fprintf(stderr, "<(%ld %ld) (%ld %ld)\n",
                depart.x, depart.y, arrive.x, arrive.y);
    #endif
}

void init_pre_calc(deux_tab_sol_2d pre_calc, chemins *data) {
    for(int i = 0; i < 2; i++) {
        tab_tab_pts *actuel = pre_calc.t[i];
        actuel->len = data->t[i]->len;
        actuel->t = malloc(sizeof(*actuel->t) * actuel->len);
        actuel->t[0].len = 1;
        actuel->t[0].distance = dEC(data->t[0]->t[0], data->t[1]->t[0]);
        actuel->t[0].t = malloc(sizeof(*actuel->t[0].t));
        actuel->t[0].t[0].x = actuel->t[0].t[0].y = 0;
        for(size_t j = 1; j < actuel->len; j++) {
            actuel->t[j].len =  j+1;
            actuel->t[j].t = malloc(sizeof(*actuel->t[j].t) * actuel->t[j].len);
            memcpy(actuel->t[j].t,
                   actuel->t[j-1].t,
                   sizeof(*actuel->t[j].t) * j);
            long candidat = dEC(data->t[!i]->t[0], data->t[i]->t[j]);
            if(candidat >= actuel->t[j-1].distance)
                actuel->t[j].distance = candidat;
            else
                actuel->t[j].distance = actuel->t[j-1].distance;
            actuel->t[j].t[j].x = j * (i == 0);
            actuel->t[j].t[j].y = j * (i != 0);
        }
    }
}

void print_result(deux_tab_sol_2d res) {
    printf("Distance: %ld\n", res.t[1]->t[res.t[1]->len - 1].distance);
    printf("Nombre pas: %ld\n", res.t[1]->t[res.t[1]->len - 1].len);
    for(size_t i=0; i < res.t[1]->t[res.t[1]->len - 1].len; i++)
        printf("\t %ld %ld\n", res.t[1]->t[res.t[1]->len - 1].t[i].x + 1,
                               res.t[1]->t[res.t[1]->len - 1].t[i].y + 1);
    }

void ecrire_fichier(FILE *out, deux_tab_sol_2d res) {
    fprintf(out, "%ld\n", res.t[1]->t[res.t[1]->len - 1].distance);
    fprintf(out, "%ld\n", res.t[1]->t[res.t[1]->len - 1].len);
    for(size_t i=0; i < res.t[1]->t[res.t[1]->len - 1].len; i++)
        fprintf(out, "%ld %ld ", res.t[1]->t[res.t[1]->len - 1].t[i].x + 1,
                               res.t[1]->t[res.t[1]->len - 1].t[i].y + 1);
}


int main(int argc, char const *argv[]) {
    for(int f = 1; f < argc; f++) {
        FILE *input = fopen(argv[f], "r");
        if(input) {
            chemins *data = lire_fichier(input);
            fclose(input);
            point depart = {0, 0};
            point arrive = {data->t[0]->len - 1, data->t[1]->len - 1};
            deux_tab_sol_2d res = {malloc(sizeof(tab_pts)),
                                     malloc(sizeof(tab_pts))};
            deux_tab_sol_2d pre_calc = {malloc(sizeof(tab_pts)),
                                          malloc(sizeof(tab_pts))};
            init_pre_calc(pre_calc, data);
            frechet_recursif(*data, depart, arrive, pre_calc, &res);
            #if F_DEBUG >= 1
                print_result(res);
            #endif
            char fname[128];
            strcpy(fname, argv[f]);
            strcat(fname, ".out");
            FILE *out = fopen(fname, "w");
            ecrire_fichier(out, res);
            fclose(out);
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
