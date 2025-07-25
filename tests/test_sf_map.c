#include "lassert.h"
#include "log.h"
#include "../lookup.h"
#include "../snowflake.h"

#define KEYS 10

static uint64_t keys[KEYS];

static void make_keys(void)
{
	sf_gen_t g;
	sf_gen_init(&g, 1);
	for (int i = 0; i < KEYS; ++i)
		keys[i] = sf_gen_next(&g, SF_EPOCH_MILLIS + i);
}

int main(void)
{
	static sf_map_t map;
	sf_map_init(&map);
	make_keys();

	for (int i = 0; i < KEYS; ++i) {
		dprintf(2, "[key #%u]: ", i);
		sf_print_snf_parts(keys[i]);
	}

	info("### populate");
	for (int i = 0; i < KEYS; ++i) {
		uint32_t id = sf_map_get_or_create(&map, keys[i]);
		info("insert  k=%lu  id=%u", keys[i], id);
		assert_eq(id, (uint32_t)i);
	}

	info("\n### lookup");
	for (int i = 0; i < KEYS; ++i) {
		uint32_t id = sf_map_get(&map, keys[i]);
		info("lookup  k=%lu  got=%u", keys[i], id);
		assert_eq(id, (uint32_t)i);
	}

	info("\n### overwrite");
	for (int i = 0; i < KEYS; ++i) {
		uint32_t newv = i + 100;
		sf_map_put(&map, keys[i], newv);
		uint32_t id = sf_map_get(&map, keys[i]);
		info("update  k=%lu  now=%u", keys[i], id);
		assert_eq(id, newv);
	}

	info("\n### miss");
	uint64_t miss = keys[0] + 1234567ULL;
	assert_eq(sf_map_get(&map, miss), UINT32_MAX);
	info("all small‑tests passed");
	return 0;
}
