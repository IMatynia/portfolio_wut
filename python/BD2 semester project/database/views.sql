CREATE MATERIALIZED VIEW offer_production_costs AS
SELECT d.*, count_cost_of_ingredients_of_offer(d.id) as price_netto,
       (d.price - count_cost_of_ingredients_of_offer(d.id)) as diffrence FROM offers d;

CREATE MATERIALIZED VIEW offer_allergies AS
    SELECT o.id as offer_id, o.name as offer_name,
           a.id as allergy_id, a.name as allergy_name FROM offers o JOIN ingredientsOffers oa on o.id = oa.offer_id
        JOIN allergiesIngredients ai on oa.ingredient_id = ai.ingredient_id JOIN allergies a on ai.allergy_id = a.id;

CREATE MATERIALIZED VIEW suppliers_orders AS
    SELECT s.id as supplier_id, s.name as supplier_name,
           so.id as order_id , i.id as ingredient_id , i.name as ingredient_name,
           iso.quantity as quantity, i.price * iso.quantity as price
    FROM suppliers s JOIN suppliersOrders so on s.id = so.supplier_id
        JOIN ingredientsSuppliersOrders iso on so.id = iso.supplier_order_id JOIN ingredients i on iso.ingredient_id = i.id;

CREATE MATERIALIZED VIEW ingredient_stats AS
    SELECT i.id as ingredient_id, i.name as ingredient_name, count(a.id) as no_allergies, count(o.id) as no_dishes, count(s.id) as no_suppliers
    FROM ingredients i
        LEFT JOIN allergiesIngredients ai on i.id = ai.ingredient_id
        LEFT JOIN allergies a on ai.allergy_id = a.id
        LEFT JOIN ingredientsOffers ino on ino.ingredient_id = i.id
        LEFT JOIN offers o on ino.offer_id = o.id 
        LEFT JOIN ingredientsSuppliers isu on isu.ingredient_id = i.id
        LEFT JOIN suppliers s on isu.supplier_id = s.id
    GROUP BY i.id
    ORDER BY i.id ASC;

CREATE MATERIALIZED VIEW supplier_stats AS
    SELECT s.id as id, s.name as name, count(so.id) as no_orders
    FROM suppliers s
        LEFT JOIN suppliersOrders so on so.supplier_id = s.id
    GROUP BY s.id
    ORDER BY s.id ASC;

CREATE VIEW customers_orders_detailed AS
    SELECT co.*,e.name, e.surname, (finish_time IS NOT NULL) as is_finished, count_cost_of_customer_order(co.id) as total_cost FROM customersorders co JOIN employees e on co.employee_id = e.id;

CREATE MATERIALIZED VIEW appetizers AS
    SELECT id, name, price, weight FROM offers
    WHERE type_id = 1;

CREATE MATERIALIZED VIEW beverages AS
    SELECT id, name, price, volume, alcohol_by_volume FROM offers
    WHERE type_id = 2;

CREATE MATERIALIZED VIEW dishes AS
    SELECT id, name, price, weight, is_served_cold FROM offers
    WHERE type_id = 3;