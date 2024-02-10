package pis.controller;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import pis.entity.User;
import pis.service.UserModel;

import java.util.List;

@RestController
@RequestMapping("/api/users")
public class UserApiController {
    @Autowired
    private UserModel userModel;

    private static final Logger logger = LoggerFactory.getLogger(UserApiController.class);

    @GetMapping
    public ResponseEntity<List<User>> getAllUsers() {
        List<User> users = userModel.getAllUsers();
        return new ResponseEntity<>(users, HttpStatus.OK);
    }

    @GetMapping("/{id}")
    public ResponseEntity<User> getUserById(@PathVariable Long id) {
        User user = userModel.getUserById(id);
        if (user != null) {
            return new ResponseEntity<>(user, HttpStatus.OK);
        } else {
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
    }

    @PostMapping
    public ResponseEntity<User> addUser(@RequestBody User user, @RequestHeader("X-Session") String sessionId) {
        userModel.createUser(user);
        Boolean success = userModel.certifyUser(user, sessionId);
        if (success) {
            return new ResponseEntity<>(user, HttpStatus.CREATED);
        } else {
            return new ResponseEntity<>(HttpStatus.INTERNAL_SERVER_ERROR);
        }
    }

    @DeleteMapping("/{id}")
    public ResponseEntity<Void> deleteUser(@PathVariable Long id, @RequestHeader("X-Session") String sessionId) {
        Boolean accessGranted = userModel.checkAuthority(id, sessionId);
        if (!accessGranted) {
            logger.warn(String.format("Deletion forbidden, invalid authority for user with session %s trying to act as %d", sessionId, id));
            return new ResponseEntity<>(HttpStatus.FORBIDDEN);
        }

        boolean deleted = userModel.deleteUser(id);
        if (deleted) {
            return new ResponseEntity<>(HttpStatus.NO_CONTENT);
        } else {
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        }
    }

    @GetMapping("/auth")
    public ResponseEntity<Void> authUser(@RequestParam("user_id") Long user_id, @RequestHeader("X-Session") String sessionId) {
        boolean success = userModel.checkAuthority(user_id, sessionId);
        if (success) {
            return new ResponseEntity<>(HttpStatus.OK);
        } else {
            return new ResponseEntity<>(HttpStatus.FORBIDDEN);
        }
    }
}
