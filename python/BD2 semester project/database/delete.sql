drop trigger check_salary ON employees;
drop trigger check_ingredients_orders ON ingredientsSuppliersOrders;
drop trigger validate_pesel ON employees;
drop trigger check_date_validity ON menus;
drop trigger check_menus_overlaps ON menus;
drop trigger check_offer_type ON offers;
drop trigger check_customer_order_table_validity on customersOrdersTables;
drop trigger check_offers_costs on ingredientsOffers;
drop trigger check_reservations_validity on reservations;
drop materialized view offer_production_costs;
drop materialized view offer_allergies;
drop materialized view suppliers_orders;
drop materialized view ingredient_stats;
drop materialized view supplier_stats;
drop materialized view appetizers;
drop materialized view beverages;
drop materialized view dishes;
drop view customers_orders_detailed;
drop function count_cost_of_ingredients_of_offer;
drop function count_cost_of_ingredients_of_order;
drop function count_cost_of_customer_order;
drop function get_menu_id_of_date;
drop function get_menu_id_of_now;
drop table allergiesIngredients;
drop table ingredientsSuppliers;
drop table ingredientsSuppliersOrders;
drop table ingredientsOffers;
drop table customersOrdersOffers;
drop table customersOrdersTables;
drop table suppliersOrders;
drop table suppliers;
drop table addresses;
drop table countries;
drop table allergies;
drop table customersOrders;
drop table menusOffers;
drop table ingredients;
drop table employees;
drop table employeesTypes;
drop table reservations;
drop table menus;
drop table tables;
drop table offers;
drop table offersTypes;