CREATE TABLE countries (
    id           SERIAL,
    name         VARCHAR(100) NOT NULL,
    country_code CHAR(3)
);

ALTER TABLE countries ADD CONSTRAINT countries_pk PRIMARY KEY ( id );

CREATE TABLE addresses (
    id            SERIAL,
    country_id    INT NOT NULL,
    postal_code   VARCHAR(10),
    city          VARCHAR(100),
    street        VARCHAR(100),
    street_number VARCHAR(10)
);

ALTER TABLE addresses ADD CONSTRAINT addresses_pk PRIMARY KEY ( id );

CREATE TABLE allergies (
    id           SERIAL NOT NULL,
    name         VARCHAR(100) NOT NULL
);

ALTER TABLE allergies ADD CONSTRAINT allergies_pk PRIMARY KEY ( id );

CREATE TABLE customersOrders (
    id              SERIAL NOT NULL,
    takeout         BOOLEAN NOT NULL,
    order_time      TIMESTAMP(0) NOT NULL,
    finish_time     TIMESTAMP(0) CHECK (finish_time > order_time),
    employee_id     INT NOT NULL
);

ALTER TABLE customersOrders ADD CONSTRAINT customerOrders_pk PRIMARY KEY ( id );

CREATE TABLE offers (
    id                SERIAL NOT NULL,
    name              VARCHAR(100) NOT NULL,
    type_id           INT NOT NULL,
    price             NUMERIC(6,2) NOT NULL,
    weight            NUMERIC(6,4) CHECK (weight > 0), --appetizers --dishes
    volume            NUMERIC(6,4) CHECK (volume > 0), --beverages
    alcohol_by_volume NUMERIC(5, 2) CHECK (alcohol_by_volume BETWEEN 0 AND 100), --beverage
    is_served_cold    BOOLEAN --dish

);

ALTER TABLE offers ADD CONSTRAINT offer_pk PRIMARY KEY ( id );

CREATE TABLE offersTypes (
    id SERIAL NOT NULL,
    name VARCHAR(100)
);

ALTER TABLE offersTypes ADD CONSTRAINT offerTypes_pk PRIMARY KEY ( id );



CREATE TABLE menusOffers (
    id        SERIAL NOT NULL,
    menu_id   INT NOT NULL,
    offer_id  INT NOT NULL
);

ALTER TABLE menusOffers ADD CONSTRAINT menusOffers_pk PRIMARY KEY
    (id);

ALTER TABLE menusOffers ADD CONSTRAINT menusOffers_unique UNIQUE
    (menu_id, offer_id );

CREATE TABLE ingredients (
    id                    SERIAL NOT NULL,
    name                  VARCHAR(100) NOT NULL,
    is_price_per_kilogram BOOLEAN NOT NULL,
    price                 NUMERIC(6,2) NOT NULL
);

ALTER TABLE ingredients ADD CONSTRAINT ingredients_pk PRIMARY KEY ( id );


CREATE TABLE employees (
    id               SERIAL NOT NULL,
    name             VARCHAR(100) NOT NULL,
    surname          VARCHAR(100) NOT NULL,
    salary           NUMERIC(7,2) NOT NULL,
    birthdate        TIMESTAMP(0) NOT NULL,
    pesel            VARCHAR(11)  UNIQUE,
    email            VARCHAR(100),
    phone_number     VARCHAR(15),
    employee_type_id INT NOT NULL
);

ALTER TABLE employees ADD CONSTRAINT employees_pk PRIMARY KEY ( id );

CREATE TABLE employeesTypes (
    id         SERIAL NOT NULL,
    name       VARCHAR(100) NOT NULL,
    min_salary NUMERIC(7,2) CHECK (min_salary>0),
    max_salary NUMERIC(7,2) CHECK (max_salary>=min_salary)
);

ALTER TABLE employeesTypes ADD CONSTRAINT employee_type_pk PRIMARY KEY ( id );

CREATE TABLE reservations (
    id         SERIAL NOT NULL,
    tables_id  INT NOT NULL,
    start_time TIMESTAMP(0) NOT NULL,
    end_time   TIMESTAMP(0) NOT NULL CHECK (end_time > start_time),
    name       VARCHAR(100)
);

ALTER TABLE reservations ADD CONSTRAINT reservations_pk PRIMARY KEY ( id );

CREATE TABLE menus (
    id              SERIAL NOT NULL,
    name            VARCHAR NOT NULL,
    month_start     INT NOT NULL CHECK (month_start BETWEEN 1 AND 12),
    day_start       INT NOT NULL,
    month_end       INT NOT NULL CHECK (month_end BETWEEN 1 AND 12),
    day_end         INT NOT NULL
);

