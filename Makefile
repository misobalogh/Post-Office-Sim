# ***************************************************************
# * File Name: Makefile
# * Description: riesnie IJC-DU2, Makefile
# * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz
# * Faculty: FIT VUT
# * Date: 04.04.2023
#
# * Comments: prekladane pomocou gcc 9.4.0
# ***************************************************************


CFLAGS = -std=gnu99 -pthread -Wall -Wextra -Werror -pedantic
CC = gcc


all: proj2
	./proj2 3 2 100 100 100 
#&& cat proj2.out

proj2: proj2.c proj2.h
	$(CC) $(CFLAGS) -o proj2 proj2.c

val:
	valgrind ./proj2 1 1 1 1 1

test:
	./test.sh

zip:
	zip proj2.zip proj2.c proj2.h Makefile

clean:
	rm proj2

