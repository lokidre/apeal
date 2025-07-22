#! /bin/sh

pem_dir=pem

mkdir -p $pem_dir

# create root certificate
openssl req -config root_ssl.cnf -newkey rsa:1024 -sha1 -keyout $pem_dir/root_key.pem -out $pem_dir/root_req.pem -passout file:root_psw || exit 1
openssl x509 -req -in $pem_dir/root_req.pem -signkey $pem_dir/root_key.pem -out $pem_dir/root_cert.pem -sha1 -days 365 -passin file:root_psw || exit 1
cat $pem_dir/root_cert.pem $pem_dir/root_key.pem > $pem_dir/root.pem
openssl x509 -subject -issuer -noout -in $pem_dir/root.pem || exit 1


# server CA
openssl req -config serverca_ssl.cnf -newkey rsa:1024 -sha1 -keyout $pem_dir/serverca_key.pem -out $pem_dir/serverca_req.pem -passout file:serverca_psw || exit 1
openssl x509 -req -in $pem_dir/serverca_req.pem -sha1 -extfile serverca_ssl.cnf -extensions v3_ca -CA $pem_dir/root.pem -CAkey $pem_dir/root.pem -CAcreateserial -out $pem_dir/serverca_cert.pem -passin file:root_psw || exit 1
cat $pem_dir/serverca_cert.pem $pem_dir/serverca_key.pem $pem_dir/root_cert.pem > $pem_dir/server_ca.pem
openssl x509 -subject -issuer -noout -in $pem_dir/server_ca.pem || exit 1


# certificate
openssl req -config server_ssl.cnf -newkey rsa:1024 -sha1 -keyout $pem_dir/server_key.pem -out $pem_dir/server_req.pem -passout file:server_psw || exit 1
openssl x509 -req -in $pem_dir/server_req.pem -sha1 -extfile server_ssl.cnf -extensions v3_ca -CA $pem_dir/server_ca.pem -CAkey $pem_dir/server_ca.pem -CAcreateserial -out $pem_dir/server_cert.pem -passin file:serverca_psw || exit 1
cat $pem_dir/server_cert.pem $pem_dir/server_key.pem $pem_dir/serverca_cert.pem $pem_dir/root_cert.pem > server.pem
openssl x509 -subject -issuer -noout -in server.pem || exit 1