ALTER TABLE menus ADD CONSTRAINT menus_pk PRIMARY KEY ( id );


CREATE TABLE suppliersOrders (
    id             SERIAL NOT NULL,
    date           TIMESTAMP(0) NOT NULL,
    supplier_id    INT NOT NULL
);

ALTER TABLE suppliersOrders ADD CONSTRAINT suppliersOrders_pk PRIMARY KEY ( id );

CREATE TABLE tables (
    id             SERIAL NOT NULL,
    seat_count     INT NOT NULL CHECK (seat_count > 0),
    name           VARCHAR(100),
    x_position     INT NOT NULL CHECK (x_position BETWEEN 0 AND 100),
    y_position     INT NOT NULL CHECK (y_position BETWEEN 0 AND 100)
);

ALTER TABLE tables ADD CONSTRAINT tables_pk PRIMARY KEY ( id );

CREATE TABLE suppliers (
    id               SERIAL NOT NULL,
    name             VARCHAR(100),
    bank_information VARCHAR(100),
    address_id       INT NOT NULL
);

ALTER TABLE suppliers ADD CONSTRAINT suppliers_pk PRIMARY KEY ( id );

CREATE TABLE allergiesIngredients (
    id              SERIAL NOT NULL,
    allergy_id      INT NOT NULL,
    ingredient_id   INT NOT NULL
);

ALTER TABLE allergiesIngredients ADD CONSTRAINT allergiesIngredients_pk PRIMARY KEY
    (id);

ALTER TABLE allergiesIngredients ADD CONSTRAINT allergiesIngredients_unique UNIQUE
    (allergy_id, ingredient_id);

CREATE TABLE ingredientsSuppliers (
    id            SERIAL NOT NULL,
    ingredient_id INT NOT NULL,
    supplier_id   INT NOT NULL
);

ALTER TABLE ingredientsSuppliers ADD CONSTRAINT ingredientsSuppliers_pk PRIMARY KEY
    (id);

ALTER TABLE ingredientsSuppliers ADD CONSTRAINT ingredientsSuppliers_unique UNIQUE
    (ingredient_id, supplier_id);

CREATE TABLE ingredientsSuppliersOrders (
    id                 SERIAL NOT NULL,
    ingredient_id      INT NOT NULL,
    supplier_order_id  INT NOT NULL,
    quantity           INT NOT NULL
);

ALTER TABLE ingredientsSuppliersOrders ADD CONSTRAINT ingredientsSuppliersOrders_pk PRIMARY KEY
    (id);

ALTER TABLE ingredientsSuppliersOrders ADD CONSTRAINT ingredientsSuppliersOrders_unique UNIQUE
    (ingredient_id, supplier_order_id );

CREATE TABLE ingredientsOffers (
    id             SERIAL NOT NULL,
    offer_id       INT NOT NULL,
    ingredient_id  INT NOT NULL,
    quantity       NUMERIC(7, 4) NOT NULL CHECK(quantity > 0)
);

ALTER TABLE ingredientsOffers ADD CONSTRAINT ingredientsOffers_pk PRIMARY KEY
    (id);

ALTER TABLE ingredientsOffers ADD CONSTRAINT ingredientsOffers_unique UNIQUE
    ( ingredient_id, offer_id );

CREATE TABLE customersOrdersOffers (
    id                  SERIAL NOT NULL,
    customer_order_id   INT NOT NULL,
    offer_id            INT NOT NULL
);

ALTER TABLE customersOrdersOffers ADD CONSTRAINT customersOrdersOffers_pk PRIMARY KEY
    (id);

ALTER TABLE customersOrdersOffers ADD CONSTRAINT customersOrdersOffers_unique UNIQUE
    ( customer_order_id, offer_id );

CREATE TABLE customersOrdersTables (
    id                SERIAL NOT NULL,
    customer_order_id INT NOT NULL,
    table_id          INT NOT NULL
);

ALTER TABLE customersOrdersTables ADD CONSTRAINT customersOrdersTables_pk PRIMARY KEY
    (id);

ALTER TABLE customersOrdersTables ADD CONSTRAINT customersOrdersTables_unique UNIQUE
    ( customer_order_id, table_id );

ALTER TABLE offers
    ADD CONSTRAINT offers_offersTypes_fk FOREIGN KEY (type_id)
        REFERENCES offersTypes (id) ON DELETE CASCADE;

