#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define max(a,b) (((a)>(b))?(a):(b))
#define dEC(a,b) (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)

struct point {
    long x;
    long y;
};

struct tableau {
    size_t len, distance;
    struct point *tab;
};

struct chemins {
    struct tableau *t[2];
};

struct tableau_sol {
    size_t len;
    struct tableau *t;
};

struct tab_solution {
    struct tableau_sol *t[2];
};

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

void liberer_pre_calc(struct tab_solution *data) {
    //TODO
}

/*
Repère selon (P, Q)
struct chemins pre_calc, res: p représente un côté gauche ou droit
struct point depart, arrive: x représente les bornes de p
*/
struct tab_solution frechet_recursif(struct chemins data,
                                     struct point depart,
                                     struct point arrive,
                                     struct tab_solution pre_calc) {
    struct tab_solution res = {malloc(sizeof(struct tableau)),
                          malloc(sizeof(struct tableau))};
    res.t[0]->len = arrive.x - depart.x + 1;
    res.t[1]->len = arrive.y - depart.y + 1;
    res.t[0]->t = malloc(res.t[0]->len * sizeof(*res.t[0]->t));
    res.t[1]->t = malloc(res.t[1]->len * sizeof(*res.t[0]->t));
    printf(">%ld %ld %ld %ld\n", depart.x, depart.y, arrive.x, arrive.y);
    if(res.t[0]->len + res.t[1]->len < 8) { //TODO: number?
        // frechet_iteratif();//TODO
        return res;
    }
    int choix = res.t[0]->len >= res.t[1]->len;
    struct point n_arrive, n_depart;
    if(choix) {
        n_arrive.x = (arrive.x + depart.x) / 2;
        n_depart.x = (arrive.x + depart.x) / 2;
        n_arrive.y = arrive.y;
        n_depart.y = depart.y;
    }
    else {
        n_arrive.y = (arrive.y + depart.y) / 2;
        n_depart.y = (arrive.y + depart.y) / 2;
        n_arrive.x = arrive.x;
        n_depart.x = depart.x;
    }
    struct tab_solution res_a = frechet_recursif(data, depart, n_arrive, pre_calc);
    struct tab_solution n_pre_calc = {malloc(sizeof(struct tableau)),
                                      malloc(sizeof(struct tableau))};
    n_pre_calc.t[!choix]->len = res_a.t[!choix]->len;
    n_pre_calc.t[!choix]->t = res_a.t[!choix]->t;
    n_pre_calc.t[choix]->len = choix * (arrive.x - n_depart.x)
                               + !choix * (arrive.y - n_depart.y) + 1;
    n_pre_calc.t[choix]->t = pre_calc.t[choix]->t + n_depart.x;
    struct tab_solution res_b = frechet_recursif(data, n_depart, arrive, n_pre_calc);
    free(n_pre_calc.t[!choix]);
    free(n_pre_calc.t[choix]);
    memcpy(res.t[choix]->t, res_a.t[choix]->t, res_a.t[choix]->len);
    memcpy(res.t[choix]->t + res_a.t[choix]->len,
           res_b.t[choix]->t, res_b.t[choix]->len);
    memcpy(res.t[!choix]->t, res_b.t[!choix]->t, res.t[!choix]->len);
    liberer_pre_calc(&res_a);
    liberer_pre_calc(&res_b);
    printf("<%ld %ld %ld %ld\n", depart.x, depart.y, arrive.x, arrive.y);
    return res;
}

void init_pre_calc(struct tab_solution pre_calc, struct chemins *data) {
    for(int i = 0; i < 2; i++) {
        struct tableau_sol *actuel = pre_calc.t[i];
        actuel->len = data->t[i]->len;
        actuel->t = malloc(sizeof(*actuel->t) * actuel->len);
        actuel->t[0].len = 1;
        actuel->t[0].distance = dEC(data->t[0]->tab[0], data->t[1]->tab[0]);
        actuel->t[0].tab = malloc(sizeof(*actuel->t[0].tab));
        actuel->t[0].tab[0].x = actuel->t[0].tab[0].y = 0;
        for(size_t j = 1; j < actuel->len; j++) {
            actuel->t[j].len = 1;
            actuel->t[j].tab = malloc(sizeof(*actuel->t[j].tab));
            long candidat = dEC(data->t[!i]->tab[0], data->t[i]->tab[j]);
            if(candidat >= actuel->t[j-1].distance) {
                actuel->t[j].distance = candidat;
                actuel->t[j].tab[0].x = j * (i == 0);
                actuel->t[j].tab[0].y = j * (i != 0);

            } else {
                actuel->t[j].distance = actuel->t[j-1].distance;
                actuel->t[j].tab[0] = actuel->t[j-1].tab[0];
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    for(int f = 1; f < argc; f++) {
        FILE *input = fopen(argv[f], "r");
        if(input) {
            struct chemins *data = lire_fichier(input);
            fclose(input);
            struct point depart = {0, 0};
            struct point arrive = {data->t[0]->len - 1, data->t[1]->len - 1};
            struct tab_solution pre_calc = {malloc(sizeof(struct tableau)),
                                       malloc(sizeof(struct tableau))};
            init_pre_calc(pre_calc, data);
            frechet_recursif(*data, depart, arrive, pre_calc);
            liberer_pre_calc(&pre_calc);
            liberer_chemins(data);
            free(data);
        }
        else perror("Error: ");
    }
    return EXIT_SUCCESS;
}
