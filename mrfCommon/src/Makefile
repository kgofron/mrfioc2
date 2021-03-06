TOP=../..
include $(TOP)/configure/CONFIG

USR_CFLAGS += -DDEBUG_PRINT
USR_CPPFLAGS += -DDEBUG_PRINT

#---------------------
# Install include files
#
INC += mrfBitOps.h
INC += mrfCommon.h        # Common MRF event system constants & definitions
INC += mrfCommonIO.h      # Common I/O access macros
INC += mrfFracSynth.h     # Fractional Synthesizer routines
INC += linkoptions.h
INC += mrfcsr.h
INC += mrfpci.h
INC += plx9030.h
INC += plx9056.h

INC += mrf/object.h

INC += mrf/version.h
INC += mrf/version-hg.h
INC += mrf/version-git.h

INC += devObj.h


TESTPROD_HOST += objectTest
objectTest_SRCS += objectTest.c
objectTest_LIBS += mrfCommon $(EPICS_BASE_IOC_LIBS)
TESTS += objectTest

ifeq ($(EPICS_VERSION)$(EPICS_REVISION),314)
ifeq ($(findstring $(EPICS_MODIFICATION),1 2 3 4 5 6 7 8 9),)

# Test framework added in 3.14.10
TESTPROD_HOST += linkoptionsTest
linkoptionsTest_SRCS += linkoptionsTest.c
linkoptionsTest_LIBS += mrfCommon
TESTS += linkoptionsTest

else

# Added to Base in 3.14.10
DBDINC += aSubRecord
mrfCommon_SRCS += aSubRecord.c
endif
endif

#---------------------
# Install DBD files
#
DBD += mrfCommon.dbd

#---------------------
# Build the MRF Common Library
#
LIBRARY_IOC += mrfCommon
mrfCommon_SRCS += mrfFracSynth.c
mrfCommon_SRCS += linkoptions.c
mrfCommon_SRCS += object.cpp
mrfCommon_SRCS += devObjAnalog.cpp
mrfCommon_SRCS += devObjLong.cpp
mrfCommon_SRCS += devObjBinary.cpp
mrfCommon_SRCS += devObjMBB.cpp
mrfCommon_SRCS += devObjMBBDirect.cpp
mrfCommon_SRCS += devObjString.cpp
mrfCommon_SRCS += devObjWf.cpp
mrfCommon_SRCS += devMbboDirectSoft.c
mrfCommon_SRCS += mrfCommon.cpp

mrfCommon_LIBS += $(EPICS_BASE_IOC_LIBS)

#OBJS_IOC += $(mrfCommon_SRCS)


PROD_LIBS += mrfCommon $(EPICS_BASE_IOC_LIBS)

#---------------------
# Build the host-side utilities for creating and analyzing
# the fractional synthesizer control words.
#
PROD_HOST += FracSynthAnalyze
PROD_HOST += FracSynthControlWord

FracSynthAnalyze_SRCS += FracSynthAnalyze.c
FracSynthControlWord_SRCS += FracSynthControlWord.c

#---------------------
# Generic EPICS build rules
#
include $(TOP)/configure/RULES

# $(COMMON_DIR)/mrf/version-git.h::
	# mkdir $(subst,/,\,$(COMMON_DIR))\mrf
# ifeq ($(OS),Windows_NT)
# else
	# [ -d $(COMMON_DIR)/mrf ] || install -d $(COMMON_DIR)/mrf
# endif
# ifneq ($(wildcard $(TOP)/.git),)
	# #printf '#define GIT_VERSION "%s"\n' `git describe --tags` > $@
	# printf '#define GIT_VERSION "2.0.4-win"\n' > $@
# else
# ifeq ($(OS),Windows_NT)
	# echo $$null >> $@
# else
	# touch $@
# endif
# endif

# $(COMMON_DIR)/mrf/version-hg.h::
# ifeq ($(OS),Windows_NT)
	# mkdir $(subst,/,\,$(COMMON_DIR))\mrf
# else
	# [ -d $(COMMON_DIR)/mrf ] || install -d $(COMMON_DIR)/mrf
# endif
# ifneq ($(wildcard $(TOP)/.hg),)
	# #printf '#define HG_VERSION "%s"\n' `hg tip|sed -n -e '/changeset/ s/changeset:\W*//p'` > $@
	# printf '#define HG_VERSION "2.0.4-win"\n' > $@
# else
# ifeq ($(OS),Windows_NT)
	# echo $$null >> $@
# else
	# touch $@
# endif
# endif

#../mrf/version.h: $(COMMON_DIR)/mrf/version-git.h $(COMMON_DIR)/mrf/version-hg.h
