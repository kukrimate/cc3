typedef int t;
typedef int t;	/* Allow re-declaring typedef (extension) */

static int s1;
/* Static can be redeclared as static */
static int s1;
/* Extern can reference static */
extern int s1;
/* int s; Static cannot be redelcared global */

int g1;
/* Global can be redeclared global */
int g1;
/* Extern can reference global */
extern int g1;
/* static int g1; Global cannot be redeclared static */

extern int g2;
/* Extern can be resolved by global */
int g2;

/* Extern with an initialzer becomes global */
extern int g3 = 0;

extern int s2;
/* Extern can be resolved by static (extension) */
static int s2;

extern int e1;
/* Extern can be re-declared extern */
extern int e1;

void f(void) {
	int a;			/* Auto */
	auto int a2;		/* Auto */
	register int a3;	/* Auto */
	static int s;		/* Local */
	extern int e2;		/* Extern */
}

int e2;				/* Resolves import for e2 inside f */
