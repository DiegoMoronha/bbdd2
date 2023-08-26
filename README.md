# bbdd2

# Formas de ejecutar la applicacion

ejecutar el archivo run.sh que se encuentra en la carpeta src

## en caso de tener windows podemos usar una imagen de ubuntu

### construye la imagen de ubuntu en dockerfile

docker build --pull --rm -f "Dockerfile" -t bbdd2:latest "."

### corremos de manera interactiva el contenedor

docker run -it --rm bbdd2

ejecutamos make y nos creara un ejecutable llamado sqlDatabase donde inicializara la aplicacion
