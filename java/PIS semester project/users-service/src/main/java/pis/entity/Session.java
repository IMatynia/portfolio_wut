package pis.entity;

import jakarta.persistence.*;
import lombok.Getter;
import lombok.Setter;

@Entity
@Table(name = "security_hashes")
@Getter
public class Session {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    @Column(name = "hash_id")
    private Long hashId;

    @OneToOne
    @JoinColumn(name = "user_id", referencedColumnName = "id", nullable = false)
    @Setter
    private User user;

    @Column(name = "password_hash", nullable = false)
    @Setter
    private String passwordHash;

    public Session() {}
    public Session(User user, String passwordHash)
    {
        this.setUser(user);
        this.setPasswordHash(passwordHash);
    }
}