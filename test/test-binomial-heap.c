/*

Copyright (c) 2005-2008, Simon Howard

Permission to use, copy, modify, and/or distribute this software 
for any purpose with or without fee is hereby granted, provided 
that the above copyright notice and this permission notice appear 
in all copies. 

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR 
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN      
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 

 */

#include <stdlib.h>
#include <assert.h>

#include "alloc-testing.h"
#include "framework.h"

#include "binomial-heap.h"
#include "compare-int.h"

int test_array[1000];

void test_binomial_heap_new_free(void)
{
	BinomialHeap *heap;
	int i;

	for (i=0; i<1000; ++i) {
		heap = binomial_heap_new(BINOMIAL_HEAP_TYPE_MIN, int_compare);
		binomial_heap_free(heap);
	}

        /* Test for out of memory */

        alloc_test_set_limit(0);

        assert(binomial_heap_new(BINOMIAL_HEAP_TYPE_MIN, int_compare) == NULL);
}

void test_binomial_heap_insert(void)
{
	BinomialHeap *heap;
	int i;

	heap = binomial_heap_new(BINOMIAL_HEAP_TYPE_MIN, int_compare);

	for (i=0; i<1000; ++i) {
		test_array[i] = i;
		assert(binomial_heap_insert(heap, &test_array[i]) != 0);
	}
	assert(binomial_heap_num_entries(heap) == 1000);

        /* Test for out of memory */

        alloc_test_set_limit(0);
        assert(binomial_heap_insert(heap, &i) == 0);

	binomial_heap_free(heap);
}

void test_min_heap(void)
{
	BinomialHeap *heap;
	int *val;
	int i;

	heap = binomial_heap_new(BINOMIAL_HEAP_TYPE_MIN, int_compare);

	/* Push a load of values onto the heap */

	for (i=0; i<1000; ++i) {
		test_array[i] = i;
		assert(binomial_heap_insert(heap, &test_array[i]) != 0);
	}

	/* Pop values off the heap and check they are in order */

	i = -1;
	while (binomial_heap_num_entries(heap) > 0) {
		val = (int *) binomial_heap_pop(heap);

		assert(*val == i + 1);
		i = *val;
	}

        /* Test pop on an empty heap */

        val = (int *) binomial_heap_pop(heap);
        assert(val == NULL);

	binomial_heap_free(heap);
}

void test_max_heap(void)
{
	BinomialHeap *heap;
	int *val;
	int i;

	heap = binomial_heap_new(BINOMIAL_HEAP_TYPE_MAX, int_compare);

	/* Push a load of values onto the heap */

	for (i=0; i<1000; ++i) {
		test_array[i] = i;
		assert(binomial_heap_insert(heap, &test_array[i]) != 0);
	}

	/* Pop values off the heap and check they are in order */

	i = 1000;
	while (binomial_heap_num_entries(heap) > 0) {
		val = (int *) binomial_heap_pop(heap);

		assert(*val == i - 1);
		i = *val;
	}

        /* Test pop on an empty heap */

        val = (int *) binomial_heap_pop(heap);
        assert(val == NULL);

	binomial_heap_free(heap);
}

static BinomialHeap *generate_heap(void)
{
	BinomialHeap *heap;
	int i;
	
	heap = binomial_heap_new(BINOMIAL_HEAP_TYPE_MIN, int_compare);

	/* Push a load of values onto the heap */

	for (i=0; i<1000; ++i) {
		test_array[i] = i;
		if (i != 400) {
			assert(binomial_heap_insert(heap, &test_array[i]) != 0);
		}
	}

	return heap;
}

/* Verify that the values read out of the specified heap are the 
 * same as those inserted in generate_heap. */

static void verify_heap(BinomialHeap *heap)
{
	int *val;
	int numvals;
	int i;

	numvals = binomial_heap_num_entries(heap);
	assert(numvals == 999);

	for (i=0; i<1000; ++i) {
		if (i == 400) {
			continue;
		}

		/* Pop off the next value and check it */

		val = binomial_heap_pop(heap);
		assert(*val == i);

		/* Decrement num values counter */

		--numvals;
		assert(binomial_heap_num_entries(heap) == numvals);
	}
}

/* Test out of memory when doing an insert */

static void test_insert_out_of_memory(void)
{
	BinomialHeap *heap;
	int i;

	/* There are various memory allocations performed during the insert;
	 * probe at different limit levels to catch them all. */

	for (i=0; i<6; ++i) {
		heap = generate_heap();

		/* Insert should fail */

		alloc_test_set_limit(i);
		test_array[400] = 400;
		assert(binomial_heap_insert(heap, &test_array[400]) == 0);
		alloc_test_set_limit(-1);

		/* Check that the heap is unharmed */

		verify_heap(heap);

		binomial_heap_free(heap);
	}
}

/* Test out of memory when doing a pop */

void test_pop_out_of_memory(void)
{
	BinomialHeap *heap;
	int i;

	/* There are various memory allocations performed as part of the merge
	 * done during the pop.  Probe at different limit levels to catch them
	 * all. */

	for (i=0; i<6; ++i) {
		heap = generate_heap();

		/* Pop should fail */

		alloc_test_set_limit(i);
		assert(binomial_heap_pop(heap) == NULL);
		alloc_test_set_limit(-1);

		/* Check the heap is unharmed */

		binomial_heap_free(heap);
	}
}

static UnitTestFunction tests[] = {
	test_binomial_heap_new_free,
	test_binomial_heap_insert,
	test_min_heap,
	test_max_heap,
	test_insert_out_of_memory,
	test_pop_out_of_memory,
	NULL
};

int main(int argc, char *argv[])
{
	run_tests(tests);

	return 0;
}

