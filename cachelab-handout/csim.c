#include "cachelab.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

typedef struct entry {
    long long int tag;
    int touch;
    int valid;
} Entry;

int main(int argc, char *argv[])
{
    /* parse the command line arguments */
    int opt, v = 0, s = 0, E = 0, b = 0;
    int S;
    char *t = NULL;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                printf("./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
                printf("./csim -v -s 4 -E 1 -b 4 -t traces/yi.trace\n");
                break;
            case 'v':
                v = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = malloc(strlen(optarg) + 1);
                strcpy(t, optarg);
                break;
            default:
                printf("getopt err");
                exit(EXIT_FAILURE);
        }
    }

    S = 1 << s;
    // printf("v = %d, s = %d, E = %d, b = %d, t = %s\n", v, s, E, b, t);

    /* allocate storage for the simulator's data structures */
    Entry *table;

    table = (Entry *)calloc(S * E , sizeof(Entry));
    assert(table != NULL);

    /* parse the trace */
    FILE *fp;

    if (!(fp = fopen(t, "r"))) {
        printf("fopen err %s\n", t);
        exit(EXIT_FAILURE);
    }

    int hit_count = 0, miss_count = 0, eviction_count = 0;
    int tag_shift = s + b;
    int set_shift = b;
    int set_mask = S - 1;
    int touch = 0;

    char operation;
    long long int address;
    int size;
    long long int tag;
    int set;

    while ((fscanf(fp, " %c %llx,%d", &operation, &address, &size)) != EOF) {
        if (operation == 'I') {
            continue;
        } else if (operation == 'L' || operation == 'S' || operation == 'M') {
            tag = address >> tag_shift;
            set = (address >> set_shift) & set_mask;

            int hit = 0;
            int target_index, target_touch;

            target_touch = ++touch;

            for (int index = set * E; index < (set + 1) * E; index++) {
                if (table[index].tag == tag && table[index].valid == 1) {
                    hit = 1;
                    target_index = index;
                    break;
                } else {
                    if (table[index].touch < target_touch) {
                        target_index = index;
                        target_touch = table[index].touch;
                    }
                }
            }

            if (v) {
                printf("%c %llx,%d", operation, address, size);
            }

            if (hit) {
                hit_count++;
                if (v) {
                    printf(" hit");
                }
            } else {
                miss_count++;
                if (v) {
                    printf(" miss");
                }
                if (table[target_index].valid) {
                    eviction_count++;
                    if (v) {
                        printf(" eviction");
                    }
                }
                table[target_index].tag = tag;
                table[target_index].valid = 1;
            }
            table[target_index].touch = touch;

            if (operation == 'M') {
                hit_count++;
                if (v) {
                    printf(" hit");
                }
            }

            if (v) {
                printf(" \n");
            }
        } else {
            printf("fscanf err %c\n", operation);
            exit(EXIT_FAILURE);
        }
    }

    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
