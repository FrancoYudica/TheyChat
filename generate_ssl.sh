#!/bin/bash

# Set variables
SSL_DIR="$HOME/.ssl/TheyChat"
CERT_FILE="$SSL_DIR/certificate.pem"
KEY_FILE="$SSL_DIR/private.key"

# Create the TheyChat directory if it doesn't exist
mkdir -p "$SSL_DIR"

# Generate the private key
openssl genpkey -algorithm RSA -out "$KEY_FILE" -aes256
chmod 600 "$KEY_FILE"

# Generate the self-signed certificate
openssl req -new -x509 -key "$KEY_FILE" -out "$CERT_FILE" -days 365 -subj "/C=US/ST=State/L=City/O=Organization/OU=OrgUnit/CN=theychat.example.com"
chmod 644 "$CERT_FILE"

# Inform the user
echo "SSL key and certificate have been generated and placed in $SSL_DIR"
echo "Private Key: $KEY_FILE"
echo "Certificate: $CERT_FILE"
