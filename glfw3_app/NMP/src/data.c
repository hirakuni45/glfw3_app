#include <stdio.h>
#include "saturn.h"
#include "cdfs.h"

const char testData[] = {
#include "test.h"
};

void SetupTestData(void);

void SetupTestData(void)
{
	int	i;
	char	*p;

	p = (char *)get_SATURN_DRAM_TOP();
	for(i=0; i<(sizeof(testData)); ++i) *p++ = testData[i];
	InitResource();
}
