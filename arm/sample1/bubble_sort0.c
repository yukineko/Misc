#define N 32768

int data[N];

int main(int argc, char* argv[]){
    int i, j, temp;
    for(i = 0; i < N - 1; i++){
        for(j = 0; j < N -1; j++){
            temp = data[j];
            data[j] = data[ j + 1];
            data[j + 1] = temp;
        }
    }
    return 1;
}
