package com.pis.elasticsearchservice.controller;

import com.pis.elasticsearchservice.entity.Recipe;
import com.pis.elasticsearchservice.service.RecipeService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.*;

import java.util.ArrayList;
import java.util.List;

@RestController
@RequestMapping("/api")
public class RecipeController {

    @Autowired
    private RecipeService recipeService;
    @GetMapping("/findAll")
    public List<Recipe> findAll() {
        List<Recipe> list = new ArrayList<>();
        for (Recipe recipe : recipeService.getRecipes()) {
            list.add(recipe);
        }
        return list;
    }

    @GetMapping("/findById/{id}")
    public Recipe findById(@PathVariable String id) {
        return recipeService.getRecipe(id);
    }

    @GetMapping("/findByTitle/{title}")
    public List<Recipe> findByTitle(@PathVariable String title) {
        List<Recipe> list = new ArrayList<>();
        for (Recipe recipe : recipeService.getRecipesByName(title)) {
            list.add(recipe);
        }
        return list;
    }

    @PostMapping("/insert")
    public Recipe insert(@RequestBody Recipe recipe) {
        return recipeService.insertRecipe(recipe);
    }

    @PutMapping("/update/{id}")
    public Recipe update(@RequestBody Recipe recipe, @PathVariable String id) {
        return recipeService.updateRecipe(recipe, id);
    }

    @DeleteMapping("/delete/{id}")
    public void delete(@PathVariable String id) {
        recipeService.deleteRecipe(id);
    }

}
