#ifndef FRECHET
#define FRECHET

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))
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

struct tab_sol {
    size_t len;
    struct tableau *t;
};

struct tab_sol_2d {
    struct tab_sol *t[2];
};

#endif
