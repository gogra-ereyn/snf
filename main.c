#include <stdio.h>
#include <time.h>
#include "snowflake.h"

static uint64_t now_ms(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int main(void)
{
	sf_gen_t gen;
	sf_parts_t parts;
	uint64_t id;

	sf_gen_init(&gen, 42);
	id = sf_gen_next(&gen, now_ms());

	printf("id: %lu (0x%016lx)\n\n", id, id);

	sf_extract_parts(id, &parts);
	printf("timestamp (ms): %lu\n", parts.ts);
	printf("node_id:        %u\n", parts.node_id);
	printf("seq:            %u\n", parts.seq);

	printf("\nbit layout (64 bits total):\n");
	printf("  [63:22] timestamp offset (42 bits)\n");
	printf("  [21:12] node_id          (10 bits)\n");
	printf("  [11:0]  sequence         (12 bits)\n");

	return 0;
}
