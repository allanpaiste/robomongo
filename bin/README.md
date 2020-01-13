Pre-requisites for build
========================

Prerequisites
-------------

Note that this guide DOES assume that all the dependencies/sources share same root path (example: every library is within
the folder /my/projects). This requirement/recommendation DOES include this project as well. 

If you don't have such a setup for dependent libraries in such a way, you are STRONGLY encouraged to do so. 

Install Mongo-Shell
"""""""""""""""""""

Resides in another repository which you should clone to the same projects root.

```shell
git clone git@github.com:allanpaiste/robomongo-shell.git
```

After this you should follow the guide in bin/README.md

Install Homebrew
""""""""""""""""

This will just make the installation of certain libraries so much easier.

```shell
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
``` 

Install Open SSL
""""""""""""""""

This guide assumes that you are in the root of the repositories 

```shell
curl -LO https://www.openssl.org/source/old/1.0.2/openssl-1.0.2u.tar.gz > openssl-1.0.2u.tar.gz
tar -xvzf openssl-1.0.2*.tar.gz
rm openssl-1.0.2u.tar.gz
mv openssl-* openssl

cd openssl
./Configure darwin64-x86_64-cc shared --openssldir="@rpath"

make -j9

# You might be getting "WARNING: can't open config file: @rpath/@rpath/openssl.cnf" at the end end of 
# the build process which you can ignore 
```

Install Gcc & CMake 
"""""""""""""""""""

```shell
brew install gcc cmake
```

Install Qt
""""""""""

This will be a ~3.5G download.

Execute it in the root folder of the repositories

```shell
curl -Lo- http://download.qt.io/archive/qt/5.9/5.9.3/qt-opensource-mac-x64-5.9.3.dmg > qt-opensource-mac-x64-5.9.3.dmg
hdiutil attach qt-opensource-mac-x64-5.9.3.dmg
open /Volumes/qt-opensource-mac-x64-5.9.3/qt-opensource-mac-x64-5.9.3.app

# Install it to the /path/to/project/roots/qt
# Make sure that the following are included in the installation:
# * macOS
# * (All Qt libraries)
```

Build
-----

Configure Robomongo project (only one of these is needed but when working with the project, it's better to have 
both release and debug configurations ready):

```shell
# Release
bin/configure

# Debug/Develop
bin/configure debug
```

Build Robomongo (release):

```shell
# Release
bin/build

# Debug/Develop
bin/build debug
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

Navigate to:

    Build, Execution, Deployment > CMake
    
Create two profiles as follows:

## Release:
   
With the following config: 
    
    Build Type => Release
    Generation path => build/release
    
## Debug:

    Build Type => Debug
    Generation path => build/debug

Save and then use RMB anywhere in the file-tree and choose 'Reload CMake Project'.

Then the run configuration dropdown should be populated and you should be able to build and run Robomongo through 
the IDE. 

The best part about this is that you can run it with a debugger which helps a lot when you're learning how 
to extend it. 

# Debug Mode 

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