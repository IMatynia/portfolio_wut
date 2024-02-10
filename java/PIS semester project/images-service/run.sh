#!/bin/bash
# uruchamiać PO minio!!!
# minio: docker run -p 9000:9000 -p 9001:9001 quay.io/minio/minio server /data --console-address ":9001"
docker run --name minio-service -p 9898:9898 minio-service