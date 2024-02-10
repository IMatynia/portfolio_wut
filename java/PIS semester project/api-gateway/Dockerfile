# docker run --name api-gateway -p 8080:8080 -d api-gateway
FROM alpine
COPY . /project
WORKDIR /project/
RUN apk update; \
    apk add openjdk17-jdk; \
    apk add curl;

CMD ["./gradlew", "bootRun"]
EXPOSE 8080