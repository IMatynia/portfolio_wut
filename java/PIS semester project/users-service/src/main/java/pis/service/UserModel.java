package pis.service;

import pis.entity.User;

import java.util.List;

public interface UserModel {
    public Boolean checkAuthority(Long userId, String sessionId);

    List<User> getAllUsers();

    User getUserById(Long id);

    void createUser(User user);

    Boolean certifyUser(User user, String sessionId);

    boolean deleteUser(Long id);
}
