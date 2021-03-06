#####################################################################
include ../../../rules.mk

MPP_PATH := /home/lroyd/pjsip/mpp
COMMON_DIR := $(MPP_PATH)/sample/common
LIBS_DIR := $(MPP_PATH)/lib

INC_FLAGS := -I$(COMMON_DIR) -I$(MPP_PATH)/include -I$(MPP_PATH)/component/acodec	
	
CHIP_FLAGS := -DCHIP_ID=CHIP_HI3518E_V200 
#SENSOR_TYPE := SAMPLE_VI_MODE_BT1120_720P
#-DHI_ACODEC_TYPE_INNER -DHI_XXXX -DSENSOR_TYPE=$(SENSOR_TYPE) -Dhi3518e -Dhi3518ev200 -DHICHIP=0x3518E200 -DHI_RELEASE  -DISP_V2 

LIB_AUDIO := -lVoiceEngine -lupvqe -ldnvqe

LIB_SENSOR := -lisp -l_iniparser -l_hiae -l_hiawb -l_hiaf -l_hidefog 

#rm -rf : -l_cmoscfg  

LIB_MPI := -lmpi -live -lmd

LIBS_FLAGS := $(LIB_AUDIO) $(LIB_SENSOR) $(LIB_MPI) 
LIBS_LD_FLAGS := -L$(LIBS_DIR)

CFLAGS += -Wall -g $(INC_FLAGS) $(CHIP_FLAGS) -lpthread -lm




