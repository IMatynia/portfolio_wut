package pis.repository;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;
import pis.entity.Session;
import pis.entity.User;

@Repository
public interface SessionRepo extends JpaRepository<Session, Long> {
    Session findByUser(User user);
}