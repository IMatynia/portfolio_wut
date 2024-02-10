package pis.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import pis.entity.Session;
import pis.entity.User;
import pis.repository.SessionRepo;
import pis.repository.UserRepo;

import java.util.List;

@Service
public class UserModelImpl implements UserModel {
    @Autowired
    private UserRepo userRepo;

    @Autowired
    private SessionRepo sessionRepo;

    @Override
    public Boolean checkAuthority(Long userId, String sessionId) {
        User user = userRepo.findById(userId).orElse(null);
        if (user == null) return false;
        Session session = sessionRepo.findByUser(user);
        return (session.getPasswordHash().equals(sessionId));
    }

    @Override
    public List<User> getAllUsers() {
        return userRepo.findAll();
    }

    @Override
    public User getUserById(Long id) {
        return userRepo.findById(id).orElse(null);
    }

    @Override
    public void createUser(User user) {
        userRepo.save(user);
    }

    @Override
    public Boolean certifyUser(User user, String sessionId) {
        Session session = new Session(user, sessionId);
        sessionRepo.save(session);
        return true;
    }

    @Override
    public boolean deleteUser(Long id) {
        User user = userRepo.findById(id).orElse(null);
        if(user != null)
        {
            Session session = sessionRepo.findByUser(user);
            sessionRepo.delete(session);
            userRepo.delete(user);
            return true;
        }
        return false;
    }
}
