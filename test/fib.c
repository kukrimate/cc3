int printf(const char *, ...);

int main(void)
{
	int i = 0, j = 1, tmp;

	for (;;) {
		printf("%d\n", i);
		tmp = i + j;
		i = j;
		j = tmp;
	}
}
