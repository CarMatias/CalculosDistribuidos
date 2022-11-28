#!/bin/bash

    if [ -f ./compiled/server ];then
        echo -e "Iniciando Servidor...\n"
        sleep 1s
        clear
        ./compiled/server
    else
        echo -e "Compilando Servidor..\n"
        sleep 0.5s
        gcc -w ./src/server.c -o ./compiled/server -pthread 
        echo -e "Servidor Compilado\n"
        sleep 0.5s
        echo -e "Iniciando Servidor...\n"
        sleep 0.5s
        clear
        ./compiled/server
    fi

