FROM alpine
COPY . /project
WORKDIR /project/
RUN apk update; \
    apk add openjdk17-jdk; \
    apk add curl;

CMD ["./gradlew", "bootRun"]
EXPOSE 9898