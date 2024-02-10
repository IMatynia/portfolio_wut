CREATE OR REPLACE FUNCTION check_employee_salary()
RETURNS TRIGGER AS
$$
DECLARE
    min_sal NUMERIC(7,2);
    max_sal NUMERIC(7,2);

BEGIN

    SELECT min_salary
    INTO min_sal
    FROM employeesTypes
    WHERE id = NEW.employee_type_id;

    SELECT max_salary
    INTO max_sal
    FROM employeesTypes
    WHERE id = NEW.employee_type_id;

    IF (NEW.salary < min_sal) THEN
        RAISE 'New salary of employee of ID % is too low (Minimum possible: %, Actual: %)', NEW.id, min_sal, NEW.salary;
    END IF;
    IF (NEW.salary > max_sal) THEN
        RAISE 'New salary of employee of ID % is too high (Maximum possible: %, Actual: %)', NEW.id, max_sal, NEW.salary;
    END IF;
    RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_salary BEFORE INSERT OR UPDATE
    ON employees FOR EACH ROW
    EXECUTE PROCEDURE check_employee_salary();

--

CREATE OR REPLACE FUNCTION check_ingredients_suppliers_integrity()
RETURNS TRIGGER AS
$$
DECLARE
    sup_id INT;

BEGIN

    SELECT supplier_id
    INTO sup_id
    FROM suppliersorders
    WHERE id = NEW.supplier_order_id;

    IF (NOT EXISTS(SELECT * FROM ingredientsSuppliers WHERE
                                                      ingredient_id = NEW.ingredient_id
                                                      AND supplier_id = sup_id)) THEN
        RAISE 'Supplier of ID % does not have an ingredient of ID %', sup_id, NEW.ingredient_id;
    END IF;
    RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_ingredients_orders BEFORE INSERT OR UPDATE
    ON ingredientsSuppliersOrders FOR EACH ROW
    EXECUTE PROCEDURE check_ingredients_suppliers_integrity();



CREATE OR REPLACE FUNCTION validate_pesel()
  RETURNS TRIGGER AS
$$
DECLARE
  pesel_arr INT[11];
  weights INT[] := ARRAY[1, 3, 7, 9, 1, 3, 7, 9, 1, 3];
  i INT;
  control_sum INT := 0;
  control_digit INT;
BEGIN

    IF NEW.pesel IS NULL THEN --Not all employees might have pesel
        RETURN NEW;
    end if;

  IF NOT NEW.pesel ~ '^\d+$' THEN
    RAISE 'PESEL must contain only digits (Tried to add: %)', NEW.pesel;
  END IF;

  FOR i IN 1..11 LOOP
    pesel_arr[i] := CAST(substring(NEW.pesel, i, 1) AS INT);
  END LOOP;

  FOR i IN 1..10 LOOP
    control_sum := control_sum + pesel_arr[i] * weights[i];
  END LOOP;

  control_digit := (10 - (control_sum % 10)) % 10;

  IF control_digit <> pesel_arr[11] THEN
    RAISE 'Invalid PESEL - wrong control sum (Tried to add: %)', NEW.pesel;
  END IF;
  RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION check_valid_date(date_month INT, date_day INT)
    RETURNS BOOLEAN AS
$$

BEGIN

    IF (date_month < 0) OR (date_day < 0) THEN
        RAISE 'Month (%) and date (%) cannot be negative', date_month, date_day;
    END IF;
    IF (date_month = 2 AND date_day > 29) THEN
        RAISE 'Invalid date: Month: %, Day: %', date_month, date_day;
    ELSIF (date_month = 4 OR date_month = 6 OR date_month = 9 OR date_month = 11) AND date_day > 30 THEN
        RAISE 'Invalid date: Month: %, Day: %', date_month, date_day;
    ELSIF (date_day > 31) THEN
        RAISE 'Invalid date: Month: %, Day: %', date_month, date_day;
    END IF;
    RETURN TRUE;
END
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION check_menu_dates()
    RETURNS TRIGGER AS
$$
DECLARE
    is_valid BOOLEAN;
BEGIN

    is_valid := check_valid_date(NEW.month_start, NEW.day_start);
    is_valid := check_valid_date(NEW.month_end, NEW.day_end);
    RETURN NEW;
END
$$ LANGUAGE plpgsql;


CREATE TRIGGER validate_pesel
  BEFORE INSERT OR UPDATE
  ON employees
  FOR EACH ROW
  EXECUTE FUNCTION validate_pesel();

CREATE TRIGGER check_date_validity
BEFORE INSERT OR UPDATE ON menus
FOR EACH ROW
EXECUTE FUNCTION check_menu_dates();

