#include <stdio.h>
int int_average(int* array, int size)
{
	int i;
	long long total = 0;

	if (size <= 0) {
		return 0;
	}
	for (i = 0; i < size; i++) {
		total += array[i];
	}
	return total / size;
}

int main()
{
    for(int i = 0; i < 10;i++){
        printf("%d\n", i);
    }
    return 0;
}
