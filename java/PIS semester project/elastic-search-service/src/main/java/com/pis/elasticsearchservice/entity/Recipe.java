package com.pis.elasticsearchservice.entity;

import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.NoArgsConstructor;
import org.springframework.data.annotation.Id;
import org.springframework.data.elasticsearch.annotations.Document;

@NoArgsConstructor
@AllArgsConstructor
@Data
@Document(indexName = "recipes")
public class Recipe {
    @Id
    private String id;
    private Long user_id;
    private String title;
    private String contents;
    private String ingredients;
    private String thumbnail_image_name;
    private String creation_time;

    // Getters and setters
}
