/**
 * @file doxygen_c.h
 * @author Bouvier Pierre, Gourgoulhon Maxime
 * @date 27 Oct 2017
 * @brief Calcul de la distance de Frechet.
 *
 */

#ifndef FRECHET
#define FRECHET

#define F_DEBUG 0

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define max(a,b) (((a)>(b))?(a):(b))
#define dEC(a,b) (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y)

typedef struct point {
    long x;
    long y;
} point;

typedef struct tab_pts {
    size_t len;
    point *t;
} tab_pts;

typedef struct chemins {
    tab_pts *t[2];
} chemins;

typedef struct l_pts {
    size_t cpt, distance;
    long x, y;
    struct l_pts *n;
} l_pts;

typedef struct tab_l_pts {
    size_t len;
    l_pts **t;
} tab_l_pts;

typedef struct deux_tab_l_pts {
    tab_l_pts *t[2];
} deux_tab_l_pts;

#endif
