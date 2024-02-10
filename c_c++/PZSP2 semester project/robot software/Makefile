APPLICATION = mapbot
BOARD ?= nucleo-l476rg

RIOTBASE ?= $(CURDIR)/RIOT

FEATURES_REQUIRED += periph_timer
FEATURES_REQUIRED += cpp
FEATURES_REQUIRED += libstdcpp
FEATURES_REQUIRED += periph_i2c

USEMODULE += periph_gpio
USEMODULE += periph_gpio_irq
USEMODULE += periph_uart
USEMODULE += periph_pwm
USEMODULE += xtimer
USEMODULE += shell
USEMODULE += cpp11-compat

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1


include $(RIOTBASE)/Makefile.include
