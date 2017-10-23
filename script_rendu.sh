mkdir TPfrechet
mkdir TPfrechet/bin
cp bin/frechet.c TPfrechet/bin/.
cp Makefile TPfrechet/.
tar cvzf TPfrechet.tgz TPfrechet
rm -rf TPfrechet
