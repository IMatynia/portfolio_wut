package pis.service;

import com.google.gson.Gson;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.*;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;
import org.springframework.web.util.UriComponentsBuilder;
import pis.entity.Recipe;

import java.net.http.HttpResponse;

@Service
public class ElasticModelImpl implements ElasticModel {
    @Value("${services.elastic.endpoint}")
    private String elasticEndpoint;

    class RecipeHelper {
        private String id;
        private Long user_id;
        private String title;
        private String contents;
        private String ingredients;
        private String thumbnail_image_name;
        private String creation_time;

        RecipeHelper(Recipe original) {
            this.id = original.getId().toString();
            this.user_id = original.getUser_id();
            this.title = original.getTitle();
            this.contents = original.getContents();
            this.ingredients = original.getIngredients();
            this.thumbnail_image_name = original.getThumbnail_image_name();
            this.creation_time = original.getCreationDate().toString();
        }
    }

    @Override
    public void insert(Recipe recipe) throws Exception {
        RecipeHelper recipeHelper = new RecipeHelper(recipe);

        RestTemplate restTemplate = new RestTemplate();
        UriComponentsBuilder uriBuilder = UriComponentsBuilder.fromUriString(elasticEndpoint + "/insert");
        HttpHeaders headers = new HttpHeaders();
        headers.setContentType(MediaType.APPLICATION_JSON);

        Gson gson = new Gson();

        HttpEntity<String> requestEntity = new HttpEntity<>(gson.toJson(recipeHelper), headers);

        ResponseEntity<String> responseEntity = restTemplate.postForEntity(
                uriBuilder.toUriString(),
                requestEntity,
                String.class
        );
    }

    @Override
    public void delete(Long recipie_id) throws Exception {
        RestTemplate restTemplate = new RestTemplate();
        UriComponentsBuilder uriBuilder = UriComponentsBuilder.fromUriString(elasticEndpoint + "/delete/" + recipie_id.toString());

        ResponseEntity<String> responseEntity = restTemplate.exchange(
                uriBuilder.toUriString(),
                HttpMethod.DELETE,
                null,
                String.class);

        if (responseEntity.getStatusCode() != HttpStatus.OK) {
            throw new Exception("Could not delete from elastic");
        }
    }
}
