# The Rust standard library references the C library's abort() from its
# last-resort handlers. On Linux and macOS the package redirects those
# references to an R error at link time (tools/config.R, src/entrypoint.c), so
# the shared object must not mention abort() at all: `R CMD check` reports it
# as "Found 'abort', possibly from 'abort' (C)" when it scans the whole object.

test_that("the shared object does not reference abort()", {
  skip_on_os("windows")
  nm <- Sys.which("nm")
  skip_if(!nzchar(nm), "nm is not available")

  dll <- getLoadedDLLs()[["polyglotSQL"]]
  skip_if(is.null(dll), "polyglotSQL.so is not loaded")

  syms <- system2(nm, c("-Pg", shQuote(dll[["path"]])), stdout = TRUE)
  names <- sub(" .*$", "", syms)
  expect_false(any(names %in% c("abort", "_abort")))
  # the R entry point is still exported
  expect_true(any(names %in% c("R_init_polyglotSQL", "_R_init_polyglotSQL")))
})
