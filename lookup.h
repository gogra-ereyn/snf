#ifndef LOOKUP_H
#define LOOKUP_H

#include <stdint.h>
#include <sys/mman.h>

//
#define GR 0x9e3779b97f4a7c15ULL
// as papa knuth commands
#define FB 11400714819323198485ULL

#ifndef MAP_BITS
#define MAP_BITS 21
#endif

#define MCAP (1u << MAP_BITS)
#define MMASK (MCAP - 1)

typedef struct {
	uint64_t k;
	uint32_t v;
} map_slot_t;

typedef struct {
	map_slot_t slot[MCAP];
	uint32_t next_id;
} sf_map_t;







typedef struct {
	uint64_t id;
	uint32_t idx; // probably jsut 64 it ?
} lookup_entry_t;

typedef struct {
	lookup_entry_t *entries;
	uint32_t capacity;
	uint32_t count;
} lookup_t;

void lookup_init(lookup_t *map, uint32_t cap)
{
	map->capacity = cap;
	map->count = 0;
	map->entries = (lookup_entry_t *)mmap(0, cap * sizeof(*map->entries),
					      PROT_READ | PROT_WRITE,
					      MAP_PRIVATE | MAP_ANONYMOUS, -1,
					      0);

	for (uint32_t i = 0; i < cap; ++i)
		map->entries[i].id = 0;
}

void lookup_put(lookup_t *map, uint64_t snf, uint32_t st_idx)
{
	uint32_t hash = (uint32_t)(snf * GR);
	uint32_t idx = hash & (map->capacity - 1);

	while (map->entries[idx].id != 0 && map->entries[idx].id != snf) {
		idx = (idx + 1) & (map->capacity - 1);
	}

	if (map->entries[idx].id == 0)
		map->count++;

	map->entries[idx].id = snf;
	map->entries[idx].idx = st_idx;
}

uint32_t id_map_get(const lookup_t *map, uint64_t snf)
{
	uint32_t hash = (uint32_t)(snf * GR);
	uint32_t idx = hash & (map->capacity - 1);
	while (map->entries[idx].id != 0) {
		if (map->entries[idx].id == snf) {
			return map->entries[idx].idx;
		}
		idx = (idx + 1) & (map->capacity - 1);
	}
	return 0;
}

#endif
