#!/bin/bash

# extraction de l'archive et compilation : le Makefile doit se trouver directement dans le répertoire TPfrechet, et le fichier exécutable obtenu doit se trouver dans le répertoire TPfrechet/bin/, et s'intituler "frechet" ; autrement dit, le make doit construire le fichier TPfrechet/bin/frechet qui sera exécuté par la suite.

tar -xf ./TPfrechet.tgz
cd TPfrechet/
make




# calcul des solutions pour les différentes entrées test, benchmark, ainsi que pour quelques fichiers d'entrée verif auxquels vous n'avez pas accès pour s'assurer de la correction du programme ; il est à noter que l'appel du programme frechet sur la chaîne de caractères "../inoutfiles/test1" doit lire le fichier "../inoutfiles/test1.in" et construire un fichier "../inoutfiles/test1.out" (c'est-à-dire dans le même répertoire que le fichier .in)

./bin/frechet ../inoutfiles/test1
./bin/frechet ../inoutfiles/test2
./bin/frechet ../inoutfiles/test3
./bin/frechet ../inoutfiles/test4

./bin/frechet ../inoutfiles/benchmark1
./bin/frechet ../inoutfiles/benchmark2
./bin/frechet ../inoutfiles/benchmark3
./bin/frechet ../inoutfiles/benchmark4

./bin/frechet ../inoutfiles/verif1
./bin/frechet ../inoutfiles/verif2
./bin/frechet ../inoutfiles/verif3
./bin/frechet ../inoutfiles/verif4




# les fichiers .out produits seront alors vérifiés grâce à un programme verif_solution qui s'assure que le fichier produit est correctement formaté, et qu'il fournit un solution correcte : la distance de Fréchet au carré est la bonne, et le parcours proposé est effectivement un parcours optimal. Le programme verif_solution ne vous est pas fourni ; vous pouvez le programmer vous-même si vous le souhaitez (mais on ne vous demande pas de le fournir).

cd ..

./verif_solution ./inoutfiles/test1
./verif_solution ./inoutfiles/test2
./verif_solution ./inoutfiles/test3
./verif_solution ./inoutfiles/test4

./verif_solution ./inoutfiles/benchmark1
./verif_solution ./inoutfiles/benchmark2
./verif_solution ./inoutfiles/benchmark3
./verif_solution ./inoutfiles/benchmark4

./verif_solution ./inoutfiles/verif1
./verif_solution ./inoutfiles/verif2
./verif_solution ./inoutfiles/verif3
./verif_solution ./inoutfiles/verif4







