/** @file hash_table.c */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif /* _MSC_VER */

#include <string.h>
#include <stdlib.h>

#include "hash_table.h"

#define COUNT_OF(x) (sizeof x / sizeof x[0])

/**
		@brief Creates a new entry with a given key and value
		@param key Pointer to the string that will be used as the entry key (will be copied)
		@param value Pointer to the value to be saved (will be copied)
		@param value_size Size of the value to be saved
		@return Pointer to the allocated entry if it could be allocated,
						null pointer otherwise
*/
static struct hash_table_entry *hash_table_create_entry(char *key, void *value, size_t value_size);

/**
		@brief Frees a given entry
		@param entry Pointer to the entry to be freed
*/
static void hash_table_free_entry(struct hash_table_entry *entry);

/**
		@brief Frees all entries linked with a given entry (inclusive)
		@param entry Pointer to the starting entry to be freed
*/
static void hash_table_free_entries(struct hash_table_entry *entry);

/**
		@brief Calculates the hash for a given key
		@param hash Pointer to the hash table where the key will be used
		@param key Pointer to the string to be hashed
		@return The computed hash
*/
static size_t hash_table_hash(size_t size, char *key);

/**
		@brief Resize and rehash the given hash table
		@param hash_table Pointer to the hash table to be rehashed
		@param size_index Index of the new size to be used
*/
static void hash_table_rehash(struct hash_table *hash_table, size_t size_index);

/** @brief Possible sizes for the hash table */
static size_t const hash_sizes[] = {
	53, 101, 211, 503, 1553, 3407,
	6803, 12503, 25013, 50261, 104729, 250007,
	500009, 1000003, 2000029, 4000037, 10000019, 25000009,
	50000047, 104395301, 217645177, 512927357, 1000000007
};

int hash_table_init(struct hash_table *hash_table) {
	if (hash_table) {
		hash_table->size_index = 0;
		hash_table->entry_count = 0;
		hash_table->entries = calloc(hash_sizes[hash_table->size_index], sizeof(struct hash_table_entry*));
		if (!hash_table->entries) {
			return 0;
		}
	}
	return -1;
}

void hash_table_free(struct hash_table *hash_table) {
	size_t size = 0;
	size_t i = 0;
	struct hash_table_entry *entry = { 0 };

	if (!hash_table) {
		return;
	}

	size = hash_sizes[hash_table->size_index];
	for (i = 0; i < size; i++) {
		if ((entry = hash_table->entries[i])) {
			hash_table_free_entries(entry);
		}
	}

	free(hash_table->entries);
}

int hash_table_put_sized(struct hash_table *hash_table, char *key, void *value, size_t value_size) {
	size_t hash = 0;
	struct hash_table_entry *previous_entry = { 0 };
	struct hash_table_entry *entry = { 0 };
	struct hash_table_entry *new_entry = { 0 };
	
	if (!hash_table || !key || !value) {
		return 0;
	}

	hash = hash_table_hash(hash_table->size_index, key);
	entry = hash_table->entries[hash];
	new_entry = hash_table_create_entry(key, value, value_size);

	if (!new_entry) {
		return 0;
	}

	while (entry) {
		if (!strcmp(entry->key, key)) {
			new_entry->next = entry->next;
			if (previous_entry) {
				previous_entry->next = new_entry; /* Middle entry */
			} else {
				hash_table->entries[hash] = new_entry; /* Starting entry */
			}
			free(entry);
			return -1;
		}
		previous_entry = entry;
		entry = entry->next;
	}

	hash_table->entry_count++;
	new_entry->next = hash_table->entries[hash];
	hash_table->entries[hash] = new_entry;

	if (hash_table->entry_count > hash_sizes[hash_table->size_index] * CHT_MAX_LOAD_FACTOR &&
		hash_table->size_index < COUNT_OF(hash_sizes) - 1) {
		hash_table_rehash(hash_table, hash_table->size_index + 1);
	}

	return -1;
}

void *hash_table_get(struct hash_table *hash_table, char *key) {
	size_t hash = 0;
	struct hash_table_entry *entry = { 0 };

	if (!hash_table || !key) {
		return 0;
	}

	hash = hash_table_hash(hash_table->size_index, key);
	entry = hash_table->entries[hash];

	while (entry && strcmp(key, entry->key)) {
		entry = entry->next;
	}

	return entry ? entry->value : 0;
}

