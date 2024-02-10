# docker run --name config-server -p 8888:8888 -d config-server
FROM alpine
COPY . /project
WORKDIR /project/
RUN apk update; \
    apk add openjdk17-jdk; \
    apk add curl;

CMD ["./gradlew", "bootRun"]
EXPOSE 8888