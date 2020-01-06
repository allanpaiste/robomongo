Bash fronted for CMake
======================

If you want to use this scripts, you need to set single environment variable:

```shell
brew install qt gcc cmake openssl

OPENSSL_ROOT=/usr/local/Cellar/openssl/1.0.2t

cp ${OPENSSL_ROOT}/lib/libssl.dylib ${OPENSSL_ROOT}/libssl.dylib
cp ${OPENSSL_ROOT}/lib/libcrypto.dylib ${OPENSSL_ROOT}/libcrypto.dylib
cp ${OPENSSL_ROOT}/libssl.dylib ${OPENSSL_ROOT}/libssl.1.0.0.dylib
cp ${OPENSSL_ROOT}/libcrypto.dylib ${OPENSSL_ROOT}/libcrypto.1.0.0.dylib
```

Configure Robomongo project (only one of these is needed but when working with the project, it's better to have 
both release and debug configurations ready):

```shell
bin/configure
bin/configure debug
```

Build Robomongo (release):

```shell
bin/build
```

Install Robomongo:

```shell
bin/install
```
    
Pack Robomongo:

```shell
bin/pack
```

# IDE changes

After running bin/configure:

    Build, Execution, Deployment > CMake > Build Type => Release
    Build, Execution, Deployment > CMake > Generation path => build/release

Then use RMB anywhere in the file-tree and choose 'Reload CMake Project'.

Then the run configuration dropdown should be populated and you should be able to build and run Robomongo through 
the IDE. 

The best part about this is that you can run it with a debugger which helps a lot when you're learning how 
to extend it. 

# Debug mode 

Good for development and exploration (otherwise IDE debug mode inspect is not very useful).

IMPORTANT: You also have to rebuild robomongo-shell in debug mode! Build it before continuing.

```shell
bin/configure debug
```

After this, you should reload the project in IDE (click anywhere in the file-tree view and choose 'Reload CMake Project')

IMPORTANT: You also have to rebuild robomongo-shell in debug mode!!

And change ...

    Build, Execution, Deployment > CMake > Generation path => build/release
    
... to build for debugging.