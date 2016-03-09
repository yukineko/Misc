#include <dlfcn.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    char* error;
    if(argc < 2){
	err(EXIT_FAILURE, "usage dlopen_test : filename");
	return 1;
    }
    dlopen(argv[1], RTLD_NOW);
    error = dlerror();
    if(error != NULL){
	err(EXIT_FAILURE, "dlopen error %s", error);
	return 1;
    }
    return 0;
}
