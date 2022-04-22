# DES

This is my DES implementation in C. The program can encrypt texts in .txt file to binary in .des file, whitch contains binary, decrypt .des files to .txt files, and read and print binary in hex in console.

The repo is just my C programming learning practice, so the source code must seemed suck. Please do not use this in any important case.

## Get start

To get start, you need to compile source code to binary using cmake:

```
cd des
mkdir build
cd build
cmake ..
make
```

then mv the example txt and key file to the build directory, where the binary is compiled:

```
cp ../example/input.txt ./
cp ../example/key.txt ./
```

## Encrypt

In build directory type:

```
./des -E input.txt
```

or by default, just type:

```
./des -E
```

To encrype file named "input.txt", and the encrypted file is named "output.des".

## Decrypt

In build directory type:

```
./des -D output.des
```

or

```
./des -D
```

to decrypt file named "output.des", and the decrypted file is named "message.txt".

## Read

In build directory type:

```
./des -R output.des
```

or

```
./des -R
```

to read the .des file named "output.des".

## Side note

* Type:

```
./des -H
```

for help.

* The key has to be 8 character long(8 bytes).



