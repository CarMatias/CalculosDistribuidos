#!/bin/bash

if [ -f ./compiled/client ];then
        echo -e "Iniciando Cliente...\n"
        ./compiled/client
    else
        echo -e "Compilando Cliente..\n"
        sleep 0.5s
        gcc -w ./src/clientPrime.c -o ./compiled/client -pthread 
        echo -e "Cliente Compilado\n"
        sleep 0.5s
        echo -e "Iniciando Cliente...\n"
        sleep 1s
        clear
        ./compiled/client
fi
