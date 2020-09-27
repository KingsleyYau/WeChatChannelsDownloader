# Copyright (C) 2020 The Mediaserver
# WeChatChannels Downloader Makefile
#
# Created on: 2020/09/27
# Author: Max.Chiu
# Email: Kingsleyyau@gmail.com
#

export MAKE	:=	make

ifeq ($(RELEASE), 1)
CXXFLAGS = -O3 
else
CXXFLAGS = -O2 -g
endif

CXXFLAGS += -std=c++11
CXXFLAGS +=	-Wall -fmessage-length=0 -Wunused-variable -Wno-write-strings -Wno-narrowing -Wno-unused-variable 
CXXFLAGS += -I.

# 注意连接依赖顺序问题, 被依赖的放后面
LIBS =		-L. \
			-Wl,-Bstatic -Lcommon -lcommon \
			-Wl,-Bdynamic -ldl -pthread
			
OBJS = main.o
TARGET = main
	
MODULEDIRS = common
CLEAN_MODULES = $(addprefix _clean_, $(MODULEDIRS))
CLEAN_DEPS = $(addprefix _clean_, $(DEPDIRS))			
.PHONY: clean $(DEPDIRS) $(MODULEDIRS) $(TARGET)
	
$(TARGET): modules $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)
	@echo '################################################################'
	@echo ''
	@echo '# Bulid completed!'
	@echo ''
	@echo '################################################################'
	
$(MODULEDIRS):
	$(MAKE) -C $@
	
$(CLEAN_MODULES):	
	$(MAKE) -C $(patsubst _clean_%, %, $@) clean
	
$(DEPDIRS):
	$(MAKE) -C $@
	
$(CLEAN_DEPS):	
	$(MAKE) -C $(patsubst _clean_%, %, $@) clean		

modules: $(MODULEDIRS)
		
all: $(TARGET)

clean: $(CLEAN_MODULES)
	rm -f $(OBJS) $(TARGET)