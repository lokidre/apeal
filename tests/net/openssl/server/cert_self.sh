
# -passin file:server_psw

openssl genrsa -des3 -passout file:server_psw -out server.key 1024 || exit 1
openssl req -new -config server_ssl.cnf -passin file:server_psw -key server.key -out server.csr -days 365 || exit 1
openssl req -x509 -config server_ssl.cnf -passin file:server_psw -key server.key -in server.csr -out server.crt -days 365 || exit 1
openssl rsa -passin file:server_psw -in server.key -out server.key

# cat server.crt server.key > server.pem


# chmod 400 /root/ispconfig/httpd/conf/ssl.key/server.key

