#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include "snowflake.h"

static uint64_t now_ms(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void usage(const char *prog, int rc)
{
	fprintf(stderr, "usage: %s [-t timestamp_ms] [-n node_id] [-s seq]\n",
		prog);
	fprintf(stderr, "  -t, --timestamp  timestamp in ms (default: now)\n");
	fprintf(stderr, "  -n, --node       node id 0-%u (default: 42)\n",
		SF_NODE_MAX);
	fprintf(stderr, "  -s, --seq        sequence 0-%u (default: 0)\n",
		SF_SEQ_MAX);
	exit(rc);
}

static void print_id(uint64_t id)
{
	sf_parts_t parts;

	sf_extract_parts(id, &parts);
	printf("id: %lu (0x%016lx)\n\n", id, id);
	printf("timestamp (ms): %lu\n", parts.ts);
	printf("node_id:        %u\n", parts.node_id);
	printf("seq:            %u\n", parts.seq);
}

// just for rushin' innit
static uint64_t build_id(uint64_t ts, uint16_t node_id, uint16_t seq)
{
	return ((ts - SF_EPOCH_MILLIS) << (SF_NODE_BITS + SF_SEQ_BITS)) |
	       ((uint64_t)(node_id & SF_NODE_MAX) << SF_SEQ_BITS) |
	       (uint64_t)(seq & SF_SEQ_MAX);
}

int main(int argc, char **argv)
{
	static struct option long_options[] = {
		{ "help", no_argument, 0, 'h' },
		{ "timestamp", required_argument, 0, 't' },
		{ "node", required_argument, 0, 'n' },
		{ "seq", required_argument, 0, 's' },
		{ "decode", required_argument, 0, 'd' },

		{ 0, 0, 0, 0 }
	};
	int c, option_index = 0;
	uint64_t ts = 0;
	uint16_t node_id = 42;
	uint16_t seq = 0;
	int ts_set = 0;
	uint64_t id;
	sf_parts_t parts;
	uint64_t decode_id = 0;
	int decode_mode = 0;

	while ((c = getopt_long(argc, argv, "ht:n:s:", long_options,
				&option_index)) != -1) {
		switch (c) {
		case 'h':
			usage(argv[0], 0);
		case 't':
			ts = strtoull(optarg, NULL, 10);
			ts_set = 1;
			break;
		case 'n':
			node_id = strtoul(optarg, NULL, 10);
			break;
		case 's':
			seq = strtoul(optarg, NULL, 10);
			break;
		case 'd':
			decode_id = strtoull(optarg, NULL, 10);
			decode_mode = 1;
		case '?':
			return 1;
		default:
			break;
		}
	}

	if (decode_mode) {
		sf_extract_parts(decode_id, &parts);
		printf("id: %lu (0x%016lx)\n\n", decode_id, decode_id);
		printf("timestamp (ms): %lu\n", parts.ts);
		printf("node_id:        %u\n", parts.node_id);
		printf("seq:            %u\n", parts.seq);
		return 0;
	}

	if (!ts_set)
		ts = now_ms();

	id = ((ts - SF_EPOCH_MILLIS) << (SF_NODE_BITS + SF_SEQ_BITS)) |
	     ((uint64_t)(node_id & SF_NODE_MAX) << SF_SEQ_BITS) |
	     (uint64_t)(seq & SF_SEQ_MAX);

	sf_extract_parts(id, &parts);

	printf("id: %lu (0x%016lx)\n\n", id, id);
	printf("timestamp (ms): %lu\n", parts.ts);
	printf("node_id:        %u\n", parts.node_id);
	printf("seq:            %u\n", parts.seq);

	return 0;
}
