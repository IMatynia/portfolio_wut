package pis.service;

import pis.entity.Recipe;

import java.util.List;

public interface RecipieModel {
    public List<Recipe> getAllRecipes();

    public Recipe getRecipeById(Long id);

    public Recipe addRecipe(Recipe recipe) throws Exception;

    public boolean deleteRecipe(Long id);
}
