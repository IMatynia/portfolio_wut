package pis.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import pis.entity.Recipe;
import pis.service.RecipieModel;
import pis.service.UsersModel;

import java.util.List;


import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@RestController
@RequestMapping("/api/recipies")
public class RecipieApiController {
    @Autowired
    private RecipieModel recipeService;

    @Autowired
    private UsersModel usersModel;

    private static final Logger logger = LoggerFactory.getLogger(RecipieApiController.class);

    @GetMapping
    public ResponseEntity<List<Recipe>> getAllRecipes() {
        List<Recipe> recipes = recipeService.getAllRecipes();
        return new ResponseEntity<>(recipes, HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<Recipe> getRecipeById(@PathVariable Long id) {
        Recipe recipe = recipeService.getRecipeById(id);
        if (recipe != null) {
            return new ResponseEntity<>(recipe, HttpStatus.OK);
        } else {
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
    }

    @PostMapping
    public ResponseEntity<Recipe> addRecipe(@RequestBody Recipe recipe, @RequestHeader("X-Session") String sessionId) throws Exception {
        Long author_id = recipe.getUser_id();
        Boolean accessGranted = usersModel.checkAuthority(author_id, sessionId);
        if (!accessGranted) {
            logger.warn(String.format("Posting forbidden, invalid authority for user with session %s trying to act as %d", sessionId, author_id));
            return new ResponseEntity<>(HttpStatus.FORBIDDEN);
        }

        Recipe createdRecipe = recipeService.addRecipe(recipe);
        return new ResponseEntity<>(createdRecipe, HttpStatus.CREATED);
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteRecipe(@PathVariable Long id, @RequestHeader("X-Session") String sessionId) {
        Recipe recipe = recipeService.getRecipeById(id);
        if (recipe == null) {
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
        Long author_id = recipe.getUser_id();
        Boolean accessGranted = usersModel.checkAuthority(author_id, sessionId);
        if (!accessGranted) {
            logger.warn(String.format("Deletion forbidden, invalid authority for user with session %s trying to act as %d", sessionId, author_id));
            return new ResponseEntity<>(HttpStatus.FORBIDDEN);
        }

        boolean deleted = recipeService.deleteRecipe(id);
        if (deleted) {
            return new ResponseEntity<>(HttpStatus.NO_CONTENT);
        } else {
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
    }
}
