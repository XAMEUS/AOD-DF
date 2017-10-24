#ifndef FRECHET
#define FRECHET

#define F_DEBUG 1

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))
#define dEC(a,b) (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)

typedef struct point {
    long x;
    long y;
} point;

typedef struct tab_pts {
    size_t len, distance;
    point *t;
} tab_pts;

typedef struct chemins {
    tab_pts *t[2];
} chemins;

typedef struct tab_tab_pts {
    size_t len;
    tab_pts *t;
} tab_tab_pts;

typedef struct tab_sol_2d {
    tab_tab_pts *t[2];
} deux_tab_sol_2d;

#endif
