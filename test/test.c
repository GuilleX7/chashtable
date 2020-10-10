#include <string.h>
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */

#include <limits.h>
#include <float.h>
#include <stdio.h>
#include <assert.h>
#include "../src/hash_table.h"

#define EXPECTED_INT INT_MIN
#define EXPECTED_UINT UINT_MAX
#define EXPECTED_FLOAT FLT_MAX
#define EXPECTED_DOUBLE DBL_MIN
#define EXPECTED_STRING "aaaaa"
#define TOTAL_PAIRS 7

int main(void) {
	struct hash_table ht;
	struct hash_table_iterator it;
	struct hash_table_entry *entry = { 0 };
	size_t i;
	char key[256];
	int a = EXPECTED_INT;
	unsigned int b = EXPECTED_UINT;
	float c = EXPECTED_FLOAT;
	double d = EXPECTED_DOUBLE;
	struct st_t {
		char chars[6];
	} st_e = { EXPECTED_STRING };
	unsigned int arr[5] = { 1, 2, 3, 4, 5 };
	unsigned int *arr_ptr = arr;

	assert(hash_table_init(&ht));

	/* hash_table_iterator (1) */
	it = hash_table_iterator_create(&ht);
	for (i = 0; i < 2; i++)
		assert(!hash_table_iterator_next(&it));
	
	/* hash_table_put (1) */
	strcpy_s(key, sizeof(key), "a");
	assert(hash_table_put(&ht, key, &a));
	strcpy_s(key, sizeof(key), "b");
	assert(hash_table_put(&ht, key, &b));
	strcpy_s(key, sizeof(key), "c");
	assert(hash_table_put(&ht, key, &c));
	strcpy_s(key, sizeof(key), "d");
	assert(hash_table_put(&ht, key, &d));
	strcpy_s(key, sizeof(key), "st_e");
	assert(hash_table_put(&ht, key, &st_e));
	strcpy_s(key, sizeof(key), "arr");
	assert(hash_table_put(&ht, key, &arr));
	strcpy_s(key, sizeof(key), "arr_ptr");
	assert(hash_table_put_sized(&ht, key, arr_ptr, sizeof(arr)));

	/* size check */
	assert(ht.entry_count == TOTAL_PAIRS);

	/* hash_table_exists */
	strcpy_s(key, sizeof(key), "a");
	assert(hash_table_exists(&ht, key));
	strcpy_s(key, sizeof(key), "b");
	assert(hash_table_exists(&ht, key));
	strcpy_s(key, sizeof(key), "c");
	assert(hash_table_exists(&ht, key));
	strcpy_s(key, sizeof(key), "d");
	assert(hash_table_exists(&ht, key));
	strcpy_s(key, sizeof(key), "st_e");
	assert(hash_table_exists(&ht, key));
	strcpy_s(key, sizeof(key), "arr");
	assert(hash_table_exists(&ht, key));
	strcpy_s(key, sizeof(key), "arr_ptr");
	assert(hash_table_exists(&ht, key));
	strcpy_s(key, sizeof(key), "dadsads");
	assert(!hash_table_exists(&ht, key));

	/* hash_table_get (1) */
	strcpy_s(key, sizeof(key), "a");
	assert(*((int *) hash_table_get(&ht, key)) == EXPECTED_INT);
	strcpy_s(key, sizeof(key), "b");
	assert(*((unsigned int *) hash_table_get(&ht, key)) == EXPECTED_UINT);
	strcpy_s(key, sizeof(key), "c");
	assert(*((float *) hash_table_get(&ht, key)) == EXPECTED_FLOAT);
	strcpy_s(key, sizeof(key), "d");
	assert(*((double *) hash_table_get(&ht, key)) == EXPECTED_DOUBLE);
	strcpy_s(key, sizeof(key), "st_e");
	assert(!strcmp(((struct st_t *) hash_table_get(&ht, key))->chars, EXPECTED_STRING));
	strcpy_s(key, sizeof(key), "arr");
	arr_ptr = ((unsigned int *) hash_table_get(&ht, key));
	for (i = 1; i < 6; i++) {
		assert(*arr_ptr == i);
		arr_ptr++;
	}
	strcpy_s(key, sizeof(key), "arr_ptr");
	arr_ptr = ((unsigned int *) hash_table_get(&ht, key));
	for (i = 1; i < 6; i++) {
		assert(*arr_ptr == i);
		arr_ptr++;
	}

	/* hash_table_put (2) */
	strcpy_s(key, sizeof(key), "a");
	a = 1;
	assert(hash_table_put(&ht, key, &a));
	
	/* hash_table_get (2) */
	assert(*((int *) hash_table_get(&ht, key)) == 1);

	/* hash_table_remove */
	assert(hash_table_remove(&ht, key));
	strcpy_s(key, sizeof(key), "dfdsf");
	assert(!hash_table_remove(&ht, key));

	/* size check */
	assert(ht.entry_count == TOTAL_PAIRS - 1);

	/* hash_table_iterator (2) */
	it = hash_table_iterator_create(&ht);
	i = 0;
	while ((entry = hash_table_iterator_next(&it))) {
		assert(entry->key);
		assert(entry->value);
		i++;
	}
	assert(i == TOTAL_PAIRS - 1);
	assert(!hash_table_iterator_next(&it));

	hash_table_free(&ht);

	/* saturation: rehashing */
	assert(hash_table_init(&ht));
	assert(ht.size_index == 0);
	for (i = 0; i < 40; i++) {
		sprintf(key, "%lu", (unsigned long) i);
		assert(hash_table_put(&ht, key, &i));
	}
	assert(ht.size_index > 0);
	/* ... */

	hash_table_free(&ht);

	printf("Test passed successfully with no errors!\n");
	return 0;
}
