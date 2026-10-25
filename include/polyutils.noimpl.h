#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

#if !defined(POLYUTILS_INCLUDE_A_H) && defined(__has_include)
	#if __has_include(<a.h>)
		#define POLYUTILS_INCLUDE_A_H
	#endif
#endif

#define templated static inline

#if SIZE_MAX == UINT64_MAX
typedef struct ring_buffer_t{
	char* data;
	size_t cap_exp:8;
	size_t l:sizeof(size_t)*CHAR_BIT-8; size_t used;
} ring_buffer_t;
typedef struct ring_iterator_t{
	char *head, *end;
	size_t cap_exp:8, remaining:sizeof(size_t)*CHAR_BIT-8;
} ring_iterator_t;
#else
typedef struct ring_buffer_t{
	char* data;
	size_t cap_exp, l, used;
} ring_buffer_t;
typedef struct ring_iterator_t{
	char *head, *end;
	size_t cap_exp, remaining;
} ring_iterator_t;
#endif

#if SIZE_MAX == UINT64_MAX
typedef struct array_buffer_t{
	char* data;
	size_t cap_exp:8, len:sizeof(size_t)*CHAR_BIT-8;
} array_buffer_t;
#else
typedef struct array_buffer_t{
	char* data;
	size_t cap_exp, len;
} array_buffer_t;
#endif
typedef struct array_iterator_t{
	char *head; size_t remaining;
} array_iterator_t;

// A fast approximation to 2^x, with a maximum relative error of about 0.088%
static inline float fast_exp2f(float x);

// Poor man's exponential. between 2^n and 2^(n+1), the function is completely linear. Maximum relative error ~6.15%
static inline float discrete_exp2f(float x);
// Poor man's logarithm. See also: `discrete_exp2f` (Poor man's exponential)
static inline float discrete_log2f(float x);

templated size_t ring_buffer_size(ring_buffer_t* obj);

templated size_t ring_buffer_push_garbage(ring_buffer_t* obj, size_t sz, bool _aligned);
templated void ring_buffer_push(ring_buffer_t* obj, void* d, size_t sz, bool aligned);
templated void ring_buffer_push_memset(ring_buffer_t* obj, char v, size_t sz, bool aligned);

templated void ring_buffer_shift_discard(ring_buffer_t* obj, size_t sz, bool aligned);
templated void ring_buffer_shift(ring_buffer_t* obj, void* d, size_t sz, bool aligned);

templated void ring_buffer_get(ring_buffer_t* obj, size_t i, void* d, size_t sz, bool aligned);
templated void ring_buffer_set(ring_buffer_t* obj, size_t i, void* d, size_t sz, bool aligned);

templated ring_iterator_t ring_buffer_iterator(ring_buffer_t* obj, size_t i, size_t count);

templated size_t ring_iterator_next(ring_iterator_t* obj, void* d, size_t sz, bool aligned);

templated void ring_buffer_destroy(ring_buffer_t* obj);


templated size_t array_buffer_size(array_buffer_t* obj);
templated void* array_buffer_data(array_buffer_t* obj);

templated void array_buffer_setsize_garbage(array_buffer_t* obj, size_t sz);

templated void* array_buffer_push_garbage(array_buffer_t* obj, size_t sz);
templated void array_buffer_push(array_buffer_t* obj, void* d, size_t sz);
templated void array_buffer_push_memset(array_buffer_t* obj, int v, size_t sz);

templated void array_buffer_pop_discard(array_buffer_t* obj, size_t sz);
templated void array_buffer_pop(array_buffer_t* obj, void* d, size_t sz);

templated void array_buffer_get(array_buffer_t* obj, size_t i, void* d, size_t sz);
templated void array_buffer_set(array_buffer_t* obj, size_t i, void* d, size_t sz);

templated array_iterator_t array_buffer_iterator(array_buffer_t* obj, size_t i, size_t count);

templated size_t array_iterator_next(array_iterator_t* obj, void* d, size_t sz);

templated void array_buffer_destroy(array_buffer_t* obj);

