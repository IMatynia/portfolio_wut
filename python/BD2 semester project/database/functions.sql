CREATE OR REPLACE FUNCTION count_cost_of_ingredients_of_offer(d_id INT)
RETURNS NUMERIC(8,4) AS $$
    DECLARE
        total_price NUMERIC(8,2);
    BEGIN
        SELECT SUM(di.quantity * i.price) FROM ingredientsOffers di JOIN ingredients i ON (di.ingredient_id = i.id)
        INTO total_price
        WHERE di.offer_id = d_id;
        RETURN total_price;
    END
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION count_cost_of_ingredients_of_order(supp_order_id INT)
RETURNS NUMERIC(8,4) AS $$
    DECLARE
        total_price NUMERIC(8,2);
    BEGIN
        SELECT SUM(iso.quantity * i.price) FROM ingredientsSuppliersOrders iso JOIN ingredients i ON (iso.ingredient_id = i.id)
        INTO total_price
        WHERE iso.supplier_order_id = supp_order_id;
        RETURN total_price;
    END
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION count_cost_of_customer_order(cus_order_id INT)
RETURNS NUMERIC(8,4) AS $$
    DECLARE
        total_price NUMERIC(8,2);
    BEGIN
        SELECT SUM(price) FROM offers o JOIN customersOrdersOffers c on o.id = c.offer_id
        INTO total_price
        WHERE c.customer_order_id = cus_order_id;
        RETURN total_price;
    END
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION get_menu_id_of_date(month INT, day INT)
RETURNS INT AS $$
    DECLARE
        m_id INT;
    BEGIN
        SELECT id FROM menus m
        INTO m_id
        WHERE (month, day) BETWEEN (m.month_start, m.day_start) AND (m.month_end, m.day_end);

        RETURN m_id;

    END
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION get_menu_id_of_now()
RETURNS INT AS $$
    DECLARE
        month   INT;
        day     INT;
        m_id    INT;
    BEGIN
        month := EXTRACT(MONTH FROM NOW());
        day   := EXTRACT(DAY   FROM NOW());
        SELECT id FROM menus m
        INTO m_id
        WHERE (month, day) BETWEEN (m.month_start, m.day_start) AND (m.month_end, m.day_end);

        RETURN m_id;

    END
$$ LANGUAGE plpgsql;