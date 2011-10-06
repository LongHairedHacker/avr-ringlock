#
#	Makefile
#	Created on: Sep 28, 2011
#	Author: sebastian
#
#	This file is part of the RingLock-library for Atmel AVR MCUS.
#
#	RingLock is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	RingLock is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with RingLock.  If not, see <http://www.gnu.org/licenses/>.
#
#	Copyright Sebastian Schumb (sebastian_at_sebastians-site_de) 2011
#


SRC = main.c include/ringlock.c
TARGET = ringlock-$(AVRMCU)-$(LOCKROLE)
OBJDIR = bin/$(AVRMCU)-$(LOCKROLE)

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

OBJ = $(SRC:%.c=$(OBJDIR)/%.o)

CFLAGS = -Os -Wall -Wstrict-prototypes 
CFLAGS += -fshort-enums -fpack-struct -funsigned-char -funsigned-bitfields
CFLAGS += -mmcu=$(AVRMCU) -DF_CPU=$(F_CPU)UL
ifeq "$(LOCKROLE)" "master"
CFLAGS += -DRL_MASTER
endif

all: start $(OBJDIR)/$(TARGET).hex size
	@echo "Done !"
	
start:
	@echo " _____ _             _                 _"
	@echo "| ___ (_)           | |               | |"   
	@echo "| |_/ /_ _ __   __ _| |      ___   ___| | __"
	@echo "|    /| | '_ \ / _\` | |     / _ \ / __| |/ /"
	@echo "| |\ \| | | | | (_| | |____| (_) | (__|   < "
	@echo "\_| \_|_|_| |_|\__, \_____/ \___/ \___|_|\_\\"
	@echo "                __/ |"
	@echo "               |___/"
	@echo
	@echo "Software based token passing mutex for AVR microcontrollers"
	@echo
	@echo ":: Building for $(AVRMCU)"
	@echo ":: Role is $(LOCKROLE)"
	@echo ":: MCU operating frequency is $(F_CPU)Hz"
	
$(OBJDIR)/%.o : %.c
	@mkdir -p $$(dirname $@)
	$(CC) $(CFLAGS) -c $< -o $@

	
$(OBJDIR)/$(TARGET).elf : $(OBJ)
	$(CC) $(CFLAGS) $+ -o $@
	
$(OBJDIR)/$(TARGET).hex : $(OBJDIR)/$(TARGET).elf
	$(OBJCOPY) -O ihex $< $@


size : $(OBJDIR)/$(TARGET).elf
	@echo
	@$(SIZE) $(OBJDIR)/$(TARGET).hex
	@echo
	
clean :
	@rm -rf $(OBJDIR)