ALTER TABLE addresses
    ADD CONSTRAINT addresses_countries_fk FOREIGN KEY ( country_id )
        REFERENCES countries ( id ) ON DELETE CASCADE;

ALTER TABLE customersOrders
    ADD CONSTRAINT customerOrders_employee_fk FOREIGN KEY ( employee_id )
        REFERENCES employees ( id ) ON DELETE NO ACTION;

ALTER TABLE employees
    ADD CONSTRAINT employee_employee_type_fk FOREIGN KEY ( employee_type_id )
        REFERENCES employeesTypes ( id ) ON DELETE NO ACTION;

ALTER TABLE menusOffers
    ADD CONSTRAINT offerMenus_offer_fk FOREIGN KEY ( offer_id )
        REFERENCES offers ( id ) ON DELETE CASCADE;

ALTER TABLE menusOffers
    ADD CONSTRAINT fk_menusOffers_menu_id_fk FOREIGN KEY (menu_id)
        REFERENCES menus ( id ) ON DELETE CASCADE;

ALTER TABLE allergiesIngredients
    ADD CONSTRAINT allergiesIngredients_allergy_id_fk FOREIGN KEY ( allergy_id )
        REFERENCES allergies ( id ) ON DELETE CASCADE;

ALTER TABLE allergiesIngredients
    ADD CONSTRAINT allergies_ingredients_ingredient_id_fk FOREIGN KEY ( ingredient_id )
        REFERENCES ingredients ( id ) ON DELETE CASCADE;

ALTER TABLE ingredientsSuppliers
    ADD CONSTRAINT ingredientsSuppliers_ingredient_id_fk FOREIGN KEY ( ingredient_id )
        REFERENCES ingredients ( id ) ON DELETE CASCADE;

ALTER TABLE ingredientsSuppliers
    ADD CONSTRAINT ingredientsSuppliers_suppliers_id_fk FOREIGN KEY ( supplier_id )
        REFERENCES suppliers ( id ) ON DELETE CASCADE;

ALTER TABLE ingredientsSuppliersOrders
    ADD CONSTRAINT ingredientsSuppliersOrders_ingredient_id_fk FOREIGN KEY ( ingredient_id )
        REFERENCES ingredients ( id ) ON DELETE CASCADE;

ALTER TABLE ingredientsSuppliersOrders
    ADD CONSTRAINT ingredientsSuppliersOrders_supplier_order_id FOREIGN KEY ( supplier_order_id )
        REFERENCES suppliersOrders ( id ) ON DELETE CASCADE;

ALTER TABLE ingredientsOffers
    ADD CONSTRAINT offerIngredients_offer_id_fk FOREIGN KEY ( offer_id )
        REFERENCES offers ( id ) ON DELETE CASCADE;

ALTER TABLE ingredientsOffers
    ADD CONSTRAINT ingredientsOffers_ingredient_id_fk FOREIGN KEY ( ingredient_id )
        REFERENCES ingredients ( id ) ON DELETE CASCADE;

ALTER TABLE customersOrdersoffers
    ADD CONSTRAINT customerOrdersOffers_customer_id_fk FOREIGN KEY ( customer_order_id )
        REFERENCES customersOrders ( id ) ON DELETE CASCADE;

ALTER TABLE customersOrdersOffers
    ADD CONSTRAINT customerOrdersOffers_offer_id_fk FOREIGN KEY ( offer_id )
        REFERENCES offers ( id ) ON DELETE CASCADE;

ALTER TABLE reservations
    ADD CONSTRAINT reservations_tables_fk FOREIGN KEY ( tables_id )
        REFERENCES tables ( id ) ON DELETE CASCADE;

ALTER TABLE suppliersOrders
    ADD CONSTRAINT suppliersOrders_supplier_id_fk FOREIGN KEY ( supplier_id )
        REFERENCES suppliers ( id ) ON DELETE NO ACTION;

ALTER TABLE suppliers
    ADD CONSTRAINT suppliers_addresses_id_fk FOREIGN KEY ( address_id )
        REFERENCES addresses ( id ) ON DELETE NO ACTION;

ALTER TABLE customersOrdersTables
    ADD CONSTRAINT customersOrdersTables_customer_order_id_fk FOREIGN KEY ( customer_order_id )
        REFERENCES customersOrders ( id );

ALTER TABLE customersOrdersTables
    ADD CONSTRAINT customersOrdersTables_Offers_id_fk FOREIGN KEY ( table_id )
        REFERENCES tables ( id );


