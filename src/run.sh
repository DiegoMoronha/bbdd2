#!/bin/bash

# Compila el programa
g++ -std=c++11 -Wall main.cpp replSql.cpp parserSql.cpp record.cpp table.cpp parserSql.h replSql.h record.h table.h page.h -o sqlDatabase

# Verifica si la compilación fue exitosa
if [ $? -eq 0 ]; then
    echo "Compilación exitosa. Ejecutando el programa..."
    ./sqlDatabase
else
    echo "Error en la compilación. El programa no se ejecutará."
fi
