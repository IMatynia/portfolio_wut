package pis.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;
import org.hibernate.annotations.CreationTimestamp;

import java.util.Date;

@Getter
@Entity
@Table(name = "users")
public class User {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private Long id;
    @CreationTimestamp
    private Date join_date;
    @Setter
    private String username;
    @Setter
    private String bio;
    @Setter
    private String profile_picture;

    public User() {
    }

    public User(String username, String bio, String profile_picture) {
        this.setBio(bio);
        this.setUsername(username);
        this.setProfile_picture(profile_picture);
    }
}
