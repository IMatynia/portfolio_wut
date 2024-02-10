package pis.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import pis.entity.Recipe;
import pis.repository.RecipieRepo;

import java.util.List;

@Service
public class RecipieModelImpl implements RecipieModel {
    @Autowired
    private RecipieRepo recipeRepository;
    @Autowired
    private ElasticModel elasticModel;

    public List<Recipe> getAllRecipes() {
        return recipeRepository.findAll();
    }

    public Recipe getRecipeById(Long id) {
        return recipeRepository.findById(id).orElse(null);
    }

    public Recipe addRecipe(Recipe recipe) throws Exception {
        Recipe recipe_ready = recipeRepository.save(recipe);
        elasticModel.insert(recipe_ready);
        return recipe_ready;
    }

    public boolean deleteRecipe(Long id) {
        if (recipeRepository.existsById(id)) {
            try {
                elasticModel.delete(id);
            } catch (Exception e) {
                return false;
            }
            recipeRepository.deleteById(id);
            return true;
        }
        return false;
    }
}
