CREATE TABLE IF NOT EXISTS users
(
    id              SERIAL PRIMARY KEY,
    join_date       TIMESTAMP DEFAULT NOW(),
    username        varchar(255) not null,
    bio             text,
    profile_picture varchar(255)
);

CREATE TABLE IF NOT EXISTS security_hashes
(
    hash_id       SERIAL PRIMARY KEY,
    user_id       INT REFERENCES users (id) unique,
    password_hash VARCHAR(255) NOT NULL
);