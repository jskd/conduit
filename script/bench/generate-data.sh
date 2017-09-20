#!/bin/bash

BUFFER=1000

echo "Génération en cour..."
echo -e "#Bytes\tConduct (en s)\tPipe (en s)\tSocket (en s)"> "script/bench/output/data.out"
echo -e "#Bytes\tConduct (en s)\tPipe (en s)\tSocket (en s)"> "script/bench/output/table.out"

state=0
tic=11
f=1
while [ $f -lt $(bc <<< "1*10^$tic") ]
do
  for u in `seq 1 9`
  do
    i=$(bc <<< "$u*$f")  
    cond=$(bin/bench/bench_conduct $BUFFER $i 0 1000000)
    pipe=$(bin/bench/bench_pipe    $BUFFER $i)
    sock=$(bin/bench/bench_socket  $BUFFER $i)
    if [ $u == 1 ]
      then
        echo "[$state/$tic] Génération de $(bc <<< "1*$f") à $(bc <<< "9*$f")"
        echo -e "$i\t$cond\t$pipe\t$sock" >> "script/bench/output/table.out"
    fi
    echo -e "$i\t$cond\t$pipe\t$sock" >> "script/bench/output/data.out"
  done
  state=$(bc <<< "$state+1")
  f=$(bc <<< "$f*10")
done

echo "[$state/$tic] Terminé, fichier généré dans script/bench/output"







