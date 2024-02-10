package pis.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import pis.entity.Recipe;

@Repository
public interface RecipieRepo extends JpaRepository<Recipe, Long> {
}