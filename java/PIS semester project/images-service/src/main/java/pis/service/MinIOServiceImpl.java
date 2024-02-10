package pis.service;

import io.minio.*;
import org.jetbrains.annotations.NotNull;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;
import org.springframework.web.multipart.MultipartFile;

import java.io.InputStream;

@Service
public class MinIOServiceImpl implements MinIOService {
    @Value("${minio.endpoint}")
    private String minioEndpoint;

    @Value("${minio.username}")
    private String minioUsername;

    @Value("${minio.password}")
    private String minioPassword;

    @Value("${minio.imagebucket}")
    private String imageBucketName;
    final private int partSize = 10000000;


    @Override
    public void uploadImage(MultipartFile file) throws Exception {
        MinioClient minioClient = getMinioClient();

        boolean bucketExists = minioClient.bucketExists(
                BucketExistsArgs.builder()
                        .bucket(imageBucketName)
                        .build()
        );
        if (!bucketExists) {
            minioClient.makeBucket(
                    MakeBucketArgs
                            .builder()
                            .bucket(imageBucketName)
                            .build());
        }
        minioClient.putObject(
                PutObjectArgs.builder()
                        .bucket(imageBucketName)
                        .contentType(file.getContentType())
                        .stream(file.getInputStream(), file.getSize(), partSize)
                        .object(file.getOriginalFilename()).build());
    }

    @Override
    public InputStream getImage(String imageName) throws Exception {
        MinioClient minioClient = getMinioClient();

        InputStream imageStream = minioClient.getObject(
                GetObjectArgs.builder()
                        .bucket(imageBucketName)
                        .object(imageName).build()
        );
        return imageStream;
    }

    @Override
    public void deleteImage(String imageName) throws Exception {
        MinioClient minioClient = getMinioClient();

        minioClient.removeObject(RemoveObjectArgs.builder()
                .bucket(imageBucketName)
                .object(imageName)
                .build());
    }

    @NotNull
    private MinioClient getMinioClient() {
        MinioClient minioClient =
                MinioClient.builder()
                        .endpoint(minioEndpoint)
                        .credentials(minioUsername, minioPassword)
                        .build();
        return minioClient;
    }
}
