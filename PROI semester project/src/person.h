#ifndef PERSON_H
#define PERSON_H

#include <string>

/// Class represening person which has a name
class Person
{
protected:
    std::string first_name;
    std::string last_name;

public:
    /**
    * @brief Construct a new Person object
    *
    * @param first_name first name of Author
    * @param last_name last name of Author
    */
    Person(std::string first_name, std::string last_name);
    Person();
    /**
     * @brief Set the first name of author
     *
     * @param new_name
     */
    void set_first_name(std::string new_name);
    /**
     * @brief Set the last name of author
     *
     * @param new_name
     */
    void set_last_name(std::string new_name);

    /**
     * @brief Get the first name object
     *
     * @return std::string
     */
    std::string get_first_name() const;
    std::string get_last_name() const;
    /**
     * @brief Get the full name of Person: "Firstname Lastname"
     *
     * @return std::string
     */
    std::string get_full_name() const;

    virtual ~Person();
};

#endif