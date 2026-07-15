# https://www.gnu.org/software/make/manual/make.html

IDIR = include
SDIR = src
ODIR = obj
DEPS = $(wildcard $(IDIR)/*.h)
OBJS = $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(wildcard $(SDIR)/*.c))

APPBIN = proc_comm_app

CC = gcc
ifeq ($(DEPS),)
CFLAGS = -Wall -Wextra -g -pthread
else
CFLAGS = -I$(IDIR) -Wall -Wextra -g -pthread
endif
LIBS = -lm

all: $(APPBIN) submission

$(ODIR):
	mkdir obj

$(ODIR)/%.o: $(SDIR)/%.c $(ODIR) $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(APPBIN): $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

submission:
ifeq ($(DEPS),)
	zip -r submission src
else
	zip -r submission src include
endif
	@if [ -f README.txt ]; then zip submission README.txt; fi

.PHONY : clean
clean:
	rm -f $(APPBIN)
	rm -f *.zip
	rm -fr $(ODIR)
