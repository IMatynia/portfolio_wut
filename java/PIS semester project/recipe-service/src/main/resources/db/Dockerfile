# docker run --name postgres-recipe-db -p 5433:5432 -d recipe-service-db
FROM postgres

ENV POSTGRES_USER imat
ENV POSTGRES_PASSWORD imat
ENV POSTGRES_DB recipie_service
ADD initdb.sql /docker-entrypoint-initdb.d/
