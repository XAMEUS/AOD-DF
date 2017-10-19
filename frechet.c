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
    size_t len;
    void *tab;
};

struct chemins {
    struct tableau *t[2];
};

struct chemins *lire_fichier(FILE* fichier) {
    struct chemins *data = malloc(sizeof(struct chemins));
    data->t[0] = malloc(sizeof(struct tableau));
    data->t[1] = malloc(sizeof(struct tableau));
    fscanf(fichier, "%lu %lu\n", &(data->t[0]->len), &(data->t[1]->len));
    data->t[0]->tab = malloc(data->t[0]->len * sizeof(struct point));
    data->t[1]->tab = malloc(data->t[1]->len * sizeof(struct point));
    for (size_t i = 0; !feof(fichier) && i < data->t[0]->len; i++)
        fscanf(fichier, "%ld %ld", &(((struct point*) data->t[0]->tab)[i]).x,
                                   &(((struct point*) data->t[0]->tab)[i]).y);
    for (size_t i = 0; !feof(fichier) && i < data->t[1]->len; i++)
        fscanf(fichier, "%ld %ld", &(((struct point*) data->t[1]->tab)[i]).x,
                                   &(((struct point*) data->t[1]->tab)[i]).y);
    return data;
}

void liberer_chemins(struct chemins *data) {
    free(data->t[0]->tab);
    free(data->t[0]);
    free(data->t[1]->tab);
    free(data->t[1]);
}
/*
Repère selon (P, Q)
struct chemins pre_calc, res: p représente un côté gauche ou droit
struct point depart, arrive: x représente les bornes de p
*/
struct chemins frechet_recursif(struct chemins data, struct point depart, struct point arrive, struct chemins pre_calc) {
    struct chemins res = {malloc(sizeof(struct tableau)),
                          malloc(sizeof(struct tableau))};
    res.t[0]->len = arrive.x - depart.x;
    res.t[1]->len = arrive.y - depart.y;
    res.t[0]->tab = malloc(res.t[0]->len * sizeof(struct point));
    res.t[1]->tab = malloc(res.t[1]->len * sizeof(struct point));
    printf("%ld %ld %ld %ld\n", depart.x, depart.y, arrive.x, arrive.y);
    if(res.t[0]->len + res.t[1]->len < 4) { //TODO: number?
        // frechet_iteratif();//TODO
        return res;
    }
    int choix = res.t[0]->len >= res.t[1]->len;
    struct point n_arrive, n_depart;
    if(choix) {
        n_arrive.x = n_depart.x = (arrive.x + depart.x) / 2;
        n_arrive.y = arrive.y;
        n_depart.y = depart.y;
    }
    else {
        n_arrive.y = n_depart.y = (arrive.y + depart.y) / 2;
        n_arrive.x = arrive.x;
        n_depart.x = depart.x;
    }
    struct chemins res_a = frechet_recursif(data, depart, n_arrive, pre_calc);
    struct chemins n_pre_calc = {malloc(sizeof(struct tableau)), malloc(sizeof(struct tableau))};
    n_pre_calc.t[!choix]->len = res_a.t[!choix]->len;
    n_pre_calc.t[!choix]->tab = res_a.t[!choix]->tab;
    n_pre_calc.t[choix]->len = arrive.x - n_depart.x;
    n_pre_calc.t[choix]->tab = pre_calc.t[choix]->tab + n_depart.x;
    struct chemins res_b = frechet_recursif(data, n_depart, arrive, n_pre_calc);
    free(n_pre_calc.t[!choix]);
    free(n_pre_calc.t[choix]);
    memcpy(res.t[choix]->tab, res_a.t[choix]->tab, res_a.t[choix]->len);
    memcpy(res.t[choix]->tab + res_a.t[choix]->len, res_b.t[choix]->tab, res_b.t[choix]->len);
    memcpy(res.t[!choix]->tab, res_b.t[!choix]->tab, res.t[!choix]->len);
    liberer_chemins(&res_a);
    liberer_chemins(&res_b);
    return res;
}

int main(int argc, char const *argv[]) {
    for(int f = 1; f < argc; f++) {
        FILE *input = fopen(argv[f], "r");
        if(input) {
            struct chemins *data = lire_fichier(input);
            fclose(input);
            struct point depart = {0, 0};
            struct point arrive = {data->t[0]->len - 1, data->t[1]->len - 1};
            struct chemins pre_calc = {malloc(sizeof(struct tableau)), malloc(sizeof(struct tableau))};
            for(int i = 0; i < 2; i++) {
                pre_calc.t[i]->len = data->t[i]->len;
                pre_calc.t[i]->tab = malloc(sizeof(long) * pre_calc.t[i]->len);
                ((long*) pre_calc.t[i]->tab)[0] = dEC(((struct point*) data->t[0]->tab)[0], ((struct point*) data->t[1]->tab)[0]);
                for(size_t j = 1; j < pre_calc.t[i]->len; j++)
                    ((long*) pre_calc.t[i]->tab)[j] = max(((long*) pre_calc.t[i]->tab)[j-1], dEC(((struct point*) data->t[!i]->tab)[0], ((struct point*) data->t[i]->tab)[j]));
            }
            frechet_recursif(*data, depart, arrive, pre_calc);
            liberer_chemins(&pre_calc);
            liberer_chemins(data);
            free(data);
        }
        else perror("Error: ");
    }
    return EXIT_SUCCESS;
}
