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
    // free(data->t[0]->t);
    // free(data->t[0]);
    // free(data->t[1]->t);
    // free(data->t[1]);
}

void liberer_pre_calc(deux_tab_l_pts *data, int debut, int fin) {
    // for(int i = debut; i < fin; i++) {
    //     tab_tab_pts *cur_i = data->t[i];
    //     for(size_t j = 0; j < cur_i->len; j++)
    //         free(cur_i->t[j].t);
    //     free(cur_i->t);
    //     free(cur_i);
    // }
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
                      deux_tab_l_pts pre_calc,
                      deux_tab_l_pts *res) {
    #if F_DEBUG > 1
    fprintf(stderr,">(%ld %ld) (%ld %ld) itératif\n",
          depart.x, depart.y, arrive.x, arrive.y);
    #endif
    l_pts *sols[arrive.y - depart.y + 1][arrive.x - depart.x + 1];
    for(int i = 0; i < 2 ; i++) {
        size_t borne = 1 + (arrive.y - depart.y) * i +
                           (arrive.x - depart.x) * !i;
        for(size_t j = i; j < borne; j++)
            sols[j * i][j * !i] = pre_calc.t[i]->t[j];
    }
    for(size_t j = 1; j <= arrive.y - depart.y; j++)
        for(size_t i = 1; i <= arrive.x - depart.x; i++) {
            sols[j][i] = malloc(sizeof(*sols[j][i]));
            sols[j][i]->n = sols[j-1][i-1];
            if(sols[j][i]->n->distance > sols[j-1][i]->distance)
                sols[j][i]->n = sols[j-1][i];
            if(sols[j][i]->n->distance > sols[j][i-1]->distance)
                sols[j][i]->n = sols[j][i-1];
            sols[j][i]->n->cpt++;
            sols[j][i]->cpt = 1;
            sols[j][i]->distance = max(sols[j][i]->n->distance,
                                       dEC(data.t[0]->t[i + depart.x],
                                           data.t[1]->t[j + depart.y]));
            sols[j][i]->x = depart.x + i;
            sols[j][i]->y = depart.y + j;
        }
    // for(size_t j = 0; j < arrive.y - depart.y; j++) //TODO FREE
    //     for(size_t i = 0; i < arrive.x - depart.x; i++)
    //         free(sols[j][i].t);
    for(int i = 0; i < 2 ; i++) {
        for(size_t j = 0; j < res->t[i]->len; j++) {
            size_t Y = i * j + !i * (arrive.y - depart.y);
            size_t X = !i * j + i * (arrive.x - depart.x);
            res->t[i]->t[j] = sols[Y][X];
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
                      deux_tab_l_pts pre_calc,
                      deux_tab_l_pts *res) {
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
    deux_tab_l_pts res_a = {malloc(sizeof(tab_l_pts)),
                            malloc(sizeof(tab_l_pts))}; //TODO voir
    frechet_recursif(data, depart, n_arrive, pre_calc, &res_a);
    deux_tab_l_pts n_pre_calc = {malloc(sizeof(tab_l_pts)),
                                 malloc(sizeof(tab_l_pts))}; //TODO voir
    n_pre_calc.t[choix]->len = res_a.t[choix]->len;
    n_pre_calc.t[choix]->t = res_a.t[choix]->t;
    n_pre_calc.t[!choix]->len = pre_calc.t[!choix]->len - choix * n_depart.x -
                                                        !choix * n_depart.y;
    n_pre_calc.t[!choix]->t = pre_calc.t[!choix]->t +
                             choix * (n_depart.x - depart.x) +
                             !choix * (n_depart.y - depart.y);
    for(int i = 0; i < 2; i++)
        for (size_t j = 0; j < n_pre_calc.t[i]->len; j++)
            n_pre_calc.t[i]->t[j]->cpt++;
    deux_tab_l_pts res_b = {malloc(sizeof(tab_l_pts)),
                            malloc(sizeof(tab_l_pts))}; //TODO voir
    frechet_recursif(data, n_depart, arrive, n_pre_calc, &res_b);
    memcpy(res->t[!choix]->t,
           res_a.t[!choix]->t,
           sizeof(*res_a.t[!choix]->t) * res_a.t[!choix]->len);
    memcpy(res->t[!choix]->t + res_a.t[!choix]->len - 1,
           res_b.t[!choix]->t,
           sizeof(*res_b.t[!choix]->t) * res_b.t[!choix]->len);
    memcpy(res->t[choix]->t,
           res_b.t[choix]->t,
           sizeof(*res_b.t[choix]->t) * res_b.t[choix]->len);
    for(size_t i = 0; i < res->t[0]->len; i++)
        res->t[0]->t[i]->cpt++;
    for(size_t i = 0; i < res->t[1]->len; i++)
        res->t[1]->t[i]->cpt++;
    //TODO FREE
    // free(n_pre_calc.t[0]);
    // free(n_pre_calc.t[1]);
    // liberer_pre_calc(&res_a, choix, choix + 1);
    // free(res_a.t[!choix]->t);
    // free(res_a.t[!choix]);
    // free(res_b.t[0]->t);
    // free(res_b.t[0]);
    // free(res_b.t[1]->t);
    // free(res_b.t[1]);
    #if F_DEBUG > 1
        fprintf(stderr, "<(%ld %ld) (%ld %ld)\n",
                depart.x, depart.y, arrive.x, arrive.y);
    #endif
}

void init_pre_calc(deux_tab_l_pts pre_calc, chemins *data) {
    pre_calc.t[0]->len = data->t[0]->len;
    pre_calc.t[0]->t = malloc(sizeof(*pre_calc.t[0]->t) * pre_calc.t[0]->len);
    pre_calc.t[1]->len = data->t[1]->len;
    pre_calc.t[1]->t = malloc(sizeof(*pre_calc.t[1]->t) * pre_calc.t[1]->len);

    pre_calc.t[0]->t[0] = malloc(sizeof(*pre_calc.t[0]->t[0]));
    pre_calc.t[1]->t[0] = pre_calc.t[0]->t[0];
    pre_calc.t[0]->t[0]->cpt = 2;
    pre_calc.t[0]->t[0]->distance = dEC(data->t[0]->t[0], data->t[1]->t[0]);
    pre_calc.t[0]->t[0]->x = 0;
    pre_calc.t[0]->t[0]->y = 0;
    pre_calc.t[0]->t[0]->n = NULL;

    for(int i = 0; i < 2; i++) {
        tab_l_pts *actuel = pre_calc.t[i];
        for(size_t j = 1; j < actuel->len; j++) {
            pre_calc.t[i]->t[j] = malloc(sizeof(*pre_calc.t[i]->t[j]));
            actuel->t[j]->cpt = 1;
            long candidat = dEC(data->t[!i]->t[0], data->t[i]->t[j]);
            if(candidat >= actuel->t[j-1]->distance)
                actuel->t[j]->distance = candidat;
            else
                actuel->t[j]->distance = actuel->t[j-1]->distance;
            actuel->t[j]->x = j * (i == 0);
            actuel->t[j]->y = j * (i != 0);
            actuel->t[j]->n = actuel->t[j-1];
            actuel->t[j-1]->cpt++;
        }
    }
}

void print_result(deux_tab_l_pts res) {
    l_pts* tete = res.t[1]->t[res.t[1]->len - 1];
    printf("Distance: %ld\n", tete->distance);
    for(l_pts* i = tete; i != NULL; i = i->n)
        printf("\t%ld %ld\n", i->x + 1, i->y + 1);
}

void ecrire_fichier(FILE *out, deux_tab_l_pts res) {
    // fprintf(out, "%ld\n", res.t[1]->t[res.t[1]->len - 1].distance);
    // fprintf(out, "%ld\n", res.t[1]->t[res.t[1]->len - 1].len);
    // for(size_t i=0; i < res.t[1]->t[res.t[1]->len - 1].len; i++)
    //     fprintf(out, "%ld %ld ", res.t[1]->t[res.t[1]->len - 1].t[i].x + 1,
    //                            res.t[1]->t[res.t[1]->len - 1].t[i].y + 1);
}


int main(int argc, char const *argv[]) {
    for(int f = 1; f < argc; f++) {
        FILE *input = fopen(argv[f], "r");
        if(input) {
            chemins *data = lire_fichier(input);
            fclose(input);
            point depart = {0, 0};
            point arrive = {data->t[0]->len - 1, data->t[1]->len - 1};
            deux_tab_l_pts res = {malloc(sizeof(tab_l_pts)),
                                     malloc(sizeof(tab_l_pts))}; //TODO voir
            deux_tab_l_pts pre_calc = {malloc(sizeof(tab_l_pts)),
                                          malloc(sizeof(tab_l_pts))}; //TODO voir
            init_pre_calc(pre_calc, data);
            frechet_recursif(*data, depart, arrive, pre_calc, &res);
            #if F_DEBUG >= 1
                print_result(res);
            #endif
            // ecrire_fichier(out, res); // tout doux
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
