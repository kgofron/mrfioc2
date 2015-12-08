# mrfioc2

[mrfioc2](https://github.psi.ch/projects/ED/repos/mrfioc2/browse) is an EPICS device support for the Micro Research Finland ([MRF](http://www.mrf.fi/)) timing system (in short mrfioc2 driver). The mrfioc2 enables us to configure and use the event generators and event receivers in the timing system. It comprises of EPICS device support for MRF timing system and uses [devlib2](https://github.com/epics-modules/devlib2/) with additional kernel modules (eg. PCIe) for communication with the hardware.

This project is a continued development from the original mrfioc2 driver available on [GitHub](https://github.com/epics-modules/mrfioc2)


## Documentation
The documentation is available in the `documentation` folder:

* `evr_manual.pdf` is a manual describing how event receiver works and how to configure it. It also provides some general information on the mrfioc2 driver.
* `tutorial.pdf` contains a set of tutorials that instruct the user on how to set up and use an event receiver. Mind that the tutorials are tailored to PSI users, though the functionality and settings of the event receiver are the same.
* `oldDocs` folder contains the old documentation from the original mrfioc2 driver.
* `doxy` folder contains the generated doxygen documentation. For information on how to generate it, inspect readme in `documentation` folder.

## Quick start (PSI)
To set up an IOC application for EVR we need to set up a startup script and a substitution file matching the timing card form factor. Suitable ones are available in the `PSI/example` folder:

* EVG
    * example startup script ([`evg_VME_startup.script`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evg_VME_startup.script?at=b284e63032f1ad4f5975ec1c66174b75ea10700b&raw))
    * example substitution files ([`evg_VME-230.subs`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evg_VME-230.subs?at=567d1746e6cbcd7e3484626383e3f720ab3dc236&raw), [`evg_VME-300.subs`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evg_VME-300.subs?at=567d1746e6cbcd7e3484626383e3f720ab3dc236&raw))
* EVR
    * example startup scripts ([`evr_VME_startup.script`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evr_VME_startup.script?at=b284e63032f1ad4f5975ec1c66174b75ea10700b&raw), [`evr_PCIe_startup.script`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evr_PCIe_startup.script?at=b284e63032f1ad4f5975ec1c66174b75ea10700b&raw))
    * example substitution files ([`evr_cPCI-230.subs`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evr_cPCI-230.subs?at=1cbfa97936f8e17f3f0b7bfe5be391451639ebb0&raw), [`evr_PCIe-300.subs`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evr_PCIe-300.subs?at=1cbfa97936f8e17f3f0b7bfe5be391451639ebb0&raw), [`evr_VME-230.subs`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evr_VME-230.subs?at=1cbfa97936f8e17f3f0b7bfe5be391451639ebb0&raw), [`evr_VME-300.subs`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evr_VME-300.subs?at=ed41ffa2d7a698345e07d8dfaf5d1d9e5df2482e&raw))

For example, to set up a basic IOC for use with EVR-VME-300 timing card, user should:

* prepare a switable IOC structure in a `TOP` folder (where `TOP` is your project folder)
* copy `PSI/example/evr_VME-300.subs` to `TOP/cfg/EVR0.subs`
* configure parameters of the EVR by setting macros in `TOP/cfg/EVR0.subs`. Individual parameters are described in [`documentation/evr_manual.pdf`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/documentation/evr_manual.pdf?at=74f4ab8a17ca77753e84dffeb757e54b1c6a13e6&raw), and tutorials for various scenarios are available in [`documentation/tutorial.pdf`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/documentation/tutorial.pdf?at=56647b281d987b2907ab94e3697e7f69ba328912&raw).
* add the following to your startup script (available in [`PSI/example/evr_VME_startup.script`](https://github.psi.ch/projects/ED/repos/mrfioc2/browse/PSI/example/evr_VME_startup.script?at=b284e63032f1ad4f5975ec1c66174b75ea10700b&raw)):
    
        require mrfioc2
    
        ##########################
        #-----! EVR Setup ------!#
        ##########################    
    
        ## The following parameters are available to set up the device. They can either be set as an epics environmental variable, or passed as a macro to the 'runScript' command:
        # The following macros are available to set up the mrfioc2:
        # SYS           MTEST-VME-TIMINGTEST ## is used as a prefix for all records. In this example it is set at the beginning using 'epicsEnvSet'
        # DEVICE		"EVR0"		## is the event receiver / timing card name. (default: EVR0)
        # EVR_SLOT		3			## is the VME crate slot where EVR is inserted. (default: 3)
        # EVR_MEMOFFSET	0x3000000	## is the base A32 address (default: 0x3000000)
        # EVR_IRQLINE 	0x5			## is the interrupt level. (default: 0x5)
        # EVR_IRQVECT	0x26		## is the interrupt vector (default: 0x26)
        # EVR_SUBS is the path to the substitution file that should be loaded. (default: cfg/$(DEVICE).subs=cfg/EVR0.subs)
    
        runScript $(mrfioc2_DIR)/mrfioc2_evr-VME.cmd, "SYS=MTEST-VME-TIMINGTEST, DEVICE=EVR0, EVR_SLOT=3, EVR_MEMOFFSET=0x3000000, EVR_IRQLINE=0x5"

* use `swit -V` to deploy the IOC
* run the GUI by issuing the following command: `start_EVR.sh -s MTEST-VME-TIMINGTEST -f VME-300`

## Using the application
As with any EPICS application, build procedure produces all the necessary database files and an IOC for each architecture built. An example application for the `linux-x86_64` architecture is available in `iocBoot` folder. For more details inspect the `evr_manual.pdf` available in the `documentation` folder.

GUIs are available:

* `evgMrmApp/opi/EVG/` contains the caQtDm GUI for event generator (event master)
* `evrMrmApp/opi/EVR/` contains the caQtDm GUI for event receiver and health monitor for EVR.

Each folder contains a readme file which explains how to run the GUIs.

### PSI
Example substitution files and startup scripts are available in the `PSI/example` folder. For more details inspect the `evr_manual.pdf` and `tutorial.pdf` available in the `documentation` folder.


## Supported hardware

* EVG VME-230: VME-230 form factor event generator. (tested and working)
* EVG VME-300: VME-300 form factor event master / event generator.  (tested, check known issues)
* EVG cPCI-230: cPCI-230 form factor event generator (not tested, no GUI)
* EVR VME-230: VME-230 form factor event receiver. (tested and working)
* EVR VME-300: VME-300 form factor event receiver.  (tested, check known issues)
* EVR PCIe-300: PCIe-300 form factor event receiver. (tested and working with firmware version 7. Not everything works with firmware version 202 - check known issues)
* EVR cPCI-230: cPCI-230 form factor event receiver (not tested, no GUI)

mrfioc2 driver supports hardware firmware versions up to and including 202.
Minimal supported firmware version for:

* EVG is 3,
* PCIe form factor EVR is 3,
* VME form factor EVR 4.

## Known issues
* 300-series hardware with firmware version 202:
    * some EVR-PCIe-300 devices with firmware version 202 do not work at 142.876 MHz event clock. They were tested, and were working, at 125 MHz event clock.
    * mapping two output sources to one output was not tested on EVR-PCIe-300 devices.
    * data buffer sending does not work if delay compensation is disabled.
    * in addition to the normal segmented data buffer operation, data buffer interrupt also gets triggered by the control logic that was used in older hardware. This in turn means reduced performance, since data buffer handling is triggered when there is no data buffer update. Awaiting firmware fix.
    * CML: only frequency mode works, since hardware addresses which store patterns are currently not accessible.
    * EVM-VME-300: Input registers for FrontInp0 and FrontInp2 are linked together in hardware. Awaiting firmware fix.
* sending the data buffer upstream does not work on EVR-VME-300 and was not tested on other cards.

## Dependencies

- [EPICS base](http://www.aps.anl.gov/epics/base/R3-14/index.php) >= 3.14.8.2
- [devlib2](https://github.com/epics-modules/devlib2/) >=2.6
- [MSI tool]( http://www.aps.anl.gov/epics/extensions/msi/index.php) to expand databases (included in EPICS base >= 3.15.1)

__Optional:__

For building the documentation (for more information inspect readme in `documentation` folder):

* [Inkscape](https://inkscape.org/en/) tool for converting _svg_ image format to _pdf_ format.
* LaTeX environment (pdflatex, bibtex) to build the documentation from latex source to pdf format.
* [doxygen](http://www.stack.nl/~dimitri/doxygen/) for generating documentation from source code


## Building from scratch

The mrfioc2 driver is structured as an ordinary EPICS application. In order to build it from source:

* clone the sources from git repository by running command `git clone https://github.psi.ch/scm/ed/mrfioc2.git`, which creates a top folder called `mrfioc2`.
* update files in `mrfioc2/configure` folder to match your system, and to include additional libraries to be build together with the driver (eg. set paths in `configure/RELEASE`).
* run `make -f Makefile` in the `mrfioc2` folder.


Outputs of the build command are:

* Libraries
    * `libevgMrm.so` is event generator/event master (EVM) library
    * `libevrMrm.so` is event receiver (EVG) library
    * `libmrfCommon.so` is required by EVG and EVM and contains common functions and definitions (eg. device support object model)
    * `libmrmShared.so` contains code shared between EVG and EVM, thus required by both. It also contains __kernel modules__ required by the mrfioc2 driver.

* IOC for each architecture that is build using the above libraries, and any other included from `configure/RELEASE`
* Database files

### PSI
Building the driver on the PSI infrastructure is a bit different, since it leverages the driver.makefile. In order to build it:

* clone the sources from git repository by running command `git clone https://github.psi.ch/scm/ed/regdev.git`, which creates a top folder called `mrfioc2`.
* run `make` in the `mrfioc2` folder on the build server.
* run 'make db' to create database files
* to install the driver run `make install` in the `mrfioc2` folder on the build server.

The driver builds as a single library, which can be loaded using `require` to your IOC. Installation process also copies all the necessary support files (eg. templates) to the appropriate module folder. For more options inspect driver.makefile and require documentation available at the PSI wiki.

## Kernel sources
mrfioc2 driver uses a special kernel module for communication with hardware. Sources of the `mrf.ko` kernel module are available in `mrmShared/linux`.

## Authors

* Tom Slejko (tom.slejko@cosylab.com)
* Jure Krašna (jure.krasna@cosylab.com)
* Sašo Skube (saso.skube@cosylab.com)

Original driver developers:

* Michael Davidsaver (mdavidsaver@bnl.gov)
* Jayesh Shah (jshah@bnl.gov)
* Eric Bjorklund (bjorklund@lanl.gov)

Please send an e-mail to saso.skube@cosylab.com if anyone is missing!