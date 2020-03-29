CC = g++
.DEFAULT_GOAL := example
objects:
	mkdir $@

objects/json_obj_internal.o : json/json_obj/json_obj_internal.cpp | objects json/json_obj/json_obj_internal.h
	$(CC) $< -o $@ -c
objects/json_obj.o : json/json_obj/json_obj.cpp | objects json/json_obj/json_obj.h
	$(CC) $< -o $@ -c
objects/json.o : json/json.cpp | objects json/json.h
	$(CC) $< -o $@ -c

objects/CFPjson.a : objects/json.o objects/json_obj.o objects/json_obj_internal.o | objects
	$(setup)
	ar rvs $@ $+

objects/example.o : test.cpp | objects
	$(setup)
	$(CC) $< -o $@ -c

example : objects/example.o objects/CFPjson.a | objects
	$(CC) $+ -o $@
