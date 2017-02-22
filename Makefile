all: generate_table

generate_table: generate_table.c
	gcc -O3 -m32 -o generate_table generate_table.c

clean: 
	rm generate_table
