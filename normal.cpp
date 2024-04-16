#include <stdio.h>
#include <time.h>

double targetFunction(double x) {
    return x * x;
}

int main() {
    double l, r, h, result = 0.0;
    int n;

    printf("Input integration segment\n");
    printf("l:");
    scanf("%lf", &l);
    printf("r:");
    scanf("%lf", &r);
    printf("Chosen segment: [%f, %f]\n", l, r);

    do {
        printf("Input number of split segments:");
        scanf("%d", &n);
        printf("Chosen number of segments: %d\n", n);
        if (n <= 0) {
            printf("n must be greater than 0!\n");
        }
    } while (n <= 0);

    h = (r - l) / n;

    clock_t start = clock(); // Start the timer

    double prevX = l;
    for (int i = 1; i <= n; i++) {
        double nextX = prevX + h;
        result += (targetFunction(prevX) + 4 * targetFunction(0.5 * (prevX + nextX)) +
                   targetFunction(nextX)) * (nextX - prevX) / 6.0;
        prevX = nextX;
    }

    clock_t end = clock(); // Stop the timer
    double time_taken = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Result = %f\n", result);
    printf("Time taken: %f seconds\n", time_taken);

    return 0;
}
