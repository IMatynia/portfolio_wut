package pis.service;

public interface UsersModel {
    public Boolean checkAuthority(Long userId, String sessionId);
}
