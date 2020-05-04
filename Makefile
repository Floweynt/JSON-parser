CC = g++
.DEFAULT_GOAL := test
objects:
	mkdir $@

objects/json_obj_internal.o : json/json_obj/json_obj_internal.cpp | objects json/json_obj/json_obj_internal.h
	$(CC) $< -o $@ -c
objects/json_obj.o : json/json_obj/json_obj.cpp | objects json/json_obj/json_obj.h
	$(CC) $< -o $@ -c
objects/json.o : json/json.cpp | objects json/json.h
	$(CC) $< -o $@ -c
objects/dumps.o : json/parser/dumps.cpp | json/parser/parser.h
	$(CC) $< -o $@ -c
objects/loads.o : json/parser/loads.cpp | json/parser/parser.h
	$(CC) $< -o $@ -c

objects/json.a : objects/json.o objects/json_obj.o objects/json_obj_internal.o objects/dumps.o objects/loads.o| objects
	ar rvs $@ $+

objects/example.o : test.cpp | objects
	$(CC) $< -o $@ -c

test : objects/example.o objects/json.a | objects
	$(CC) $+ -o $@
lib : objects/json.a
	@echo "creating statically-linked libary objects/json.a"
