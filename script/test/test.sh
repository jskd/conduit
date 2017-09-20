#!/bin/bash

T_RED=`tput setaf 1`
T_GREEN=`tput setaf 2`
T_BOLD=`tput bold`
T_NORMAL=`tput sgr0`

echo "${T_BOLD} ----- Test ----- ${T_NORMAL}"

for filename in bin/test/*; do
  NAME=$(basename $filename)
  if [[ "$NAME" =  "julia" ]]; 
  then
    while true; do
      read -p "Lancer julia? [y/n]" yn
      case $yn in
        [Yy]* )  $filename; break;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
      esac
    done
  else
    $filename > "script/test/output/$NAME.out"
    if diff "script/test/output/$NAME.expected" "script/test/output/$NAME.out" > /dev/null;
    then
      echo "${T_GREEN}Sucess: $NAME ${T_NORMAL}"
    else
      echo "${T_RED}${T_BOLD}Error: $NAME ${T_NORMAL}"
    fi
  fi
done
