*This project has been created as part of the 42 curriculum by \<your_login\>.*

# Codexion

## Description

Codexion is a concurrency simulation inspired by the classic Dining Philosophers
problem. `number_of_coders` coders sit around a table with one dongle between
each pair of neighbors. To compile, a coder must hold **two** dongles. After
compiling, the coder debugs, then refactors, then tries to compile again. If a
coder does not start compiling within `time_to_burnout` milliseconds of their
last compile start, they burn out and the simulation stops.

Two twists distinguish it from the classic problem:

- **Dongle cooldown**: after being released, a dongle is unavailable for
  `dongle_cooldown` milliseconds.
- **Arbitration policy**: when several coders wait for the same dongle, it is
  granted according to a scheduler — `fifo` (arrival order) or `edf`
  (Earliest Deadline First, deadline = `last_compile_start + time_to_burnout`),
  implemented with a hand-written binary min-heap.

## Instructions

### Compilation

```sh
make
```

Compiles with `-Wall -Wextra -Werror -pthread` and produces the `codexion`
binary. Other rules: `make clean`, `make fclean`, `make re`.

### Execution

```sh
./codexion number_of_coders time_to_burnout time_to_compile time_to_debug \
           time_to_refactor number_of_compiles_required dongle_cooldown scheduler
```

Example:

```sh
./codexion 5 800 200 200 100 3 50 fifo
```

All arguments are mandatory. Times are in milliseconds. The scheduler must be
exactly `fifo` or `edf`. Invalid input (negative numbers, non-integers, values
overflowing `int`, unknown scheduler) is rejected with an error message.

The simulation stops when every coder has compiled at least
`number_of_compiles_required` times, or as soon as one coder burns out
(the burnout log is printed within 10 ms of the actual burnout).

## Blocking cases handled

- **Deadlock prevention (Coffman's conditions)**: a deadlock requires four
  simultaneous conditions — mutual exclusion, hold and wait, no preemption,
  and circular wait. We break *circular wait* with an asymmetric acquisition
  order: even-numbered coders take their right dongle first, odd-numbered
  coders their left dongle first. A cycle of waiting threads therefore cannot
  form.
- **Lost-wakeup prevention at shutdown**: when the monitor stops the
  simulation, it broadcasts every dongle's condition variable **while holding
  that dongle's mutex** (`stop_and_wake_all`). Because a waiting coder holds
  the same mutex between checking its predicate and entering
  `pthread_cond_wait`, the wake-up signal cannot fall into that gap. Both stop
  paths (burnout and successful completion) go through this function, so no
  coder can remain blocked forever on a condition variable.
- **Starvation prevention / fair arbitration**: each dongle owns a priority
  queue of pending requests. Under FIFO, a monotonic per-dongle ticket counter
  guarantees strict arrival order even when two requests land in the same
  millisecond. Under EDF, the most endangered coder (earliest deadline) is
  always served first, and the ticket counter acts as a deterministic
  tie-breaker for equal deadlines, as required by the subject.
- **Cooldown handling**: a released dongle stores its release timestamp. The
  head of the queue waits the remaining cooldown with
  `pthread_cond_timedwait` (absolute deadline), so it wakes exactly when the
  dongle becomes legal to take, without busy-waiting and without blocking
  other state changes.
- **Precise burnout detection**: a dedicated monitor thread polls every
  millisecond, comparing `now - last_compile_start` to `time_to_burnout`,
  so the burnout log appears well within the 10 ms requirement.
- **Log serialization**: all output goes through `log_action`, which holds a
  dedicated print mutex, so two messages can never interleave. The same mutex
  protects the simulation's running flag: once "burned out" is printed, the
  flag is cleared in the same critical section, so no message can ever be
  printed after a burnout announcement.

## Thread synchronization mechanisms

- **`pthread_mutex_t` (one per dongle)**: protects the dongle's state
  (`is_taken`, `release_time`, and its waiting queue). Every read or write of
  that state happens under the lock, which prevents two coders from
  "duplicating" a dongle: the take operation (check availability → mark taken
  → dequeue) is atomic with respect to other coders.
- **`pthread_cond_t` (one per dongle)**: coders that cannot take a dongle
  wait on its condition variable instead of spinning. `release_dongle`
  broadcasts under the mutex so all waiters re-evaluate the queue head, the
  taken flag, and the cooldown.
- **`pthread_cond_timedwait`**: used by the queue head to sleep until the
  cooldown expiry (absolute `timespec` built from `gettimeofday`), and to
  remain responsive to broadcasts that arrive earlier.
- **`pthread_mutex_t` per coder**: protects `last_compile_time` and
  `compile_count`, which are written by the coder thread and read by the
  monitor and by the EDF priority computation. This is how race conditions on
  shared per-coder data are prevented.
- **Print/state mutex**: serializes `printf` calls and guards the global
  `is_running` flag. Every thread reads the flag through `is_sim_running()`
  and stops it through `stop_sim()` / `stop_and_wake_all()`, so there is a
  single, consistent locking discipline for simulation state — verified with
  ThreadSanitizer (no data races reported).
- **Monitor ↔ coders communication**: the monitor never touches dongle
  payload state directly; it communicates by clearing the running flag and
  broadcasting the dongles' condition variables, which is a thread-safe,
  lock-ordered handshake (dongle mutex → print mutex is the only nesting
  order used, so no lock-order inversion is possible).

## Resources

- *Operating Systems: Three Easy Pieces* — chapters on threads, locks and
  condition variables: https://pages.cs.wisc.edu/~remzi/OSTEP/
- The Dining Philosophers problem:
  https://en.wikipedia.org/wiki/Dining_philosophers_problem
- POSIX threads documentation (`man pthread_mutex_lock`,
  `man pthread_cond_timedwait`, `man pthread_create`)
- Coffman conditions: https://en.wikipedia.org/wiki/Deadlock#Necessary_conditions
- Earliest Deadline First scheduling:
  https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling
- Binary heap / priority queue: https://en.wikipedia.org/wiki/Binary_heap

### How AI was used

AI (Anthropic's Claude) was used as a code reviewer and debugging assistant on
this project: after a first working implementation was written by hand, the AI
reviewed it and helped identify and fix concurrency bugs (a lost-wakeup
deadlock at shutdown, inconsistent locking of the running flag, a heap buffer
overflow in the single-coder case, a dongle release leak in the stop path, and
missing deterministic tie-breaking in the scheduler), as well as memory-leak
and input-validation edge cases. It was also used to discuss synchronization
concepts (condition-variable semantics, lock ordering, Coffman's conditions)
and to verify the result with ThreadSanitizer and AddressSanitizer. The overall
design (threads per coder, mutex + condition variable per dongle, monitor
thread, heap-based scheduler) and the project structure were defined before
that review.
