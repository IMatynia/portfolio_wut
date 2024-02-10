package pis.service;

import org.springframework.web.multipart.MultipartFile;

import java.io.InputStream;

public interface MinIOService {
    public void uploadImage(MultipartFile file) throws Exception;

    public InputStream getImage(String imageName) throws Exception;

    public void deleteImage(String imageName) throws Exception;
}
