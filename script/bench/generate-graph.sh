#!/bin/bash

if [ ! -f script/bench/output/data.out ]; then
    echo "Erreur: Fichier de donnée non généré"
    echo "Pour génerer les données: make bench-data"
    exit
fi

gnuplot script/bench/gnuplot.in

echo "Terminé, graph généré dans script/bench/output"
