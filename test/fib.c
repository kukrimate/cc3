int printf(const char *, ...);

int main(void)
{
	int i = 0, j = 1;

	for (int k = 0; k < 10; ++k) {
		printf("%d\n", i);
		int tmp = i + j;
		i = j;
		j = tmp;
	}
}
