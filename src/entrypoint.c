// We need to forward routine registration from C to Rust
// to avoid the linker removing the static library.

#include <R.h>
#include <Rinternals.h>

#ifndef _WIN32
#include <pthread.h>
#include <unistd.h>
#endif

void R_init_polyglotSQL_extendr(void *dll);
void register_extendr_panic_hook(void);

#ifndef _WIN32
// Thread on which R loaded the package (R's main thread). Set once in
// R_init_polyglotSQL(), read by polyglotsql_rust_abort() below.
static pthread_t polyglotsql_main_thread;
static int polyglotsql_main_thread_set = 0;
#endif

void R_init_polyglotSQL(void *dll) {
#ifndef _WIN32
    polyglotsql_main_thread = pthread_self();
    polyglotsql_main_thread_set = 1;
#endif
    register_extendr_panic_hook();
    R_init_polyglotSQL_extendr(dll);
}

// Replacement for the C library's abort(), reached from the Rust standard
// library's runtime only.
//
// Rust panics unwind and are caught by extendr at the FFI boundary, where
// they become ordinary R errors; nothing in this package or in the vendored
// crates calls abort(). The Rust standard library does reference abort() for
// the conditions it considers unrecoverable: a panic raised while another
// panic is being handled, a panic that reaches a frame which cannot unwind,
// and a failed memory allocation. On Linux and macOS the package is linked so
// that those references resolve here instead of in libc (see tools/config.R),
// so the shared object does not refer to abort() at all and the R session is
// never terminated by compiled code.
//
// On R's main thread we raise an R error, which is the best available outcome:
// the longjmp skips the Rust frames still on the stack, so the message asks
// the user to restart R. The only other threads that can get here are the
// short-lived helper threads the engine spawns to parse deeply nested SQL
// with a larger stack. R's error handling cannot be used from them, so the
// thread reports the condition and parks itself; the main thread, which is
// waiting for it, stays blocked and the user has to end the session. That
// trades a crash for a hang in a situation that is fatal either way, and it
// is only reachable from one transpilation path of one dialect.
void polyglotsql_rust_abort(void) {
#ifndef _WIN32
    if (polyglotsql_main_thread_set &&
        !pthread_equal(pthread_self(), polyglotsql_main_thread)) {
        REprintf("polyglotSQL: unrecoverable error in the Rust runtime on a "
                 "helper thread; please interrupt and restart R.\n");
        for (;;) {
            pause();
        }
    }
#endif
    Rf_error("polyglotSQL: unrecoverable error in the Rust runtime (for "
             "example a failed memory allocation or a panic while another "
             "panic was being handled). The R session may be in an "
             "inconsistent state: please save your work and restart R.");
}

// References to abort() are sent here by GNU ld / lld (`--wrap=abort`, Linux)
// and by Apple's ld (`-alias ___wrap_abort _abort`, macOS); see tools/config.R.
void __wrap_abort(void) {
    polyglotsql_rust_abort();
}