#ifdef POLYUTILS_INCLUDE_A_H
	#include <a.h>
	
	#define SHARED_LOCK_INIT LOCK_MAX
	typedef lock_t shared_lock_t;

	templated void shared_lock_init(shared_lock_t* s);
	// Acquire the "shared" part of a shared lock. This will block if any thread is trying to or has already obtained an exclusive lock. This will also cause all future exclusive lock acquires to block until the shared lock is released
	templated void shared_lock_acquire(shared_lock_t* s);
	// Wait for the "shared" part of a shared lock to become available, if it isn't. This will block if any thread is trying to or has already obtained an exclusive lock. No lock is acquired, another thread may immediately acquire the lock while this function returns, you should not call `shared_lock_release` after this.
	templated void shared_lock_wait(shared_lock_t* s);
	// Try to acquire the "shared" part of a shared lock. This will return false if any thread is trying to or has already obtained an exclusive lock. See `shared_lock_acquire`
	templated bool shared_lock_try_acquire(shared_lock_t* s);
	// Release the "shared" part of a shared lock. Releasing more times than was acquired (i.e, calling this function when no shared part is currently acquired) is UB
	templated void shared_lock_release(shared_lock_t* s);
	// Upgrade a "shared" lock to an exclusive one. This operation may be performed without ever releasing the shared part of the lock, in which case it will return `true`. Note that this cannot be guaranteed due to the possibility of deadlocks. This function avoids deadlocks by briefly dropping the shared part if necessary, in which case it will return `false`. In all cases, upgrading when no shared part was acquired to begin with is UB
	templated bool shared_lock_upgrade(shared_lock_t* s);
	// Try to upgrade a "shared" lock to an exclusive one. This operation is performed without ever releasing the shared part of the lock. If another thread owns or is trying to obtain the exclusive lock, there is a possibility of deadlocks, and the function will return false (it follows that this function cannot block on the exclusive lock, however it can block if other threads hold the shared part of the lock). In all cases, upgrading when no shared part was acquired to begin with is UB
	templated bool shared_lock_try_upgrade(shared_lock_t* s);
	// Acquire the "exclusive" part of a shared lock. This will block if any thread is trying to or has already obtained an exclusive lock. This will also cause all future shared/exclusive lock acquires to block until the exclusive lock is released
	templated void exclusive_lock_acquire(shared_lock_t* s);
	// Wait for the "exclusive" part of a shared lock to become available. This will block if any thread is trying to or has already obtained an exclusive lock. No lock is acquired, another thread may immediately acquire the lock while this function returns, you should not call `exclusive_lock_release` after this.
	templated void exclusive_lock_wait(shared_lock_t* s);
	// Try to acquire the "exclusive" part of a shared lock. This will return false if any thread is trying to or has already obtained an exclusive lock. See `exclusive_lock_acquire`
	templated bool exclusive_lock_try_acquire(shared_lock_t* s);
	// Release the "exclusive" part of a shared lock. Releasing when no exclusive part was acquired is UB
	templated void exclusive_lock_release(shared_lock_t* s);
	// Downgrade from an "exclusive" lock to a shared lock, without releasing the shared part. Unlike `shared_lock_upgrade`, this does not have the same deadlock danger, and will therefore always succeed. Downgrading when no exclusive part was acquired is UB
	templated void exclusive_lock_downgrade(shared_lock_t* s);
#endif

#if SIZE_MAX >= UINT64_MAX
	typedef struct hash_table_t{ uintptr_t cap_exp:8, data:sizeof(uintptr_t)*CHAR_BIT-8; } hash_table_t;
#else
	typedef struct hash_table_t{ uintptr_t cap_exp, data; } hash_table_t;
#endif

// 
templated void* hash_table_find(const hash_table_t* t, uint64_t hash);
templated void hash_table_put(hash_table_t* t, uint64_t hash, void* v);
templated void hash_table_set_rank(hash_table_t* t, unsigned rank);
templated unsigned hash_table_rank(hash_table_t* t);

#undef templated