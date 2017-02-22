#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

#include "md5.h" 

// input : ./prog filename

#define CHAIN_LENGTH 10000
#define LIST_SIZE 1000000

int chain_contents[CHAIN_LENGTH] = {0};
int initial_value[LIST_SIZE] = {0};
int last_value[LIST_SIZE] = {0};


const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnopqrstuvwxyz";

const char delimiter[] = {' '};
const char newline[] = {'\n'};

char pass[4]; // 4 character password
MD5_CTX mdContext;	// needed to compute MD5

void generate_hash_chain(int *pass){
	
	for(int i = 0; i < CHAIN_LENGTH; i++){
		MD5Init(&mdContext);  // compute MD5 of password
		MD5Update(&mdContext, pass, 4);
		MD5Final(&mdContext);
		pass = (int *) &mdContext.digest[12]; 
		chain_contents[i] = *pass;
	}
}

void add_chain(int *list_index){

	for(int i = 0; i < *list_index; i++){
		for(int j = 0; j < CHAIN_LENGTH; j++){
			if(chain_contents[j] == last_value[i]){
				return;
			}
		}
	}
	initial_value[*list_index] = chain_contents[0];
	last_value[*list_index] = chain_contents[CHAIN_LENGTH-1];
	*list_index +=1;
}

void write_chain(int file, int list_index){

	for(int i = 0 ; i < list_index; i++){
		write(file, &(initial_value[i]), sizeof(int));
		write(file, delimiter, sizeof(char));
		write(file, &(last_value[i]), sizeof(int));
		write(file, newline, sizeof(char));
	}


}

int main(int argc, char *argv[])
{
	int i = 0, j = 0, k = 0, l = 0;

	int file = open("rainbow_table", O_CREAT|O_TRUNC|O_RDWR);
	int list_index = 0;

	for( ; l < sizeof(alphabet); i++){
		sprintf(pass,"%c%c%c%c", alphabet[i], alphabet[j], alphabet[k], alphabet[l]); 
		generate_hash_chain((int*)pass);
		add_chain(&list_index);
		j += i < sizeof(alphabet) ? 0 : !(i=0);
		k += j < sizeof(alphabet) ? 0 : !(j=0);
		l += k < sizeof(alphabet) ? 0 : !(k=0);
		if(i == 0){
			printf("Progress  \t%i : %i : %i \t|| %i chains \t|| %f\%\n", 
					j, k, l, list_index,
					(float)(l * sizeof(alphabet) * sizeof(alphabet) * sizeof(alphabet) 
					 + k * sizeof(alphabet) * sizeof(alphabet) 
					 + j * sizeof(alphabet) + i) 
					/ (float)(sizeof(alphabet) * sizeof(alphabet) *sizeof(alphabet) *sizeof(alphabet))*100.);
		}
	}
	write_chain (file, list_index);
	close (file);

	return 0;
};

