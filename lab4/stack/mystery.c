int blue(int *ptr) {
    return *ptr;
}

void red(int *ptr, int n) {
    for (int i = 0; i < n; i++)
        ptr[i] = i;
}

int yellow(void) {
    int array[4];

    red(array, 4);
    return blue(array);
}

int green(int a) {
    return a * a;
}

int main(void) {
    int x = green(17);
    int y = yellow();
    return x - y;
}
