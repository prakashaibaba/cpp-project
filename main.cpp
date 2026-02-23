#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

static void normalize(const char *in, char *out, size_t out_sz) {
	size_t i = 0;
	while (*in && i + 1 < out_sz) {
		if (!isspace((unsigned char)*in)) {
			out[i++] = (char)tolower((unsigned char)*in);
		}
		in++;
	}
	out[i] = '\0';
}

// Parse an argument like "10x", "(10x)", "-x", "x", "10".
// Returns: 1 if argument contains x (sets *k to coefficient),
// 2 if argument is a numeric constant (sets *c),
// 0 on parse failure.
static int parse_arg(const char *arg, double *k, double *c) {
	// strip surrounding parentheses
	const char *s = arg;
	size_t len = strlen(s);
	if (len >= 2 && s[0] == '(' && s[len - 1] == ')') {
		// create a temporary substring
		char tmp[128];
		size_t copy_len = (len - 2 < sizeof(tmp) - 1) ? len - 2 : sizeof(tmp) - 1;
		memcpy(tmp, s + 1, copy_len);
		tmp[copy_len] = '\0';
		s = tmp;
	}

	// check if there's an 'x' anywhere
	const char *xpos = strchr(s, 'x');
	if (xpos != NULL) {
		// coefficient is substring before 'x'
		if (xpos == s) {
			*k = 1.0;
			return 1;
		}
		// substring length
		size_t clen = xpos - s;
		char coeff[64];
		if (clen >= sizeof(coeff)) return 0;
		memcpy(coeff, s, clen);
		coeff[clen] = '\0';
		// allow an optional '*' at end of coeff
		// remove trailing '*'
		if (coeff[clen - 1] == '*') coeff[clen - 1] = '\0';
		// handle signs like '+' or '-' only
		if (strcmp(coeff, "") == 0 || strcmp(coeff, "+") == 0) {
			*k = 1.0;
			return 1;
		}
		if (strcmp(coeff, "-") == 0) {
			*k = -1.0;
			return 1;
		}
		char *endp;
		double val = strtod(coeff, &endp);
		if (endp == coeff) return 0;
		*k = val;
		return 1;
	}

	// no x: try parse as number constant
	char *endp;
	double val = strtod(s, &endp);
	if (endp != s) {
		*c = val;
		return 2;
	}
	return 0;
}

void printIntegrationResult(const char* function) {
	// Simple symbolic integrator for common forms, with more robust parsing
	if (function == NULL) {
		printf("Invalid function\n");
		return;
	}

	char norm[256];
	normalize(function, norm, sizeof(norm));

	// trig: sin(...) and cos(...)
	if (strncmp(norm, "sin", 3) == 0) {
		const char *arg = norm + 3;
		double k = 0.0, c = 0.0;
		int t = parse_arg(arg, &k, &c);
		if (t == 1) {
			if (k == 0.0) { printf("The indefinite integral of %s is 0 + C\n", function); return; }
			double factor = -1.0 / k; // integral = factor * cos(kx)
			char kx[64];
			if (fabs(k - 1.0) < 1e-12) snprintf(kx, sizeof(kx), "x");
			else if (fabs(k + 1.0) < 1e-12) snprintf(kx, sizeof(kx), "-x");
			else snprintf(kx, sizeof(kx), "%gx", k);
			if (fabs(factor - 1.0) < 1e-12) {
				printf("The indefinite integral of %s is cos(%s) + C\n", function, kx);
			} else if (fabs(factor + 1.0) < 1e-12) {
				printf("The indefinite integral of %s is -cos(%s) + C\n", function, kx);
			} else {
				printf("The indefinite integral of %s is %g cos(%s) + C\n", function, factor, kx);
			}
			return;
		} else if (t == 2) {
			// sin(constant) -> constant * x
			printf("The indefinite integral of %s is %g x + C\n", function, sin(c));
			return;
		}
		// fallback for weird forms
	}

	if (strncmp(norm, "cos", 3) == 0) {
		const char *arg = norm + 3;
		double k = 0.0, c = 0.0;
		int t = parse_arg(arg, &k, &c);
		if (t == 1) {
			if (k == 0.0) { printf("The indefinite integral of %s is x + C\n", function); return; }
			double factor = 1.0 / k; // integral = factor * sin(kx)
			char kx[64];
			if (fabs(k - 1.0) < 1e-12) snprintf(kx, sizeof(kx), "x");
			else if (fabs(k + 1.0) < 1e-12) snprintf(kx, sizeof(kx), "-x");
			else snprintf(kx, sizeof(kx), "%gx", k);
			if (fabs(factor - 1.0) < 1e-12) printf("The indefinite integral of %s is sin(%s) + C\n", function, kx);
			else printf("The indefinite integral of %s is %g sin(%s) + C\n", function, factor, kx);
			return;
		} else if (t == 2) {
			printf("The indefinite integral of %s is %g x + C\n", function, cos(c));
			return;
		}
	}

	// exponential: exp(kx), e^(kx) or exp(const)
	if (strncmp(norm, "exp", 3) == 0 || strncmp(norm, "e^", 2) == 0) {
		const char *arg = NULL;
		if (strncmp(norm, "exp", 3) == 0) arg = norm + 3;
		else arg = norm + 2;
		double k = 0.0, c = 0.0;
		int t = parse_arg(arg, &k, &c);
		if (t == 1) {
			if (k == 0.0) { printf("The indefinite integral of %s is exp(0) x + C\n", function); return; }
			double inv = 1.0 / k;
			if (inv == 1.0) printf("The indefinite integral of %s is exp(x) + C\n", function);
			else printf("The indefinite integral of %s is %g exp(%gx) + C\n", function, inv, k);
			return;
		} else if (t == 2) {
			printf("The indefinite integral of %s is %g x + C\n", function, exp(c));
			return;
		}
	}

	// reciprocal forms like 1/x with optional whitespace
	if (strcmp(norm, "1/x") == 0 || strcmp(norm, "1.0/x") == 0) {
		printf("The indefinite integral of %s is ln|x| + C\n", function);
		return;
	}

	// Try polynomial forms: ax^n, x^n, ax, x, constant
	double a = 0.0;
	int n = 0;

	// pattern: coefficient and power, e.g. 3x^2 or -2.5x^3
	if (sscanf(norm, "%lfx^%d", &a, &n) == 2) {
		if (n == -1) { printf("The indefinite integral of %s is ln|x| + C\n", function); return; }
		int newpow = n + 1;
		double newcoeff = a / (double)newpow;
		printf("The indefinite integral of %s is %g x^%d + C\n", function, newcoeff, newpow);
		return;
	}

	// pattern: x^n (coefficient 1)
	if (sscanf(norm, "x^%d", &n) == 1) {
		if (n == -1) { printf("The indefinite integral of %s is ln|x| + C\n", function); return; }
		int newpow = n + 1;
		double newcoeff = 1.0 / (double)newpow;
		printf("The indefinite integral of %s is %g x^%d + C\n", function, newcoeff, newpow);
		return;
	}

	// pattern: ax (power 1)
	if (sscanf(norm, "%lfx", &a) == 1) {
		double newcoeff = a / 2.0;
		printf("The indefinite integral of %s is %g x^2 + C\n", function, newcoeff);
		return;
	}

	// pattern: x (coefficient 1)
	if (strcmp(norm, "x") == 0) {
		printf("The indefinite integral of x is 0.5 x^2 + C\n");
		return;
	}

	// constant
	if (sscanf(norm, "%lf", &a) == 1) {
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