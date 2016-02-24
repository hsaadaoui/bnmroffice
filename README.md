# BnmrOffice
BnmrOffice is a data analysis program used to analyze scientific data, especially 
TRIUMF Beta-NMR data, and ASCII data in general. The program is also useful to carry 
simulations, database query, and converting units.
BnmrOffice is open source and released under the General Pulbic Licence (GPL).
No warranty or guarrantee of the results is implied.
If you are using this software in offline data analysis, please acknowledge the author
in your acknowledgments section, if he is not a co-author of the project.
It is the least you can do to encourage future developments and maintenance of the code.
For any questions, please email at "saadaoui ^at^ triumf.ca".

![alt tag](https://github.com/hsaadaoui/bnmroffice/blob/master/etc/doc/bnmroffice.png)

Install instructions

 - In addition to bnmroffice, you also need: (I) Qt, (II) Minuit, (III) MUD, (IV) XMGR, (V) GSL.

1) Qt: 
  - Download the Qt online installer from http://www.qt.io/download-open-source/
  
  - Install Qt.4.x.x or 5.x binary & source packages.


2) Minuit:

-i) Download latest Minuit2 located at:

      http://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/release/download.html

-ii) Unpack and cd

      $ tar -xvf minuit.tar.gz -C ~/

      $ cd ~/minuit

-ii) To install follor the instructions at:

      http://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/gettingStarted/autoconf.html

-iii) Make SURE that the tests in the tutorial are running as described in the link below

      http://seal.web.cern.ch/seal/snapshot/work-packages/mathlibs/minuit/gettingStarted/testOutput.html

-iv) - Copy (as superuser) the miniut libraries from  minuit/src/.lib/liblcg_Minuit.* to /usr/lib/

      $ sudo cp minuit/src/.lib/liblcg_Minuit.* /usr/lib/

-v) - Update ldconfig

      $ sudo ldconfig

3) MUD: 

-i) Download the MUD library source archive mud.tar.gz from http://musr.ca/mud.

     $ wget http://musr.ca/mud/mud.tar.gz

-ii) Unpack and cd

     $ tar -zxvf mud.tar.gz -C ~/

     $ cd ~/mud

-iii) Run make with root access (read the install instructions withing the mud package)

     $ sudo make all

-iv) Copy the files mud.h and libmud.a into /usr/lib/ and /usr/include/

     $ sudo cp ./lib/libmud.a /usr/lib/

     $ sudo cp ./src/mud.h /usr/include/


4) XMGR: 

-i) downloand and Unpack  xmgr from https://github.com/mlund/xmgr-resurrection

     $ wget https://github.com/mlund/xmgr-resurrection/archive/master.zip

     $ unzip master.zip -d ~/

     $ cd ~/xmgr-resurrection-master

-ii) XMGR has a feature of displaying the copyright each time it starts from a terminal.
     This can obscure the user from seen the error messages produced by bnmroffice.
     To avoid this, comment the lines 107-111 in main.c and save.

iii) Typically the following packages are required: libice-dev libx11-dev lesstif2-dev libxmu-dev libxpm-dev.
     Install if not found in your OS. Try, on ubunto/related systems, get-all (or your system's alternative).

    $ sudo get-all install libice-dev libx11-dev lesstif2-dev libxmu-dev libxpm-dev

-iv) Read README.md to compile the code. Each system is different; try these given steps:

    $ cmake . -DENABLE_NETCDF=on

    $ cmake . -DCMAKE_INSTALL_PREFIX=/usr/local

    $ make

    $ sudo make install

-v) If all went well, open the execuatble "xmgr" located likely in src/ or somewhere else within your folder.

-vi) You must locate this file and copy it to /usr/bin /usr/local/bin/ and if it is not already there.

     $ sudo cp xmgr /usr/bin/

     $ sudo cp xmgr /usr/local/bin/

Note: Please contact  for other options if your are unable to install XMGR.


5)  gsl: (optional) 

-i) download latest gsl from http://gnu.mirror.iweb.com/gsl/

-ii) Unpack and cd

    $  tar -xvf gsl-latest.tar.gz -C ~/

    $  cd ~/gsl-latest

-iii) Run configure

    $ ./configure

-iv) Run make

    $ make

-v) Run make install as root

    $ sudo make install

-vi) Update libraries cache

    $ sudo ldconfig



6) bnmroffice:

-i) cd to the downloaded package

    $ cd ~/bnmroffice

-ii) change the path to bnmr Data, and bnqr Data as defined in constants.h

    $ gedit src/constants.h

-iii) If Qt binaries are not in your path, set the env (Must locate where qmake is)

    $ PATH=/usr/Qt/5.4/gcc_64/bin:$PATH  (change "/usr/Qt/5.4/gcc_64/bin" as per your system)
    $ export PATH

    Note: You don't need to do this, if you intalled Qt with an rpm manager.

-iv) Run qmake. 

    $ qmake (or $qmake-qt5)

-v ) Run make and make install (as root)

    $ make
    $ sudo make install

-vi) cd to directory fct/ and compile all the libraries using the script compile

    $ cd fct
    $ sudo ./compile

-vi) To test the gui, invoke

    $ bnmroffice

    or

    $./bnmroffice if not installed as root.

Read the instructions/tutorials under ------> menubar/Help <--------
Hassan Saadaoui
saadaoui ^at^ triumf.ca
Copyright© 2015
