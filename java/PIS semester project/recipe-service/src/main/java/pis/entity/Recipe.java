package pis.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import org.hibernate.annotations.CreationTimestamp;
import java.util.Date;

@Getter
@Entity
@Table(name="recipe")
public class Recipe {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;

    @Setter
    private Long user_id;

    @CreationTimestamp
    private Date creationDate;

    @Setter
    private String title;
    @Setter
    private String contents;
    @Setter
    private String ingredients;
    @Setter
    private String thumbnail_image_name;

    public Recipe(Long user_id, String title, String contents, String ingredients, String thumbnail_image_name)
    {
        this.user_id=user_id;
        this.title=title;
        this.contents=contents;
        this.ingredients=ingredients;
        this.thumbnail_image_name=thumbnail_image_name;
    }

    public Recipe() {

    }
}
