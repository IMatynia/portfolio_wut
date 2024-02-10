package pis.controller;

import io.minio.errors.ErrorResponseException;
import io.minio.errors.MinioException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.core.io.InputStreamResource;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.MediaType;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.multipart.MultipartFile;
import pis.service.MinIOService;

import java.io.IOException;
import java.io.InputStream;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;

@RestController
@RequestMapping("/api/images")
public class ImageUploadController {
    @Autowired
    private MinIOService minIOService;

    @PostMapping("/upload")
    public ResponseEntity<String> handleFileUpload(@RequestPart("file") MultipartFile file) {
        try {
            minIOService.uploadImage(file);
            return new ResponseEntity<>("File uploaded successfully" + file.getOriginalFilename(), HttpStatus.OK);
        } catch (MinioException | IOException | InvalidKeyException | NoSuchAlgorithmException e) {
            return new ResponseEntity<>("Error uploading file", HttpStatus.INTERNAL_SERVER_ERROR);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    @GetMapping("/{imageName}")
    public ResponseEntity<InputStreamResource> getImage(@PathVariable String imageName) {
        try {
            InputStream imageStream = minIOService.getImage(imageName);

            HttpHeaders headers = new HttpHeaders();
            headers.setContentType(MediaType.IMAGE_JPEG);
            return new ResponseEntity<>(new InputStreamResource(imageStream), headers, HttpStatus.OK);
        } catch (ErrorResponseException e) {
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        } catch (MinioException | IOException | InvalidKeyException | NoSuchAlgorithmException e) {
            e.printStackTrace();
            return new ResponseEntity<>(HttpStatus.INTERNAL_SERVER_ERROR);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    @DeleteMapping("/{imageName}")
    public ResponseEntity<String> deleteImage(@PathVariable String imageName) {
        try {
            minIOService.deleteImage(imageName);
            return new ResponseEntity<>(HttpStatus.OK);
        } catch (ErrorResponseException e) {
            return new ResponseEntity<>(HttpStatus.NOT_FOUND);
        } catch (MinioException | IOException | InvalidKeyException | NoSuchAlgorithmException e) {
            e.printStackTrace();
            return new ResponseEntity<>(HttpStatus.INTERNAL_SERVER_ERROR);
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }
}
