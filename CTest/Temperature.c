#include <stdio.h>

int main()
{   
    int fahrenheit;
    float celsius;
    printf("Enter Fahrenheit temperature: \n"); 
    scanf("%d", &fahrenheit);
    celsius = (fahrenheit - 32) * 5 / 9;
    printf("Celsius eqivalent:  %.2f", celsius);
}