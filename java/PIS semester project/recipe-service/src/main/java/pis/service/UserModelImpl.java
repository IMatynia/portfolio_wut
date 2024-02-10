package pis.service;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.http.*;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestTemplate;
import org.springframework.web.util.UriComponentsBuilder;

@Service
public class UserModelImpl implements UsersModel{
    @Value("${services.users.endpoint}")
    private String usersAuthEnpoint;

    @Override
    public Boolean checkAuthority(Long userId, String sessionId) {

        RestTemplate restTemplate = new RestTemplate();
        UriComponentsBuilder uriBuilder = UriComponentsBuilder.fromUriString(usersAuthEnpoint)
                .queryParam("user_id", userId);
        HttpHeaders headers = new HttpHeaders();
        headers.set("X-Session", sessionId);
        HttpEntity<?> requestEntity = new HttpEntity<>(headers);

        ResponseEntity<Void> responseEntity = restTemplate.exchange(
                uriBuilder.toUriString(),
                HttpMethod.GET,
                requestEntity,
                Void.class);

        return responseEntity.getStatusCode() == HttpStatus.OK;
    }
}
