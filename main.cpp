#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void printIntegrationResult(const char* function) {
	// Simple symbolic integrator for a few common forms
	if (function == NULL) {
		printf("Invalid function\n");
		return;
	}

	// trig: sin(x) -> -cos(x), cos(x) -> sin(x)
	if (strstr(function, "sin") != NULL) {
		printf("The indefinite integral of %s is -cos(x) + C\n", function);
		return;
	}
	if (strstr(function, "cos") != NULL) {
		printf("The indefinite integral of %s is sin(x) + C\n", function);
		return;
	}

	// exponential
	if (strstr(function, "exp") != NULL || strstr(function, "e^") != NULL) {
		printf("The indefinite integral of %s is exp(x) + C\n", function);
		return;
	}

	// reciprocal
	if (strcmp(function, "1/x") == 0 || strcmp(function, "1.0/x") == 0) {
		printf("The indefinite integral of %s is ln|x| + C\n", function);
		return;
	}

	// Try polynomial forms: ax^n, x^n, ax, x, constant
	double a = 0.0;
	int n = 0;

	// pattern: coefficient and power, e.g. 3x^2
	if (sscanf(function, "%lfx^%d", &a, &n) == 2) {
		int newpow = n + 1;
		double newcoeff = a / (double)newpow;
		printf("The indefinite integral of %s is %g x^%d + C\n", function, newcoeff, newpow);
		return;
	}

	// pattern: x^n (coefficient 1)
	if (sscanf(function, "x^%d", &n) == 1) {
		int newpow = n + 1;
		double newcoeff = 1.0 / (double)newpow;
		printf("The indefinite integral of %s is %g x^%d + C\n", function, newcoeff, newpow);
		return;
	}

	// pattern: ax (power 1)
	if (sscanf(function, "%lfx", &a) == 1) {
		double newcoeff = a / 2.0;
		printf("The indefinite integral of %s is %g x^2 + C\n", function, newcoeff);
		return;
	}

	// pattern: x (coefficient 1)
	if (strcmp(function, "x") == 0) {
		printf("The indefinite integral of x is 0.5 x^2 + C\n");
		return;
	}

	// constant
	if (sscanf(function, "%lf", &a) == 1) {
		printf("The indefinite integral of %s is %g x + C\n", function, a);
		return;
	}

	// fallback
	printf("The indefinite integral of %s is F(x) + C\n", function);
}

int main()
{
	char l_input[100];
	printf("Indefinite integration of a function f(x)\n");
	if (scanf("%99s", l_input) != 1) {
		fprintf(stderr, "Invalid input\n");
		return 1;
	}
	printIntegrationResult(l_input);
	return 0;
}