#!/usr/bin/env bash

sudo apt install openssl

openssl genrsa -out mmng.key 2048
openssl req -new -key mmng.key -out mmng.csr -subj "/CN=mmng"
openssl x509 -req -in mmng.csr -signkey mmng.key -out mmng.crt -days 365

sudo sudo cp mmng.crt /usr/local/share/ca-certificates/
sudo sudo update-ca-certificates
sudo ls /etc/ssl/certs
