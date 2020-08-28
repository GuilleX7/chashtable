# chashtable

Simple and lightweight implementation of generic hash tables in strict C89 standard.
This implementation doesn't aim to be the best, the fastest or the most lightweight. Just a workaround when a simple generic, dynamic hash table is needed. It uses void pointers to achieve genericity. Memory management calls have been slightly reduced by asking standard library to reserve entire blocks for both struct and dynamic needed memory.

Hash map will automatically double its size and rehash when a pair is added and load factor is greater than 0.75, and will reduce by half when a pair is removed and load factor is less than 0.15. However, those parameters are modifiable.

## Usage
**Usage is straighforward. First, declare a new hash_table struct and initialize it:**

    struct hash_table ht;
    hash_table_init(&ht);

**Putting elements in:**

    char *key = "some-key-here";
    int value = 123;
    hash_table_put(&ht, key, &value);

When putting objects using a pointer that hides size information from sizeof operator, you need to use **hash_table_put_sized**, as the library can't know the size of the entire object.

    char *key = "some-key-here";
    char *val = "sizeof can't guess my size haha!";
    hash_table_put_sized(&ht, key, &value, strlen(val) + 1);

**Getting elements:**

    char *key = "previous-used-key";
    /* No need to cast from void* in C */
    int *retrieved_ptr = hash_table_get(&ht, key);
    if (retrieved_ptr)
	    /* Value is at *retrieved_ptr; */

**Removing elements:**

    char *key = "previous-used-key";
    hash_table_remove(&ht, key);

**Checking for existence:**

    char *key = "previous-used-or-not-key";
    if (hash_table_exists(&ht, key)) {
        /* It already exists */
    } else {
        /* It doesn't exists yet */
    }

**Freeing the hash table** (mandatory, otherwise bad things will happen...):

    hash_table_free(&ht);

**Iterating over the hash_table:**
Just declare a new iterator and use a loop to iterate over:

    struct hash_table_iterator it = hash_table_iterator_create(&ht);
    struct hash_table_entry *entry = { 0 }; /* null pointer */
    while ((entry = hash_table_iterator_next(&it)) {
        /* entry->key is the pointer to the key
           entry->val is the pointer to the object */
    }