# docker run --name users-service -p 9696:9696 -d users-service
FROM alpine
COPY . /project
WORKDIR /project/
RUN apk update; \
    apk add openjdk17-jdk; \
    apk add curl;

CMD ["./gradlew", "run"]
EXPOSE 9696