int hash_table_remove(struct hash_table *hash_table, char *key) {
	size_t hash = 0;
	struct hash_table_entry *previous_entry = { 0 };
	struct hash_table_entry *entry = { 0 };

	if (!hash_table || !key) {
		return 0;
	}

	hash = hash_table_hash(hash_table->size_index, key);
	entry = hash_table->entries[hash];

	while (entry) {
		if (!strcmp(entry->key, key)) {
			if (previous_entry) {
				previous_entry->next = entry->next; /* Middle entry */
			} else {
				hash_table->entries[hash] = entry->next; /* Starting entry */
			}
			break;
		}
		previous_entry = entry;
		entry = entry->next;
	}

	if (!entry) {
		return 0;
	}

	free(entry);
	hash_table->entry_count--;

	if (hash_table->entry_count < hash_sizes[hash_table->size_index] * CHT_MIN_LOAD_FACTOR &&
		hash_table->size_index > 0) {
		hash_table_rehash(hash_table, hash_table->size_index - 1);
	}

	return -1;
}

int hash_table_exists(struct hash_table *hash_table, char *key) {
	size_t hash = 0;
	struct hash_table_entry *entry = { 0 };

	if (!hash_table || !key) {
		return 0;
	}

	hash = hash_table_hash(hash_table->size_index, key);
	entry = hash_table->entries[hash];

	while (entry && strcmp(entry->key, key)) {
		entry = entry->next;
	}

	return entry ? -1 : 0;
}

struct hash_table_iterator hash_table_iterator_create(struct hash_table *hash_table) {
	struct hash_table_iterator it = { 0, NULL, 0 };
	it.hash_table = hash_table;
	return it;
}

struct hash_table_entry *hash_table_iterator_next(struct hash_table_iterator *iterator) {
	if (!iterator || !iterator->hash_table || !iterator->hash_table->entry_count) {
		return 0;
	}
	
	if (iterator->entry) {
		iterator->entry = iterator->entry->next;
	}

	while (!iterator->entry && iterator->next_index < hash_sizes[iterator->hash_table->size_index]) {
		iterator->entry = iterator->hash_table->entries[iterator->next_index];
		iterator->next_index++;
	}

	return iterator->entry;
}

struct hash_table_entry *hash_table_create_entry(char *key, void *value, size_t value_size) {
	struct hash_table_entry *entry = { 0 };
	size_t key_length = 0;

	if (!key || !value) {
		return 0;
	}

	key_length = strlen(key) + 1;
	entry = malloc(sizeof(struct hash_table_entry) + key_length + value_size);
	if (entry) {
		entry->key = (char *) entry + sizeof(struct hash_table_entry);
		strcpy(entry->key, key);
		entry->value = (char *) entry->key + key_length;
		memcpy(entry->value, value, value_size);
	}

	return entry;
}

static void hash_table_free_entry(struct hash_table_entry *entry) {
	if (entry) {
		free(entry);
	}
}

static void hash_table_free_entries(struct hash_table_entry *entry) {
	struct hash_table_entry *next = { 0 };
	while (entry) {
		next = entry->next;
		hash_table_free_entry(entry);
		entry = next;
	}
}

static size_t hash_table_hash(size_t size_index, char *key) {
	size_t size = hash_sizes[size_index];
	size_t hash = 0;
	char ch = 0;

	if (key) {
		while ((ch = *key++)) {
			hash = 31 * hash + ch;
		}
	}

	return hash % size;
}

static void hash_table_rehash(struct hash_table *hash_table, size_t size_index) {
	size_t old_size = 0;
	size_t new_hash = 0;
	size_t i = 0;
	struct hash_table_entry **new_entries = { 0 };
	struct hash_table_entry *entry = { 0 };
	struct hash_table_entry *next_entry = { 0 };

	if (!hash_table) {
		return;
	}

	new_entries = calloc(hash_sizes[size_index], sizeof(struct hash_table_entry *));
	if (!new_entries) {
		return;
	}

	old_size = hash_sizes[hash_table->size_index];
	for (i = 0; i < old_size; i++) {
		entry = hash_table->entries[i];
		while (entry) {
			new_hash = hash_table_hash(size_index, entry->key);
			next_entry = entry->next;
			entry->next = new_entries[new_hash];
			new_entries[new_hash] = entry;
			entry = next_entry;
		}
	}

	free(hash_table->entries);
	hash_table->entries = new_entries;
	hash_table->size_index = size_index;
}
