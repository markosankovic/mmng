#!/usr/bin/env bash

# https://stackoverflow.com/questions/7580508/getting-chrome-to-accept-self-signed-localhost-certificate/

################################
# Become a Certificate Authority
################################

# Generate private key
openssl genrsa -des3 -out myCA.key 2048
# Generate root certificate
openssl req -x509 -new -nodes -key myCA.key -sha256 -days 7305 -out myCA.pem

########################
# Create CA-signed certs
########################

NAME=mmng.local
# Generate a private key
openssl genrsa -out $NAME.key 2048
# Create a certificate-signing request
openssl req -new -key $NAME.key -out $NAME.csr
# Create a config file for the extensions
>$NAME.ext cat <<-EOF
authorityKeyIdentifier=keyid,issuer
basicConstraints=CA:FALSE
keyUsage = digitalSignature, nonRepudiation, keyEncipherment, dataEncipherment
subjectAltName = @alt_names
[alt_names]
DNS.1 = $NAME # Be sure to include the domain name here because Common Name is not so commonly honoured by itself
EOF
# Create the signed certificate
openssl x509 -req -in $NAME.csr -CA myCA.pem -CAkey myCA.key -CAcreateserial \
-out $NAME.crt -days 7305 -sha256 -extfile $NAME.ext



# sudo apt install openssl

#openssl genrsa -out mmng.key 2048
#openssl req -new -key mmng.key -out mmng.csr -subj "/CN=mmng"
#openssl x509 -req -in mmng.csr -signkey mmng.key -out mmng.crt -days 365

#sudo sudo cp mmng.crt /usr/local/share/ca-certificates/
#sudo sudo update-ca-certificates
#sudo ls /etc/ssl/certs
