Installation on a Linux system is relatively straightforward. The instructions below will guide you through installing all the required packages and compiling the source on a Debian-based system. These instructions have been tested under Ubuntu 'Hardy Heron' 8.04

### Install cmake, subversion (cvs) and newest compiler suite ###
```
sudo apt-get install cmake subversion gcc
```

### Install python modules, tk, tcl ###
```
sudo apt-get install python2.5-dev python-numpy python-scipy python-setuptools python-vtk python-wxgtk2.8 python-pyrex freeglut3-dev libxtst-dev swig tk-dev tcl-dev
```

### Install Enthought ETS ###
(make sure ETS 2.7.1 isn't installed)
_new instructions_
sudo easy\_install ets[nonets](nonets.md)

_old instructions_
Different easy install eggs are available for different Linux distributions. The example below installs the egg for Ubuntu 'Gusty Gibbon'. You can see the different eggs [here](http://code.enthought.com/enstaller/eggs/), and will need to change 'ubuntu/gusty' depending on the distribution you are using.
```
sudo easy_install -f http://code.enthought.com/enstaller/eggs/ubuntu/gutsy --prefix=/usr/local ets
```

### Install VTK ###
```
sudo apt-get install libvtk5 libvtk5-dev
```

_old instructions_
You need to build VTK from source to get the right header files and python bindings, so it is not possible to use apt. You can download and build VTK using CMake

```
mkdir ~/install
cd ~/install
wget http://www.vtk.org/files/release/5.2/vtk-5.2.0.tar.gz
tar xzf vtk-5.2.0.tar.gz 
cd VTK
ccmake .
```

Press 'c' to configure, then set the options as shown below

```
BUILD_EXAMPLES                  OFF                                          
BUILD_SHARED_LIBS               *ON*                                           
BUILD_TESTING                   ON                                           
CMAKE_BUILD_TYPE                                                             
CMAKE_INSTALL_PREFIX            /usr/local                                   
VTK_DATA_ROOT                   VTK_DATA_ROOT-NOTFOUND                       
VTK_USE_INFOVIS                 ON                                           
VTK_USE_PARALLEL                OFF                                          
VTK_USE_RENDERING               ON                                           
VTK_USE_VIEWS                   ON                                           
VTK_WRAP_JAVA                   OFF                                          
VTK_WRAP_PYTHON                 *ON*                                           
VTK_WRAP_TCL                    *ON*                                          
CMAKE_BACKWARDS_COMPATIBILITY    2.4                                          
```

Press 'c' to configure again, then 'g' to generate. CMake will then automatically exit. To build VTK type

```
sudo make
```

which will take some time. To install type

```
sudo make install
```

### Checkout and compile open-ebsd source ###
mkdir ~/open-ebsd
cd ~/open-ebsd
svn checkout http://open-ebsd.googlecode.com/svn/trunk/ open-ebsd-read-only
mkdir build
cd build
cmake ../open-ebsd-read-only
make