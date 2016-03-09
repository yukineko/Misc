#include <dlfcn.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    // dlopenを確認したい
    // dlopen("任意のlib****.so", RTLD_LAZY | ...);
    
    char* error;
    if(argc < 2){
	err(EXIT_FAILURE, "usage dlopen_test : filename");
	return 1;
    }
    dlopen(argv[1], RTLD_LAZY | RTLD_GLOBAL);
    error = dlerror();
    if(error != NULL){
	err(EXIT_FAILURE, "dlopen error %s", error);
	return 1;
    }
    return 0;
}
