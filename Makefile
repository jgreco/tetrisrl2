CC = gcc
#modify the CFLAGS variable as needed.  specifically you will probably need to modify the include directory arguments
CFLAGS = -O2 -std=c99 -D_GNU_SOURCE -Wall -Wextra -Wno-unused-parameter -pedantic -pipe -g
LIBS = -lncurses -lm -liniparser -lpthread
OBJDIR = .build
OBJECTS = main.o level.o monster.o arraylist.o hashtable.o tetromino.o util.o
OBJECTS :=  $(addprefix ${OBJDIR}/,${OBJECTS})

tetrisrl2: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o tetrisrl2

test: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o test

${OBJDIR}/%.o : %.c
	@if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi #create directory if it doesn't exist
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) tetrisrl2
