CREATE TABLE IF NOT EXISTS recipe (
    id SERIAL PRIMARY KEY,
    user_id BIGINT,
    creation_date TIMESTAMP,
    title VARCHAR(255),
    contents TEXT,
    ingredients TEXT,
    thumbnail_image_name VARCHAR(255)
);