CREATE OR REPLACE FUNCTION check_date_overlaps()
RETURNS TRIGGER AS
$$

BEGIN
    IF (EXISTS(SELECT * FROM menus m
                        WHERE (m.month_start, m.day_start) <= (NEW.month_end, NEW.day_end) AND
                              (m.month_end, m.day_end) >= (NEW.month_start, NEW.day_start) AND
                              m.id <> NEW.id)) THEN
        RAISE 'Cannot add new menu of ID % due to its overlapping with another menu', NEW.id;
    end if;

    RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_menus_overlaps
BEFORE INSERT OR UPDATE ON menus
FOR EACH ROW
EXECUTE FUNCTION check_date_overlaps();



CREATE OR REPLACE FUNCTION check_type_of_offer()
RETURNS TRIGGER AS
$$
DECLARE
    type_name VARCHAR;
BEGIN
    if NEW.type_id = 1 AND NOT
        (NEW.weight IS NOT NULL AND NEW.volume IS NULL AND
         NEW.alcohol_by_volume IS NULL AND NEW.is_served_cold IS NULL) THEN
            SELECT name INTO type_name FROM offersTypes WHERE id = NEW.type_id;
            RAISE 'Tried to add offer % with invalid attributes', type_name;
    elseif NEW.type_id = 2 AND NOT
        (NEW.weight IS NULL AND NEW.volume IS NOT NULL AND
         NEW.alcohol_by_volume IS NOT NULL AND NEW.is_served_cold IS NULL) THEN
            SELECT name INTO type_name FROM offersTypes WHERE id = NEW.type_id;
            RAISE 'Tried to add offer % with invalid attributes', type_name;
    elseif NEW.type_id = 3 AND NOT
        (NEW.weight IS NOT NULL AND NEW.volume IS NULL AND
         NEW.alcohol_by_volume IS NULL AND NEW.is_served_cold IS NOT NULL) THEN
            SELECT name INTO type_name FROM offersTypes WHERE id = NEW.type_id;
            RAISE 'Tried to add offer % with invalid attributes', type_name;
    end if;

    RETURN NEW;

    RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_offer_type
BEFORE INSERT OR UPDATE ON offers
FOR EACH ROW
EXECUTE FUNCTION check_type_of_offer();

CREATE OR REPLACE FUNCTION check_if_customer_order_is_for_table()
RETURNS TRIGGER AS
$$
DECLARE
    is_takeout BOOLEAN;
BEGIN
    SELECT takeout INTO is_takeout FROM customersOrders
        WHERE id = NEW.customer_order_id;

    if is_takeout THEN
        RAISE 'Cannot connect table with customer order which is a takeout';
    end if;

    RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_customer_order_table_validity
BEFORE INSERT OR UPDATE ON customersOrdersTables
FOR EACH ROW
EXECUTE FUNCTION check_if_customer_order_is_for_table();



CREATE OR REPLACE FUNCTION check_costs_of_offer()
RETURNS TRIGGER AS
$$
DECLARE
    costs_of_ingredients NUMERIC(8, 2);
    cost_of_offer        NUMERIC(8, 2);
BEGIN
    SELECT count_cost_of_ingredients_of_offer(NEW.offer_id)
    INTO costs_of_ingredients;

    SELECT price
    INTO cost_of_offer
    FROM offers o
    WHERE o.id = NEW.offer_id;

    if costs_of_ingredients > cost_of_offer THEN
        RAISE WARNING 'Costs of ingredients of Offer with ID % is greater than its price (% > %)', NEW.offer_id, costs_of_ingredients, cost_of_offer;
    end if;

    RETURN NEW;
END
$$ LANGUAGE plpgsql;

CREATE TRIGGER check_offers_costs
AFTER INSERT OR UPDATE ON ingredientsOffers
FOR EACH ROW
EXECUTE FUNCTION check_costs_of_offer();



CREATE OR REPLACE FUNCTION check_reservations_overlaps()
RETURNS TRIGGER AS
$$
BEGIN
    IF exists(SELECT * FROM reservations r WHERE r.tables_id = NEW.tables_id AND "overlaps"(NEW.start_time, NEW.end_time, r.start_time, r.end_time)) THEN
        RAISE EXCEPTION 'Cannot add new reservation with ID of %, because its time overlaps with another reservation', NEW.id;
    end if;

    RETURN NEW;
END
$$ LANGUAGE plpgsql;


CREATE TRIGGER check_reservations_validity
BEFORE INSERT OR UPDATE ON reservations
FOR EACH ROW
EXECUTE FUNCTION check_reservations_overlaps();