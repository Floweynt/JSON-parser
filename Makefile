CC = g++
.DEFAULT_GOAL := example
object:
	mkdir $@

objects/json_obj_internal.o : json/json_obj/json_obj_internal.cpp
	$(CC) $< -o $@ -c
objects/json_obj.o : json/json_obj/json_obj.cpp
	$(CC) $< -o $@ -c
objects/json.o : json/json.cpp
	$(CC) $< -o $@ -c

objects/CFPjson.a : objects/json.o objects/json_obj.o objects/json_obj_internal.o
	$(setup)
	ar rvs $@ $+

objects/example.o : test.cpp
	$(setup)
	$(CC) $< -o $@ -c

example : objects/example.o objects/CFPjson.a
	$(CC) $+ -o $@
