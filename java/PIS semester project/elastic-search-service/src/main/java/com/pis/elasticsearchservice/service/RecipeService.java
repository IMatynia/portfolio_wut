package com.pis.elasticsearchservice.service;

import com.pis.elasticsearchservice.entity.Recipe;
import com.pis.elasticsearchservice.repository.RecipeRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.Optional;

@Service
public class RecipeService {
    @Autowired
    private RecipeRepository recipeRepository;

    public Iterable<Recipe> getRecipes() {
        return recipeRepository.findAll();
    }

    public Recipe insertRecipe(Recipe recipe) {
        return recipeRepository.save(recipe);
    }

    public Recipe getRecipe(String id) {
        Optional<Recipe> r = recipeRepository.findById(id);
        return r.orElse(null);
    }

    public Iterable<Recipe> getRecipesByName(String name) {
        return recipeRepository.findByTitle(name);
    }

    public Recipe updateRecipe(Recipe recipe, String id) {
        Recipe recipe_to_update = getRecipe(id);
        if (recipe_to_update == null) {
            return null;
        }
        recipe_to_update.setContents(recipe.getContents());
        recipe_to_update.setIngredients(recipe.getIngredients());
        recipe_to_update.setThumbnail_image_name(recipe.getThumbnail_image_name());
        recipe_to_update.setTitle(recipe.getTitle());
        recipe_to_update.setUser_id(recipe.getUser_id());
        recipe_to_update.setCreation_time(recipe.getCreation_time());
        recipeRepository.save(recipe_to_update);
        return recipe_to_update;
    }

    public void deleteRecipe(String id) {
        recipeRepository.deleteById(id);
    }
}
