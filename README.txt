Running keygen:
gcc -std=gnu99 -o keygen keygen.c
keygen 256 > mykey

Running encryption files:

gcc -std=gnu99 -o enc_client enc_client.c
gcc -o enc_server enc_server.c

./enc_server 1080 &

./enc_client localhost 1080 plaintext mykey
