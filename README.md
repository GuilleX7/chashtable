# chashtable

Simple and lightweight implementation of generic hash tables following the C89 standard.

The has table will automatically double its size and rehash when an entry is added and the load factor (i.e. the amount of used entries divided by the amount of available entries) is greater than 0.75, and will reduce by half when an entry is removed and the load factor is less than 0.15. This behaviour can be customized by changing `CHT_MAX_LOAD_FACTOR` and `CHT_MIN_LOAD_FACTOR`, respectively.

## Usage
**Usage is straighforward. First, declare a new hash table and initialize it:**

    struct hash_table ht;
    hash_table_init(&ht);

**Inserting an entry:**

    char *key = "some-key-here";
    int value = 123;
    hash_table_put(&ht, key, &value);

If you need to put a more complex value whose size can't be obtained using the sizeof operator, you'll need to use **hash_table_put_sized**:

    char *key = "some-key-here";
    char *value = "sizeof can't guess my size haha!";
    hash_table_put_sized(&ht, key, &value, strlen(value) + 1);

**Retrieving an entry:**

    char *key = "previous-used-key";
    /* No need to cast from void* in C */
    int *retrieved_ptr = hash_table_get(&ht, key);
    if (retrieved_ptr)
	    /* Value is at *retrieved_ptr */

**Removing an entry:**

    char *key = "previous-used-key";
    hash_table_remove(&ht, key);

**Checking if entry exists:**

    char *key = "previous-used-or-not-key";
    if (hash_table_exists(&ht, key)) {
        /* It already exists */
    } else {
        /* It doesn't exists yet */
    }

**Freeing the hash table** (mandatory, otherwise bad things will happen...):

    hash_table_free(&ht);

**Iterating over the hash table:**

Just declare a new iterator and use a loop to iterate over:

    struct hash_table_iterator it = hash_table_iterator_create(&ht);
    struct hash_table_entry *entry = { 0 }; /* null pointer */
    while ((entry = hash_table_iterator_next(&it)) {
        /* entry->key is the pointer to the key
           entry->val is the pointer to the object */
    }
