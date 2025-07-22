# APEAL - Application Library
---

APEAL is a general purpose C++ class library.

+ All the code is placed in header files, so no compilation is required.
  You just have to include the header file.

+ All classes are implemented using Policy Based Design. When you
  use the class you decide how it should behave.

+ Includes rich set of routines working with
  . file systems
  . sockets
  . threads
  . mutexes
  . internet protocols
  . networks
  . various parsers (including XML)
  . databases
  . process management
  . and much more...

+ Platform and compiler independent.


For installation instructions please read the INSTALL file.

For terms and conditions please read the LICENSE file.

For ready-to-use code fragments please visit
http://www.asperix.org/copymycode and select APL category

For article about Policy Based Design and its implementation please visit
http://www.freakycpp.com/policies



## Installation

### Prerequisits

google/zx


There are no prerequisites and also no compilation required.
Installation process is basically copying the source files into the system.





## Linux/Unix

Install libpng and libuuid libraries

    sudo apt install libpng-dev uuid-dev




./configure [--prefix=<prefix>]
make
make install (or more likely su -c "make install" or sudo make install )

## Mac OS

Install libpng.

If you are using brew then

    brew install libpng
    
    



## WINDOWS with Cygwin

The installation under Cygwin should be the same as with Linux/Unix case
with few exceptions:
.   Windows doesn't have "execute" attribute, so you can't just execute
    the configure script. You have to run bash and tell it to execute the
    configure script.
.   su command under Windows doesn't make much sense.
.   There are many 'make' utilities exist for Windows. It is safer
    to call GNU make by gmake instead of make.
.   The current version of cygwin seems to prefer /usr instead of /usr/local
    It also places the pkg-config information into /lib/pkgconfig

The installation under Cygwin will look like this:

bash configure --prefix=/usr
gmake
gmake install




## WINDOWS without Cygwin


- pick some directory as installation folder (e.g. '/sources')
- create a directory called 'apl' in that folder. (e.g. '/sources/apl)
- copy or extract the whole library into that new directory (e.g. '/sources/apl')
- make sure that the parent of apl directory (e.g. '/sources') included in
  the compiler's include directory list

Environmental variable


	CMAKE_PREFIX_PATH=c:\local


