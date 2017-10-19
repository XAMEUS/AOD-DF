#include<stdio.h>
#include<stdlib.h>

struct point {
    long x;
    long y;
};

struct tableau {
    size_t len;
    struct point *tab;
};

struct chemins {
    struct tableau *p, *q;
};

struct chemins *lire_fichier(FILE* fichier) {
    struct chemins *data = malloc(sizeof(struct chemins));
    data->p = malloc(sizeof(struct tableau));
    data->q = malloc(sizeof(struct tableau));
    fscanf(fichier, "%lu %lu\n", &(data->p->len), &(data->q->len));
    data->p->tab = malloc(data->p->len * sizeof(struct point));
    data->q->tab = malloc(data->q->len * sizeof(struct point));
    for (size_t i = 0; !feof(fichier) && i < data->p->len; i++)
        fscanf(fichier, "%ld %ld", &data->p->tab[i].x, &data->p->tab[i].y);
    for (size_t i = 0; !feof(fichier) && i < data->q->len; i++)
        fscanf(fichier, "%ld %ld", &data->q->tab[i].x, &data->q->tab[i].y);
    return data;
}

void liberer_chemins(struct chemins *data) {
    free(data->p->tab);
    free(data->p);
    free(data->q->tab);
    free(data->q);
    free(data);
}

int main(int argc, char const *argv[]) {
    for(int i=1; i < argc; i++) {
        FILE *input = fopen(argv[i], "r");
        if(input) {
            struct chemins * data = lire_fichier(input);
            fclose(input);
            liberer_chemins(data);
        }
        else perror("Error: ");
    }
    return 0;
}
