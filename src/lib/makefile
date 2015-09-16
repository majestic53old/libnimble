# libnimble
# Copyright (C) 2015 David Jolly
# ----------------------
#
# libnimble is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libnimble is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC=clang++
CC_FLAGS=-march=native -pthread -std=gnu++11 -O3 -Wall -Werror -fPIC
DIR_BIN=./../../bin/
DIR_BUILD=./../../build/
DIR_INC=./include/
DIR_SRC=./src/
LIB=libnimble.a

all: build archive

archive:
	@echo ''
	@echo '--- BUILDING LIBRARY -----------------------'
	ar rcs $(DIR_BUILD)$(LIB) $(DIR_BUILD)nimble.o $(DIR_BUILD)nimble_color.o $(DIR_BUILD)nimble_command.o $(DIR_BUILD)nimble_exception.o $(DIR_BUILD)nimble_language.o $(DIR_BUILD)nimble_lexer.o $(DIR_BUILD)nimble_node.o $(DIR_BUILD)nimble_token.o $(DIR_BUILD)nimble_trace.o $(DIR_BUILD)nimble_uid.o
	@echo '--- DONE -----------------------------------'
	@echo ''

build: nimble.o nimble_color.o nimble_command.o nimble_exception.o nimble_language.o nimble_lexer.o nimble_node.o nimble_token.o nimble_trace.o nimble_uid.o

nimble.o: $(DIR_SRC)nimble.cpp $(DIR_INC)nimble.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble.cpp -o $(DIR_BUILD)nimble.o

nimble_color.o: $(DIR_SRC)nimble_color.cpp $(DIR_INC)nimble_color.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_color.cpp -o $(DIR_BUILD)nimble_color.o

nimble_exception.o: $(DIR_SRC)nimble_exception.cpp $(DIR_INC)nimble_exception.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_exception.cpp -o $(DIR_BUILD)nimble_exception.o

nimble_language.o: $(DIR_SRC)nimble_language.cpp $(DIR_INC)nimble_language.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_language.cpp -o $(DIR_BUILD)nimble_language.o

nimble_trace.o: $(DIR_SRC)nimble_trace.cpp $(DIR_INC)nimble_trace.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_trace.cpp -o $(DIR_BUILD)nimble_trace.o

# COMPONENTS

nimble_command.o: $(DIR_SRC)nimble_command.cpp $(DIR_INC)nimble_command.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_command.cpp -o $(DIR_BUILD)nimble_command.o

nimble_node.o: $(DIR_SRC)nimble_node.cpp $(DIR_INC)nimble_node.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_node.cpp -o $(DIR_BUILD)nimble_node.o

nimble_token.o: $(DIR_SRC)nimble_token.cpp $(DIR_INC)nimble_token.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_token.cpp -o $(DIR_BUILD)nimble_token.o

nimble_uid.o: $(DIR_SRC)nimble_uid.cpp $(DIR_INC)nimble_uid.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_uid.cpp -o $(DIR_BUILD)nimble_uid.o

# LANGUAGE

nimble_lexer.o: $(DIR_SRC)nimble_lexer.cpp $(DIR_INC)nimble_lexer.h
	$(CC) $(CC_FLAGS) -c $(DIR_SRC)nimble_lexer.cpp -o $(DIR_BUILD)nimble_lexer.o
