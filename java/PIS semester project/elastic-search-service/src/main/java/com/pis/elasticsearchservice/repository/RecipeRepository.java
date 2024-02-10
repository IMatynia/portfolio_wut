package com.pis.elasticsearchservice.repository;

import com.pis.elasticsearchservice.entity.Recipe;
import org.springframework.data.elasticsearch.repository.ElasticsearchRepository;

import java.util.List;

public interface RecipeRepository extends ElasticsearchRepository<Recipe, String> {
    List<Recipe> findByTitle(String title);
}
