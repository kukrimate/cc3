/*
 * Test for grammar ambiguities
 */

typedef int T, U;
static int a;

/*
 * I. Context sensitivity of identifiers
 */

void test1_decl(void)
{
	T * a;	/* Declare "a" as a pointer to an "int" */
}

void test1_expr(void)
{
	int T;
	T * a;	/* Expression multiplying "T" with "a" */
}

/*
 * II. Dangling else
 */

void test2_else(void)
{
	if (1)
		if (0)	/* Else should be grouped with this "if" */
			;
		else
			;
}

/*
 * III. Typename grouping with specifier qualifier list versus
 *      first declarator
 */

void test3(void)
{
	T U;	/* Declare "U" as a variable with type int */
}

/*
 * IV. Abstract declarator with typename and re-declaration
 *     ambiguity in a parameter list
 */

void test4(int (T));	/* Function taking a function type as a parameter */
