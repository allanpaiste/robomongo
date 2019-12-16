Bash fronted for CMake
======================

If you want to use this scripts, you need to set single environment variable:

```shell script
brew install qt gcc cmake openssl

cp ${OPENSSL_ROOT}/lib/libssl.dylib ${OPENSSL_ROOT}/libssl.dylib
cp ${OPENSSL_ROOT}/lib/libcrypto.dylib ${OPENSSL_ROOT}/libcrypto.dylib
cp /usr/local/Cellar/openssl/1.0.2t/libssl.dylib /usr/local/Cellar/openssl/1.0.2t/libssl.1.0.0.dylib
cp /usr/local/Cellar/openssl/1.0.2t/libcrypto.dylib /usr/local/Cellar/openssl/1.0.2t/libcrypto.1.0.0.dylib
```

Build Robomongo:

```shell script
bin/configure
bin/build
```

Install Robomongo:

```shell script
bin/install
```
    
Pack Robomongo:

```shell script
bin/pack
```

# IDE changes

After running bin/configure and bin/build ...

Build, Execution, Deployment > CMake > Generation path => build/release

Then use RMB anywhere in the file-tree and choose 'Reload CMake Project'.

Then the run configuration dropdown should be populated and you should be able to build and run Robomongo through 
the IDE. 

The best part about this is that you can run it with a debugger which helps a lot when you're learning how 
to extend it. 
