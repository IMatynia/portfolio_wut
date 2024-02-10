# docker run --name postgres-users-db -p 5434:5432 -d postgres-users-db
FROM postgres

ENV POSTGRES_USER imat
ENV POSTGRES_PASSWORD imat
ENV POSTGRES_DB user_service
ADD table_creation.sql /docker-entrypoint-initdb.d/