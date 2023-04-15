# ***************************************************************
# * File Name: Makefile
# * Description: riesnie IJC-DU2, Makefile
# * Author: MICHAL BALOGH <xbalog06@stud.fit.vutbr.cz
# * Faculty: FIT VUT
# * Date: 04.04.2023
#
# * Comments: prekladane pomocou gcc 9.4.0
# ***************************************************************


CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
CC = gcc


proj2: proj2.c proj2.h
	$(CC) $(CFLAGS) -o proj2 proj2.c

zip:
	zip proj2.zip proj2.c proj2.h Makefile

clean:
	rm proj2
