# Usa la imagen base de Ubuntu
FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    make \
    g++ \
    && rm -rf /var/lib/apt/lists/*


WORKDIR /app

COPY . ./
VOLUME /app

CMD ["bash"]