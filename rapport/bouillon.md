/!\ PREREQUIS LOGICIEL /!\

- cmake
- gnuplot

Compilation et exécutinon
-----------------------------------------------------

Nettoyage et compilation
make mrproper
make clean
make 

Lancement de test
make test

Génération de graphique
make bench-data 
make bench-graph

Les diff érents fichiers
-------------------------------------------------------
 - bin / (Ensemble des exécutable)
   -> bench: exécutable utilisé pour la génération de graph 
   -> test: tout les test
 - script / (Ensemble des script utilisé)
   -> bench / 
      -> generate-data.sh : génération de donnée necessaire aux graphs
      -> generate-graph.sh : génération des graphique (necessite gnuplot)
      -> gnuplot.in : configuration de gnuplot
      -> output /
         -> data.out : donnée pour les graphs
         -> table.out : tableau pour le rapport
         -> graph.png : graphique généré
   -> test /
      -> test.sh : script de test
      -> output /
           *.expected : output attendu des programmes de test
           *.out : output lors de la dernière exécution
 - src : tout les code source
   -> bench 
   -> conduct
   -> tests 
   -> utils 

2) Fonctionnement basique
-------------------------------------------------------

Nous avons choisi d'utiliser un buffer cirtulaire avec des mutex.















3) Comment faire les tests et qu'est ce qu'ils font

test 1 : simple ecriture et lecture dans un conduit annonyme
test 2 : simple ecriture et lecture dans un conduit utilisant un fichier
test 3 : simple ecriture et lecture de vecteur dans un conduit annonyme
test 4 : test EoF sur read (return 0)
test 5 : test Eof sur write (error)
test 6 : test de fork sur conduit annonyme
test 7 : test de fork sur conduit nommé

4) Benchmarks, fonctionnement et résultats 

Pour tester nous avons utilisé deux processus. Un écrivains et un lecteur. 
Chacun utilisant un buffer de 1000 octets pour ecrire ou lire.

Génération de graphique
make bench-data 
make bench-graph (Necessite gnuplot)

Configuration materiel utlisé:
- Intel(R) Core(TM) i7-4790K CPU @ 4.00GHz
- 16Go RAM @ 1600MHz CAS9

Table:
/rapport/resources/table.out

Coubre constaté:
- Conduct:  2.1e-4 + ( x - 3e5) / 70.0e8) 
- Pipe:     3.1e-4 + ( x - 3e5) / 17.7e8) 
- Socket:   2.7e-4 + ( x - 3e5) / 22.4e8) 

Graph sur une page
/rapport/resources/graph.out





